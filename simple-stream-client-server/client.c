#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>

#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<netdb.h>

#define PORT "3490"
#define BACKLOG 10

void *get_addr_in(struct sockaddr *sa);

int main(int argc,char *argv[])
{
  int sockfd;
  char buffer[1024];
  char ip[INET6_ADDRSTRLEN];
  struct addrinfo hints,*res,*p;
  if(argc!=2)
  {
    printf("require arguments");
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
    sockfd=socket(p->ai_family,p->ai_socktype,p->ai_protocol);
    
    if(sockfd==-1)
    {
      printf("socket creation failed");
      return 1;
    }
    
    if(connect(sockfd,(struct sockaddr *)p->ai_addr,p->ai_addrlen)==0)
    {
      break;
    }
  }
  
  if(p==NULL)
  {
    printf("connection failed");
    return 1;
  }
  
  inet_ntop(p->ai_family,get_addr_in((struct sockaddr *)p->ai_addr),ip,sizeof ip);
  
  printf("client connected to %s\n",ip);
  
  freeaddrinfo(res);
  
  int n;
  
  if((n=recv(sockfd,buffer,sizeof buffer,0))==-1)
  {
    printf("error in receiving messages");
    return 1;
  }
  
  buffer[n]='\0';
  
  printf("the received message is %s\n",buffer);
  
  close(sockfd);
  return 0;
}

void *get_addr_in(struct sockaddr *sa)
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
