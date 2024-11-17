#include "HttpListener.hpp"

#include <iostream>
#include <net/exception.hpp>

#include "HttpClient.hpp"

HttpListener::HttpListener() : _listenSock(net::protocol::tcp)
{
}

void HttpListener::MapGet(std::string_view path, Action action)
{
    _getMap[path.data()] = action;
}

void HttpListener::Start(net::endpoint endpoint)
{
    try
    {
        if (!_listenSock.bind(endpoint))
            throw net::exception("socket.bind()");
        if (!_listenSock.listen())
            throw net::exception("socket.listen()");

        while (true)
        {
            auto connection = new net::socket(_listenSock.accept());
            net::native::observe(connection);

            auto client = std::make_shared<HttpClient>(this);
            client->Start(std::unique_ptr<net::socket>(connection));
        }
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}
