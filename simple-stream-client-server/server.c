#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<sys/wait.h>
#include<errno.h>
#include<netinet/in.h>
#include<signal.h>

#define PORT "3490"
#define BACKLOG 10

void sigchild_handler(int s);
void *get_addr_in(struct sockaddr *sa);

int main()
{
  int sockfd,newfd;
  int yes=1;
  char ip[INET6_ADDRSTRLEN];
  struct addrinfo hints,*res,*p;
  struct sockaddr_storage client_addr;
  struct sigaction sa;
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
    sockfd=socket(p->ai_family,p->ai_socktype,p->ai_protocol);
    if(sockfd==-1)
    {
      printf("socket creation failed");
      return 1;
    }
    
    if(setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int))==-1)
    {
      printf("address already in use error");
      return 1;
    }
    
    if(bind(sockfd,p->ai_addr,p->ai_addrlen)==0)
    {
      break;
    }
    close(sockfd);
  }
  
  if(p==NULL)
  {
    printf("bind failed");
    return 1;
  }
  
  freeaddrinfo(res);
  
  if(listen(sockfd,BACKLOG)==-1)
  {
    printf("listen failed");
    return 1;
  }
  
  sa.sa_handler=sigchild_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags=SA_RESTART;
  if(sigaction(SIGCHLD,&sa,NULL)==-1)
  {
    printf("sigaction failed");
    return 1;
  }
  
  printf("Server : listening ...\n");
  
  while(1)
  {
    socklen_t size=sizeof client_addr;
    if((newfd=accept(sockfd,(struct sockaddr *)&client_addr,&size))==-1)
    {
      continue;
    }
    
    inet_ntop(client_addr.ss_family,get_addr_in((struct sockaddr *)&client_addr),ip,sizeof ip);
    
    printf("client connected from %s\n",ip);
    
    
    if(!fork())
    {
      close(sockfd);
      
      send(newfd,"Hey hello",9,0);
      
      close(newfd);
      exit(0);
    }
    
    close(newfd);
  }

}

void sigchild_handler(int s)
{
  (void)s;
  
  int saved_errno=errno;
  
  while(waitpid(-1,NULL,WNOHANG)>0);
  
  errno=saved_errno;
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
