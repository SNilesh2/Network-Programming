#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<netdb.h>
#include<errno.h>
#include<sys/socket.h>

#include "proxy.h"
#include "epoll_mgr.h"

static int listen_fd=-1;

void proxy_init(int port)
{
  struct addrinfo hints,*res,*p;
  char port_str[16];
  
  int opt=1;
  snprintf(port_str,sizeof(port_str),"%d",port);
  
  memset(&hints,0,sizeof(hints));
  hints.ai_family=AF_UNSPEC;
  hints.ai_socktype=SOCK_STREAM;
  hints.ai_flags=AI_PASSIVE;
  
  if(getaddrinfo(NULL,port_str,&hints,&res)!=0)
  {
    perror("getaddrinfo");
    exit(1);
  }
  
  for(p=res;p!=NULL;p=p->ai_next)
  {
    listen_fd=socket(p->ai_family,p->ai_socktype,p->ai_protocol);
    
    if(listen_fd<0)
    {
      continue;
    }
    
    if(setsockopt(listen_fd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt))<0)
    {
      perror("setsockopt");
      close(listen_fd);
      continue;
    }
    
    if(p->ai_family==AF_INET6)
    {
      int no=0;
         if(setsockopt(listen_fd,IPPROTO_IPV6,IPV6_V6ONLY,&no,sizeof(no))<0)
         {
           perror("setsockopt");
           close(listen_fd);
           continue;
         }
    }
      
    if(bind(listen_fd,p->ai_addr,p->ai_addrlen)==0)
    {
      break;
    }
      
      close(listen_fd);
      listen_fd=-1;
    }
    
    freeaddrinfo(res);
    
    if(listen_fd<0)
    {
      perror("bind");
      exit(1);
    }
    
    if(listen(listen_fd,128)<0)
    {
      perror("listen");
      close(listen_fd);
      exit(1);
    }
    
    epoll_add(listen_fd);
    
    printf("Proxy listening on port %d\n",port);
}

void proxy_run()
{
  epoll_run();
}

int proxy_is_listen_fd(int fd)
{
  return fd==listen_fd;
}

void proxy_accept_client(int listen_fd)
{
  int client_fd=accept(listen_fd,NULL,NULL);
  if(client_fd<0)
  {
    perror("accept");
    return;
  }
  
  printf("Client connected to proxy (fd=%d)\n", client_fd);
  
  epoll_add(client_fd);
  
}

void proxy_cleanup()
{
  if(listen_fd>=0)
  {
    close(listen_fd);
    listen_fd=-1;
  }
}


