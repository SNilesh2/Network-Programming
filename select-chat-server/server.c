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

int get_listener();
void create_new_connection(int listener,fd_set *master,int *fdmax);
void inet_ntop2(void *addr,char *buffer,int size);
void handle_client_connection(int s,int listener,fd_set *master,int fdmax);

int main()
{
  fd_set master;
  fd_set read_fds;
  int fdmax;
  int listener;
  
  FD_ZERO(&master);
  FD_ZERO(&read_fds);
  
  listener=get_listener();
  
  if(listener==-1)
  {
    printf("listener failed");
    return 1;
  }
  
  FD_SET(listener,&master);
  fdmax=listener;
  
  for(;;)
  {
    read_fds=master;
    
    if(select(fdmax+1,&read_fds,NULL,NULL,NULL)==-1)
    {
      printf("select failed");
      return 1;
    }
    
    for(int i=0;i<=fdmax;i++)
    {
      if(FD_ISSET(i,&read_fds))
      {
        if(i==listener)
        {
          create_new_connection(i,&master,&fdmax);
        }
        else
        {
          handle_client_connection(i,listener,&master,fdmax);
        }
      }
    }
  }
  
  return 0;
}


void create_new_connection(int listener,fd_set *master,int *fdmax)
{
  int newfd;
  struct sockaddr_storage client_addr;
  char buffer[INET6_ADDRSTRLEN];
  
  int len=sizeof client_addr;
  newfd=accept(listener,(struct sockaddr *)&client_addr,&len);
  
  if(newfd==-1)
  {
    printf("accept failed");
    return;
  }
  else
  {
    FD_SET(newfd,master);
    if(newfd>*fdmax)
    {
      *fdmax=newfd;
    }
    inet_ntop2(&client_addr,buffer,sizeof buffer);
    printf("new connection to %s\n",buffer);
  }
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


void handle_client_connection(int s,int listener,fd_set *master,int fdmax)
{
  char buffer[1024];
  int n;
  
  n=recv(s,buffer,sizeof buffer-1,0);
  
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
    
    close(s);
    FD_CLR(s,master);
  }
  else
  {
    buffer[n]='\0';
    int sender_fd=s;
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
