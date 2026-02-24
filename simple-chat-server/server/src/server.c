#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<errno.h>
#include<fcntl.h>
#include "server.h"

int set_nonblocking(int fd)
{
  int flags=fcntl(fd,F_GETFL,0);
  if(flags==-1)
  {
    perror("fcntl F_GETFL");
    return -1;
  }
  
  if(fcntl(fd,F_SETFL,flags | O_NONBLOCK)==-1)
  {
    perror("fcntl F_SETFL");
    return -1;
  }
  return 0;
}

//creating the listening socket (which supports both ipv4 and v6)
int create_server_socket(const char *port)
{
  struct addrinfo hints,*res,*p;
  int server_fd=-1;
  int yes=1;
  
  memset(&hints,0,sizeof(hints));
  hints.ai_family=AF_UNSPEC;
  hints.ai_socktype=SOCK_STREAM;
  hints.ai_flags=AI_PASSIVE;
  
  int status=getaddrinfo(NULL,port,&hints,&res);
  if(status!=0)
  { 
    fprintf(stderr,"getaddrinfo: %s\n",gai_strerror(status));
    return -1;
  }
  
  for(p=res;p!=NULL;p=p->ai_next)
  {
    server_fd=socket(p->ai_family,p->ai_socktype,p->ai_protocol);
    
    if(server_fd==-1)
    {
      continue;
    }
    
//allow reuse port address
if(setsockopt(server_fd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(yes))==-1)
    {
      perror("setsockopt");
      close(server_fd);
      continue;
    }
    
    //allows both ipv4 and v6 connections
    if(p->ai_family==AF_INET6)
    {
      int no=0;
      if(setsockopt(server_fd,IPPROTO_IPV6,IPV6_V6ONLY,&no,sizeof(no))==-1)
      {
        perror("setsockopt IPV6_V6ONLY");
        close(server_fd);
        continue;
      }
    }
    
    if(bind(server_fd,p->ai_addr,p->ai_addrlen)==0)
    {
      break;
    }
    
    close(server_fd);
    server_fd=-1;
  }
  
  freeaddrinfo(res);
  
  if(server_fd==-1)
  {
    perror("bind");
    return -1;
  }
  
  if(listen(server_fd,BACKLOG)==-1)
  {
    perror("listen");
    close(server_fd);
    return -1;
  }
  
  if(set_nonblocking(server_fd)==-1)
  {
    close(server_fd);
    return -1;
  }
  
  printf("Chat Server listening on port %s\n",port);
  return server_fd;
}

//accepting new clients
int accept_new_client(int server_fd)
{
  struct sockaddr_storage client_addr;
  socklen_t addr_len=sizeof(client_addr);
  
  int client_fd=accept(server_fd,(struct sockaddr *)&client_addr,&addr_len);
  
  if(client_fd==-1)
  {
    if(errno!=EAGAIN && errno!=EWOULDBLOCK)
    {
      perror("accept");
    }
    return -1;
  }
  
  //making client socket non blocking 
  if(set_nonblocking(client_fd)==-1)
  {
    close(client_fd);
    return -1;
  }
  
  char ip_str[INET6_ADDRSTRLEN];
  
  if(client_addr.ss_family==AF_INET)
  {
    struct sockaddr_in *s=(struct sockaddr_in *)&client_addr;
    inet_ntop(AF_INET,&s->sin_addr,ip_str,sizeof(ip_str));
  }
  else if(client_addr.ss_family==AF_INET6)
  {
    struct sockaddr_in6 *s=(struct sockaddr_in6 *)&client_addr;
    inet_ntop(AF_INET6,&s->sin6_addr,ip_str,sizeof(ip_str));
  }
  
  printf("New connection from %s (fd=%d)\n",ip_str,client_fd);
  
  return client_fd;
}
