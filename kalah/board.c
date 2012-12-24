/**
 * C interface for Mankalah boards.
 *
 * @author John Kloosterman
 * @date Dec. 24, 2012
 *
 * Based off of Prof. Plantinga's C# code for CS212.
 */

#include "board.h"

/**
 * Initialize a board with positions for the beginning
 *  of a game.
 *
 * @param b
 *  A board to initialize
 * @param to_move
 *  The player who will be going first.
 */
void board_initialize( Board *b, PlayerPosition to_move )
{
	for ( int i = 0; i < 13; i++ ) 
		b->board[i] = 4;
	b->board[6] = 0;
	b->board[13] = 0;
	b->player_to_move = to_move;
}

/**
 * Determine whether a move is legal for the current
 *  player on a board.
 *
 * @param b
 *  The board in question
 * @param move
 *  The cell in the board whose stones to move.
 *
 * @return
 *  TRUE if a legal move, FALSE otherwise.
 */
int board_legal_move( Board *b, int move )
{
	return FALSE;
}
