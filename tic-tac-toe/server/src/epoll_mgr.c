#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<sys/epoll.h>
#include "epoll_mgr.h"

int create_epoll_instance()
{
  int epoll_fd=epoll_create1(0);
  
  if(epoll_fd==-1)
  {
    perror("epoll_create1");
    return -1;
  }
  
  return epoll_fd;
}


int add_fd_to_epoll(int epoll_fd,int fd,uint32_t events)
{
  struct epoll_event ev;
  ev.events=events;
  ev.data.fd=fd;
  
  if(epoll_ctl(epoll_fd,EPOLL_CTL_ADD,fd,&ev)==-1)
  {
    perror("epoll_ctl ADD");
    return -1;
  }
  
  return 0;
}

int modify_fd_in_epoll(int epoll_fd,int fd,uint32_t events)
{
  struct epoll_event ev;
  ev.events=events;
  ev.data.fd=fd;
  
  if(epoll_ctl(epoll_fd,EPOLL_CTL_MOD,fd,&ev)==-1)
  {
    perror("epoll_ctl MOD");
    return -1;
  }
  
  return 0;
}

int remove_fd_from_epoll(int epoll_fd,int fd)
{
  if(epoll_ctl(epoll_fd,EPOLL_CTL_DEL,fd,NULL)==-1)
  {
    perror("epoll_ctl DEL");
    return -1;
  }
  
  return 0;
}

int wait_for_events(int epoll_fd,struct epoll_event *events,int max_events,int timeout)
{
  int n=epoll_wait(epoll_fd,events,max_events,timeout);
  
  if(n==-1)
  {
    if(errno==EINTR)
    {
      return 0;
    }
    perror("epoll_wait");
    return -1;
  }
  
  return n;
}
