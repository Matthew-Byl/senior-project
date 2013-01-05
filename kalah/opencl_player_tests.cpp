/* Test the OpenCL player and its kernels. */
extern "C" {
#include "board.h"
#include "tree_array.h"
}

#include <CLKernel.h>
#include <fstream>
#include <iostream>
#include <cassert>
#include <cstdlib>
using namespace std;

Board generate_valid_board()
{
	Board board;

	board.score = -1;
	board.legal_move = TRUE;

	do {
		if ( rand() % 2 )
			board.player_to_move = TOP;
		else
			board.player_to_move = BOTTOM;
		
		int stones_left = 12 * 4;

		// This isn't uniform, but works.
		for ( int i = 0; i < 14; i++ )
		{
			if ( stones_left < 8 )
				break;

			int st = rand() % 8;
			stones_left -= st;

			board.board[i] = st;
		}

//		board.board[13] += stones_left;

	} while ( board_game_over( &board ) );

	return board;
}

void check_boards( Board *board, Board *opencl_boards, int idx )
{
	assert( board_equal( board, &opencl_boards[idx] ) );

	int move_offset;
	if ( board->player_to_move == TOP )
		move_offset = 7;
	else
		move_offset = 0;

	int child_offset = tree_array_first_child( 6, idx );
	for ( int i = 0; i < 6; i++ )
	{
		if ( board_legal_move( board, i + move_offset ) )
		{
			Board m_board = *board;
			board_make_move( &m_board, i + move_offset );
			check_boards( &m_board, opencl_boards, child_offset + i );
		}
	}
}

void test_generate_boards( string src )
{
	Board boards[259];
	Board start_board = generate_valid_board();

	CLKernel generate_boards_test( "generate_boards_test", src );

	CLUnitArgument host_boards( "Board", boards, 259 );
	CLUnitArgument start_boards( "Board", start_board );
	vector<CLUnitArgument> args;
	args.push_back( start_boards );
	args.push_back( host_boards );

	generate_boards_test.setGlobalDimensions( 1, 216 );
	generate_boards_test.setLocalDimensions( 1, 216 );
	generate_boards_test( args );

	check_boards( &start_board, boards, 0 );

	for ( int i = 0; i < 259; i++ )
	{
		board_print( &boards[i] );
	}
}

int main ( void )
{
	srand( time( NULL ) );

	ifstream t("opencl_player.cl");
	string src((std::istreambuf_iterator<char>(t)),
			   std::istreambuf_iterator<char>());

	Board b;
	for ( int i = 0; i < 10; i++ )
	{
		b = generate_valid_board();
		board_print( &b );
	}

	test_generate_boards( src );


	return 0;
}
