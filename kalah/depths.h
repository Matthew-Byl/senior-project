#ifndef _DEPTHS_H

#define WORKGROUP_SIZE 1000000

#define PRE_DEPTH 1
#define SEQUENTIAL_DEPTH 9
#define PARALLEL_DEPTH 4
// Actual tree depth:
//  MINIMAX_DEPTH = SEQUENTIAL_DEPTH + PARALLEL_DEPTH - 1 (one of the parallel levels overlaps with the sequential levels)

#define MINIMAX_DEPTH (PRE_DEPTH + SEQUENTIAL_DEPTH + PARALLEL_DEPTH - 1)

#endif
