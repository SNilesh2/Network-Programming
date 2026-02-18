#ifndef GAME_H
#define GAME_H 

#include<stdbool.h>

#define BOARD_SIZE 9

//player symbols
#define PLAYER_X 'X'
#define PLAYER_O 'O'
#define EMPTY_CELL ' '

//game result states
typedef enum
{
  GAME_ONGOING=0,
  GAME_WIN_X,
  GAME_WIN_O,
  GAME_DRAW
} game_result_t;

//game structure
typedef struct game
{
  char board[BOARD_SIZE];
  char current_player;
  game_result_t result;
}game_t;

//create new game
game_t *game_create();

//destroy game
void game_destroy(game_t *game);

//reset game
void game_reset(game_t *game);

//make a move return true if the move is valid
bool game_make_move(game_t *game,int position);

//check winner
game_result_t game_check_result(game_t *game);

//switch player
void game_switch_player(game_t *game);

//get board as string
void game_get_board_string(const game_t *game,char *buffer);

#endif

