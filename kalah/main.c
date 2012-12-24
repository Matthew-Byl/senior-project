#include "board.h"
#include "kalah_player.h"

#include <stdio.h>

int play_game( KalahPlayer top, KalahPlayer bottom, PlayerPosition first_player )
{
	Board b_data;
	Board *b = &b_data;  // as shorthand

	board_initialize( b, first_player );
	
	if ( first_player == TOP )
		printf( "Player %s begins.\n", top.get_name() );
	else
		printf( "Player %s begins.\n", bottom.get_name() );

	board_print( b );

	while ( !board_game_over( b ) )
	{
		int move;
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
	KalahPlayer top;
	KalahPlayer bottom;

	printf( "============ Game 1 ============\n" );
	play_game( top, bottom, TOP );

	printf( "============ Game 2 ============\n" );
	play_game( top, bottom, BOTTOM );

	return 0;
}
