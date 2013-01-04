#ifndef _DEPTHS_H

#define WORKGROUP_SIZE 32768
#define SEQUENTIAL_DEPTH 5
#define PARALLEL_DEPTH 3 // This has to match the value MINIMAX_DEPTH in the kernel. @TODO: consistency
// Actual tree depth:
//  MINIMAX_DEPTH = SEQUENTIAL_DEPTH + PARALLEL_DEPTH - 1

#define MINIMAX_DEPTH (SEQUENTIAL_DEPTH + PARALLEL_DEPTH - 1)

#endif
