#include "client.h"

HttpClient *http_client_create(SOCKET socket, struct sockaddr_in address, int address_len)
{
    HttpClient *client = malloc(sizeof(HttpClient));
    client->socket = socket;
    client->address = address;
    client->address_len = address_len;
    return client;
}

void http_client_destroy(HttpClient *client)
{
    // Close the socket
    closesocket(client->socket);

    // Free the client
    free(client);
}