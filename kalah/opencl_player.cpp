extern "C" {
#include "board.h"
#include "opencl_player.h"
#include "tree_array.h"
#include "simple_players.h"
}

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

class OpenCLPlayer
{
public:
	OpenCLPlayer( Board start, int sequentialDepth, string src )
		: mySequentialDepth( sequentialDepth ),
		  myBoardsSize( get_board_array_size( sequentialDepth ) ),
		  myStartBoard( start ),
//		  myBoards( new Board[myBoardsSize] ),
		  myStartBoards( new Board[ get_leaf_nodes( mySequentialDepth ) ] ),
		  generate_boards( "generate_boards", src ),
		  evaluate_board( "evaluate_board", src ),
		  minimax( "minimax", src ),
		  get_results( "get_results", src ),
		  host_boards( "Board", (Board *) nullptr, myBoardsSize, false, false ),
		  start_boards( "Board", myStartBoards, get_leaf_nodes( mySequentialDepth ) )
		{

		};

	~OpenCLPlayer()
		{
//			delete[] myBoards;
		};

	int leaf_start();
	int get_leaf_nodes( int sequentialDepth );
	int get_board_array_size( int sequentialDepth );

	void generate_board( Board parent, int idx, int depth );
	MinimaxResult run_minimax( Board parent, int idx, int depth );
	void generate_start_boards();
	int makeMove();
	int minimax_eval( Board b );

private:
	int mySequentialDepth;
	int myBoardsSize;
	Board myStartBoard;
//	Board *myBoards;
	Board *myStartBoards;

	CLKernel generate_boards;
	CLKernel evaluate_board;
	CLKernel minimax;
	CLKernel get_results;

	CLUnitArgument host_boards;
	CLUnitArgument start_boards;
};

int OpenCLPlayer::get_leaf_nodes( int sequentialDepth )
{
	// The number of leaf nodes of the given sequential depth.
	return ipow( 6, sequentialDepth );
}

int OpenCLPlayer::get_board_array_size( int sequentialDepth )
{
	int leaf = get_leaf_nodes( sequentialDepth );

	return ( leaf * 6 ) + ( leaf * 6 * 6 ) + ( leaf * 6 * 6 * 6 );
}

int OpenCLPlayer::leaf_start()
{
	int half_6ary_tree_size = ( ( (int) pow( 6.0f, mySequentialDepth + 1 ) - 1 ) / ( 6 - 1 ) );
	return half_6ary_tree_size - 1;
}

void OpenCLPlayer::generate_board( Board parent, int idx, int depth )
{
	if ( depth == 0 )
	{
		myStartBoards[idx - leaf_start()] = parent;
		return;
	}
	
	int child_idx = 6 * ( idx + 1 );
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

	// 42 -> 216 to do more levels.
	generate_boards.setGlobalDimensions( num_leaf_nodes, 36 );
	generate_boards.setLocalDimensions( 1, 36 ); // this needs to stay with x-dimension 1
	generate_boards( start_boards, host_boards );
			
	evaluate_board.setGlobalDimensions( num_leaf_nodes, 36, 14 );
	evaluate_board( host_boards );
			
	minimax.setGlobalDimensions( num_leaf_nodes, 6 ); // 6 ^ depth - 1
	minimax.setLocalDimensions( 1, 6 ); // this needs to stay with x-dimension 1.
	minimax( host_boards );

	get_results.setGlobalDimensions( num_leaf_nodes );
	get_results( start_boards, host_boards );

/*			
	for ( int i = 0; i < 41; i++ )
	{
		board_print( &myBoards[i] );
		printf( "\n" );
	}
*/

//	for ( int i = 7; i < 41; i++ )
//	{
//		assert( myBoards[i].score == minimax_eval( myBoards[i] ) );
//	}

//	return -1;

/*
  for ( int i = 0; i < 6; i++ )
  {
  cout << "Minimax picked " << myStartBoards[i].score << " for " << endl;
  if ( myStartBoards[i].player_to_move == TOP )
  cout << "MAX";
  else
  cout << "MIN";
  cout << " node:" << endl;

  for ( int j = 0; j < 6; j++ )
  {
  cout << myBoards[42*i + j].score << " ";
  }
  cout << endl;
  }
*/

/*
  cout << "Board scores"
  for ( int i = 0; i < 258; i++ )
  {
  printf( "*** %d ***\n" , i );
  board_print( &myBoards[i] );
  printf( "\n" );
  }
*/

/*
  cout << "Scores for leaf nodes: " << endl;
  for ( int i = 0; i < num_leaf_nodes; i++ )
  printf( "%d ", myStartBoards[i].score );
  printf( "\n" );
*/

	// Run minimax on the start boards.
	MinimaxResult move = run_minimax( myStartBoard, 0, mySequentialDepth );

	// Hack to return something legal when it's our move, even when
	//  minimax doesn't make it to the GPU.
/*
	if ( !board_legal_move( &myStartBoard, move.move ) )
	{
		cout << "Being Bonzo because our move, " << move.move << " makes no sense." << endl;

		int move_offset;
		if ( myStartBoard.player_to_move == TOP )
			move_offset = 7;
		else
			move_offset = 0;

		for( int i = move_offset; i < 6 + move_offset; i++ )
		{
			if ( myStartBoard.board[i] != 0 )
				return i;
		}
	}
*/

	assert( board_legal_move( &myStartBoard, move.move ) );

	return move.move;
}

MinimaxResult OpenCLPlayer::run_minimax( Board parent, int idx, int depth )
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

extern "C" int opencl_player_move( Board *b )
{
	ifstream t("opencl_player.cl");
    string src((std::istreambuf_iterator<char>(t)),
               std::istreambuf_iterator<char>());

	// This is a recompile every move. Fix it later.
	OpenCLPlayer player( *b, 1, src );
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

int main ( void )
{
	KalahPlayer minimax = minimax_player();

	ifstream t("opencl_player.cl");
    string src((std::istreambuf_iterator<char>(t)),
               std::istreambuf_iterator<char>());

	for ( int i = 7; i < 12; i++ )
	{
		Board b;
		board_initialize( &b, TOP );
		board_make_move( &b, i );

		OpenCLPlayer player( b, 1, src );
		cout << "OpenCL Move: " << player.makeMove() << endl;
		cout << "Minimax Move: " << minimax.make_move( &b ) << endl;
	}
		
/*
	OpenCLPlayerTester tester;
	tester.runTests();
*/
}
#endif
