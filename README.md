# http_server

EnebzHTTP is a lightweight HTTP server created with C for Windows.
It supports HTTP/1.1 only (for now).

## Technicals
The server will only serve content based on configured routing. Routes can have a file related to them.
Routes are stored in a binary search tree

When a client makes a request, a new thread is opened to handle the request, making this sevrer a thread-per-client server system.

### Rate limiting
I have implemented a simple way of ratelimiting.
I am using Token Buckets to manage the amount of requests a user can make within N seconds.
Each route can be configured with their own ratelimit.
To refill the token buckets for each IP, a background worker constantly checks for and fills the token buckets.

## Purpose
This project was built simply because I wanted to learn more about C programming and various techniques within computer science.
*The server is NOT secure* nor profesionally optimized for real world usage.

## What I learned
While building this server I have learnt much about:

- Networking
    - Socket programming
    - HTTP/1.1 Protocol
    - Rate limiting
- Data structures
    - Linked lists
    - Binary Search Trees
    - Hashtables
- Programming concepts
    - Multithreadding
    - Thread safety and locking
    - C Programming