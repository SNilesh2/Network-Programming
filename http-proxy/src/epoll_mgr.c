#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<sys/epoll.h>

#include "epoll_mgr.h"
#include "client.h"
#include "proxy.h"

#define MAX_EVENTS 64

static int epfd=-1;

void epoll_init()
{
  epfd=epoll_create1(0);
  
  if(epfd<0)
  {
    perror("epoll_create1");
    exit(1);
  }
}

void epoll_add(int fd)
{
  struct epoll_event ev;
  
  ev.events=EPOLLIN;
  ev.data.fd=fd;
  
  if(epoll_ctl(epfd,EPOLL_CTL_ADD,fd,&ev)<0)
  {
    perror("epoll_ctl_add");
  }
}

void epoll_del(int fd)
{
  if(epoll_ctl(epfd,EPOLL_CTL_DEL,fd,NULL)<0)
  {
    perror("epoll_ctl_del");
  }
}

void epoll_run()
{
  struct epoll_event events[MAX_EVENTS];
  
  while(1)
  {
    int nfds=epoll_wait(epfd,events,MAX_EVENTS,-1);
    
    if(nfds<0)
    {
      if(errno==EINTR)
      {
        continue;
      }
      perror("epoll_wait");
      break;
    }
    
    for(int i=0;i<nfds;i++)
    {
      int fd=events[i].data.fd;
      
      if(proxy_is_listen_fd(fd))
      {
        proxy_accept_client(fd);
        continue;
      }
      
      if(events[i].events & EPOLLIN)
      {
        client_handle_request(fd);
      }
    }
  }
}


void epoll_cleanup()
{
  if(epfd>=0)
  {
    close(epfd);
    epfd=-1;
  }
}


