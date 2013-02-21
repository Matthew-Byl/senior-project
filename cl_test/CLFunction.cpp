#include "CLFunction.h"

/*
 * These need to correspond to the calls
 *  to _GEN_CL_FUNCTION_RUN_P at the end
 *  of CLFunction.h.
 */
#define _GEN_CL_FUNCTION_RUN( host, kernel ) \
	template<>								 \
	host CLFunction<host>::run()			 \
	{									   \
		return run( #kernel );			   \
	}

_GEN_CL_FUNCTION_RUN( cl_int, int )
_GEN_CL_FUNCTION_RUN( cl_float, float )
_GEN_CL_FUNCTION_RUN( cl_double, double )
_GEN_CL_FUNCTION_RUN( cl_float3, float3 )

/**
 * Special case for functions that return void.
 */
template<>
void CLFunction<void>::run()
{
	std::string src;
	std::string kernelFunction;

	generateKernelSource( "void", src, kernelFunction );

	copyBuffersToDevice();

	cl::Kernel kernel = generateKernel( src, kernelFunction );

	std::vector<int> globalDimensions;
	globalDimensions.push_back( 1 );
	std::vector<int> localDimensions;
	std::vector<int> globalOffset;
	enqueueKernel( kernel, globalDimensions, globalOffset, localDimensions );

	copyBuffersFromDevice();
}
