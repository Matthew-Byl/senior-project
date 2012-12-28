// a GPU board needs to be an efficient size for copying around.
// There are 14 cells on each side of the board, plus one for whose move it is, and one for a score.
// The score can be positive or negative, and the numbers are never larger than 128.
typedef GPUBoard char[16]

#define TOP 0
#define BOTTOM 1
#define BOTTOM_KALAH 6
#define TOP_KALAH 13
#define PLAYER_TO_MOVE 14
#define SCORE 15

int gpu_board_legal_move( GPUBoard b, int move )
{
    if ( b[PLAYER_TO_MOVE] == TOP
         && move >= 7
         && move <= 12
         && b[move] != 0)
    {
        return TRUE;
    }
    else if (
        b[PLAYER_TO_MOVE] == BOTTOM
        && move >= 0
        && move <= 5
        && b[move] != 0 )
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
