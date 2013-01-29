#include "CLFunction.h"

#define _GEN_CL_FUNCTION_RUN( host, kernel ) \
	template<>								 \
	host CLFunction<host>::run()			 \
	{									   \
		return run( #kernel );			   \
	}

_GEN_CL_FUNCTION_RUN( cl_int, int )
_GEN_CL_FUNCTION_RUN( cl_float, float )
_GEN_CL_FUNCTION_RUN( cl_float3, float3 )

template<>
void CLFunction<void>::run()
{
	std::string src;
	std::string kernelFunction;

	generateKernelSource( "void", src, kernelFunction );

/*
	std::cout << "FULL SOURCE" << std::endl;
	std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
	std::cout << src << std::endl;
	std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
*/

	copyBuffersToDevice();
	cl::Kernel kernel = generateKernel( src, kernelFunction );

	std::vector<int> globalDimensions;
	globalDimensions.push_back( 1 );
	std::vector<int> localDimensions;
	std::vector<int> globalOffset;
	enqueueKernel( kernel, globalDimensions, globalOffset, localDimensions );

	copyBuffersFromDevice();
}
