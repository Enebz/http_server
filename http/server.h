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
#include "tokenbucket.h"

#define CR '\r'
#define LF '\n'
#define CRLF "\r\n"
#define CRLFCRLF "\r\n\r\n"

typedef struct HttpServer HttpServer;
typedef struct HttpClientHandlerArgs HttpClientHandlerArgs;
typedef struct HttpRateLimitWorkerArgs HttpRateLimitWorkerArgs;

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
    HANDLE *client_threads;
    HANDLE *rate_limit_worker;
    int running;
    tree* method_routes[9]; // One tree of routes per method
};

struct HttpClientHandlerArgs
{
    HttpClient *client;
    HttpServer *server;
};

struct HttpRateLimitWorkerArgs
{
    HttpServer *server;
};

extern volatile int stop_rate_limit_worker;

int checkSocketError(int socket, int error_code, char *message);

HttpServer *http_server_create(int port, int backlog, int max_connections, int max_request_size, int buffer_size);
void http_server_destroy(HttpServer *server);

int http_handle_request(HttpServer *server, HttpClient *client, HttpRequest *http_request, HttpResponse *http_response);
int http_send_response(HttpClient *client, HttpResponse *response);
int http_recv_request(HttpClient *client, short bufferSize, HttpRequest *request);
int http_parse_headers(char *request, HttpRequest *http_request_p);

int http_server_listen(HttpServer* server);
DWORD WINAPI http_handle_client(LPVOID lpParam);

HttpClientHandlerArgs *http_client_handler_args_create(HttpClient *client, HttpServer *server);
void http_client_handler_args_destroy(HttpClientHandlerArgs *args);

HttpRateLimitWorkerArgs *http_rate_limit_worker_args_create(HttpServer *server);
void http_rate_limit_worker_args_destroy(HttpRateLimitWorkerArgs *args);

#endif // HTTP_SERVER_H