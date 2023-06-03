#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H


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



typedef struct HttpClient HttpClient;


struct HttpClient
{
    SOCKET socket;
    struct sockaddr_in address;
    int address_len;
    char *ip;
    DWORD thread_id;
};

HttpClient *http_client_create(SOCKET socket, struct sockaddr_in address, int address_len);
void http_client_destroy(HttpClient *client);

#endif // HTTP_CLIENT_H