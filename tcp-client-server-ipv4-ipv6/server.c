#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>

#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
#include<arpa/inet.h>

#define PORT "8080"
#define BUF_SIZE 1024
#define BACK_LOG 5

void *get_addr(struct sockaddr *sa);

int main()
{
  struct addrinfo hints,*res,*p;
  int sock_fd,client_fd;
  struct sockaddr_storage client_addr;
  char ipstr[INET6_ADDRSTRLEN];
  char buff[BUF_SIZE];
  int len;
  
  memset(&hints,0,sizeof hints);
  hints.ai_family=AF_UNSPEC;
  hints.ai_socktype=SOCK_STREAM;
  hints.ai_flags=AI_PASSIVE;
  
  if(getaddrinfo(NULL,PORT,&hints,&res)!=0)
  {
    printf("getaddrinfo failed");
    return 1;
  }
  
  for(p=res;p!=NULL;p=p->ai_next)
  {
    sock_fd=socket(p->ai_family,p->ai_socktype,p->ai_protocol);
    
    if(sock_fd==-1)
    {
      printf("socket creation failed");
      return 1;
    }
    
    if(bind(sock_fd,p->ai_addr,p->ai_addrlen)==0)
    {
      break;
    }
    
    close(sock_fd);
  }
  
  if(p==NULL)
  {
    printf("bind failed");
    return 1;
  }
  
  freeaddrinfo(res);
  
  if(listen(sock_fd,BACK_LOG)!=0)
  {
    printf("listening failed");
    return 1;
  }
  
  printf("Server is listening...");
  
  int size=sizeof client_addr;
  client_fd=accept(sock_fd,(struct sockaddr *)&client_addr,&size);
  
  if(client_fd==-1)
  {
    printf("accept failed");
    return 1;
  }
  
  inet_ntop(client_addr.ss_family,get_addr((struct sockaddr *)&client_addr),ipstr,sizeof ipstr);
  
  printf("connected from %s\n",ipstr);
  
  
  
  len=recv(client_fd,buff,BUF_SIZE-1,0);
  buff[len]='\0';
  
  printf("message received from client : %s\n",buff);
  
  send(client_fd,buff,BUF_SIZE-1,0);
  
  close(sock_fd);
  close(client_fd);
  
  return 0;
}

void *get_addr(struct sockaddr *sa)
{
  if(sa->sa_family==AF_INET)
  {
    return &(((struct sockaddr_in *)sa)->sin_addr);
  }
  else
  {
    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
  }
}
