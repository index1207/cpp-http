#include <net/native.hpp>

#include "HttpListener.hpp"

int main()
{
    net::native::option::thread_count = 2;

    net::native::initialize();

    HttpListener listener;
    listener.MapGet("/", [](auto path) -> Response {
        Response response;
        response.contentType = "text/plain";
        response.body = "Hello, World";

        return response;
    });
    listener.Start(net::endpoint(net::ip_address::loopback, 5000));
}
