#include "board.h"
#include "kalah_player.h"
#include "simple_players.h"

#include <stdio.h>

#define MAX_SCORE 96

int play_game( KalahPlayer top, KalahPlayer bottom, PlayerPosition first_player )
{
	Board b_data;
	Board *b = &b_data;  // as shorthand

	board_initialize( b, first_player );
	
	if ( first_player == TOP )
		printf( "Player %s begins.\n", top.get_name() );
	else
		printf( "Player %s begins.\n", bottom.get_name() );

	while ( !board_game_over( b ) )
	{
		int move;

		board_print( b );		

		printf( "\n" );

		if ( b->player_to_move == TOP )
		{
			move = top.make_move( b );
			printf( "TOP (%s) chooses move %d.\n", top.get_name(), move );
		}
		else
		{
			move = bottom.make_move( b );
			printf( "BOTTOM (%s) chooses move %d.\n", bottom.get_name(), move );
		}

		board_make_move( b, move );

		if ( board_game_over( b ) )
		{
			if ( board_winner( b ) == TOP )
			{
				printf( "TOP (%s) wins, %d to %d.\n", 
						top.get_name(),
						board_top_score( b ),
						board_bottom_score( b ) );
			}
			else if ( board_winner( b ) == BOTTOM )
			{
				printf( "BOTTOM (%s) wins, %d to %d.\n", 
						bottom.get_name(),
						board_bottom_score( b ),
						board_top_score( b ) );
			}
			else
			{
				printf( "TIE!\n" );
			}
		}
		else
		{
			if ( b->player_to_move == TOP )
				printf( "TOP (%s) to move.\n", top.get_name() );
			else
				printf( "BOTTOM (%s) to move.\n", top.get_name() );
		}
	}

	return board_top_score( b );
}

int main ( void )
{
	KalahPlayer top = minimax_player();
	KalahPlayer bottom = bonzo_player();

	printf( "============ Game 1 ============\n" );
	int game1 = play_game( top, bottom, TOP );

	printf( "============ Game 2 ============\n" );
	int game2 = play_game( top, bottom, BOTTOM );

	printf( "================================\n\n" );

	int top_score = game1 + game2;
	if ( top_score > ( MAX_SCORE / 2 ) )
		printf( "TOP (%s) wins, %d to %d.\n", top.get_name(), top_score, MAX_SCORE - top_score );
	else if ( top_score < ( MAX_SCORE / 2 ) )
		printf( "BOTTOM (%s) wins, %d to %d.\n", bottom.get_name(), MAX_SCORE - top_score, top_score );
	else
		printf( "TIE!\n" );

	return 0;
}
