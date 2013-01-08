/**
 * OpenCL minimax player.
 * 
 * Minimax trees have the property that every subtree of a 
 *  minimax tree is itself a minimax tree. We use that to
 *  split up the computation into 3 stages, whose depths are
 *  defined in depths.h:
 *
 * (1) PRE_DEPTH levels of sequential minimax on the CPU, whose 
 *     leaf nodes are the level (2) trees.
 * (2) SEQUENTIAL_DEPTH levels of sequential minimax on the CPU.
 *     This is done in 3 stages:
 *       a) All the leaf nodes of this tree for which the game is
 *          not over are put into an array.
 *       b) The parallel minimax implementation, level (3) is called
 *          on each of those leaf nodes.
 *       c) Minimax is run on the leaf nodes.
 *     The leaf nodes have to remain in memory in order to be passed
 *      off to the parallel level, which limits how deep this level
 *      can search. If it's too small, the GPU is under-utilized, and if
 *      it's too big, we use and transfer around too much memory.
 * (3) PARALLEL_DEPTH levels of minimax are run on the GPU.
 * 
 * @author John Kloosterman
 * @date Dec. 26, 2012 - Jan. 5, 2013
 */

#include "opencl_player.h"

extern "C" {
#include "board.h"
#include "tree_array.h"
#include "simple_players.h"
#include "evaluate.h"
}

#include "depths.h"

#include <iostream>
#include <cmath>
#include <climits>
#include <fstream>
#include <string>
#include <list>
using namespace std;

/**
 * Set the board that the OpenCLPlayer should evaluate
 *  when makeMove() is called.
 */
void OpenCLPlayer::set_board( Board b )
{
	myStartBoard = b;
}

/**
 * Run levels (2) and (3) of minimax on the current board.
 */
MinimaxResult OpenCLPlayer::makeMove()
{
	// Handle the case when the game ends at our root.
	if ( board_game_over( &myStartBoard ) )
	{
		MinimaxResult mr;

		mr.move = -1;
		mr.score = minimax_eval( &myStartBoard );

		return mr;
	}

	// Create the start boards
	generate_start_boards();

	// C++ guarantees vector elements are stored contiguously.
	CLUnitArgument start_boards( 
		"Board", 
		&myStartBoards[0],
		myStartBoards.size(), 
		false, true );
	start_boards.makePersistent( myContext );
	int num_leaf_nodes = myStartBoards.size();

	if ( num_leaf_nodes )
	{
		int offset = 0;
		int items;
		int iterations = 0;

		// We have to batch the work to the GPU, because if one batch
		//  takes too long, the system gets unreponsive, and after a 
		//  certain point, the watchdog timer goes off and the system
		//  locks up.
		do {
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
	}

	// Run minimax on the start boards.
	minimax_idx = 0;
	MinimaxResult move = run_minimax( myStartBoard, mySequentialDepth );
	assert( board_legal_move( &myStartBoard, move.move ) );

	return move;
}

/**
 * Recursively find the leaf nodes of the level (2) minimax tree,
 *  and put then in the myStartBoards vector.
 */
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

/**
 * Generate all the level (2) leaf nodes for the current
 *  board.
 */
void OpenCLPlayer::generate_start_boards()
{
	myStartBoards.clear();
	generate_board( myStartBoard, mySequentialDepth );
};

/**
 * Run minimax on the level (2) leaf nodes in myStartBoards,
 *  which have had their scores populated by the level (3)
 *  GPU component.
 */
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

/**
 * Recursively run level (1) minimax on a board.
 *
 * @param player
 *  The OpenCLPlayer to use to evaluate leaf nodes.
 * @param b
 *  The parent of the minimax subtree
 * @param depth
 *  The current minimax tree depth, which increases
 *   as we go down levels.
 */
MinimaxResult opencl_player_pre_minimax( OpenCLPlayer &player, Board &b, int depth )
{
	MinimaxResult ret;

	// Use the OpenCLPlayer object to go deeper.
	if ( depth == PRE_DEPTH )
	{
		player.set_board( b );
		ret = player.makeMove();
		return ret;
	}
	else if ( board_game_over( &b ) )
	{
		ret.score = minimax_eval( &b );
		ret.move = -1;

		return ret;
	}

	MinimaxResult rec_result;
	MinimaxResult best_result;
	best_result.move = -1;

	if ( b.player_to_move == TOP )
	{
		// MAX
		best_result.score = INT_MIN;

		for ( int i = 7; i < 13; i++ )
		{
			if ( board_legal_move( &b, i ) )
			{
				Board moved_board;
				moved_board = b;
				board_make_move( &moved_board, i );

				rec_result = opencl_player_pre_minimax( player, moved_board, depth + 1 );

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
			if ( board_legal_move( &b, i ) )
			{
				Board moved_board;
				moved_board = b;
				board_make_move( &moved_board, i );

				rec_result = opencl_player_pre_minimax( player, moved_board, depth + 1 );

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

/**
 * Bindings from the OpenCL player object to the
 *  C world.
 */

// Instantiating an OpenCLPlayer involves recompiling
//  the kernel, which is slow, so instiantiate only one.
ifstream t("opencl_player.cl");
string src((std::istreambuf_iterator<char>(t)),
               std::istreambuf_iterator<char>());
OpenCLPlayer player( SEQUENTIAL_DEPTH, src );

extern "C" const char *opencl_player_name( void )
{
	return "OpenCL Minimax";
}

extern "C" int opencl_player_move( Board *b )
{	
	MinimaxResult ret = opencl_player_pre_minimax( player, *b, 0 );

	return ret.move;
}

KalahPlayer opencl_minimax_player( void )
{
	KalahPlayer k;

	k.get_name = opencl_player_name;
	k.make_move = opencl_player_move;

	return k;
}
