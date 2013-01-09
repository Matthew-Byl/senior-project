/**
 * C interface for Mankalah boards.
 *
 * @author John Kloosterman
 * @date Dec. 24, 2012
 *
 * Based off of Prof. Plantinga's C# code for CS212.
 */

#include "board.h"

#ifndef _OPENCL_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif

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
	int i;

	for ( i = 0; i < 13; i++ ) 
		b->board[i] = 4;
	b->board[6] = 0;
	b->board[13] = 0;
	b->player_to_move = to_move;
	b->score = 0;
	b->legal_move = TRUE;
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

/**
 * Determine whether the game is over, given
 *  a board from a game.
 *
 * @param b
 *  The board in question
 */
int board_game_over( Board *b )
{
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

/**
 * Determine who won the game, given a board for 
 *  which the game is over.
 *
 * @param b
 *  A board for which the game is over.
 */
PlayerPosition board_winner( Board *b )
{
	int top = board_top_score( b );
	int bottom = board_bottom_score( b );

	if ( top > bottom )
		return TOP;
	else if ( bottom > top )
		return BOTTOM;
	else
		return NOBODY;
}

/**
 * Return the score of the player in TOP
 *  position.
 */
int board_top_score( Board *b )
{
	int i;

	int score = 0;
	for ( i = 7; i <= 13; i++ )
		score += b->board[i];

	return score;
}

/**
 * Return the score of the player in
 *  BOTTOM position.
 */
int board_bottom_score( Board *b )
{
	int i;

	int score = 0;
	for ( i = 0; i <= 6; i++ )
		score += b->board[i];

	return score;
}

/**
 * Perform a move on a board.
 *
 * @param b
 *  The board to make the move on. This board
 *   will be modified by this function.
 * @param move
 *  The index of the move to make.
 *
 * @return
 *  The number of stones captured in the move,
 *   if any.
 */
int board_make_move( Board *b, int move )
{
	int stones, capture, pos;

	// Check if a legal move.
	if ( !board_legal_move( b, move ) )
	{
#ifndef _OPENCL_
		printf( "Illegal move: %d; terminating.", move );
		abort();
#endif

		return -1;
	}

	// Pick up the stones
	stones = b->board[move];
	b->board[move] = 0;
	capture = 0;
	
    // distribute the stones
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

/**
 * Output a board to standard output.
 */
void board_print( Board *b )
{
#ifndef _OPENCL_
	int i;

	printf("\n    ");
	for (i=12; i>=7; i--)
		printf("%d  ", b->board[i] );
	printf("\n");
	printf("%d                     %d\n", b->board[13], b->board[6]);
	printf("    ");
	for (i=0; i<=5; i++)
		printf("%d  ", b->board[i]);
	printf("\n");
	printf( "Score: %d\n", b->score );
	printf( "Legal move: %d\n", b->legal_move );
	printf( "Player to move: %d\n", b->player_to_move );
#endif
}

/**
 * Determine whether two boards are equal. This doesn't
 *  check the score field.
 *
 * @param a, b
 *  The boards to compare.
 */
int board_equal( Board *a, Board *b )
{
	int i;

	if ( !(a->legal_move) && !(b->legal_move) )
		return TRUE;

	if ( a->legal_move != b->legal_move )
		return FALSE;	

	if ( a->player_to_move != b->player_to_move )
		return FALSE;

	for ( i = 0; i < 14; i++ )
	{
		if ( a->board[i] != b->board[i] )
			return FALSE;
	}

	return TRUE;
}
