#ifndef _BOARD_H
#define _BOARD_H

#include "types.h"

/*
 * C interface for Mankalah boards.
 *
 * @author John Kloosterman
 * @date Dec. 24, 2012
 */

#define BOARD_SIZE 14
#define TOP_KALAH 13
#define BOTTOM_KALAH 6

typedef CL_CHAR PlayerPosition;
#define TOP 0
#define	BOTTOM 1
#define	NOBODY 2

typedef struct {
	CL_CHAR board[BOARD_SIZE];
	PlayerPosition player_to_move;
	CL_INT score;
	CL_CHAR legal_move;
} Board;

void board_initialize( Board *b, PlayerPosition to_move );
int board_legal_move( Board *b, int move );
int board_game_over( Board *b );
int board_make_move( Board *b, int move );
PlayerPosition board_winner( Board *b );
int board_top_score( Board *b );
int board_bottom_score( Board *b );
void board_print( Board *b );
int board_equal( Board *a, Board *b );

#endif
