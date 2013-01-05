#ifndef _SIMPLE_PLAYERS_H
#define _SIMPLE_PLAYERS_H

#include "kalah_player.h"

KalahPlayer human_player( void );
KalahPlayer bonzo_player( void );

MinimaxResult minimax_move( Board *b, int depth );

KalahPlayer minimax_player( void );
KalahPlayer stackless_minimax_player( void );

#endif
