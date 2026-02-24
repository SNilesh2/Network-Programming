#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#include<stddef.h>
#include<stdint.h>
#include<sys/types.h>

#define MAX_BUFFER 8192
#define MAX_USERNAME_LEN 32

typedef enum
{
  WS_STATE_HTTP_HANDSHAKE=0,
  WS_STATE_CONNECTED,
  WS_STATE_CLOSING
} ws_state_t;

typedef struct websocket_client
{
  int fd;
  ws_state_t state;
  
  char username[MAX_USERNAME_LEN];
  int has_set_username;//boolean yes or not
  
  char recv_buffer[8192];
  size_t recv_len;
  
  char send_buffer[8192];
  size_t send_len;
}websocket_client_t;

//initialize websocket client 
websocket_client_t *ws_create(int fd);

//free client resources
void ws_destroy(websocket_client_t *client);

//handle epollin
int ws_handle_read(websocket_client_t *client);

//handle epollout
int ws_handle_write(websocket_client_t *client);

//perform http handshake
int ws_perform_handshake(websocket_client_t *client);

//send text frame to client
int ws_send_text(websocket_client_t *client,const char *msg,size_t len);

//close websocket connection
int ws_close(websocket_client_t *client);

#endif
