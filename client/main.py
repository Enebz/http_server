import socket
import time
import argparse

# Parse command line arguments
parser = argparse.ArgumentParser(description='Send HTTP requests to a server')
parser.add_argument('-p', '--port', type=int, default=80, help='The port to connect to the server on')
parser.add_argument('-d', '--delay', type=float, default=0.1, help='The delay between each part of the request')
parser.add_argument('-s', '--split', type=int, default=4, help='The number of parts to split the request into')
parser.add_argument('-r', '--request', type=str, default="GET", help='The type of request to send to the server')

args = parser.parse_args()


# Make a dictionary of various test HTTP requests to send to the server to test functionality and error handling
# The key is the name of the test and the value is the HTTP request
test_requests = {
    "GET": b'GET / HTTP/1.1\r\nAccept: text/html\r\nHost: localhost\r\nConnection: close\r\n\r\n',
    "GET1": b'GET /notfound HTTP/1.1\r\nAccept: text/html\r\nHost: localhost\r\nConnection: close\r\n\r\n',
    "GET2": b'GET /about HTTP/1.1\r\n\r\n',
    "POST": b'POST / HTTP/1.1\r\nAccept: text/html\r\nHost: localhost\r\nConnection: close\r\n\r\n',
    "PUT": b'PUT / HTTP/1.1\r\nAccept: text/html\r\nHost: localhost\r\nConnection: close\r\n\r\n',
    "DELETE": b'DELETE / HTTP/1.1\r\nAccept: text/html\r\nHost: localhost\r\nConnection: close\r\n\r\n',
    "HEAD": b'HEAD / HTTP/1.1\r\nAccept: text/html\r\nHost: localhost\r\nConnection: close\r\n\r\n',
    "OPTIONS": b'OPTIONS / HTTP/1.1\r\nAccept: text/html\r\nHost: localhost\r\nConnection: close\r\n\r\n',
    "TRACE": b'TRACE / HTTP/1.1\r\nAccept: text/html\r\nHost: localhost\r\nConnection: close\r\n\r\n',
    "CONNECT": b'CONNECT / HTTP/1.1\r\nAccept: text/html\r\nHost: localhost\r\nConnection: close\r\n\r\n',
    "GET with no Host": b'GET / HTTP/1.1\r\nAccept: text/html\r\nConnection: close\r\n\r\n',
    "GET with no Connection": b'GET / HTTP/1.1\r\nAccept: text/html\r\nHost: localhost\r\n\r\n',
    "GET with no Accept": b'GET / HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n',
    "GET with no HTTP version": b'GET / \r\nAccept: text/html\r\nHost: localhost\r\nConnection: close\r\n\r\n',
    "GET with no URI": b'GET HTTP/1.1\r\nAccept: text/html\r\nHost: localhost\r\nConnection: close\r\n\r\n',
    "GET with no method": b' / HTTP/1.1\r\nAccept: text/html\r\nHost: localhost\r\nConnection: close\r\n\r\n',
    "GET with no request line": b'Accept: text/html\r\nHost: localhost\r\nConnection: close\r\n\r\n',
    "GET with no request": b'',
    "WRONG_CRLF": b'GET / HTTP/1.1\nAccept: text/html\nHost: localhost\nConnection: close\n\n',
    "POST_WITH_BODY": b'POST / HTTP/1.1\r\nAccept: text/html\r\nHost: localhost\r\nConnection: close\r\nContent-Length: 11\r\n\r\nHello World',
}

def send_request(client, request, delay=0.1, parts=4):
    """
    Send a request to the server and print the response, split up the request in parts to test the server's ability to handle partial requests, wait for a delay between each part
    """

    print(f"Requesting {request.decode()} with a delay of {delay} seconds and {parts} parts")

    # Split the request into parts
    request_parts = [request[i:i+int(len(request)/parts)] for i in range(0, len(request), int(len(request)/parts))]
    # Send each part of the request
    for part in request_parts:
        time.sleep(delay)
        client.send(part)
    # Receive the response from the server (assuming the response is less than 4096 bytes)
    response = client.recv(4096)
    # Print the response
    print(response.decode())



if __name__ == "__main__":
    # create a socket client
    client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    # connect to the server
    client.connect(('localhost', 8080))

    # send each test request to the server
    send_request(client, test_requests[args.request], args.delay, args.split)