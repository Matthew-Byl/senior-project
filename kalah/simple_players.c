#include "simple_players.h"

#include <stdio.h>

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

