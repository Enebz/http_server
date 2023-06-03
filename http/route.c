#include "route.h"



HttpRoute *http_route_create(HttpMethod method, char *url, int (*handler)(HttpRequest *request, HttpResponse *response))
{
    // Allocate memory for route
    HttpRoute *route = (HttpRoute*)malloc(sizeof(HttpRoute));

    // Set route data
    route->method = method;
    route->url = url;
    route->handler = handler;

    // Return route
    return route;
}

void http_route_destroy(HttpRoute *route)
{
    // Free route
    free(route);
}

int http_route_serve(HttpServer *server, HttpRoute *route)
{
    // Add the route to the tree
    server->method_routes[route->method] = tree_insert(server->method_routes[route->method], route, http_route_compare);
    return 0;
}

tree_direction http_route_compare(void *new_node, void *comp_node)
{
    // Cast to route_data
    HttpRoute *new_node_c = (HttpRoute*)new_node;
    HttpRoute *comp_node_c = (HttpRoute*)comp_node;

    // If route if befor the compare route, go left
    // If route is the same or over, go right
    int result = strcmpi(new_node_c->url, comp_node_c->url);
    return (result == 0 ? 0 : result / abs(result));
}

tree_direction http_route_search(void *route_url, void *comp_node)
{
    // Cast to route_data
    char *url = (char*)route_url;
    HttpRoute *comp_node_c = (HttpRoute*)comp_node;

    // If route if befor the compare route, go left
    // If route is the same or over, go right
    int result = strcmpi(url, comp_node_c->url);
    return (result == 0 ? 0 : result / abs(result));
}