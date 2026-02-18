#ifndef SESSION_H
#define SESSION_H

#include "websocket.h"
#include "game.h"

//two players in one game
#define MAX_PLAYERS 2

//session structure
typedef struct session
{
  websocket_client_t *players[MAX_PLAYERS];
  game_t *game;
  int player_count;
} session_t;

//create new session
session_t *session_create();

//destroy session
void session_destroy(session_t *session);

//add player to session
//return player index (0 or 1) or -1 if session is full
int session_add_player(session_t *session,websocket_client_t *client);

//remove player from session if the player is disconnected
void session_remove_player(session_t *session,websocket_client_t *client);

//process move from client
void session_handle_move(session_t *session,websocket_client_t *client,int position);

//broadcast message to two clients like "GAME STARTED"
void session_broadcast(session_t *session,const char *msg);

//send message to one player
void session_send_to_player(session_t *session,int player_index,const char *msg);

//get opponent player
websocket_client_t *session_get_opponent(session_t *session,websocket_client_t *client);

#endif
