#ifndef SERVER_H
#define SERVER_H

#include<netinet/in.h>

#define BACKLOG 128

//creates a listening socket
int create_server_socket(const char *port);

//accepts a connection from the listening socket
int accept_new_client(int server_fd);

//sets a file descriptor to non blocking mode 
int set_nonblocking(int fd);

#endif

