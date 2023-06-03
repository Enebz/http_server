#include "response.h"

// Function Definitions
HttpResponse *http_response_create(HttpRequest *request)
{
    HttpResponse *response = malloc(sizeof(HttpResponse));
    response->request = request;
    response->status.code = 0;
    response->status.reason = NULL;
    response->headers = ht_create(30);
    response->body = NULL;
    return response;
}

void http_response_destroy(HttpResponse *response)
{
    ht_destroy(response->headers);
    free(response);
}

char* http_response_to_string(HttpResponse *response)
{
    // Create the response string
    char *response_str = (char*)malloc(4096);
    sprintf(response_str, "HTTP/1.1 %d %s\r\n", response->status.code, response->status.reason);

    // Add headers (not implemented, therefore we add a blank line)
    char headers[] = "\r\n";
    strcat(response_str, headers);

    // Add body
    strcat(response_str, response->body);

    return response_str;
}