#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>

#include "server.h"

int server_connect(const char *host,int port)
{
  struct addrinfo hints,*res,*p;
  char port_str[16];
  int sockfd;
  memset(&hints,0,sizeof(hints));
  hints.ai_family=AF_UNSPEC;
  hints.ai_socktype=SOCK_STREAM;
  
  snprintf(port_str,sizeof(port_str),"%d",port);
  
  if(getaddrinfo(host,port_str,&hints,&res)!=0)
  { 
    perror("getaddrinfo failed");
    return -1;
  }
  
  for(p=res;p!=NULL;p=p->ai_next)
  {
    sockfd=socket(p->ai_family,p->ai_socktype,p->ai_protocol);
    
    if(sockfd<0)
    {
      continue;
    }
    
    if(connect(sockfd,p->ai_addr,p->ai_addrlen)==0)
    {
      printf("Connected to upstream server: %s:%d\n", host, port);
      break;
    }
    
    close(sockfd);
  }
  
  freeaddrinfo(res);
  
  if(sockfd<0)
  {
    perror("connect");
    return -1;
  }
  
  return sockfd;
}

void server_close(int server_fd)
{
  if(server_fd>=0)
  {
    close(server_fd);
  }
}

