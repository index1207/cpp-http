#include "HttpClient.hpp"
#include "HttpListener.hpp"

#include <iostream>
#include <sstream>
#include <format>

#include <net/exception.hpp>

HttpClient::HttpClient(HttpListener* listener) : _sock(nullptr), _buffer { "" }, _listener(listener)
{
    _recvCtx.set_buffer(_buffer, 0, 0x10000);
    _recvCtx.completed = [&](net::context* ctx, bool success) {
        OnReceiveCompleted(ctx, success);
    };
}

HttpClient::~HttpClient()
{
}

void HttpClient::Start(std::unique_ptr<net::socket> sock)
{
    _sock = std::move(sock);
    _thisPtr = shared_from_this();

    if (!_sock->receive(&_recvCtx))
        OnReceiveCompleted(nullptr, false);
}

void HttpClient::OnReceiveCompleted(net::context* ctx, bool success)
{
    if (!success || ctx->length == 0)
    {
        _thisPtr = nullptr;
        return;
    }

    std::vector<std::string> requestHeaders;
    std::string requestStartline, requestBody;
    std::stringstream ss(std::string(_buffer).substr(0, ctx->length));
    std::string t;

    bool isStart = true, isBody = false;
    while (std::getline(ss, t, '\n'))
    {
        std::string line(t.begin(), t.begin() + t.length() - 1);
        if (isStart)
        {
            requestStartline = line;
            isStart = false;
        }
        else if (isBody)
        {
            requestBody = t;
            isBody = false;
        }
        else if (t == "")
        {
            requestBody = true;
        }
        else
        {
            requestHeaders.emplace_back(line);
        }
    }

    std::string method, path;
    int endOfMethodStrIdx = 0;
    for (int i = 0; i < requestStartline.length(); ++i)
    {
        if (requestStartline[i] == ' ')
        {
            if (method.empty())
            {
                method = requestStartline.substr(0, 3);
                endOfMethodStrIdx = i;
            }
            else
            {
                path = requestStartline.substr(endOfMethodStrIdx + 1, i - endOfMethodStrIdx - 1);
            }
        }
    }

    if (method == "GET")
    {
        std::string responseStr;
        auto action = _listener->_getMap[path.c_str()];
        if (action == nullptr)
        {
            responseStr = "HTTP/1.1 404 NotFound\r\n"\
                "Content-Type: text/plain\r\n"\
                "Content-Length: 14\r\n"\
                "\r\n"\
                "Page not found\r\n";
        }
        else
        {
            auto response = action(path);
            responseStr = std::format("HTTP/1.1 200 OK\r\n"\
                "Content-Type: {}\r\n"\
                "Content-Length: {}\r\n"\
                "\r\n"\
                "{}", response.contentType, response.body.length(), response.body);
        }

        if (!_sock->send(responseStr))
            _thisPtr = nullptr;
    }
    else if (method == "POST")
    {

    }

    if (!_sock->receive(&_recvCtx))
        OnReceiveCompleted(nullptr, false);
}
