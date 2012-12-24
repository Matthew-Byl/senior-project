#ifndef _KALAH_PLAYER_H
#define _KALAH_PLAYER_H

#include "board.h"

typedef int (*PlayerMoveFunction)( Board *b );
typedef char *(*PlayerNameFunction)( void );

typedef struct
{
	PlayerMoveFunction make_move;
	PlayerNameFunction get_name;
} KalahPlayer;

#endif
