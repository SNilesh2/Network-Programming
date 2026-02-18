#include "session.h"
#include<stdlib.h>
#include<string.h>
#include<stdio.h>

//create new session
session_t *session_create()
{
  session_t *session=malloc(sizeof(session_t));
  if(!session)
  {
    return NULL;
  }
  
  session->player_count=0;
  
  //initialize players to NULL
  for(int i=0;i<MAX_PLAYERS;i++)
  {
    session->players[i]=NULL;
  }
  
  //create game instance
  session->game=game_create();
  
  if(!session->game)
  {
    free(session);
    return NULL;
  }
  return session;
}

//destroy session
void session_destroy(session_t *session)
{
  if(!session)
  {
    return;
  }
  
  for(int i=0;i<MAX_PLAYERS;i++)
  {
    if(session->players[i])
    {
      ws_close(session->players[i]);
      ws_destroy(session->players[i]);
    }
  }
  
  if(session->game)
  {
    game_destroy(session->game);
  }
  
  free(session);
}

//add player to session
int session_add_player(session_t *session,websocket_client_t *client)
{
  if(!session || !client)
  {
    return -1;
  }
  
  if(session->player_count>=MAX_PLAYERS)
  {
    return -1;
  }
  
  int index=-1;
  
  for(int i=0;i<MAX_PLAYERS;i++)
  {
    if(session->players[i]==NULL)
    {
      index=i;
      break;
    }
  }
  
  if(index==-1)
  {
    return -1;
  }
  
  session->players[index]=client;
  session->player_count++;
  
  printf("Player added at index %d (fd=%d)\n",index,client->fd);
  
  //player symbol notification 
  if(index==0)
  {
    ws_send_text(client,"SYMBOL:X",8);
  }
  else
  {
    ws_send_text(client,"SYMBOL:O",8);
  }
  
  //if both players connected start the game 
  if(session->player_count==MAX_PLAYERS)
  {
    session_broadcast(session,"GAME START");
    
    char board[BOARD_SIZE+1];
    game_get_board_string(session->game,board);
    
    char msg[64];
    snprintf(msg,sizeof(msg),"BOARD:%s",board);
    
    session_broadcast(session,msg);
  }
  
  return index;
}

//remove player
void session_remove_player(session_t *session,websocket_client_t *client)
{
  if(!session || !client)
  { 
    return;
  }
  
  for(int i=0;i<MAX_PLAYERS;i++)
  {
    if(session->players[i]==client)
    {
      printf("Player removed index %d (fd=%d)\n",i,client->fd);
      
      session->players[i]=NULL;
      session->player_count--;
      
      ws_close(client);
      ws_destroy(client);
      
      websocket_client_t *opponent=session_get_opponent(session,client);
      
      if(opponent)
      {
        ws_send_text(opponent,"OPPONENT LEFT",13);
      }
      break;
    }
  }
}


//broadcast message to all players
void session_broadcast(session_t *session,const char *msg)
{
  if(!session || !msg)
  {
    return;
  }
  
  for(int i=0;i<MAX_PLAYERS;i++)
  {
    if(session->players[i])
    {
      ws_send_text(session->players[i],msg,strlen(msg));
    }
  }
}

//send message to one player
void session_send_to_player(session_t *session,int player_index,const char *msg)
{
  if(!session || !msg)
  {
    return;
  }
  
  if(player_index<0 || player_index>=MAX_PLAYERS)
  {
    return;
  }
  
  websocket_client_t *player=session->players[player_index];
  
  if(player)
  {
    ws_send_text(player,msg,strlen(msg));
  }
}

//get opponent player (to send opponent left message)
websocket_client_t *session_get_opponent(session_t *session,websocket_client_t *client)
{
  if(!session || !client)
  {
    return NULL;
  }
  
  for(int i=0;i<MAX_PLAYERS;i++)
  {
    if(session->players[i] && session->players[i]!=client)
    {
      return session->players[i];
    }
  }
  return NULL;
}


//handle move from client
void session_handle_move(session_t *session,websocket_client_t *client,int position)
{
  if(!session || !client)
  {
    return;
  }
  
  if(session->player_count < 2)
  {
    ws_send_text(client,"WAIT FOR OPPONENT",17);
    return;
  }
  
  game_t *game=session->game;
  
  if(game->result != GAME_ONGOING)
  {
    ws_send_text(client,"GAME OVER",9);
    return;
  }
  
  //find player index
  int player_index=-1;
  
  for(int i=0;i<MAX_PLAYERS;i++)
  {
    if(session->players[i]==client)
    {
      player_index=i;
      break;
    }
  }
  
  if(player_index==-1)
  {
    return;
  }
  
  char expected_player=(player_index==0)?PLAYER_X : PLAYER_O;
  
  //validate client's turn
  if(game->current_player!=expected_player)
  {
    ws_send_text(client,"NOT YOUR TURN",13);
    return;
  }
  
  //make move
  if(!game_make_move(game,position))
  {
    ws_send_text(client,"INVALID MOVE",12);
    return;
  }
  
  printf("Player %c played at %d\n",expected_player,position);
  
  //send updated board to both the players
  char board[BOARD_SIZE+1];
  game_get_board_string(game,board);
  
  char msg[64];
  snprintf(msg,sizeof(msg),"BOARD:%s",board);
  
  session_broadcast(session,msg);
  
  //check result
  switch(game->result)
  {
    case GAME_WIN_X:
      session_broadcast(session,"WIN:X");
      break;
    case GAME_WIN_O:
      session_broadcast(session,"WIN:O");
      break;
    case GAME_DRAW:
      session_broadcast(session,"DRAW");
      break;
    default:
      break;
  }
}
