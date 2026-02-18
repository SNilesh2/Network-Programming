#ifndef EPOLL_MGR_H
#define EPOLL_MGR_H

#include<sys/epoll.h>

//creating epoll instance happens only once
int create_epoll_instance();

//adding fd to epoll set
int add_fd_to_epoll(int epoll_fd,int fd,uint32_t events);

//modify events
int modify_fd_in_epoll(int epoll_fd,int fd,uint32_t events);

//removing fd from epoll set
int remove_fd_from_epoll(int epoll_fd,int fd);

//epoll wait loop
int wait_for_events(int epoll_fd,struct epoll_event *events,int max_events,int timeout);

#endif

