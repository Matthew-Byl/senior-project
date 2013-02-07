#ifndef _TYPES_H
#define _TYPES_H

/**
 * Defines the types to be used in any structure
 *  passed between host and OpenCL code. These 
 *  guarantee the correct alignment and naming for
 *  both worlds.
 *
 * @author John Kloosterman
 * @date Jan. 17, 2013
 */

#ifdef _OPENCL_
/* Types used in OpenCL code */

#define CL_INT int
#define CL_FLOAT_T float
#define CL_UCHAR uchar

#else
/* Types used in host code */
#include <CL/cl.hpp>

#define CL_INT cl_int
#define CL_FLOAT_T cl_float
#define CL_UCHAR cl_uchar

#endif

#endif
