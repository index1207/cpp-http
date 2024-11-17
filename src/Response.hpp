#pragma once

#include <string>

class Response
{
public:
    Response() = default;
    Response(std::string_view body);
public:
    std::string contentType;
    std::string body;
private:
    int statusCode = 200;
};
