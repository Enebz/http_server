#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

// Includes
#include "../hashtable/hashtable.h"

#include "client.h"
#include "method.h"
#include "version.h"

typedef struct HttpRequest HttpRequest;

struct HttpRequest
{
    HttpClient *client;
    HttpMethod method;
    char *url;
    HttpVersion version;

    hashtable *query;
    hashtable *headers;

    char *body;
};

HttpRequest *http_request_create(HttpClient *client, HttpMethod method, char *url, HttpVersion version);
void http_request_destroy(HttpRequest *request);

#endif // HTTP_REQUEST_H