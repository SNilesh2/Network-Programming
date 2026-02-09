#ifndef SERVER_H
#define SERVER_H

int server_connect(const char *host,int port);
void server_close(int server_fd);

#endif
