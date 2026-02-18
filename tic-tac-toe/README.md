TIC TAC TOE SERVER

A real-time multiplayer Tic Tac Toe game implemented using a high-performance WebSocket server written in C and a browser-based client built with HTML, CSS, and JavaScript. The server efficiently handles multiple concurrent games using non-blocking sockets and the epoll event notification mechanism.

Project Overview

This project implements a scalable Tic Tac Toe multiplayer server that connects clients via WebSockets and manages complete game sessions. The server pairs players, validates moves, maintains game state, detects wins or draws, and synchronizes updates in real time.

The client provides an interactive graphical interface where users can connect, play against an opponent, and receive instant updates.

This project demonstrates real-time networking, socket programming, WebSocket protocol implementation, and event-driven server architecture using epoll.

Features
Server Features

Supports multiple simultaneous game sessions

Automatic player matchmaking

Full WebSocket protocol implementation

Non-blocking sockets for high performance

Event-driven architecture using epoll

Server-side move validation

Win and draw detection

Real-time message broadcasting

Handles client disconnections safely

IPv4 and IPv6 support

Client Features

Clean and interactive web interface

Real-time game updates using WebSockets

Automatic connection to server

Turn-based gameplay

Visual game board

Game status display

Win, lose, draw notifications

Technologies Used
Server

Language: C

Networking: POSIX TCP sockets

Event handling: epoll

Protocol: WebSocket (RFC 6455)

Cryptography: OpenSSL (SHA1, Base64)

Architecture: Event-driven, non-blocking

Client

HTML5

CSS3

JavaScript

WebSocket API

Folder Structure
tic-tac-toe/
│
├── server/
│   ├── include/
│   │   ├── server.h
│   │   ├── epoll_mgr.h
│   │   ├── websocket.h
│   │   ├── game.h
│   │   ├── session.h
│   │
│   ├── src/
│   │   ├── server.c
│   │   ├── epoll_mgr.c
│   │   ├── websocket.c
│   │   ├── game.c
│   │   ├── session.c
│   │   ├── main.c
│   │
│   ├── build/
│   │   ├── obj/
│   │   └── ttt_server
│   │
│   └── Makefile
│
├── client/
│   ├── index.html
│   ├── style.css
│   ├── script.js
│
└── README.md

Server Architecture

The server uses an event-driven architecture built around epoll for efficient handling of thousands of concurrent connections.

Core Components

server.c

Creates listening socket

Accepts incoming connections

Configures sockets as non-blocking

epoll_mgr.c

Manages epoll instance

Registers and monitors file descriptors

Handles event polling

websocket.c

Implements WebSocket protocol

Performs HTTP handshake

Encodes and decodes WebSocket frames

Sends and receives messages

game.c

Implements Tic Tac Toe game logic

Tracks board state

Validates moves

Detects win and draw conditions

session.c

Manages game sessions

Handles player pairing

Broadcasts game updates

Handles player disconnections

main.c

Main event loop

Integrates all components

Handles client events and sessions

Client Architecture

The client connects to the server using WebSockets and provides a graphical interface.

Components

index.html

Game layout

Board structure

Status displays

style.css

UI styling

Animations

Responsive design

script.js

WebSocket communication

Board updates

Game state handling

User input handling

How It Works
Connection Flow

Client connects to server using WebSocket

Server performs WebSocket handshake

Server assigns player symbol (X or O)

Server pairs two players into a session

Game begins

Game Flow

Player clicks on board cell

Client sends move to server

Server validates move

Server updates game state

Server broadcasts new board state

Server checks for win or draw

Server ends game if complete

Game Protocol Messages
Server → Client
SYMBOL:X
SYMBOL:O

GAME START

BOARD:X O X O  

WIN:X
WIN:O

DRAW

WAIT FOR OPPONENT

INVALID MOVE

NOT YOUR TURN

OPPONENT LEFT

Client → Server
0
1
2
3
4
5
6
7
8


Each number represents a board position.

Board Index Layout
0 | 1 | 2
---------
3 | 4 | 5
---------
6 | 7 | 8

Installation and Setup
Requirements

Linux system recommended

Install dependencies:

sudo apt install build-essential libssl-dev

Build the Server

Navigate to server directory:

cd server


Build using Makefile:

make


Executable will be created:

build/ttt_server

Run the Server
./build/ttt_server


Server runs on:

Port: 8080

Run the Client

Open client in browser:

client/index.html


How to Play

Open client in two browser windows

Both connect to server

Server assigns symbols automatically

Players take turns clicking board

First to align 3 symbols wins

Scalability

Server supports:

Up to 1024 concurrent clients

Up to 512 simultaneous game sessions

Non-blocking event-driven processing

High-performance epoll architecture

Learning Outcomes

This project demonstrates:

TCP socket programming

WebSocket protocol implementation

Event-driven server design

epoll usage in Linux

Real-time multiplayer game logic

Client-server architecture

Network protocol design

Future Improvements

Player usernames

Game lobby system

Spectator mode

Score tracking

Reconnection support

TLS encryption (WSS)

Deployment on cloud server

Author

Nilesh.S,
B.Tech Information Technology,
Coimbatore Institute of Technology,
Final Year

License

This project is developed for educational purposes.
