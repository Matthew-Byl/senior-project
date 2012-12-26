#include "simple_players.h"

#include <stdio.h>
#include <limits.h>

/*** Human ***/
char *human_name( void )
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

/*** Bonzo ***/

int bonzo_move( Board *b )
{
	if (b->player_to_move == TOP)
	{
		for (int i=12; i>=7; i--)               // try first go-again
			if (b->board[i] == 13-i) return i;
		for (int i=12; i>=7; i--)               // otherwise, first
			if (b->board[i] > 0) return i;    // available move
	} else {
		for (int i=5; i>=0; i--)
			if (b->board[i] == 6-i) return i;
		for (int i=5; i>=0; i--)
			if (b->board[i] > 0) return i;
	}
	return -1;              // an illegal move if there aren't any legal ones.
}                           // this can't happen unless game is over.


char *bonzo_name( void )
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

/********* Minimax Player ***************/

char *minimax_name( void )
{
	return "Minimax";
}

int minimax_eval( Board *b )
{
	// Kalah counts double, but stones count too.

	int score = 2 * ( b->board[13] - b->board[6] );
	
	for ( int i = 0; i <= 5; i++ )
		score -= b->board[i];

	for ( int i = 7; i <= 12; i++ )
		score += b->board[i];

	return score;
}

typedef struct
{
	int move;
	int score;
} MinimaxResult;

MinimaxResult minimax_move( Board *b, int depth )
{
	if ( depth == 0 || board_game_over( b ) )
	{
		MinimaxResult ret;
		
		ret.move = -1;
		ret.score = minimax_eval( b );

		return ret;
	}

	MinimaxResult rec_result;
	MinimaxResult best_result;
	best_result.move = -1;

	if ( b->player_to_move == TOP )
	{
		// MAX
		best_result.score = INT_MIN;

		for ( int i = 7; i <= 12; i++ )
		{
			if ( board_legal_move( b, i ) )
			{
				Board moved_board;
				board_copy( b, &moved_board );
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

		for ( int i = 0; i <= 5; i++ )
		{
			if ( board_legal_move( b, i ) )
			{
				Board moved_board;
				board_copy( b, &moved_board );
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
	MinimaxResult res = minimax_move( b, 9 );

	return res.move;
}

KalahPlayer minimax_player( void )
{
	KalahPlayer minimax;

	minimax.get_name = minimax_name;
	minimax.make_move = minimax_make_move;

	return minimax;
}

/*********** Stackless Minimax player ****************/

#define DEPTH 3
#define TREE_SIZE 216  // 6 ^ DEPTH

int stackless_minimax_move( Board *b )
{
	int tree[TREE_SIZE];
	Board boards[TREE_SIZE];

	// A node's children are in boards[(6 * node) + k]
	//  this is a 6-ary tree.
	// Populate the board tree.
	board_copy( b, &boards[0] );

	int move_offset;
	for ( int i = 0; i < TREE_SIZE / 2; i++ )
	{
		if ( boards[i].player_to_move == TOP )
			move_offset = 7;
		else
			move_offset = 0;

		for ( int j = 0; j < 6; j++ )
		{
			board_copy( &boards[i], &boards[6*i + j] );
			
			if ( board_legal_move( &boards[6*i + j], j + move_offset )
				 && !board_game_over( &boards[i] ) )
				board_make_move( &boards[6*i + j], j + move_offset );

			// Just leave end of games, etc. be.
		}
	}

	// Run the evaluate function on the bottom half of the tree.
	for ( int i = TREE_SIZE / 2; i <= TREE_SIZE; i++ )
	{
		tree[i] = minimax_eval( &boards[i] );
	}

	// Run minimax backwards. We can probably do this with more parallelism.
	// @XXX: PROBLEM: go-agains. This makes the tree not uniformly min-max.

	for ( int i = ( TREE_SIZE / 2 ) - 1; i > 0; i-- ) // we intentionally don't do tree[0].
	{
		if ( boards[i].player_to_move == TOP )
		{
			tree[i] = INT_MIN;
			for ( int j = 0; j < 6; j++ )
			{
				if ( tree[6*i + j] > tree[i] )
					tree[i] = tree[6*i + j];
			}
		}
		else
		{
			tree[i] = INT_MAX;
			for ( int j = 0; j < 6; j++ )
			{
				if ( tree[6*i + j] < tree[i] )
					tree[i] = tree[6*i + j];
			}
		}
	}

	int best_move;
	int best_score;
	if ( b->player_to_move == TOP )
	{
		best_score = INT_MIN;
		for ( int i = 1; i < 7; i++ )
		{
			if ( tree[i] > best_score )
			{
				best_score = tree[i];
				best_move = i;
			}
		}
	}
	else
	{
		best_score = INT_MAX;
		for ( int i = 1; i < 7; i++ )
		{
			if ( tree[i] < best_score )
			{
				best_score = tree[i];
				best_move = i;
			}
		}
	}

	return best_move;
}

char *stackless_minimax_name()
{
	return "Stackless Minimax";
}

KalahPlayer stackless_minimax_player( void )
{
	KalahPlayer st_minimax;

	st_minimax.get_name = stackless_minimax_name;
	st_minimax.make_move = stackless_minimax_move;

	return st_minimax;	
}
