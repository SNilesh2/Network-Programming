# Poll-based TCP Client-Server

A single-process, event-driven TCP client-server implementation using `poll()`.

## Features
- Multiple clients handled concurrently
- Single-threaded, no `fork()` or threads
- Clients can send multiple messages per connection
- Message + timestamp protocol
- Global message counter on server side

## Technologies
- C
- POSIX sockets
- poll()

## How to Run
See client/README.md and server/README.md

