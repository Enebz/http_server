#include "request.h"

// Function Definitions
HttpRequest *http_request_create(HttpClient *client, HttpMethod method, char *url, HttpVersion version)
{
    HttpRequest *request = malloc(sizeof(HttpRequest));
    request->client = client;
    request->method = method;
    request->url = url;
    request->version = version;
    request->query = ht_create(10);
    request->headers = ht_create(30);
    request->body = NULL;
    return request;
}

void http_request_destroy(HttpRequest *request)
{
    ht_destroy(request->headers);
    free(request);
}