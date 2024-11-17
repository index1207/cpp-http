#pragma once
#include <memory>
#include <net/socket.hpp>
#include <net/context.hpp>

class HttpClient : public std::enable_shared_from_this<HttpClient>
{
public:
    explicit HttpClient(class HttpListener* listener);
    ~HttpClient();
public:
    void Start(std::unique_ptr<net::socket> sock);
private:
    void OnReceiveCompleted(net::context*, bool);
private:
    class HttpListener* _listener;
    std::unique_ptr<net::socket> _sock;
    std::shared_ptr<HttpClient> _thisPtr;

    net::context _recvCtx;

    char _buffer[0x10000];
};
