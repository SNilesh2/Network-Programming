#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "chat_mgr.h"
#include "protocol.h"

#define MAX_USERNAME_LEN 32

websocket_client_t *clients[MAX_CLIENTS]={NULL};

int chat_add_client(websocket_client_t *client)
{
  for(int i=0;i<MAX_CLIENTS;i++)
  {
    if(clients[i]==NULL)
    {
      clients[i]=client;
      return 0;
    }
  }
  return -1;//server full
}

void chat_remove_client(int fd)
{
  for(int i=0;i<MAX_CLIENTS;i++)
  {
    if(clients[i] && clients[i]->fd==fd)
    {
      ws_destroy(clients[i]);
      clients[i]=NULL;
      break;
    }
  }
}

void chat_broadcast(const char *msg,int exclude_fd)
{
  for(int i=0;i<MAX_CLIENTS;i++)
  {
    //send to everyone
    if(clients[i] && clients[i]->state==WS_STATE_CONNECTED)
    {
      if(clients[i]->fd!=exclude_fd)
      {
        ws_send_text(clients[i],msg,strlen(msg));
      }
    }
  }
}


int chat_send_private(const char *target_username,const char *msg,const char *sender_name)
{
  char formatted_pvt[MAX_BUFFER];
  
  //format : "PVT:sendername:actual msg"
  snprintf(formatted_pvt,sizeof(formatted_pvt),"%s%s:%s",PREFIX_PRIVATE,sender_name,msg);
  
  for(int i=0;i<MAX_CLIENTS;i++)
  {
    if(clients[i] && clients[i]->has_set_username)
    {
      if(strcmp(clients[i]->username,target_username)==0)
      {
        ws_send_text(clients[i],formatted_pvt,strlen(formatted_pvt));
        return 0;
      }
    }
  }
  
  return -1;
}


void chat_handle_message(websocket_client_t *client,char *raw_msg)
{
  //remove any trailing values 
  size_t len=strlen(raw_msg);
  while(len>0 && (raw_msg[len-1]=='\n' || raw_msg[len-1]=='\r'))
  {
    raw_msg[--len]='\0';
  }
  
  //handle join "JOIN:Username"
  if(strncmp(raw_msg,PREFIX_JOIN,strlen(PREFIX_JOIN))==0)
  {
    char *name=raw_msg + strlen(PREFIX_JOIN);
    
    //DEBUG : see if there are any hidden messages
    printf("DEBUG: user trying to join with name:[%s]\n",name);
    
    memset(client->username,0,MAX_USERNAME_LEN);
    strncpy(client->username,name,MAX_USERNAME_LEN-1);
    client->has_set_username=1;
    
    char system_msg[128];
    snprintf(system_msg,sizeof(system_msg),"%s%s joined the chat!",PREFIX_SYSTEM,client->username);
    
    chat_broadcast(system_msg,-1);
  }
  //handle public msg "MSG:Hello"
  else if(strncmp(raw_msg,PREFIX_MSG,strlen(PREFIX_MSG))==0)
  {
    if(!client->has_set_username)
    {
      return;
    }
    
    char *text=raw_msg + strlen(PREFIX_MSG);
    char broadcast_payload[MAX_BUFFER];
   
   //send as "MSG:username:content"
   snprintf(broadcast_payload,sizeof(broadcast_payload),"%s%s:%s",PREFIX_MSG,client->username,text);
    chat_broadcast(broadcast_payload,-1);
  }
  //handle private msg "PVT:targetname:msg"
  else if(strncmp(raw_msg,PREFIX_PRIVATE,strlen(PREFIX_PRIVATE))==0)
  {
    char *target=raw_msg+strlen(PREFIX_PRIVATE);
    char *message=strchr(target,':');
    
    if(message)
    {
      *message='\0';
      message++;
      if(chat_send_private(target,message,client->username)==-1)
      {
        char error_msg[128];
        snprintf(error_msg,sizeof(error_msg),"SYS: User %s not found",target);
        ws_send_text(client,error_msg,strlen(error_msg));
      }
    }
  }
}
