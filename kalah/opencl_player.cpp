extern "C" {
#include "board.h"
#include "opencl_player.h"
}

#include <CLKernel.h>

#include <cmath>
#include <fstream>
#include <string>
using namespace std;

extern "C" const char *opencl_player_name( void )
{
	return "OpenCL Minimax";
}

int ipow( int n, int k )
{
	return (int) pow( n, k );
}

int get_leaf_nodes( int sequentialDepth )
{
	// The number of leaf nodes of the given sequential depth.
	return ipow( 6, sequentialDepth );
}

int get_board_array_size( int sequentialDepth )
{
	int leaf = get_leaf_nodes( sequentialDepth );

	return ( leaf * 6 ) + ( leaf * 6 * 6 ) + ( leaf * 6 * 6 * 6 );
}

class OpenCLPlayer
{
public:
	OpenCLPlayer( Board start, int sequentialDepth, string src )
		: mySequentialDepth( sequentialDepth ),
		  myBoardsSize( get_board_array_size( sequentialDepth ) ),
		  myStartBoard( start ),
		  myBoards( new Board[myBoardsSize] ),
		  myStartBoards( new Board[ get_leaf_nodes( mySequentialDepth ) ] ),
		  generate_boards( "generate_boards", src ),
		  evaluate_board( "evaluate_board", src ),
		  minimax( "minimax", src ),
		  host_boards( "Board", myBoards, myBoardsSize ),
		  start_boards( "Board", myStartBoards, get_leaf_nodes( mySequentialDepth ) )
		{

		};

	~OpenCLPlayer()
		{
			delete[] myBoards;
		};

	int leaf_start ( void )
		{
			int half_6ary_tree_size = ( ( (int) pow( 6.0f, mySequentialDepth + 1 ) - 1 ) / ( 6 - 1 ) );
			return half_6ary_tree_size - 1;
		}

	void generate_board( Board parent, int idx, int depth )
		{
			if ( depth == 0 )
			{
				printf( "Leaf start: %d\n", leaf_start() );

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

				generate_board( m_board, child_idx + i, depth - 1 );
			}
		}

	void generate_start_boards()
		{
			generate_board( myStartBoard, 0, mySequentialDepth );
		};

	int makeMove()
		{
			int num_leaf_nodes = get_leaf_nodes( mySequentialDepth );

			// Create start boards
			generate_start_boards();

			generate_boards.setGlobalDimensions( 1, 216 );
			generate_boards.setLocalDimensions( 1, 216 ); // this needs to stay with x-dimension 1
			generate_boards( start_boards, host_boards );
			
			evaluate_board.setGlobalDimensions( 1, 216, 14 );
			evaluate_board( host_boards );
			
			minimax.setGlobalDimensions( 1, 42 );
			minimax.setLocalDimensions( 1, 42 ); // this needs to stay with x-dimension 1.
			minimax( host_boards );
			/*	
			for ( int i = 0; i < 258; i++ )
			{
				printf( "*** %d ***\n" , i );
				board_print( &myBoards[i] );
				printf( "\n" );
				}*/

			return 0;
		}

private:
	int mySequentialDepth;
	int myBoardsSize;
	Board myStartBoard;
	Board *myBoards;
	Board *myStartBoards;

	CLContext myContext;
	CLKernel generate_boards;
	CLKernel evaluate_board;
	CLKernel minimax;

	CLUnitArgument host_boards;
	CLUnitArgument start_boards;
};

extern "C" int opencl_player_move( Board *b )
{
	ifstream t("opencl_player.cl");
    string src((std::istreambuf_iterator<char>(t)),
               std::istreambuf_iterator<char>());
	
	OpenCLPlayer player( *b, 1, src );
	return player.makeMove();
}

KalahPlayer opencl_player( void )
{
	KalahPlayer k;

	k.get_name = opencl_player_name;
	k.make_move = opencl_player_move;

	return k;
}

#ifdef MAKE_MAIN

int main ( void )
{
	Board b;
	board_initialize( &b, TOP );

	ifstream t("opencl_player.cl");
    string src((std::istreambuf_iterator<char>(t)),
               std::istreambuf_iterator<char>());
	
	OpenCLPlayer player( b, 2, src );
	player.makeMove();

}
#endif
