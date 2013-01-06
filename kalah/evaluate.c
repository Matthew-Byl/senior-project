#include "evaluate.h"

int minimax_eval( Board *b )
{
	// Kalah counts double, but stones count too.
	int score = 5 * ( b->board[13] - b->board[6] );
	
	for ( int i = 0; i <= 5; i++ )
		score -= b->board[i];

	for ( int i = 7; i <= 12; i++ )
		score += b->board[i];

	return score;
}
