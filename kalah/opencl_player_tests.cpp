/**
 * Tests to ensure that the components of the OpenCL minimax
 *  player work properly and that its output matches the
 *  sequential version.
 *
 * @author John Kloosterman
 * @date Jan. 6, 2013
 */

extern "C" {
#include "board.h"
#include "tree_array.h"
#include "simple_players.h"
#include "depths.h"
#include "evaluate.h"
}

#include <sys/time.h>

#include "opencl_player.h"

#include <CLKernel.h>
#include <fstream>
#include <iostream>
#include <cassert>
#include <cstdlib>
#include <climits>
#include <ctime>
using namespace std;

// The number of iterations of each test to run.
#define NUM_ITERATIONS 512

/**
 * Determine whether a given board is a valid
 *  randomly generated board. It doesn't have to be
 *  a possible one, just one that isn't unreasonable.
 *
 * @param b
 *  A board.
 */
bool valid_board( Board b )
{
	if ( board_game_over( &b ) ) 
		return FALSE;

	for ( int i = 0; i < 14; i++ )
	{
		if ( b.board[i] < 0 || b.board[i] > 96 )
			return false;
	}

	return true;
}

/**
 * Randomly generate a valid board.
 * 
 * @return
 *  A random board.
 */
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

	} while ( !valid_board( board ) );

	return board;
}

/**
 * Helper method for @c test_generate boards that recursively
 *  determines whether the OpenCL kernel generated a correct
 *  tree of boards based on a start board.
 *
 * @param board
 *  The start board for this subtree.
 * @param opencl_boards
 *  The array of boards generated by the OpenCL player.
 * @param idx
 *  The index in the tree array of the root of this subtree.
 */
void check_boards( Board *board, Board *opencl_boards, int idx )
{
	assert( board_equal( board, &opencl_boards[idx] ) );

	int move_offset;
	if ( board->player_to_move == TOP )
		move_offset = 7;
	else
		move_offset = 0;

	int child_offset = tree_array_first_child( 6, idx );
	// If our child would be deeper than the OpenCL tree is (4 levels)
	//  we are a leaf node.
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

/**
 * Test to determine whether the OpenCL player 
 *  correctly generates a tree of possible boards.
 *
 * @param src
 *  The source code for the OpenCL player.
 */
void test_generate_boards( string src )
{
	cout << "Testing generate_boards..." << endl;

	Board boards[259];
	Board start_board;

	CLKernel generate_boards_test( "generate_boards_test", src );

	CLArgument host_boards( "Board", boards, 259 );
	CLArgument start_boards( "Board", &start_board, 1 );
	vector<CLArgument> args;
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

/**
 * Test to determine whether the OpenCL player correctly
 *  assigns a score to boards.
 *
 * @param src
 *  The source code for the OpenCL player.
 */
void test_evaluate_boards( string src )
{
	cout << "Testing evaluate_boards..." << endl;

	Board boards[259];

	CLKernel evaluate_boards_test( "evaluate_boards_test", src );
	CLArgument host_boards( "Board", boards, 259 );
	vector<CLArgument> args;
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

/**
 * Helper method for test_minimax to determine whether the minimax
 *  implementation in the OpenCL player worked properly.
 *
 * @param opencl_boards
 *  The array of boards returned from the OpenCL player.
 * @param idx
 *  The root of the current minimax subtree in the array.
 */
int check_minimax( Board *opencl_boards, int idx )
{
	// We are a leaf node
	if ( idx >= 43 )
		return opencl_boards[idx].score;

	int best_score;
	if ( opencl_boards[idx].player_to_move == TOP )
		best_score = INT_MIN;
	else
		best_score = INT_MAX;

	int child_offset = tree_array_first_child( 6, idx );
	bool has_legal_move = false;
	for ( int i = 0; i < 6; i++ )
	{
		if ( !opencl_boards[child_offset + i].legal_move )
			continue;

		int score = check_minimax( opencl_boards, child_offset + i );

		if ( ( opencl_boards[idx].player_to_move == TOP && score > best_score )
			 || ( opencl_boards[idx].player_to_move == BOTTOM && score < best_score ) )
		{
			has_legal_move = true;
			best_score = score;
		}
	}

	if ( !has_legal_move )
		return opencl_boards[idx].score;

	assert( opencl_boards[idx].score == best_score );
	return best_score;
}

/**
 * Test to determine whether the minimax implementation in the 
 *  OpenCL player works correctly.
 *
 * @param src
 *  The source code for the OpenCL player.
 */
void test_minimax( string src )
{
	cout << "Testing minimax..." << endl;

	Board boards[259];

	CLKernel minimax_test( "minimax_test", src );
	CLArgument host_boards( "Board", boards, 259 );
	vector<CLArgument> args;
	args.push_back( host_boards );
	minimax_test.setGlobalDimensions( 1, 216 );
	minimax_test.setLocalDimensions( 1, 216 );

	for ( int j = 0; j < NUM_ITERATIONS; j++ )
	{
		// Generate a random array, put scores in the bottom,
		//  and randomly mark some nodes as illegal moves.
		for ( int i = 0; i < 259; i++ )
		{
			boards[i] = generate_valid_board();
			if ( ( rand() % 5 ) == 0 )
				boards[i].legal_move = FALSE;
		}

		if ( !boards[0].legal_move )
			boards[0].legal_move = TRUE;
		
		// The OpenCL code assumes that all the boards are scored, for game-overs.
		for ( int i = 0; i < 259; i++ )
			boards[i].score = minimax_eval( &boards[i] );

		minimax_test( args );
		check_minimax( boards, 0 );

		cout << "* " << flush;
	}

	cout << endl << "All tests passed." << endl;
}

/**
 * Test to determine whether the parallel component of the OpenCL
 *  player returns the same results as the sequential minimax
 *  algorithm.
 *
 * @param src
 *  The source code for the OpenCL player.
 */
void test_combination( string src )
{
	cout << "Testing OpenCL kernel..." << endl;

	Board start_boards[256];

	CLKernel opencl_player( "opencl_player", src );
	CLArgument host_start_boards( "Board", start_boards, 256 );
	vector<CLArgument> args;
	args.push_back( host_start_boards );
	opencl_player.setGlobalDimensions( 256, 216 );
	opencl_player.setLocalDimensions( 1, 216 );

	for ( int i = 0; i < NUM_ITERATIONS; i++ )
	{
		// Generate start boards.
		for ( int j = 0; j < 256; j++ )
			start_boards[j] = generate_valid_board();
		
		// Run kernel
		opencl_player( args );
		
		// Test result
		for ( int j = 0; j < 256; j++ )
		{
			MinimaxResult mr = minimax_move( &start_boards[j], PARALLEL_DEPTH - 1 );

			if ( start_boards[j].score != mr.score )
			{
				cout << "Parallel: " << start_boards[j].score << " Sequential: " << mr.score << endl;
				board_print( &start_boards[j] );
			}
			
			assert( start_boards[j].score == mr.score );
		}

		cout << "* " << flush;
	}

	cout << endl << "All tests passed." << endl;
}

/**
 * Test to determine whether the parallel component and the
 *  bottom-most sequential minimax component of the OpenCL player
 *  give output identical with the sequential minimax algorithm.
 *
 * @param src
 *  The source code for the OpenCL player.
 */
void test_opencl_object( string src )
{
	cout << "Testing entire OpenCL component..." << endl;

	OpenCLPlayer opencl_player( SEQUENTIAL_DEPTH, src );

	for ( int i = 0; i < NUM_ITERATIONS; i++ )
	{
		Board start = generate_valid_board();

		opencl_player.set_board( start );
		MinimaxResult ocl_result = opencl_player.makeMove();
		MinimaxResult seq_result = minimax_move( &start, SEQUENTIAL_DEPTH + PARALLEL_DEPTH - 1 );

		assert( ocl_result.move == seq_result.move );
		assert( ocl_result.score == seq_result.score );

		cout << "* " << flush;
	}

	cout << endl << "All tests passed." << endl;
}

/**
 * Test to determine whether all 3 levels of the OpenCL player
 *  together give the same result as the sequential minimax
 *  algorithm.
 *
 * @param src
 *  The source code for the OpenCL player.
 */
void test_pre_minimax( string src )
{
	cout << "Testing pre-OpenCL component minimax..." << endl;
    OpenCLPlayer opencl_player( SEQUENTIAL_DEPTH, src );
	
	for ( int i = 0; i < NUM_ITERATIONS; i++ )
	{
		Board start = generate_valid_board();

		MinimaxResult p_ocl_result = opencl_player_pre_minimax( opencl_player, start, 0 );
		MinimaxResult seq_result = minimax_move( &start, PRE_DEPTH + SEQUENTIAL_DEPTH + PARALLEL_DEPTH - 1 );

		assert( p_ocl_result.move == seq_result.move );
		assert( p_ocl_result.score == seq_result.score );

		cout << "* " << flush;
	}

	cout << endl << "All tests passed." << endl;
}

/**
 * Test to determine whether being TOP or BOTTOM makes a difference 
 *  for the minimax player's choices.
 *
 * @param src
 *  The source code for the OpenCL player.
 */
void test_top_bottom( string src )
{
	cout << "Testing whether being TOP or BOTTOM makes a difference..." << endl;
	OpenCLPlayer opencl_player( 4, src );

	for ( int i = 0; i < NUM_ITERATIONS; i++ )
	{
		Board start = generate_valid_board();

		MinimaxResult ocl_result = opencl_player_pre_minimax( opencl_player, start, 0 );
		
		// Flip the board around.
		Board flipped;
		board_initialize( &flipped, TOP );
		if ( start.player_to_move == TOP )
			flipped.player_to_move = BOTTOM;
		else
			flipped.player_to_move = TOP;

		for ( int i = 0; i < 7; i++ )
		{
			flipped.board[i] = start.board[i+7];
			flipped.board[i+7] = start.board[i];
		}

		MinimaxResult flipped_result = opencl_player_pre_minimax( opencl_player, flipped, 0 );
		
		if ( start.player_to_move == TOP )
			assert( ocl_result.move == ( flipped_result.move + 7 ) );
		else
			assert( ( ocl_result.move + 7 ) == flipped_result.move );

		assert( ocl_result.score == -flipped_result.score );
		cout << "* " << flush;
	}

	cout << endl << "All tests passed." << endl;
}

void speed_test( string src )
{
	KalahPlayer cl_player = minimax_player();

	Board start;
	board_initialize( &start, TOP );
	struct timeval tv_start;
	struct timeval tv_end;
	
	for ( int i = 0; i < 5; i++ )
	{
		gettimeofday( &tv_start, NULL );
		cl_player.make_move( &start );
		gettimeofday( &tv_end, NULL );
		
		int diff_sec = tv_end.tv_sec - tv_start.tv_sec;
		int diff_usec = tv_end.tv_usec - tv_start.tv_usec;

		double diff = diff_sec + ( 0.000001 * diff_usec );

		cout << i << ": " << diff << endl;
	}
}

int main ( void )
{
	// Seed the random number generator
	srand( (unsigned) time( NULL ) );

	// Read the source code for the OpenCL player from the file.
	ifstream t("opencl_player.cl");
	string src((std::istreambuf_iterator<char>(t)),
			   std::istreambuf_iterator<char>());

	test_generate_boards( src );
	test_evaluate_boards( src );
	test_minimax( src );
	test_combination( src );
	test_opencl_object( src );
	test_pre_minimax( src );
	test_top_bottom( src );
//	speed_test( src );

	return 0;
}
