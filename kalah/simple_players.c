#include "simple_players.h"

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define MINIMAX_DEPTH 5

/*** Human ***/
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

/********* Minimax Player ***************/

const char *minimax_name( void )
{
	return "Minimax";
}

int num_evaled;
int minimax_eval( Board *b )
{
	// Kalah counts double, but stones count too.

	int score = 5 * ( b->board[13] - b->board[6] );
	
	for ( int i = 0; i <= 5; i++ )
		score -= b->board[i];

	for ( int i = 7; i <= 12; i++ )
		score += b->board[i];

	num_evaled++;
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

//		printf( "%d ", ret.score );

		return ret;
	}

	MinimaxResult rec_result;
	MinimaxResult best_result;
	best_result.move = -1;

	if ( b->player_to_move == TOP )
	{
		// MAX
		best_result.score = INT_MIN;

		for ( int i = 7; i < 13; i++ )
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

		for ( int i = 0; i < 6; i++ )
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
	num_evaled = 0;
	MinimaxResult res = minimax_move( b, MINIMAX_DEPTH );
	printf( "Number of boards evaluated: %d\n", num_evaled );

//	printf( "Best move has score %d\n", res.score );

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

#define DEPTH 7
#define TREE_SIZE 1679616  // 6^1 + 6^2 + 6^3 + 6^4

// Having the b param makes the first one not a special case.
void populate_children( Board *boards, Board *b, int n )
{
	// Populate the rest of the nodes.
	int move_offset;
	if ( b->player_to_move == TOP )
		move_offset = 7;
	else
		move_offset = 0;

	for ( int k = 0; k < 6; k++ )
	{
		board_copy( b, &boards[6*(n+1) + k] );
	}

	// If the game is over, a copy is enough.
	if ( board_game_over( b ) )
	{
		return;
	}

	for ( int k = 0; k < 6; k++ )
	{
		if ( board_legal_move( &boards[6*(n+1) + k], k + move_offset ) )
		{
			board_make_move( &boards[6*(n+1) + k], k + move_offset );
		}
		// Just leave end of games, etc. be.
	}
}

int int_pow(int x, int p) {
	int i = 1;
	for (int j = 0; j < p; j++)  i *= x;
	return i;
}

int stackless_minimax_move( Board *b )
{
	int *tree = malloc( sizeof( int ) * TREE_SIZE );
	Board *boards = malloc( sizeof( Board ) * TREE_SIZE );

	// A node's children are in boards[(6 * (node + 1) ) + k]. The root node is left off of this tree.
	//  this is a 6-ary tree.
	//
	// The array ends up looking like
	//  [ 1 everything else][ 6 leaf nodes ]
	// So the leaf nodes start at the 0 + 6 + ... + (depth - 1)th step.
	// I'm too lazy to find a theorem for what this is, so compute it.
	int leaf_start = 0;
	for ( int i = 1; i < DEPTH; i++ )
		leaf_start += int_pow( 6, i );

	// Populate the board tree.	
	// Populate the first 6 nodes.
	populate_children( boards, b, -1 );

	// Populate the rest of the nodes
	for ( int i = 0; i < leaf_start; i++ )
	{
		populate_children( boards, &boards[i], i );
	}

	// Run the evaluate function on the bottom half of the tree.
	for ( int i = leaf_start; i < TREE_SIZE; i++ )
	{
		tree[i] = minimax_eval( &boards[i] );
	}

	// Run minimax backwards. We can probably do this with more parallelism.
	// @XXX: PROBLEM: go-agains. This makes the tree not uniformly min-max.

	for ( int i = leaf_start - 1; i >= 0; i-- )
	{
		if ( board_game_over( &boards[i] ) )
		{
			tree[i] = minimax_eval( &boards[i] );
//			printf( "E " );
			continue;
		}

		if ( boards[i].player_to_move == TOP )
		{
			tree[i] = INT_MIN;
			for ( int j = 0; j < 6; j++ )
			{
				if ( tree[6*(i+1) + j] > tree[i]
					 && board_legal_move( &boards[i], j + 7 ) )
					tree[i] = tree[6*(i+1) + j];
			}

//			printf( "X " );
		}
		else
		{
			tree[i] = INT_MAX;
			for ( int j = 0; j < 6; j++ )
			{
				if ( tree[6*(i+1) + j] < tree[i]
					 && board_legal_move( &boards[i], j ) )
					tree[i] = tree[6*(i+1) + j];
			}

//			printf( "N " );
		}
	}

//	printf( "\n" );

	printf( "Player to move: %d\n", b->player_to_move );

	int best_move;
	int best_score;
	if ( b->player_to_move == TOP )
	{
		best_score = INT_MIN;
		for ( int i = 0; i < 6; i++ )
		{
			if ( tree[i] > best_score 
				 && board_legal_move( b, i + 7 ) )
			{
				best_score = tree[i];
				best_move = i + 7; // 7 is the move offset.
			}
		}
	}
	else
	{
		best_score = INT_MAX;
		for ( int i = 0; i < 6; i++ )
		{
			if ( tree[i] < best_score
				 && board_legal_move( b, i ) )
			{
				best_score = tree[i];
				best_move = i;
			}
		}
	}

/*
	printf( "Score tree:\n" );
	for ( int i = 0; i < TREE_SIZE; i++ )
	{
		printf( "%d ", tree[i] );
	}
	printf( "\n" );
*/

	free( boards );
	free( tree );

	printf( "Best move has score %d\n", best_score );
	return best_move;
}

const char *stackless_minimax_name( void )
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
