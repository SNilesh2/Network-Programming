#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>
#include<sys/socket.h>

#include "client.h"
#include "server.h"
#include "http.h"
#include "epoll_mgr.h"

#define BUF_SIZE 8192
#define MAX_FDS 1024



static int client_to_server[MAX_FDS];
static fd_role_t fd_role[MAX_FDS];
static conn_type_t conn_type[MAX_FDS];

static void init_fd_tables();

__attribute__((constructor))
static void client_module_init()
{
  init_fd_tables();
}


static void init_fd_tables()
{
  for(int i=0;i<MAX_FDS;i++)
  {
    client_to_server[i]=-1;
    fd_role[i]=FD_ROLE_NONE;
    conn_type[i]=CONN_TYPE_NONE;
  }
}

void client_handle_request(int fd)
{ 
  char buf[BUF_SIZE];
  int n;
  
  n=recv(fd,buf,sizeof(buf),0);
  
  if(n==0)
  { 
    client_close(fd);
    return;
  }
  
  if(n<0)
  {
    perror("recv");
    client_close(fd);
    return;
  }
  
  int peer_fd=client_to_server[fd];
  
  if(peer_fd>=0 && conn_type[fd]==CONN_HTTPS_TUNNEL)
  {
    send(peer_fd,buf,n,0);
    return;
  }
  
  if(fd_role[fd]==FD_SERVER)
  {
    if(peer_fd>=0)
    {
      send(peer_fd,buf,n,0);
    }
    return;
  }
  
  http_request_t req;
  
  if(http_parse_request(buf,&req)<0)
  {
    client_close(fd);
    return;
  }
  
  printf("Request: ");

  if(req.method == HTTP_GET)
    printf("GET ");
  else if(req.method == HTTP_POST)
    printf("POST ");
  else if(req.method == HTTP_CONNECT)
    printf("CONNECT ");

  printf("%s\n", req.host);

  //connect
  if(req.method==HTTP_CONNECT)
  {
    int server_fd=server_connect(req.host,req.port);
    if(server_fd<0)
    {
      client_close(fd);
      return;
    }
    
    client_to_server[fd]=server_fd;
    client_to_server[server_fd]=fd;
    
    fd_role[fd]=FD_CLIENT;
    fd_role[server_fd]=FD_SERVER;
    
    conn_type[fd]=CONN_HTTPS_TUNNEL;
    conn_type[server_fd]=CONN_HTTPS_TUNNEL;
    
    const char *resp="HTTP/1.1 200 Connection Established\r\n\r\n";
    
    send(fd,resp,strlen(resp),0);
    
    epoll_add(server_fd);
    
    return ;
  }
  
  //HTTP get/post
  
  int server_fd=server_connect(req.host,req.port);
  if(server_fd<0)
  { 
    client_close(fd);
    return;
  }
  
  client_to_server[fd]=server_fd;
  client_to_server[server_fd]=fd;
  
  fd_role[fd]=FD_CLIENT;
  fd_role[server_fd]=FD_SERVER;
  
  conn_type[fd]=CONN_HTTP;
  conn_type[server_fd]=CONN_HTTP;
  
  char server_req[BUF_SIZE];
  if(http_build_server_request(buf,server_req,sizeof(server_req))<0)
  {
    client_close(fd);
    server_close(server_fd);
    return;
  }
  
  send(server_fd,server_req,strlen(server_req),0);
  
  epoll_add(server_fd);
  
}

//handled in client_handle_read();
void client_handle_write(int client_fd)
{
  (void)client_fd;
}

void client_close(int client_fd)
{
  int server_fd=client_to_server[client_fd];
  
  if(server_fd>=0)
  {
    client_to_server[server_fd]=-1;
    epoll_del(server_fd);
    close(server_fd);
  }
  
  client_to_server[client_fd]=-1;
  epoll_del(client_fd);
  close(client_fd);
}
