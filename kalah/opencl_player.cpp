#include "opencl_player.h"

extern "C" {
#include "board.h"
#include "tree_array.h"
#include "simple_players.h"
#include "evaluate.h"
}

#include "depths.h"

/*
 * Things to fix:
 *  -when there isn't a legal move before we get to the OpenCL part.
 */

// #include <CLKernel.h>

#include <iostream>
#include <cmath>
#include <climits>
#include <fstream>
#include <string>
#include <list>
using namespace std;

void easy_break ( void )
{
	return;
}


extern "C" const char *opencl_player_name( void )
{
	return "OpenCL Minimax";
}

int ipow( int n, int k )
{
	return (int) pow( n, k );
}

void OpenCLPlayer::set_board( Board b )
{
	myStartBoard = b;
}

int OpenCLPlayer::get_leaf_nodes( int sequentialDepth )
{
	// The number of leaf nodes of the given sequential depth.
	return ipow( 6, sequentialDepth );
}

int OpenCLPlayer::get_board_array_size( int sequentialDepth )
{
	int leaf = get_leaf_nodes( sequentialDepth );

	return leaf * tree_array_size( 6, PARALLEL_DEPTH );
}

int OpenCLPlayer::leaf_start()
{
	return tree_array_size( 6, mySequentialDepth - 1 );
}

void OpenCLPlayer::generate_board( Board parent, int depth )
{
	if ( depth == 0 )
	{
		myStartBoards.push_back( parent );
		return;
	}
	else if ( board_game_over( &parent ) )
	{ 
		return;
	}
	
	for ( int i = 0; i < 6; i++ )
	{
		int move_offset = 0;
		Board m_board = parent;
		
		if ( m_board.player_to_move == TOP )
			move_offset = 7;
		
		if ( board_legal_move( &m_board, i + move_offset ) )
		{
			board_make_move( &m_board, i + move_offset );
			generate_board( m_board, depth - 1 );
		}
	}
}

void OpenCLPlayer::generate_start_boards()
{
	myStartBoards.clear();
	generate_board( myStartBoard, mySequentialDepth );
};

MinimaxResult OpenCLPlayer::makeMove()
{
	// @todo: we might exceed a dimension (like when n=7), so run kernels multiple times with a global offset.

//	int num_leaf_nodes = get_leaf_nodes( mySequentialDepth );
//			cout << "Leaf nodes: " << num_leaf_nodes << endl;

	if ( board_game_over( &myStartBoard ) )
	{
		MinimaxResult mr;

		mr.move = -1;
		mr.score = minimax_eval( &myStartBoard );

		return mr;
	}

	// Create start boards
	generate_start_boards();

	// C++ guarantees vector elements are stored contiguously.
	CLUnitArgument start_boards( "Board", &myStartBoards[0], myStartBoards.size(), false, true );
	start_boards.makePersistent( myContext );
	int num_leaf_nodes = myStartBoards.size();

/*
	cout << "LEAF NODES: " << endl;
	for ( int i = 0; i < num_leaf_nodes; i++ )
	{
		board_print( &myStartBoards[i] );
		cout << endl;
	}
	cout << "================" << endl;
*/
	if ( num_leaf_nodes )
	{
		// Assume the maximum local dimension is 512.
		int offset = 0;
		int items;
		int iterations = 0;
//	int evaled = 0;
		do {
//			cout << "Running iteration " << iterations << ", offset " << offset << endl;
			
			if ( ( num_leaf_nodes - offset ) < WORKGROUP_SIZE )
				items = num_leaf_nodes - offset;
			else
				items = WORKGROUP_SIZE;
			
			vector<CLUnitArgument> args;
			args.push_back( start_boards );
			opencl_player.setGlobalDimensions( items, 216 );
			opencl_player.setGlobalOffset( offset, 0 );
			opencl_player.setLocalDimensions( 1, 216 );
			opencl_player( args );
			
			offset += WORKGROUP_SIZE;
			iterations++;
		} while ( offset < num_leaf_nodes );
		
//	cout << "Number of boards to evaluate: " << evaled << endl;
//		cout << "Did " << iterations << " iterations." << endl;
	}

	// Copy back from device.
//	start_boards.copyFromDevice( myContext.getCommandQueue() );

	// Run minimax on the start boards.
	minimax_idx = 0;
	MinimaxResult move = run_minimax( myStartBoard, mySequentialDepth );

	assert( board_legal_move( &myStartBoard, move.move ) );
	// Test our move against the minimax player.

	// To make it an independent player, return move.move.
	return move;
}

MinimaxResult OpenCLPlayer::run_minimax( Board &parent, int depth )
{
	MinimaxResult best_result;
	best_result.move = -1;

	if ( depth == 0 )
	{
		MinimaxResult mr;

		// Make sure we are reading everything in the same order.
		assert( board_equal( &myStartBoards[minimax_idx], &parent ) );

		mr.score = myStartBoards[minimax_idx].score;
		minimax_idx++;

		mr.move = -1;

		return mr;
	}
	else if ( board_game_over( &parent ) )
	{
		MinimaxResult mr;

		mr.score = minimax_eval( &parent );
		mr.move = -1;

		return mr;
	}

	int move_offset;
	if ( parent.player_to_move == TOP )
	{
		move_offset = 7;
		best_result.score = INT_MIN;
	}
	else
	{
		move_offset = 0;
		best_result.score = INT_MAX;
	}

	MinimaxResult rec_result;
	for ( int i = 0; i < 6; i++ )
	{
		if ( board_legal_move( &parent, i + move_offset ) )
		{
			Board moved_board = parent;

			board_make_move( &moved_board, i + move_offset );
			rec_result = run_minimax( moved_board, depth - 1 );
					
			if ( ( parent.player_to_move == TOP && rec_result.score > best_result.score )
				 || ( parent.player_to_move == BOTTOM && rec_result.score < best_result.score ) )
			{
				best_result.move = i + move_offset;
				best_result.score = rec_result.score;
			}
		}
	}
			
	return best_result;
}

// depth will increase.
MinimaxResult opencl_player_pre_minimax( OpenCLPlayer *player, Board *b, int depth )
{
	MinimaxResult ret;

	if ( depth == PRE_DEPTH )
	{
		player->set_board( *b );
		ret = player->makeMove();
		return ret;
	}
	else if ( board_game_over( b ) )
	{
		// I think there is a compiler bug here. We have to dereference b or else
		//  strange things happen. We have to modify bd to avoid a warning.
		Board bd = *b;
		bd.score = 0;

		ret.score = minimax_eval( b );
		ret.move = -1;

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
				moved_board = *b;
				board_make_move( &moved_board, i );

				rec_result = opencl_player_pre_minimax( player, &moved_board, depth + 1 );

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

				rec_result = opencl_player_pre_minimax( player, &moved_board, depth + 1 );

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

extern "C" int opencl_player_move( Board *b )
{
	ifstream t("opencl_player.cl");
    string src((std::istreambuf_iterator<char>(t)),
               std::istreambuf_iterator<char>());

	OpenCLPlayer player( SEQUENTIAL_DEPTH, src );
	
	MinimaxResult ret = opencl_player_pre_minimax( &player, b, 0 );

	return ret.move;
}

KalahPlayer opencl_minimax_player( void )
{
	KalahPlayer k;

	k.get_name = opencl_player_name;
	k.make_move = opencl_player_move;

	return k;
}

#ifdef MAKE_MAIN

void easy_breakpoint()
{
	return;
}

#include <time.h>
clock_t startm, stopm;
#define START if ( (startm = clock()) == -1) {printf("Error calling clock");exit(1);}
#define STOP if ( (stopm = clock()) == -1) {printf("Error calling clock");exit(1);}
#define PRINTTIME printf( "%6.3f seconds used by the processor.", ((double)stopm-startm)/CLOCKS_PER_SEC);

#include <google/profiler.h>

int main ( void )
{
	KalahPlayer minimax = minimax_player();
	KalahPlayer opencl_minimax = opencl_minimax_player();

//	ProfilerStart( "opencl_player.perf" );
	for ( int i = 7; i < 12; i++ )
	{
		for ( int j = 0; j < 6; j++ )
		{
			Board b;
			board_initialize( &b, TOP );
			board_make_move( &b, i );

			// Go-agains
			if ( b.player_to_move == TOP )
				continue;
			
			board_make_move( &b, j );

			START;
			cout << "OpenCL: " << endl;

			int ocl_move = opencl_minimax.make_move( &b );

			STOP;
			PRINTTIME;
			cout << endl;

			START;
			cout << "Minimax: " << endl;
			int min_move = minimax.make_move( &b );
			STOP;
			PRINTTIME;
			cout << endl;

//			cerr << "OpenCL Move: " << ocl_move << endl;
//			cerr << "Minimax Move: " << min_move << endl;

			if ( ocl_move != min_move )
			{
			cerr << "OpenCL Move: " << ocl_move << endl;
			cerr << "Minimax Move: " << min_move << endl;

		   			}
		}
	}

//			ProfilerStop();
		
/*
	OpenCLPlayerTester tester;
	tester.runTests();
*/
}
#endif
