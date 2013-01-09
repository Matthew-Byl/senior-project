/**
 * Definitions of some simple Kalah players:
 *  -Human
 *  -Bonzo (go-again if possible, then first possible move)
 *  -Random
 *  -Sequential Minimax
 *
 * @author John Kloosterman
 * @date Dec. 24, 2012
 */

#include "simple_players.h"
#include "depths.h"
#include "evaluate.h"

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>

/**
 * Human player
 */
const char *human_name( void )
{
	return "Human";
}

int human_move( Board *b )
{
	int move = -1;

	while ( !board_legal_move( b, move ) )
	{
		printf( "Move: " );
		if ( scanf( "%d", &move ) != 1 )
			continue;

		if ( !board_legal_move( b, move ) )
			printf( "Illegal move. Try again.\n" );
	}

	return move;
}

KalahPlayer human_player( void )
{
	KalahPlayer human;

	human.get_name = human_name;
	human.make_move = human_move;

	return human;
}

/**
 * Bonzo player.
 */
int bonzo_move( Board *b )
{
	int i;

	if (b->player_to_move == TOP)
	{
		for (i=12; i>=7; i--)               // try first go-again
			if (b->board[i] == 13-i) return i;
		for (i=12; i>=7; i--)               // otherwise, first
			if (b->board[i] > 0) return i;    // available move
	} else {
		for (i=5; i>=0; i--)
			if (b->board[i] == 6-i) return i;
		for (i=5; i>=0; i--)
			if (b->board[i] > 0) return i;
	}
	return -1;              // an illegal move if there aren't any legal ones.
}                           // this can't happen unless game is over.


const char *bonzo_name( void )
{
	return "Bonzo";
}

KalahPlayer bonzo_player( void )
{
	KalahPlayer bonzo;
	
	bonzo.get_name = bonzo_name;
	bonzo.make_move = bonzo_move;

	return bonzo;
}

/**
 * Sequential minimax player
 */
const char *minimax_name( void )
{
	return "Minimax";
}

MinimaxResult minimax_move( Board *b, int depth )
{
	int i;
	MinimaxResult ret;
	MinimaxResult rec_result;
	MinimaxResult best_result;
	Board moved_board;
	best_result.move = -1;

	if ( depth == 0 || board_game_over( b ) )
	{	
		ret.move = -1;
		ret.score = minimax_eval( b );

		return ret;
	}

	if ( b->player_to_move == TOP )
	{
		// MAX
		best_result.score = INT_MIN;

		for ( i = 7; i < 13; i++ )
		{
			if ( board_legal_move( b, i ) )
			{
				moved_board = *b;
				board_make_move( &moved_board, i );

				rec_result = minimax_move( &moved_board, depth - 1 );

				if ( rec_result.score > best_result.score )
				{
					best_result.move = i;
					best_result.score = rec_result.score;
				}
			}
		}
	}
	else
	{
		// MIN
		best_result.score = INT_MAX;

		for ( i = 0; i < 6; i++ )
		{
			if ( board_legal_move( b, i ) )
			{
				moved_board = *b;
				board_make_move( &moved_board, i );

				rec_result = minimax_move( &moved_board, depth - 1 );

				if ( rec_result.score < best_result.score )
				{
					best_result.move = i;
					best_result.score = rec_result.score;
				}
			}
		}
	}

	return best_result;
}

int minimax_make_move( Board *b )
{
	MinimaxResult res = minimax_move( b, MINIMAX_DEPTH );

	return res.move;
}

KalahPlayer minimax_player( void )
{
	KalahPlayer minimax;

	minimax.get_name = minimax_name;
	minimax.make_move = minimax_make_move;

	return minimax;
}

/**
 * Random-move player.
 */
const char *random_name( void )
{
	srand( (unsigned) time( NULL ) );
	return "Random";
}

int random_move( Board *b )
{
	int move;

	do {
		move = rand() % 14;
	} while ( !board_legal_move( b, move ) );

	return move;
}

KalahPlayer random_player( void )
{
	KalahPlayer random;

	random.get_name = random_name;
	random.make_move = random_move;

	return random;
}
