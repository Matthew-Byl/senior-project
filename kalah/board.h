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

typedef enum {
	TOP,
	BOTTOM
} PlayerPosition;

typedef struct {
	CL_INT board[BOARD_SIZE];
	PlayerPosition player_to_move;
} Board;

void board_initialize( Board *b, PlayerPosition to_move );
int board_legal_move( Board *b, int move );
int board_game_over( Board *b );
int board_make_move( Board *b, int move );

#endif
