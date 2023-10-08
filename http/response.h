#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

// Includes
#include "../hashtable/hashtable.h"

#include "request.h"
#include "status.h"

typedef struct HttpResponse HttpResponse;

struct HttpResponse
{
    HttpRequest *request;
    HttpStatusCode status;

    hashtable *headers;

    char *body;
};

HttpResponse *http_response_create(HttpRequest *request);
void http_response_destroy(HttpResponse *response);

int http_response_set_file(HttpResponse *response, char *file_path);

char* http_response_to_string(HttpResponse *response);

#endif // HTTP_RESPONSE_H