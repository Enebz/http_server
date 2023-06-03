#ifndef HTTP_METHOD_H
#define HTTP_METHOD_H

#include <stddef.h>
#include <string.h>

typedef enum HttpMethod HttpMethod;

enum HttpMethod {
    HTTP_GET,
    HTTP_POST,
    HTTP_PUT,
    HTTP_DELETE,
    HTTP_HEAD,
    HTTP_CONNECT,
    HTTP_OPTIONS,
    HTTP_TRACE,
    HTTP_PATCH,
    HTTP_INVALID = -1,
};

enum HttpMethod str_to_http_method(char *str);

#endif // HTTP_METHOD_H