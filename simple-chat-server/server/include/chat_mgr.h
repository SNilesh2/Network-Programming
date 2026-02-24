#ifndef CHAT_MGR_H
#define CHAT_MGR_H

#include "websocket.h"

#define MAX_CLIENTS 100

//global array to keep track of all connected clients
extern websocket_client_t *clients[MAX_CLIENTS];

//add a new client
int chat_add_client(websocket_client_t *client);

//removes the client when someone leaves
void chat_remove_client(int fd);

//sends a message to all users
void chat_broadcast(const char *msg,int exclude_fd);

//finds a user by name and sends a message only to them
int chat_send_private(const char *target_username,const char *msg,const char *sender_name);

//parses JOIN,MSG,PVT and acts accordingly
void chat_handle_message(websocket_client_t *client,char *raw_msg);

#endif


