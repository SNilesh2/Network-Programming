#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<time.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<netdb.h>

#define PORT "3490"
#define BUFFER_SIZE 1024

void *get_addr_in(struct sockaddr *sa);

int main(int argc,char *argv[])
{
  int sockfd;
  char ip[INET6_ADDRSTRLEN];
  char sendbuf[BUFFER_SIZE];
  char recvbuf[BUFFER_SIZE];
  
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
    printf("getaddrinfo");
    return 1;
  }
  
  for(p=res;p!=NULL;p=p->ai_next)
  {
    sockfd=socket(p->ai_family,p->ai_socktype,p->ai_protocol);
    
    if(sockfd==-1)
    {
      continue;
    }
    
    if(connect(sockfd,(struct sockaddr *)p->ai_addr,p->ai_addrlen)==0)
    {
      break;
    }
    close(sockfd);
  }
  
  if(p==NULL)
  {
    printf("connection failed");
    return 1;
  }
  
  inet_ntop(p->ai_family,get_addr_in((struct sockaddr *)p->ai_addr),ip,sizeof ip);
  
  printf("client connected to %s\n",ip);
  
  freeaddrinfo(res);
  
  while(1)
  {
  
  printf("\nEnter a message : ");
  fgets(sendbuf,BUFFER_SIZE,stdin);
  sendbuf[strcspn(sendbuf,"\n")]='\0';
  
  if(strcmp(sendbuf,"exit")==0)
  {
    break;
  }
  
  time_t now=time(NULL);
  struct tm tm_info;
  localtime_r(&now,&tm_info);
  char timestamp[64];
  strftime(timestamp,sizeof(timestamp),"%Y-%m-%d %H:%M:%S",&tm_info);
  
  char finalbuf[BUFFER_SIZE];
  snprintf(finalbuf,BUFFER_SIZE,"%s|%s",sendbuf,timestamp);
  
  if(send(sockfd,finalbuf,strlen(finalbuf),0)==-1)
  {
    perror("send");
    break;
  }
  
  memset(recvbuf,0,BUFFER_SIZE);
  
  int n=recv(sockfd,recvbuf,BUFFER_SIZE-1,0);
  
  if(n<=0)
  {
    printf("connection closed");
    break;
  }
  
  printf("\nserver response :\n%s\n",recvbuf);
  
  }
  printf("client disconnected\n");
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
