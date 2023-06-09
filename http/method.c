#include "method.h"


enum HttpMethod string_to_http_method(char *str)
{
    static const struct {
        const char* methodString;
        HttpMethod httpMethod;
    } methodMap[] = {
        { "GET", HTTP_GET },
        { "POST", HTTP_POST },
        { "PUT", HTTP_PUT },
        { "DELETE", HTTP_DELETE },
        { "HEAD", HTTP_HEAD },
        { "CONNECT", HTTP_CONNECT },
        { "OPTIONS", HTTP_OPTIONS },
        { "TRACE", HTTP_TRACE },
        { "PATCH", HTTP_PATCH }
    };

    const size_t numMethods = sizeof(methodMap) / sizeof(methodMap[0]);

    for (size_t i = 0; i < numMethods; ++i) {
        if (strcmp(str, methodMap[i].methodString) == 0) {
            return methodMap[i].httpMethod;
        }
    }

    // Return an appropriate value for an unknown or unsupported method
    return HTTP_INVALID;
}

char* http_method_to_string(HttpMethod method)
{
    switch (method)
    {
    case HTTP_GET:
        return "GET";
    case HTTP_POST:
        return "POST";
    case HTTP_PUT:
        return "PUT";
    case HTTP_DELETE:
        return "DELETE";
    case HTTP_HEAD:
        return "HEAD";
    case HTTP_CONNECT:
        return "CONNECT";
    case HTTP_OPTIONS:
        return "OPTIONS";
    case HTTP_TRACE:
        return "TRACE";
    case HTTP_PATCH:
        return "PATCH";
    default:
        return "INVALID";
    }
}