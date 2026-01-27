//server program


#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>

int main()
{
  int sockfd,clientfd;
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
  server.sin_addr.s_addr=INADDR_ANY;
  
  if(bind(sockfd,(struct sockaddr*)&server,sizeof(server))<0)
  {
    printf("bind failed");
    exit(1);
  }
  
  printf("server listening...\n");
  listen(sockfd,1);
  
  
  
  clientfd=accept(sockfd,NULL,NULL);
  
  if(clientfd<0)
  {
    printf("accept failed");
    exit(1);
  }
  int n;
  n=recv(clientfd,buffer,sizeof(buffer),0);
  buffer[n]='\n';
  printf("message received from client %s:\n",buffer);
  
  send(clientfd,"Hello Client",12,0);
  printf("message sent to client\n");
  
  
  close(clientfd);
  close(sockfd);
  
  return 0;
}
