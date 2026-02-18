#include "game.h"
#include<stdlib.h>
#include<string.h>

//winning combinations
static const int win_patterns[8][3]={{0,1,2},{3,4,5},{6,7,8},{0,3,6},{1,4,7},{2,5,8},{0,4,8},{2,4,6}};

game_t *game_create()
{
  game_t *game=malloc(sizeof(game_t));
  if(!game)
  {
    return NULL;
  }
  
  game_reset(game);
  
  return game;
}

void game_reset(game_t *game)
{
  if(!game)
  {
    return;
  }
  
  for(int i=0;i<BOARD_SIZE;i++)
  {
    game->board[i]=EMPTY_CELL;
  }
  
  //always x starts first 
  game->current_player=PLAYER_X;
  
  game->result=GAME_ONGOING;
}

void game_destroy(game_t *game)
{
  if(game)
  {
    free(game);
  }
}

bool game_make_move(game_t *game,int position)
{
  if(!game)
  {
    return false;
  }
  
  //invalid position
  if(position<0 || position>=BOARD_SIZE)
  {
    return false;
  }
  
  //already occupied 
  if(game->board[position]!=EMPTY_CELL)
  {
    return false;
  }
  
  //game already finished
  if(game->result!=GAME_ONGOING)
  {
    return false;
  }
  
  //place move 
  game->board[position]=game->current_player;
  
  //check result
  game->result=game_check_result(game);
  
  //switch player only if game is ongoing
  if(game->result==GAME_ONGOING)
  {
    game_switch_player(game);
  }
  
  return true;
}

game_result_t game_check_result(game_t *game)
{
  if(!game)
  {
    return GAME_ONGOING;
  }
  
  //check for win patterns
  for(int i=0;i<8;i++)
  {
    int a=win_patterns[i][0];
    int b=win_patterns[i][1];
    int c=win_patterns[i][2];
    
    //without this if else,if empty cells are same in 3 positions then it returns as win but its not
    if(game->board[a]==EMPTY_CELL)
    {
      continue;
    }
    
    if(game->board[a]==game->board[b] && game->board[b]==game->board[c])
    {
      if(game->board[a]==PLAYER_X)
      {
        return GAME_WIN_X;
      }
      else
      {
        return GAME_WIN_O;
      }
    }
  }
  
  //check for draw
  bool draw=true;
  for(int i=0;i<BOARD_SIZE;i++)
  {
    if(game->board[i]==EMPTY_CELL)
    {
      draw=false;
      break;
    }
  }
  
  if(draw)
  {
    return GAME_DRAW;
  }
  
  return GAME_ONGOING;
}

void game_switch_player(game_t *game)
{
  if(!game)
  {
    return ;
  }
  
  if(game->current_player==PLAYER_X)
  {
    game->current_player=PLAYER_O;
  }
  else
  {
    game->current_player=PLAYER_X;
  }
}

void game_get_board_string(const game_t *game,char *buffer)
{
  if(!game || !buffer)
  {
    return;
  }
  
  for(int i=0;i<BOARD_SIZE;i++)
  {
    buffer[i]=game->board[i];
  }
  
  buffer[BOARD_SIZE]='\0';
}


