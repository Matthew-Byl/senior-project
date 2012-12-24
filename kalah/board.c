/**
 * C interface for Mankalah boards.
 *
 * @author John Kloosterman
 * @date Dec. 24, 2012
 *
 * Based off of Prof. Plantinga's C# code for CS212.
 */

#include "board.h"

#include <stdio.h>
#include <stdlib.h>

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
	if ( b->player_to_move == TOP
		 && move >= 7 
		 && move <= 12 
		 && b->board[move] != 0) 
	{
		return TRUE;
	}
	else if ( 
		b->player_to_move == BOTTOM 
		&& move >= 0 
		&& move <= 5 
		&& b->board[move] != 0 )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

int board_game_over( Board *b )
{
	// Optimize for memory usage? It might be faster to copy the board
	//  to a local variable than to use double indirection so many times.

	if ( b->board[0] == 0 && b->board[1] == 0 && b->board[2] == 0 &&
		 b->board[3] == 0 && b->board[4] == 0 && b->board[5] == 0 )
	{
		return TRUE;
	}
	else if ( b->board[7] == 0 && b->board[8] == 0 && b->board[9] == 0 &&
			  b->board[10] == 0 && b->board[11] == 0 && b->board[12] == 0 )
	{
        return TRUE;
	}
	else
	{
		return FALSE;
	}
}

int board_make_move( Board *b, int move )
{
	const int MAKE_MOVE_DEBUG = 1;

	// Check if a legal move.
	if ( !board_legal_move( b, move ) )
	{
		if ( MAKE_MOVE_DEBUG )
		{
			printf( "Illegal move: %d; terminating.", move );
			exit( 1 );
		}

		return -1;
	}

	// Pick up the stones
	int stones = b->board[move];
	b->board[move] = 0;
	int capture = 0;
	
    // distribute the stones
	int pos;
	for ( pos = move + 1; stones > 0; pos++ )
	{
		// Don't add stones to opponent's kalah
		if ( b->player_to_move == TOP && pos == 6 )
			pos++;
		if ( b->player_to_move == BOTTOM && pos == 13 ) 
			pos++;
		if ( pos==14 )
			pos=0;

		b->board[pos]++;
		stones--;
	}
	pos--;

	// was there a capture by TOP?
	if ( b->player_to_move == TOP && pos > 6 && pos < 13 && b->board[pos]==1 && b->board[12-pos] > 0 )
	{
		capture = b->board[12-pos] + 1;
		b->board[13] += b->board[12-pos];
		b->board[12-pos] = 0;
		b->board[13]++;
		b->board[pos]=0;
	}

	// was there a capture by BOTTOM?
	if ( b->player_to_move == BOTTOM && pos >= 0 && pos < 6 && b->board[pos] == 1 && b->board[12-pos] > 0 )
	{
		capture = b->board[12-pos] + 1;
		b->board[6] += b->board[12-pos];
		b->board[12-pos] = 0;
		b->board[6]++;
		b->board[pos] = 0;
	}

	// who gets the next move?
	if ( b->player_to_move == TOP )
	{
		if ( pos != 13 )
			b->player_to_move = BOTTOM;
	}
	else 
	{ 
		if ( pos != 6 )
			b->player_to_move = TOP;
	}

	return capture;
}

void board_print( Board *b )
{
	printf("\n    ");
	for (int i=12; i>=7; i--)
		printf("%d  ", b->board[i] );
	printf("\n");
	printf("%d                     %d", b->board[13], b->board[6]);
	printf("    ");
	for (int i=0; i<=5; i++)
		printf("%d  ", b->board[i]);
	printf("\n");

}
