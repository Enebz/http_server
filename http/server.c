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

int http_recv_headers(HttpClient *client, short bufferSize, char **request_p)
{
    // Read incoming HTTP request
    char *buffer = (char*)malloc(bufferSize);
    SSIZE_T request_len = 0;
    SSIZE_T total_bytes_received = 0;
    SSIZE_T bytes_recieved = 0;

    do
    {
        bytes_recieved = recv(client->socket, buffer, bufferSize, 0);   
        if (bytes_recieved == 0)
        {
            // Client disconnected
            printf("Client disconnected.\n");

            // Free memory
            free(buffer);
            return 2;
        }

        if (bytes_recieved == SOCKET_ERROR)
        {
            printf("\nrecv failed with error code : %d\n", WSAGetLastError());

            // Free memory
            free(buffer);
            return 1;
        }
        
        request_len += bytes_recieved;

        *request_p = (char*)realloc(*request_p, request_len); // +1 for null terminator
        if (*request_p == NULL)
        {
            printf("\nrealloc failed with error code : %d\n", WSAGetLastError());
            free(buffer);
            return 1;
        }

        memccpy(*request_p + total_bytes_received, buffer, '\0', bytes_recieved);

        total_bytes_received += bytes_recieved; 
    // While the last 4 bytes of the request are not "\r\n\r\n"       
    } while (strcmp(*request_p + total_bytes_received - 4, "\r\n\r\n") != 0);

    // Add null terminator
    *request_p = (char*)realloc(*request_p, request_len + 1);
    (*request_p)[request_len] = '\0';

    free(buffer);
    return 0;
}

int http_parse_headers(char *request, HttpRequest *http_request_p)
{
    // This function parses the HTTP headers and stores them in the http_request_p struct
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

    // Build the request and response
    HttpRequest *http_request = http_request_create(client, HTTP_INVALID, NULL, http_version(1, 1));
    HttpResponse *http_response = http_response_create(NULL);

    // Link request to response
    http_response->request = http_request;

    // Log the thread
    log_message("TID%d\t\t| %s CONNECTED", client->thread_id, client->ip);

    // Recieve HTTP request
    char *request = malloc(1); // malloc 1 for http_recv_headers to be able to realloc
    
    if (http_recv_headers(client, server->buffer_size, &request) != 0)
    {
        printf("\nHTTP Request couldn't be read.\n");
        log_message("TID%d\t\t| %s RECIEVED BAD HTTP", client->thread_id, client->ip);
        log_message("TID%d\t\t| %s REQUEST FOLLOWS", client->thread_id, client->ip);
        log_message("TID%d\t\t| %s", client->thread_id, request);

        // Handle error
        // TODO: Send 400 Bad Request
        http_response_destroy(http_response);
        free(request);
        return 1;
    }
    log_message("TID%d\t\t| %s RECIEVED HTTP", client->thread_id, client->ip);

    if (http_parse_headers(request, http_request) != 0)
    {
        printf("\nHTTP Request couldn't be parsed.\n");
        log_message("TID%d\t\t| %s CANT PARSE HTTP", client->thread_id, client->ip);

        // Handle error
        // TODO: Send 400 Bad Request

        http_request_destroy(http_request);
        http_response_destroy(http_response);
        free(request);
        return 1;
    }
    log_message("TID%d\t\t| %s PARSED HTTP", client->thread_id, client->ip);

    printf("parsed route\n");

    http_request->method = HTTP_GET;
    http_request->url = "/about";
    http_request->version = http_version(1, 1);

    printf("Getting route\n");

    // Handle the request
    void *data1 = tree_get(server->method_routes[0], "/", http_route_search);
    HttpRoute *route1 = (HttpRoute*)data1;

    printf("Route: %s\n", route1->url);

    printf("Method int: %d", (http_request->method));

    void *data = tree_get(server->method_routes[http_request->method], http_request->url, http_route_search);

    printf("Got route\n");

    if (data == NULL)
    {
        // No route found (404 Not Found)
        log_message("TID%d\t\t| %s REQUESTED URL NOT ROUTED", client->thread_id, client->ip);
    }
    log_message("TID%d\t\t| %s ROUTE FOUND", client->thread_id, client->ip);

    // Route found, let handler fucntion do what's needed, and modify response as it will.
    HttpRoute *route = (HttpRoute*)data;
    int success = route->handler(http_request, http_response);
    log_message("TID%d\t\t| %s HANDLED REQUEST", client->thread_id, client->ip);

    printf("Handled with success: %d\n", success);

    if (success != 0)
    {
        // Unhandled error (server error)
    }

    // Send the response
    log_message("TID%d\t\t| %s BUILDING RESPONSE", client->thread_id, client->ip);
    char *response = http_response_to_string(http_response);

    log_message("TID%d\t\t| %s SENDING RESPONSE", client->thread_id, client->ip);
    send(client->socket, response, strlen(response), 0);

    log_message("TID%d\t\t| %s DISCONNECTING SOCKET", client->thread_id, client->ip);

    http_request_destroy(http_request);
    http_response_destroy(http_response);
    free(request);
    free(response);

    // Exit the thread
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