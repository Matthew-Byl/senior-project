extern "C" {
#include "board.h"
#include "opencl_player.h"
}

#include <CLKernel.h>

#include <fstream>
#include <string>
using namespace std;

/*
extern "C" const char *opencl_player_name( void )
{
	return "OpenCL Minimax";
}

extern "C" int opencl_player_move( Board *b )
{
	return 0;
}

KalahPlayer opencl_player( void )
{
	KalahPlayer k;

	k.get_name = opencl_player_name;
	k.make_move = opencl_player_move;

	return k;
}
*/

#ifdef MAKE_MAIN
int main ( void )
{
	Board b;
	board_initialize( &b, TOP );

	ifstream t("opencl_player.cl");
    string src((std::istreambuf_iterator<char>(t)),
               std::istreambuf_iterator<char>());

	Board boards[42];
	CLUnitArgument board_array( "Board", boards, 42 );
	CLUnitArgument board( "Board", b );

	CLContext ctx( 1, 0 );
	CLKernel generate_boards( "generate_boards", src );
	generate_boards.setGlobalDimensions( 1, 42 );
	generate_boards.setLocalDimensions( 1, 42 );
	generate_boards( board, board_array );

	CLKernel evaluate_board( "evaluate_board", src );
	evaluate_board.setGlobalDimensions( 1, 36, 14 );
	evaluate_board( board_array );

	CLKernel minimax( "minimax", src );
	minimax.setGlobalDimensions( 1, 6 );
	minimax.setLocalDimensions( 1, 6 );
	minimax( board_array );


	for ( int i = 0; i < 42; i++ )
	{
		printf( "*** %d ***\n" , i );
		board_print( &boards[i] );
		printf( "\n" );
	}

}
#endif
