//client program

#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<stdlib.h>

int main()
{
  int sockfd;
  struct sockaddr_in server;
  char buffer[1024];
  
  sockfd=socket(AF_INET,SOCK_STREAM,0);
  
  if(sockfd<0)
  {
    printf("error");
    exit(1);
  }
  
  server.sin_family=AF_INET;
  server.sin_port=htons(8080);
  server.sin_addr.s_addr=inet_addr("127.0.0.1");
  
  if(connect(sockfd,(struct sockaddr*)&server,sizeof(server))<0)
  {
    printf("connection error");
    exit(1);
  }
  
  send(sockfd,"Hello server",12,0);
  printf("client sent the message\n");
  int n;
  n=recv(sockfd,buffer,sizeof(buffer),0);
  buffer[n]='\n';
  printf("message received from server is %s \n: ",buffer);
  
  close(sockfd);
  
  return 0;
}
