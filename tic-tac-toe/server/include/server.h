#ifndef SERVER_H
#define SERVER_H

#include<netinet/in.h>

//for creating listening socket
int create_server_socket(const char *port);

//for creating client communication socket
int accept_new_client(int server_fd);

//sets file descriptors in non blocking mode 
int set_nonblocking(int fd);

#endif

