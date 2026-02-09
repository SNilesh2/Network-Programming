#ifndef CLIENT_H
#define CLIENT_H

typedef enum
{
  FD_CLIENT,
  FD_SERVER,
  FD_ROLE_NONE
}fd_role_t;

typedef enum
{
  CONN_HTTP,
  CONN_HTTPS_TUNNEL,
  CONN_TYPE_NONE
}conn_type_t;


//read data/request from client
void client_handle_request(int client_fd);

//send response/data to client
void client_handle_write(int client_fd);

//close client
void client_close(int client_fd);

#endif
