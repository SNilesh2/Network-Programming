---

# Simple C Chat Server

A real-time, multi-client chat application featuring a high-performance C backend using `epoll` and a modern web frontend. This project demonstrates low-level socket programming, WebSocket protocol handling (handshaking/framing), and event-driven architecture.

## 🚀 Features

* Multi-client Support: Handles multiple connections simultaneously using Linux `epoll`.
* Real-time Broadcasting: Instant message delivery to all connected users.
* Private Messaging: Send direct messages using the `PVT:username:message` syntax.
* System Notifications: Alerts when users join or leave the chat.
* Non-blocking I/O: Efficient handling of network events without thread overhead.

## 🛠 Prerequisites

Before building, ensure you have the following installed on your Linux system:

* GCC (C Compiler)
* Make
* OpenSSL Development Libraries (used for WebSocket Handshake SHA-1/Base64)
* *Ubuntu/Debian:* `sudo apt-get install libssl-dev`
* *Fedora:* `sudo dnf install openssl-devel`

---

## 📁 Project Structure

```text
simple-chat-server/
├── server/
│   ├── include/       # Headers (server.h, websocket.h, chat_mgr.h, etc.)
│   ├── src/           # Implementation (.c files)
│   └── Makefile       # Compilation script
├── client/
│   ├── index.html     # User Interface
│   ├── style.css      # Styling
│   └── script.js      # WebSocket logic
└── README.md

```

---

## 🏗 Installation & Setup

### 1. Compile the Server

Navigate to the server directory and run `make`:

```bash
cd server
make

```

This will generate an executable named `chat_server` in the `server/` directory.

### 2. Run the Server

Start the backend on the default port (9001):

```bash
./chat_server

```

You should see: `Chat Server listening on port 9001`

### 3. Launch the Client

You do not need a web server to run the client.

1. Navigate to the `client/` folder.
2. Open `index.html` in any modern web browser (Chrome, Firefox, Edge).
3. Enter a username and click Join Chat.

---

## 💬 How to Use

* Public Chat: Simply type your message in the input box and press Enter or click "Send".
* Private Message: Use the format: `PVT:RecipientName:Your message here`.
* Example: If you want to message Nilesh, type: `PVT:nilesh:Hey, how are you?`


* System Messages: You will see italicized text when new users join.

---

## 🛠 Technical Implementation Details

* Networking: Uses POSIX TCP Sockets.
* Event Management: Linux `epoll` monitors file descriptors for `EPOLLIN` (incoming data) and `EPOLLOUT` (readiness to send).
* Protocol: Implements a custom subset of the RFC 6455 (WebSocket Protocol), including the HTTP `101 Switching Protocols` handshake and XOR unmasking for client-to-server frames.
* Memory Management: Uses a custom `websocket_client_t` struct to manage per-client buffers and states.

---

## ⚠️ Troubleshooting

* Handshake Error: Ensure you are linking against `-lssl -lcrypto`. The provided Makefile handles this.
* Connection Refused: Ensure the server is running before clicking "Join" in the browser and that port `9001` is not blocked by a firewall.
* Address already in use: If the server crashed, wait a few seconds for the OS to release the port, or use `netstat -nlp | grep 9001` to find and kill the hanging process.

---
