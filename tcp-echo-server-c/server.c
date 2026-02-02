#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<pthread.h>
#include<netinet/in.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void *handle_client(void *arg);

int message_count=0;
pthread_mutex_t count_mutex=PTHREAD_MUTEX_INITIALIZER;

int main()
{
  int sockfd;
  struct sockaddr_in server_addr;
  
  sockfd=socket(AF_INET,SOCK_STREAM,0);
  
  if(sockfd<0)
  {
    perror("socket");
    return 1;
  }
  
  server_addr.sin_family=AF_INET;
  server_addr.sin_port=htons(PORT);
  server_addr.sin_addr.s_addr=INADDR_ANY;
  
  int bindVal=bind(sockfd,(struct sockaddr *)&server_addr,sizeof(server_addr));
  if(bindVal<0)
  {
    perror("bind");
    return 1;
  }
  
  listen(sockfd,5);
  
  while(1)
  {
    int *clientfd=malloc(sizeof(int));
    *clientfd=accept(sockfd,NULL,NULL);
    
    pthread_t tid;
    pthread_create(&tid,NULL,handle_client,clientfd);
    pthread_detach(tid);
  }
  return 0;
}

void *handle_client(void *arg)
{
  int client_fd=*(int *)arg;
  free(arg);
  
  char buffer[BUFFER_SIZE];
  char response[BUFFER_SIZE];
  memset(buffer,0,BUFFER_SIZE);
  int bytes=recv(client_fd,buffer,BUFFER_SIZE-1,0);
  
  if(recv<0)
  {
    close(client_fd);
    perror("recv");
    exit(1);
  }
  
  pthread_mutex_lock(&count_mutex);
  message_count++;
  int count=message_count;
  pthread_mutex_unlock(&count_mutex);
  
  char *message=strtok(buffer, "|");
  char *timestamp=strtok(NULL, "|");

  snprintf(response,BUFFER_SIZE,
             "Echo: %s\nTimestamp: %s\nTotal Messages: %d\n",
             message ? message : "NULL",
             timestamp ? timestamp : "NULL",
             count);

    send(client_fd, response, strlen(response), 0);
    close(client_fd);

    return NULL;
  
}
