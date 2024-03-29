#ifndef _TYPES_H
#define _TYPES_H

/*
 * C/C++ to OpenCL C interoperability types.
 *
 * All structures that can be passed between the host
 *  and OpenCL code *must* use these types.
 */

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifdef HOST
#define CL_INT cl_int 
#define CL_CHAR cl_int 
#else /* OpenCL */
#define CL_INT int
#define CL_CHAR char
#endif

// This is only used on the host side, so it's OK to 
//  use regular types.
typedef struct
{
	int move;
	int score;
} MinimaxResult;

#endif
