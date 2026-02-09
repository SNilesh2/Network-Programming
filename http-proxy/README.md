📌 HTTP Proxy Server in C
🚀 Project Overview

This project implements a multi-client HTTP Proxy Server in C using:

POSIX TCP sockets

epoll for scalable I/O multiplexing

Support for HTTP (GET, POST)

Support for HTTPS via CONNECT tunneling

The proxy efficiently handles multiple simultaneous client connections and forwards requests to upstream servers.

This project demonstrates low-level networking, protocol parsing, and high-performance I/O handling in Linux.

🏗 Project Structure
http-proxy/
├── src/
│   ├── main.c
│   ├── proxy.c
│   ├── server.c
│   ├── client.c
│   ├── http.c
│   └── epoll_mgr.c
│
├── include/
│   ├── proxy.h
│   ├── server.h
│   ├── client.h
│   ├── http.h
│   └── epoll_mgr.h
│
├── build/
├── Makefile
└── README.md

🧠 Features Implemented
✅ Multi-Client Support

Uses epoll

Event-driven architecture

Handles multiple concurrent connections

✅ HTTP Support

Parses HTTP requests

Supports:

GET

POST

Rewrites proxy request format to server format

Forwards responses back to client

✅ HTTPS Support

Implements CONNECT method

Establishes TCP tunnel

Relays encrypted TLS data transparently

No decryption performed (correct proxy behavior)

✅ Efficient I/O

Non-blocking event-based handling

Avoids thread-per-client model

Designed toward C10K scalability

🛠 Technologies Used
Component	Technology
Language	C
Networking	POSIX TCP Sockets
Multiplexing	epoll
Protocol	HTTP/1.1
HTTPS Handling	CONNECT Tunneling
🔧 Build Instructions
make


This will generate:

build/http_proxy


To clean:

make clean

▶️ Running the Proxy

Start proxy on port 8080:

./build/http_proxy 8080


Expected output:

Proxy listening on port 8080

🧪 Testing the Proxy
🌐 1. HTTP GET Test
curl -v -x http://127.0.0.1:8080 http://example.com/


✔ Proxy receives request
✔ Connects to example.com
✔ Returns HTML page

🔐 2. HTTPS Test (CONNECT Method)
curl -v -x http://127.0.0.1:8080 https://example.com


Flow:

Client sends CONNECT example.com:443

Proxy connects to server

Proxy returns:

HTTP/1.1 200 Connection Established


TLS handshake happens directly between client and server

Proxy forwards encrypted data transparently

✔ HTTPS tunnel working correctly

📤 3. HTTP POST Test
curl -v -x http://127.0.0.1:8080 \
     -X POST http://httpbin.org/post \
     -d "name=nilesh&project=proxy"


Or test via browser:

http://httpbin.org/forms/post


✔ Proxy forwards POST body
✔ Content-Length handled
✔ Server response returned

🔄 Request Flow Architecture
HTTP GET / POST
Client → Proxy → Upstream Server
Client ← Proxy ← Upstream Server

HTTPS CONNECT
Client → Proxy (CONNECT)
Proxy → Server
Tunnel Established
Encrypted Data ↔ Relayed

⚙ Internal Design
proxy.c

Creates listening socket

Accepts clients

Registers FDs with epoll

epoll_mgr.c

Central event loop

Handles EPOLLIN events

Dispatches to client handler

client.c

Handles:

HTTP parsing

CONNECT tunneling

Forwarding data

Connection mapping

server.c

Connects to upstream servers

DNS resolution via getaddrinfo

http.c

Parses:

Method

Host

Port

Path

Content-Length

Rewrites proxy-style request to server-style request

🎯 Project Goals Achieved

✔ Implemented HTTP proxy in C
✔ Implemented HTTPS CONNECT tunneling
✔ Used epoll for scalable I/O
✔ Implemented request parsing and rewriting
✔ Handled GET and POST correctly
✔ Used curl and browser for validation
✔ Demonstrated concurrent connection handling

🚧 Future Enhancements

Persistent HTTP/1.1 connections

Connection pooling

Proxy authentication

Logging system

Caching layer

Support for PUT / DELETE / HEAD

Rate limiting

Access control lists

Load balancing

🏁 Conclusion

This project demonstrates:

Strong understanding of TCP networking

HTTP protocol parsing

HTTPS tunneling mechanism

Event-driven scalable architecture

epoll-based high-performance server design

The proxy successfully supports:

HTTP GET

HTTP POST

HTTPS via CONNECT

Multi-client concurrent handling

👨‍💻 Author

Nilesh S
HTTP Proxy Server Project
C | POSIX | epoll | Linux Networking
