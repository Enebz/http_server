/*
	Author: Enebz
	Description: A simple HTTP server written in C
*/


#define  _GNU_SOURCE

// Include the standard libraries
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

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
#include "hashtable/hashtable.h"
#include "tree/tree.h"

// Include the logger
#include "logger/logger.h"

// Include the HTTP related files
#include "http/server.h"
#include "http/request.h"
#include "http/response.h"
#include "http/status.h"
#include "http/route.h"


/* Definitions */
#define HTTP_DEFAULT_PORT 8080
#define HTTP_DEFAULT_BUFFER_SIZE 8192
#define HTTP_DEFAULT_BACKLOG 10
#define HTTP_DEFAULT_MAX_CONNECTIONS 100



/* Function definitions */
int init_winsock()
{
	// Initialize Winsock
	WSADATA wsa;
	
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d",WSAGetLastError());
		exit(1);
	}

	return 0;
}


int get_index(HttpRequest *request, HttpResponse *response)
{
	if (http_response_set_file(response, "index.html") < 0)
	{
		return 2;
	}
	return 0;
}

int get_about(HttpRequest *request, HttpResponse *response)
{
	if (http_response_set_file(response, "about.html") < 0)
	{
		return 2;
	}
	return 0;
}

int get_contact(HttpRequest *request, HttpResponse *response)
{
	// This demonstrates how to read a file and send it as a response
	if (http_response_set_file(response, "contact.html") < 0)
	{
		return 2;
	}
	return 0;
}

int main(int argc, char *argv[])
{
	// Initialize logger
	init_logger();

	// Initialize Winsock (Windows only)
	int res = init_winsock();
	if (res != 0)
	{
		printf("Failed to initialize Winsock!\n");
		return 1;
	}

	// Create server
	HttpServer *server = http_server_create(
		HTTP_DEFAULT_PORT,
		HTTP_DEFAULT_BACKLOG,
		HTTP_DEFAULT_MAX_CONNECTIONS,
		HTTP_DEFAULT_BUFFER_SIZE * 64,
		HTTP_DEFAULT_BUFFER_SIZE
	);

	// Create routes
	HttpRoute *index = http_route_create(HTTP_GET, "/", get_index);
	HttpRoute *about = http_route_create(HTTP_GET, "/about", get_about);
	HttpRoute *file = http_route_create(HTTP_GET, "/contact", get_contact);

	http_route_serve(server, index);
	http_route_serve(server, about);
	http_route_serve(server, file);

	// Start server
	http_server_listen(server);

	// Cleanup
	http_server_destroy(server);
	close_logger();
	WSACleanup();
	return 0;
}
