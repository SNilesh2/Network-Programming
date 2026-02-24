#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/epoll.h>
#include "server.h"
#include "epoll_mgr.h"
#include "websocket.h"
#include "chat_mgr.h"

#define MAX_EVENTS 64
#define PORT "9001"

int main()
{
  int server_fd=create_server_socket(PORT);
  if(server_fd==-1)
  {
    exit(1);
  }
  
  int epoll_fd=create_epoll_instance();
  if(epoll_fd==-1)
  {
    exit(1);
  }
  
  //watch the server socket for new connections
  add_fd_to_epoll(epoll_fd,server_fd,EPOLLIN);
  
  struct epoll_event events[MAX_EVENTS];
  
  while(1)
  {
    int n=wait_for_events(epoll_fd,events,MAX_EVENTS,-1);
    
    for(int i=0;i<n;i++)
    {
      int event_fd=events[i].data.fd;
      
      //case 1 : new connection
      if(event_fd==server_fd)
      {
        int client_fd=accept_new_client(server_fd);
        if(client_fd!=-1)
        {
          websocket_client_t *client=ws_create(client_fd);
          if(chat_add_client(client)==0)
          {
            //watch for read and write
            add_fd_to_epoll(epoll_fd,client_fd,EPOLLIN|EPOLLOUT);
          }
          else
          {
            ws_destroy(client);
          }
        }
      }
      //existing client
      else
      {
        //find the client object
        websocket_client_t *client=NULL;
        for(int j=0;j<MAX_CLIENTS;j++)
        {
          if(clients[j] && clients[j]->fd==event_fd)
          {
            client=clients[j];
            break;
          }
        }
        if(!client)
        {
          continue;
        }
        
        //Handle Incoming Data
        if(events[i].events & EPOLLIN)
        {
          printf("DEBUG: Data arriving on fd=%d\n", event_fd);
          int res=ws_handle_read(client);
          if(res==-1)
          {
            printf("client fd=%d disconnected\n",event_fd);
            remove_fd_from_epoll(epoll_fd,event_fd);
            chat_remove_client(event_fd);
            continue;
          }
          else if(res==1)
          {
            //full websocket message is received
            chat_handle_message(client,client->recv_buffer);
            memset(client->recv_buffer,0,MAX_BUFFER);
            client->recv_len=0;
          }
        }
        
        //handle outgoing data (buffered write)
        if(events[i].events & EPOLLOUT)
        {
          if(ws_handle_write(client)==-1)
          {
            remove_fd_from_epoll(epoll_fd,event_fd);
            chat_remove_client(event_fd);
          }
        }
      }
    }
  }
  close(server_fd);
  return 0;
}
