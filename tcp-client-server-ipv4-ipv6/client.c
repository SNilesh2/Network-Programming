#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
#include<arpa/inet.h>

#define PORT "8080"
#define BUF_SIZE 1024

void *get_addr(struct sockaddr *sa);

int main(int argc,char *argv[])
{
  struct addrinfo hints,*res,*p;
  char ipstr[INET6_ADDRSTRLEN];
  int sock_fd;
  int len;
  char buff[BUF_SIZE];
  if(argc!=2)
  {
    printf("Arguments required");
    return 1;
  }
  
  memset(&hints,0,sizeof hints);
  hints.ai_family=AF_UNSPEC;
  hints.ai_socktype=SOCK_STREAM;
  
  if(getaddrinfo(argv[1],PORT,&hints,&res)!=0)
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
    
    if(connect(sock_fd,p->ai_addr,p->ai_addrlen)==0)
    {
      break;
    }
    
    close(sock_fd);
  }
  
  if(p==NULL)
  {
    printf("connection failed");
    return 1;
  }
  
  inet_ntop(p->ai_family,get_addr((struct sockaddr *)p->ai_addr),ipstr,sizeof ipstr);
  
  printf("connected to %s \n",ipstr);
  
  freeaddrinfo(res);
  
  send(sock_fd,"Just a quick chat",18,0);
  
  len=recv(sock_fd,buff,BUF_SIZE-1,0);
  buff[len]='\0';
  
  printf("received message %s\n",buff);
  
  close(sock_fd);
  
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

