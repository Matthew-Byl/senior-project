#ifndef _CL_PLAYER
#define _CL_PLAYER

#include <CLKernel.h>

extern "C" {
#include "board.h"
#include "kalah_player.h"
}

/*
 * OpenCL Mankalah Player.
 */

KalahPlayer opencl_minimax_player( void );

class OpenCLPlayer
{
public:
	OpenCLPlayer( int sequentialDepth, std::string src )
		: mySequentialDepth( sequentialDepth ),
		  opencl_player( "opencl_player", src, myContext )
		{
		};

	~OpenCLPlayer()
		{
//			delete[] myBoards;
		};

	int leaf_start();
	int get_leaf_nodes( int sequentialDepth );
	int get_board_array_size( int sequentialDepth );

	void generate_board( Board parent, int depth );
	MinimaxResult run_minimax( Board &parent, int depth );
	void generate_start_boards();
	MinimaxResult makeMove();
	int minimax_eval( Board b );

	void set_board( Board b );

private:
	int minimax_idx;

	CLContext myContext;
	int mySequentialDepth;
	int myBoardsSize;
	Board myStartBoard;
	CLKernel opencl_player;
//	CLUnitArgument start_boards;

	std::vector<Board> myStartBoards;
};

MinimaxResult opencl_player_pre_minimax( OpenCLPlayer *player, Board *b, int depth );

#endif
