#ifndef PROXY_H
#define PROXY_H

void proxy_init(int port);
void proxy_run();
void proxy_cleanup();
int proxy_is_listen_fd(int fd);
void proxy_accept_client(int listen_fd);

#endif
