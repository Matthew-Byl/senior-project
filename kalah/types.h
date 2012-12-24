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
#else /* OpenCL */
#define CL_INT int
#endif

#endif
