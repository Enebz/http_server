#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#ifndef _WIN32
    // TODO: Add support for linux
#else
    // On windows, we need to include the winsock2.h header
    #include <WinSock2.h>
    #include <ws2tcpip.h>
    #include <Windows.h>
    #ifdef _MSC_VER
        #pragma comment(lib, "ws2_32.lib")
    #endif
#endif

// Include the data structures
#include "tree/tree.h"
#include "logger/logger.h"

#include "client.h"
#include "request.h"
#include "response.h"


typedef struct HttpServer HttpServer;
typedef struct HttpClientHandlerArgs HttpClientHandlerArgs;

struct HttpServer
{
    SOCKET socket;
    struct sockaddr_in *address;
    int address_len;
    int port;
    int backlog;
    int max_connections;
    int max_request_size;
    int buffer_size;
    HttpClient **clients;
    int client_count;
    HANDLE *threads;
    int running;
    tree* method_routes[9];
};

struct HttpClientHandlerArgs
{
    HttpClient *client;
    HttpServer *server;
};

int checkSocketError(int socket, int error_code, char *message);

HttpServer *http_server_create(int port, int backlog, int max_connections, int max_request_size, int buffer_size);
void http_server_destroy(HttpServer *server);

int http_recv_headers(HttpClient *client, short bufferSize, char **request_p);
int http_parse_headers(char *request, HttpRequest *http_request_p);

int http_server_listen(HttpServer* server);
DWORD WINAPI http_handle_client(LPVOID lpParam);

HttpClientHandlerArgs *http_client_handler_args_create(HttpClient *client, HttpServer *server);
void http_client_handler_args_destroy(HttpClientHandlerArgs *args);

#endif // HTTP_SERVER_H