#include "server.h"
#include "route.h"

int checkSocketError(int socket, int error_code, char *message)
{
    if (socket == error_code)
    {
        printf("%s with error code: %d", message, WSAGetLastError());
        WSACleanup();
        exit(1);
    }
    return 0;
}

// For HTTP Server
HttpServer *http_server_create(int port, int backlog, int max_connections, int max_request_size, int buffer_size)
{
    // Allocate memory for the server
    HttpServer *server = malloc(sizeof(HttpServer));

    // Create a socket, which is now unbound to any specific local address
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET)
    {
        printf("Could not create socket : %d", WSAGetLastError());
        WSACleanup();
        exit(1);
    }

    server->socket = sock;

    // Allocate memory for the server
    server->address = malloc(sizeof(*(server->address)));

    // Prepare the server structure
    server->address->sin_family = AF_INET;
    server->address->sin_addr.s_addr = INADDR_ANY;
    server->address->sin_port = htons(port);
    server->address_len = sizeof(*(server->address));
    server->port = port;
    server->backlog = backlog;
    server->max_connections = max_connections;
    server->max_request_size = max_request_size;
    server->buffer_size = buffer_size;
    server->clients = (HttpClient**)malloc(sizeof(HttpClient*) * max_connections);
    server->threads = (PHANDLE)malloc(sizeof(HANDLE) * max_connections);

    // Create the method routes
    for (int i = 0; i < 9; i++)
    {
        server->method_routes[i] = tree_create(NULL);
    }

    // Bind the socket to the local address and port and tell it which addresses it can receive messages from
    checkSocketError(bind(server->socket, (struct sockaddr *)server->address, server->address_len), SOCKET_ERROR, "Bind failed");
    return server;
}

void http_server_destroy(HttpServer *server)
{
    // Close the socket
    closesocket(server->socket);

    // Free the address
    free(server->address);

    // Free the clients
    for (int i = 0; i < server->max_connections; i++)
    {
        if (server->clients[i] != NULL)
        {
            http_client_destroy(server->clients[i]);
        }
    }

    free(server->clients);

    // Free the threads
    free(server->threads);

    // Free the server
    free(server);
}

int http_recv_request(HttpClient *client, short bufferSize, HttpRequest *request)
{
    char *buffer = (char*)malloc(bufferSize);


    char *header = (char*)malloc(1);
    char *header_end = NULL;
    int header_len;
    SSIZE_T header_bytes_recieved = 0;
    SSIZE_T total_header_bytes_recieved = 0;
    

    // Receive the headers
    while (1)
    {
        // Receive the request
        header_bytes_recieved = recv(client->socket, buffer, bufferSize, 0);
        if (header_bytes_recieved == SOCKET_ERROR)
        {
            printf("recv failed: %d", WSAGetLastError());
            WSACleanup();
            return 1;
        }

        if (header_bytes_recieved == 0)
        {
            // The client has disconnected
            printf("Client disconnected\n");
            return 2;
        }


        header = (char*)realloc(header, total_header_bytes_recieved + header_bytes_recieved);
        memcpy(header + total_header_bytes_recieved, buffer, header_bytes_recieved);

        total_header_bytes_recieved += header_bytes_recieved;

        if ((header_end = strstr(header, CRLFCRLF)) != NULL)
        {
            header_len = (int)(header_end - header) + 2;
            
            // Reallocation to remove the body
            header = (char*)realloc(header, header_len + 1);
            header[header_len] = '\0';
            break;
        }
    }
    
    // Parse the headers
    http_parse_headers(header, request);
    return 0;
}

int http_parse_headers(char *request, HttpRequest *http_request_p)
{
    // Get the method
    char *method_end = strstr(request, " ");
    int method_len = (int)(method_end - request);
    char *method = (char*)malloc(method_len + 1); // +1 for the null terminator
    memcpy(method, request, method_len);
    method[method_len] = '\0';
    http_request_p->method = str_to_http_method(method);
    free(method);

    // Get the url
    char *url_start = method_end + 1;
    char *url_end = strstr(url_start, " "); // +1 to skip the space after the method
    int url_len = (int)(url_end - url_start);
    char *url = (char*)malloc(url_len + 1); // +1 for the null terminator
    memcpy(url, url_start, url_len);
    url[url_len] = '\0';
    http_request_p->url = url;
    // Don't free the url because the pointer is what is stored in the request

    // Get the version major and minor using sscanf
    char *version_start = url_end + 1;
    char *version_end = strstr(version_start, CRLF); // +1 to skip the space after the url
    int version_len = (int)(version_end - version_start);
    char *version = (char*)malloc(version_len + 1); // +1 for the null terminator
    memcpy(version, version_start, version_len);
    version[version_len] = '\0';
    sscanf(version, "HTTP/%hi.%hi", &http_request_p->version.major, &http_request_p->version.minor);
    free(version);

    // Get the headers
    return 0;
}

int http_server_listen(HttpServer* server)
{
    // Listen for incoming connections
    printf("Enebz HTTP running on port %d!\n", server->port);
    checkSocketError(listen(server->socket, 50), SOCKET_ERROR, "Listen failed");

    // Set the server to running
    server->running = 1;
    server->client_count = 0;

    while (server->running == 1)
    {
        SOCKET client_socket;
        struct sockaddr_in client_address;
        int client_address_len = sizeof(client_address);

        // Malloc the client
        server->clients[server->client_count] = http_client_create(client_socket, client_address, client_address_len);
        HttpClient *client = server->clients[server->client_count];

        // Accept needs the address length to be set
        client->address_len = sizeof(client->address);

        // Accept an incoming connection        
        client_socket = accept(server->socket, (struct sockaddr*)&client->address, &client->address_len);

        if (client_socket == INVALID_SOCKET)
        {
            printf("\nAccept failed with error code : %d\n", WSAGetLastError());

            // Set client to null, so it can be reused
            continue;
        }

        // Set the client socket
        client->socket = client_socket;        

        // Handle the client in a new thread
        PHANDLE thread = &(server->threads[server->client_count]);
        DWORD dwThreadId = rand();

        client->ip = inet_ntoa(client->address.sin_addr);
        client->thread_id = dwThreadId;

        // Create the client handler args
        HttpClientHandlerArgs *client_handler_args = http_client_handler_args_create(client, server);

        *thread = CreateThread(NULL, 0, http_handle_client, (LPVOID)client_handler_args, 0, &dwThreadId);

        if (*thread == NULL)
        {
            printf("\nThread could not be created.\n");

            // Set client to null, so it can be reused
            http_client_destroy(client);
            http_client_handler_args_destroy(client_handler_args);
            continue;
        }

        server->client_count++;
    }

    // Close the server
    server->running = 0;

    // Wait for all threads to finish
    WaitForMultipleObjects(server->client_count, server->threads, TRUE, INFINITE);

    // Close all the threads
    for (int i = 0; i < server->max_connections; i++)
    {
        CloseHandle(server->threads[i]);
    }

    return 0;
}

/// PICKUP HERE

DWORD WINAPI http_handle_client(LPVOID lpParam)
{
    // Unpack parameters
    HttpClientHandlerArgs *args = (HttpClientHandlerArgs*)lpParam;
    HttpClient *client = args->client;
    HttpServer *server = args->server;

    // After unpacking, free the args
    free(args);

    // Log the thread
    log_message("TID%d\t\t| %s CONNECTED", client->thread_id, client->ip);

    // Build the request and response
    HttpRequest *http_request = http_request_create(client, HTTP_INVALID, NULL, http_version(1, 1));
    HttpResponse *http_response = http_response_create(NULL);
    http_response->request = http_request;

    // Handle the request
    int handle_request_result = http_handle_request(server, client, http_request, http_response);

    if (handle_request_result != -1)
    {
        // Client disconnected
        printf("Sending response\n");
        http_send_response(client, http_response);  
    }

    // Cleanup
    closesocket(client->socket);
    server->client_count--;
    server->clients[server->client_count] = NULL;
    http_client_destroy(client);

    http_request_destroy(http_request);
    http_response_destroy(http_response);
    log_message("TID%d\t\t| %s DISCONNECTED", client->thread_id, client->ip);
    return 0;
}

int http_handle_request(HttpServer *server, HttpClient *client, HttpRequest *http_request, HttpResponse *http_response)
{    
    int recv_result = http_recv_request(client, server->buffer_size, http_request);

    // Client disconnected
    if (recv_result == 2)
    {
        return -1;
    }

    // Recv failed (500 Internal Server Error)
    if (recv_result == 1)
    {
        http_response->status = http_status_code(500, "Internal Server Error");
        log_message("TID%d\t\t| %s RECV FAILED", client->thread_id, client->ip);
        return 1;
    }

    void *data = tree_get(server->method_routes[http_request->method], http_request->url, http_route_search);
    
    // Route not found (404 Not Found)
    if (data == NULL)
    {
        http_response->status = http_status_code(404, "Not Found");
        log_message("TID%d\t\t| %s REQUESTED URL NOT ROUTED", client->thread_id, client->ip);
        return 1;
    }

    HttpRoute *route = (HttpRoute*)data;
    int success = route->handler(http_request, http_response);

    // If user set status code, use that
    if (http_response->status.code != 0)
    {
        log_message("TID%d\t\t| %s USER SET STATUS CODE", client->thread_id, client->ip);
        return 0;
    }

    // Handler returned fail (500 Internal Server Error)
    if (success == 1)
    {
        http_response->status = http_status_code(500, "Internal Server Error");
        log_message("TID%d\t\t| %s UNHANDLED ERROR", client->thread_id, client->ip);
        return 1;
    }

    // Handler returned unimplemented (501 Not Implemented)
    if (success == 2)
    {
        http_response->status = http_status_code(501, "Not Implemented");
        log_message("TID%d\t\t| %s UNIMPLEMENTED", client->thread_id, client->ip);
        return 1;
    }

    // Handler returned success (200 OK)
    http_response->status = http_status_code(200, "OK");
    log_message("TID%d\t\t| %s SUCCESS", client->thread_id, client->ip);
    return 0;
}

int http_send_response(HttpClient *client, HttpResponse *response)
{
    // Before sending the response, set the default headers
    ht_insert(response->headers, "Cache-Control", "no-cache");
    ht_insert(response->headers, "Server", "EnebzHTTP");

    // Response headers related to body (Content-Length, Content-Type, Last-Modified)
    if (response->body != NULL)
    {
        // Content-Length
        int n = strlen(response->body);
        short content_length_digits = 0;
        do {
            n /= 10;
            content_length_digits++;
        } while (n > 0);

        char content_length[content_length_digits + 1]; // +1 for null terminator
        sprintf(content_length, "%lld", strlen(response->body));
        content_length[content_length_digits] = '\0';
        ht_insert(response->headers, "Content-Length", content_length);

        // Content-Type
        if (ht_get(response->headers, "Content-Type") == NULL)
        {
            ht_insert(response->headers, "Content-Type", "text/plain");
        }
    }

    // Send the response
    char *response_str = http_response_to_string(response);
    send(client->socket, response_str, strlen(response_str), 0);
    log_message("TID%d\t\t| %s SENT RESPONSE", client->thread_id, client->ip);

    // Free the response string
    free(response_str);
    return 0;
}

HttpClientHandlerArgs *http_client_handler_args_create(HttpClient *client, HttpServer *server)
{
    HttpClientHandlerArgs *args = malloc(sizeof(HttpClientHandlerArgs));
    args->client = client;
    args->server = server;
    return args;
}

void http_client_handler_args_destroy(HttpClientHandlerArgs *args)
{
    free(args);
}