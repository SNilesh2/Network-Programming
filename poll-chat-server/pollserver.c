#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netdb.h>
#include<netinet/in.h>
#include<poll.h>

#define BACKLOG 10
#define PORT "3490"

void process_connection(int listener,struct pollfd *pfds,int *fd_size,int *fd_count);
int get_listener();
void create_new_connection(int listener,struct pollfd **pfds,int *fd_size,int *fd_count);
void inet_ntop2(void *addr,char *buffer,int size);
void handle_client_connection(int listener,int *client_fd,struct pollfd *pfds,int *fd_count);

int main()
{
  int listener;
  int fd_size=5;
  int fd_count=0;
  struct pollfd *pfds=malloc(sizeof *pfds * fd_size);
  listener=get_listener();
  
  if(listener==-1)
  {
    printf("listener failed");
    return 1;
  }
  
  pfds[0].fd=listener;
  pfds[0].events=POLLIN;
  
  fd_count=1;
  
  for(;;)
  {
    int num_of_events=poll(pfds,fd_count,-1);
    
    if(num_of_events==-1)
    {
      printf("error in poll");
      return 1;
    }
    
    process_connection(listener,pfds,&fd_size,&fd_count);
  }
  
  free(pfds);
  
  return 0;
}


void process_connection(int listener,struct pollfd *pfds,int *fd_size,int *fd_count)
{
  for(int i=0;i<*fd_count;i++)
  {
    if(pfds[i].revents & (POLLIN | POLLHUP)) 
    {
      if(pfds[i].fd==listener)
      {
        create_new_connection(listener,&pfds,fd_size,fd_count);
      }
      else
      {
        handle_client_connection(listener,&i,pfds,fd_count);
      }
    }
    pfds[i].revents=0;
  }
}


void create_new_connection(int listener,struct pollfd **pfds,int *fd_size,int *fd_count)
{
  int newfd;
  struct sockaddr_storage client_addr;
  char buffer[1024];
  
  int len=sizeof client_addr;
  newfd=accept(listener,(struct sockaddr *)&client_addr,&len);
  
  if(newfd==-1)
  {
    printf("accept failed");
    return;
  }
  
  if(*fd_count==*fd_size)
  {
    *fd_size=*fd_size * 2;
    *pfds=realloc(*pfds,sizeof(struct pollfd) * (*fd_size));
  }
  
  (*pfds)[*fd_count].fd=newfd;
  (*pfds)[*fd_count].events=POLLIN;
  (*pfds)[*fd_count].revents=0;
  
  (*fd_count)++;
  
  inet_ntop2(&client_addr,buffer,sizeof buffer);
  printf("new connection to %s\n",buffer);
  
}

void inet_ntop2(void *addr,char *buffer,int size)
{
  struct sockaddr_storage *sas=addr;
  struct sockaddr_in *sa4;
  struct sockaddr_in6 *sa6;
  void *src;
  switch(sas->ss_family)
  {
    case AF_INET:
      sa4=(struct sockaddr_in *)sas;
      src=&(sa4->sin_addr);
      break;
    case AF_INET6:
      sa6=(struct sockaddr_in6 *)sas;
      src=&(sa6->sin6_addr);
      break;
    default:
      return;
  }
  inet_ntop(sas->ss_family,src,buffer,size);
}


void handle_client_connection(int listener,int *client_fd,struct pollfd *pfds,int *fd_count)
{
  char buffer[1024];
  int n;
  
  n=recv(pfds[*client_fd].fd,buffer,sizeof buffer-1,0);
  
  if(n<=0)
  {
    if(n==0)
    {
      printf("connection lost\n");
    }
    else
    {
      printf("error in receiving message\n");
    }
    
    close(pfds[*client_fd].fd);
    pfds[*client_fd]=pfds[*fd_count-1];
    (*fd_count)--;
    
    (*client_fd)--;
  }
  else
  {
    buffer[n]='\0';
    int sender_fd=pfds[*client_fd].fd;
    printf("receive from file descriptor %d and the message %s\n",sender_fd,buffer);
    
  }
}

int get_listener()
{
  int listener;
  int yes=1;
  char ip[INET6_ADDRSTRLEN];
  struct addrinfo hints,*res,*p;
  
  
  memset(&hints,0,sizeof hints);
  hints.ai_family=AF_UNSPEC;
  hints.ai_socktype=SOCK_STREAM;
  hints.ai_flags=AI_PASSIVE;
  
  if(getaddrinfo(NULL,PORT,&hints,&res)!=0)
  {
    printf("gedaddinfo failed");
    return 1;
  }
  
  for(p=res;p!=NULL;p=p->ai_next)
  {
    listener=socket(p->ai_family,p->ai_socktype,p->ai_protocol);
    if(listener==-1)
    {
      printf("socket creation failed");
      return 1;
    }
    
    if(setsockopt(listener,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int))==-1)
    {
      printf("address already in use error");
      return 1;
    }
    
    if(bind(listener,p->ai_addr,p->ai_addrlen)==0)
    {
      break;
    }
    close(listener);
  }
  
  if(p==NULL)
  {
    printf("bind failed");
    return 1;
  }
  
  freeaddrinfo(res);
  
  if(listen(listener,BACKLOG)==-1)
  {
    printf("listen failed");
    return 1;
  }
  
  printf("Server : listening ...\n");
  
  return listener;
}
