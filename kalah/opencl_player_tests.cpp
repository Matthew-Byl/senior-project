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
#include <climits>
using namespace std;

#define NUM_ITERATIONS 512

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
	if ( child_offset > 258 )
		return;

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
	cout << "Testing generate_boards..." << endl;

	Board boards[259];
	Board start_board;

	CLKernel generate_boards_test( "generate_boards_test", src );

	CLUnitArgument host_boards( "Board", boards, 259 );
	CLUnitArgument start_boards( "Board", &start_board, 1 );
	vector<CLUnitArgument> args;
	args.push_back( start_boards );
	args.push_back( host_boards );
	generate_boards_test.setGlobalDimensions( 1, 216 );
	generate_boards_test.setLocalDimensions( 1, 216 );

	for ( int i = 0; i < NUM_ITERATIONS; i++ )
	{
		start_board = generate_valid_board();
		generate_boards_test( args );

		check_boards( &start_board, boards, 0 );
		cout << "* " << flush;
	}

	cout << endl << "All tests passed." << endl;
}

int minimax_eval( Board *b )
{
	return b->board[13] - b->board[6];
}

void test_evaluate_boards( string src )
{
	cout << "Testing evaluate_boards..." << endl;

	Board boards[259];

	CLKernel evaluate_boards_test( "evaluate_boards_test", src );
	CLUnitArgument host_boards( "Board", boards, 259 );
	vector<CLUnitArgument> args;
	args.push_back( host_boards );
	evaluate_boards_test.setGlobalDimensions( 1, 216 );
	evaluate_boards_test.setLocalDimensions( 1, 216 );	

	for ( int j = 0; j < NUM_ITERATIONS; j++ )
	{
		for ( int i = 0; i < 259; i++ )
		{
			boards[i] = generate_valid_board();
		}
		
		evaluate_boards_test( args );
		
		for ( int i = 0; i < 259; i++ )
		{
			assert( boards[i].score == minimax_eval( &boards[i] ) );
		}
		cout << "* " << flush;
	}

	cout << endl << "All tests passed." << endl;
}

int check_minimax( Board *opencl_boards, int idx )
{
	if ( idx > 43 )
		return opencl_boards[idx].score;

	int best_score;
	if ( opencl_boards[idx].player_to_move == TOP )
		best_score = INT_MIN;
	else
		best_score = INT_MAX;

	int child_offset = tree_array_first_child( 6, idx );
	for ( int i = 0; i < 6; i++ )
	{
		if ( !opencl_boards[child_offset + i].legal_move )
			continue;

		int score = check_minimax( opencl_boards, child_offset + i );

		if ( ( opencl_boards[idx].player_to_move == TOP && score > best_score )
			 || ( opencl_boards[idx].player_to_move == BOTTOM && score < best_score ) )
		{
			best_score = score;
		}
	}

	assert( opencl_boards[idx].score == best_score );
	return best_score;
}

void test_minimax( string src )
{
	cout << "Testing minimax..." << endl;

	Board boards[259];

	CLKernel minimax_test( "minimax_test", src );
	CLUnitArgument host_boards( "Board", boards, 259 );
	vector<CLUnitArgument> args;
	args.push_back( host_boards );
	minimax_test.setGlobalDimensions( 1, 216 );
	minimax_test.setLocalDimensions( 1, 216 );

	// Generate a random array, put scores in the bottom,
	//  randomly mark some nodes as illegal moves, and go.
	for ( int i = 0; i < 259; i++ )
	{
		boards[i] = generate_valid_board();
		if ( ( rand() % 5 ) == 0 )
			boards[i].legal_move = FALSE;
	}

	if ( !boards[0].legal_move )
		boards[0].legal_move = TRUE;
		

	for ( int i = 43; i < 259; i++ )
		boards[i].score = minimax_eval( &boards[i] );

	minimax_test( args );
	check_minimax( boards, 0 );

	cout << endl << "All tests passed." << endl;
}

int main ( void )
{
	srand( time( NULL ) );

	ifstream t("opencl_player.cl");
	string src((std::istreambuf_iterator<char>(t)),
			   std::istreambuf_iterator<char>());

/*
	Board b;
	for ( int i = 0; i < 10; i++ )
	{
		b = generate_valid_board();
		board_print( &b );
	}
*/
	test_generate_boards( src );
//	test_evaluate_boards( src );
	test_minimax( src );

	return 0;
}
