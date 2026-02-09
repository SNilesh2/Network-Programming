#ifndef EPOLL_MGR_H
#define EPOLL_MGR_H

void epoll_init();
void epoll_add(int fd);
void epoll_del(int fd);
void epoll_run();
void epoll_cleanup();

#endif
