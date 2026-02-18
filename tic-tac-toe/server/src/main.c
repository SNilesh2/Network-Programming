#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<errno.h>

#include<sys/epoll.h>

#include "server.h"
#include "epoll_mgr.h"
#include "websocket.h"
#include "session.h"

#define PORT "9001"
#define MAX_EVENTS 64
#define MAX_CLIENTS 1024
#define MAX_SESSIONS 512

int main()
{
  int server_fd,epoll_fd;
  struct epoll_event events[MAX_EVENTS];
  
  //client lookup by fd when epoll return 
  websocket_client_t *clients[MAX_CLIENTS];
  memset(clients,0,sizeof(clients));
  
  //session list
  session_t *sessions[MAX_SESSIONS];
  memset(sessions,0,sizeof(sessions));
  
  //map client fd -> session
  session_t *client_session[MAX_CLIENTS];
  memset(client_session,0,sizeof(client_session));
  
  
  //create server socket for listening 
  server_fd=create_server_socket(PORT);
  
  if(server_fd==-1)
  {
    return 1;
  }
  
  //create epoll instance
  epoll_fd=create_epoll_instance();
  
  if(epoll_fd==-1)
  {
    close(server_fd);
    return 1;
  }
  
  //add server socket to epoll
  add_fd_to_epoll(epoll_fd,server_fd,EPOLLIN);
  
  printf("Tic Tac Toe Server started on port %s\n",PORT);
  
  //main event loop
  while(1)
  {  
    int n=wait_for_events(epoll_fd,events,MAX_EVENTS,-1);
    
    if(n==-1)
    {
      break;
    }
    
    for(int i=0;i<n;i++)
    {
      int fd=events[i].data.fd;
      
      //new connection
      if(fd==server_fd)
      {
        while(1)
        {
          int client_fd=accept_new_client(server_fd);
          
          if(client_fd==-1)
          {
            break;
          }
          
          if(client_fd>=MAX_CLIENTS)
          {
            close(client_fd);
            continue;
          }
          
          websocket_client_t *client=ws_create(client_fd);
          
          if(!client)
          {
            close(client_fd);
            continue;
          }
          
          clients[client_fd]=client;
          
          add_fd_to_epoll(epoll_fd,client_fd,EPOLLIN | EPOLLOUT);
        }
      }
      //client event
      else
      {
        websocket_client_t *client=clients[fd];
        
        if(!client)
        {
          continue;
        }
        
        //read event
        if(events[i].events & EPOLLIN)
        {
           
           if(ws_handle_read(client)==-1)
           {
             session_t *sess=client_session[fd];
           
             if(sess)
             {
               session_remove_player(sess,client);
             }
           
             remove_fd_from_epoll(epoll_fd,fd);
           
             clients[fd]=NULL;
             client_session[fd]=NULL;
             continue;
           }
           
           //handshake finished assign session
           if(client->state==WS_STATE_CONNECTED && client_session[fd]==NULL)
           {
             session_t *target=NULL;
             
             //find available session
             for(int j=0;j<MAX_SESSIONS;j++)
             {
               if(sessions[j] && sessions[j]->player_count<MAX_PLAYERS)
               {
                 target=sessions[j];
                 break;
               }
             }
             
             //if there is no free session create new session
             if(!target)
             {
               for(int j=0;j<MAX_SESSIONS;j++)
               {
                 if(!sessions[j])
                 {
                   sessions[j]=session_create();
                   
                   if(!sessions[j])
                   {
                     printf("Session creation failed\n");
                     break;
                   }
                   target=sessions[j];
                   printf("New session created (%d)\n",j);
                   break;
                 }
               }
             }
             
             if(target)
             {
               session_add_player(target,client);
               client_session[fd]=target;
             }
           }
           
           //handle move
           if(client->state==WS_STATE_CONNECTED && client->recv_len>0)
           {
             char *payload = client->recv_buffer;

             int position = atoi(payload);

             session_t *sess = client_session[fd];
             if (sess)
             {
                session_handle_move(sess, client, position);
             }

             client->recv_len = 0;
             memset(client->recv_buffer, 0, sizeof(client->recv_buffer));
           }
        }//end of EPOLLIN if
        
        
        //write event
        if(events[i].events & EPOLLOUT)
        {
          if(ws_handle_write(client)==-1)
          {
            session_t *sess=client_session[fd];
            
            if(sess)
            {
              session_remove_player(sess,client);
            }
            
            remove_fd_from_epoll(epoll_fd,fd);
            
            clients[fd]=NULL;
            client_session[fd]=NULL;
          }
        }
        
        //error event
        if(events[i].events & (EPOLLERR | EPOLLHUP))
        {
          session_t *sess=client_session[fd];
          
          if(sess)
          {
            session_remove_player(sess,client);
          }
          
          remove_fd_from_epoll(epoll_fd,fd);
          
          clients[fd]=NULL;
          client_session[fd]=NULL;
        }
  
      } //end of else 
    }
  }
  
  
  close(server_fd);
  close(epoll_fd);
  
  for(int i=0;i<MAX_SESSIONS;i++)
  {
    if(sessions[i])
    {
      session_destroy(sessions[i]);
    }
  }
  
  return 0;
}
