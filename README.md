# EnebzHTTP
EnebzHTTP is a simple HTTP server written in C.

At the moment, it supports GET requests with HTTP/1.1 protocol.

The server also comes with a configurable rate limiter.

## Technicals
The HTTP Server is written in C and uses Winsock2 for networking. 

It is multithreaded and uses a per-client-thread to handle incoming requests. 

The server is not secure and is not optimized for real world usage. 

It is simply a project to learn more about C programming and various techniques within computer science.

### Rate limiting
Rate limiting is done using a token bucket algorithm.

Each client has a token bucket per route with a maximum capacity of a configurable amount of tokens.

Each token represents 1 request.

When a client makes a request, a token is removed from the bucket. If the bucket is empty, the client is rate limited. 

The bucket is refilled at a rate of 1 token per a given interval.


## Purpose
This project was built to learn more about C programming and various techniques within computer science. I would not recommend using this server in a production environment.

## What I learned
While building this server, I learned about the following topics:

- Networking
    - HTTP protocol
    - TCP/IP
    - Sockets
    - Client/Server architecture
- Data structures
    - Linked lists
    - Binary Search Trees
    - Hashtables
- Programming concepts
    - Multithreadding
    - Thread safety and locking
    - Memory management
    - File I/O
    - Error handling
    - Makefiles

## Future plans
- [ ] Parse HTTP headers
- [ ] Add support for requests with bodies
- [ ] Add support for HTTPS
- [ ] Add support for IPv6
- [ ] Thread pool (optional over per-client-thread)
- [ ] Make server more secure
- [ ] Optimize memory management