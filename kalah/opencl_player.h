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

	void set_board( Board b );
	MinimaxResult makeMove();

private:
	void generate_start_boards();
	void generate_board( Board parent, int depth );
	MinimaxResult run_minimax( Board &parent, int depth );

	int minimax_idx;
	CLContext myContext;
	int mySequentialDepth;
	int myBoardsSize;
	Board myStartBoard;
	CLKernel opencl_player;

	std::vector<Board> myStartBoards;
};

// Exposed for testing.
MinimaxResult opencl_player_pre_minimax( OpenCLPlayer &player, Board &b, int depth );

#endif
