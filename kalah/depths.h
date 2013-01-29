#ifndef _DEPTHS_H

/**
 * Constants that determine how deep minimax
 *  searching goes for minimax players.
 */

// The number of parallel trees that are evaluated in a batch
//  in the OpenCL player.
#define WORKGROUP_SIZE 500000

// Depth of first level of sequential Minimax. Increase this value to
//  go deeper without changing the performance tuning for the GPU.
#define PRE_DEPTH 0
// Depth of second level of sequential Minimax. The larger this is,
//  the more parallel workgroups are spawned at the same time, and 
//  the more memory is needed and transferred. The optimal size depends 
//  on the type of OpenCL device being used.
#define SEQUENTIAL_DEPTH 10
// Depth of the parallel minimax component. This is limited to 4 on
//  my test system because the maximum local workgroup size on AMD
//  GPUs is 256, and 2^4 is 216.
#define PARALLEL_DEPTH 4

// One of the sequential levels overlaps with the parallel level,
//  which is why we have to subtract 1.
#define MINIMAX_DEPTH (PRE_DEPTH + SEQUENTIAL_DEPTH + PARALLEL_DEPTH - 1)

#endif
