extern "C" {
#include "board.h"
#include "opencl_player.h"
#include "tree_array.h"
#include "simple_players.h"
}

#define SEQUENTIAL_DEPTH 3
#define PARALLEL_DEPTH 3 // This has to match the value MINIMAX_DEPTH in the kernel. @TODO: consistency
// Actual tree depth:
//  MINIMAX_DEPTH = SEQUENTIAL_DEPTH + PARALLEL_DEPTH - 1

/*
 * Things to fix:
 *  -when there isn't a legal move before we get to the OpenCL part.
 */

#include <CLKernel.h>

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

typedef struct {
	int move;
	int score;
} MinimaxResult;

#define _WG_SIZE 65536

class OpenCLPlayer
{
public:
	OpenCLPlayer( int sequentialDepth, string src )
		: mySequentialDepth( sequentialDepth ),
		  myBoardsSize( _WG_SIZE * tree_array_size( 6, PARALLEL_DEPTH ) ),
		  myBoards( new Board[myBoardsSize] ), // @TODO: this dependent on the work size below.
		  myStartBoards( new Board[ get_leaf_nodes( mySequentialDepth ) ] ),
		  myEvaluateBoards( new int[ myBoardsSize + get_leaf_nodes( mySequentialDepth ) ] ),
		  zero_evaluate_board( "zero_evaluate_board", src, myContext ),
		  generate_boards( "generate_boards", src, myContext ),
		  evaluate_board( "evaluate_board", src, myContext ),
		  minimax( "minimax", src, myContext ),
		  get_results( "get_results", src, myContext ),
		  host_boards( "Board", myBoards, myBoardsSize, false, false ),
		  start_boards( "Board", myStartBoards, get_leaf_nodes( mySequentialDepth ) ),
		  evaluate_boards( "int", myEvaluateBoards, myBoardsSize + get_leaf_nodes( mySequentialDepth ), false, true )
		{
			host_boards.makePersistent( myContext );
			evaluate_boards.makePersistent( myContext );
		};

	~OpenCLPlayer()
		{
			delete[] myBoards;
		};

	int leaf_start();
	int get_leaf_nodes( int sequentialDepth );
	int get_board_array_size( int sequentialDepth );

	void generate_board( Board parent, int idx, int depth );
	MinimaxResult run_minimax( Board &parent, int idx, int depth );
	void generate_start_boards();
	int makeMove();
	int minimax_eval( Board b );

	void set_board( Board b );

private:
	CLContext myContext;
	int mySequentialDepth;
	int myBoardsSize;
	Board myStartBoard;
	Board *myBoards;
	Board *myStartBoards;
	cl_int *myEvaluateBoards;

	CLKernel zero_evaluate_board;
	CLKernel generate_boards;
	CLKernel evaluate_board;
	CLKernel minimax;
	CLKernel get_results;

	CLUnitArgument host_boards;
	CLUnitArgument start_boards;
	CLUnitArgument evaluate_boards;
};

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

void OpenCLPlayer::generate_board( Board parent, int idx, int depth )
{
	if ( depth == 0 )
	{
		myStartBoards[idx - leaf_start()] = parent;
		return;
	}
	
	int child_idx = tree_array_first_child( 6, idx );
	for ( int i = 0; i < 6; i++ )
	{
		int move_offset = 0;
		Board m_board = parent;
		
		if ( m_board.player_to_move == TOP )
			move_offset = 7;
		
		if ( board_legal_move( &m_board, i + move_offset ) )
		{
			board_make_move( &m_board, i + move_offset );
		}
		else
		{
			m_board.legal_move = FALSE;
		}
		
		generate_board( m_board, child_idx + i, depth - 1 );
	}
}

void OpenCLPlayer::generate_start_boards()
{
	generate_board( myStartBoard, 0, mySequentialDepth );
};

int OpenCLPlayer::minimax_eval( Board b )
{
    int score = 5 * ( b.board[13] - b.board[6] );

    for ( int i = 0; i <= 5; i++ )
        score -= b.board[i];

    for ( int i = 7; i <= 12; i++ )
		score += b.board[i];

    return score;
}

int OpenCLPlayer::makeMove()
{
	// @todo: we might exceed a dimension (like when n=7), so run kernels multiple times with a global offset.

	int num_leaf_nodes = get_leaf_nodes( mySequentialDepth );
//			cout << "Leaf nodes: " << num_leaf_nodes << endl;

	// Create start boards
	generate_start_boards();

/*
	cout << "LEAF NODES: " << endl;
	for ( int i = 0; i < num_leaf_nodes; i++ )
	{
		board_print( &myStartBoards[i] );
		cout << endl;
	}
	cout << "================" << endl;
*/

	// Assume the maximum local dimension is 512.
	const int WORKGROUP_SIZE = _WG_SIZE;
	int offset = 0;
	int items;
	int iterations = 0;
	int evaled = 0;
	do {
		if ( ( num_leaf_nodes - offset ) < WORKGROUP_SIZE )
			items = num_leaf_nodes - offset;
		else
			items = WORKGROUP_SIZE;

		// Write a zero where we need one.
		zero_evaluate_board.setGlobalDimensions( 1 );
		vector<CLUnitArgument> zero_evaluate_board_args;
		zero_evaluate_board_args.push_back( evaluate_boards );
		zero_evaluate_board( zero_evaluate_board_args );

		generate_boards.setGlobalDimensions( items, ipow( 6, PARALLEL_DEPTH ) );
		generate_boards.setGlobalOffset( offset, 0 );
		generate_boards.setLocalDimensions( 1, ipow( 6, PARALLEL_DEPTH ) ); // this needs to stay with x-dimension 1
		
		vector<CLUnitArgument> generate_boards_args;
		generate_boards_args.push_back( start_boards );
		generate_boards_args.push_back( evaluate_boards );
		generate_boards_args.push_back( host_boards );
		generate_boards( generate_boards_args );

		evaled += myEvaluateBoards[0];
		for ( int i = 0; i < 100; i++ )
			cout << myEvaluateBoards[i] << " ";
		cout << endl;
		
		vector<CLUnitArgument> evaluate_boards_args;
		evaluate_boards_args.push_back( evaluate_boards );
		evaluate_boards_args.push_back( host_boards );
		// More selectively evaluate boards.
		evaluate_board.setGlobalDimensions( myEvaluateBoards[0], 14 );
		evaluate_board( evaluate_boards_args );
		
		minimax.setGlobalDimensions( items, ipow( 6, PARALLEL_DEPTH - 1 ) ); // 6 ^ depth - 1;
		minimax.setLocalDimensions( 1, ipow( 6, PARALLEL_DEPTH - 1 ) ); // this needs to stay with x-dimension 1.
		vector<CLUnitArgument> minimax_args;
		minimax_args.push_back( host_boards );
		minimax( minimax_args );
		
		get_results.setGlobalDimensions( items );
		get_results.setGlobalOffset( offset );
		vector<CLUnitArgument> get_results_args;
		get_results_args.push_back( start_boards );
		get_results_args.push_back( host_boards );
		get_results( get_results_args );

		offset += WORKGROUP_SIZE;
		iterations++;
	} while ( offset < num_leaf_nodes );

	cout << "Number of boards to evaluate: " << evaled << endl;
	cout << "Did " << iterations << " iterations." << endl;

	// Run minimax on the start boards.
	MinimaxResult move = run_minimax( myStartBoard, 0, mySequentialDepth );

	assert( board_legal_move( &myStartBoard, move.move ) );

	return move.move;
}

MinimaxResult OpenCLPlayer::run_minimax( Board &parent, int idx, int depth )
{
	MinimaxResult best_result;
	best_result.move = -1;

	if ( depth == 0 )
	{
		MinimaxResult mr;

		mr.score = myStartBoards[idx - leaf_start()].score;
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

	int child_idx = tree_array_first_child( 6, idx );
	MinimaxResult rec_result;
	bool has_legal_move = false;
	for ( int i = 0; i < 6; i++ )
	{
		if ( board_legal_move( &parent, i + move_offset ) )
		{
			has_legal_move = true;
			Board moved_board = parent;

			board_make_move( &moved_board, i + move_offset );
					
			rec_result = run_minimax( moved_board, child_idx + i, depth - 1 );
					
			if ( ( parent.player_to_move == TOP && rec_result.score > best_result.score )
				 || ( parent.player_to_move == BOTTOM && rec_result.score < best_result.score ) )
			{
				best_result.move = i + move_offset;
				best_result.score = rec_result.score;
			}
		}
	}
			
	if ( !has_legal_move )
	{
		best_result.move = -1;
		best_result.score = minimax_eval( parent );
	}

	return best_result;
}

ifstream t("opencl_player.cl");
string src((std::istreambuf_iterator<char>(t)),
		   std::istreambuf_iterator<char>());
OpenCLPlayer player( SEQUENTIAL_DEPTH, src );

extern "C" int opencl_player_move( Board *b )
{
	ifstream t("opencl_player.cl");
    string src((std::istreambuf_iterator<char>(t)),
               std::istreambuf_iterator<char>());

	player.set_board( *b );
	return player.makeMove();
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
			
			player.set_board( b );

			START;
			cout << "OpenCL: " << endl;

			int ocl_move = player.makeMove();

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
