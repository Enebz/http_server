#ifndef HTTP_ROUTE_H
#define HTTP_ROUTE_H

// Includes
#include "../tree/tree.h"

#include "server.h"
#include "request.h"
#include "response.h"
#include "method.h"

// Header
typedef struct HttpRoute HttpRoute;

struct HttpRoute
{
    HttpMethod method;
    char *url;
    int (*handler)(HttpRequest *request, HttpResponse *response);

    // Rate limiting
    int max_requests;
    int seconds;

    // Rate limiting bucket for IP addresses
    hashtable *token_buckets;

    // Critical section
    LPCRITICAL_SECTION cs;  
};

// Prototypes
HttpRoute *http_route_create(HttpMethod method, char *url, int (*handler)(HttpRequest *request, HttpResponse *response));
void http_route_destroy(HttpRoute *route);

int http_route_limit(HttpRoute *route, int max_requests, int seconds);

int http_route_serve(HttpServer *server, HttpRoute *route);
tree_direction http_route_compare(void *new_node, void *comp_node);
tree_direction http_route_search(void *route_url, void *comp_node);


#endif // HTTP_ROUTE_H