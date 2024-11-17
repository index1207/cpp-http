#pragma once
#include <net/socket.hpp>
#include <net/context.hpp>

#include "Response.hpp"

using Action = std::function<Response(std::string_view)>;

class HttpListener
{
    friend class HttpClient;
public:
    HttpListener();
public:
    void MapGet(std::string_view path, Action action);
    void Start(net::endpoint endpoint);
private:
    net::socket _listenSock;
    std::unordered_map<std::string, Action> _getMap;
};
