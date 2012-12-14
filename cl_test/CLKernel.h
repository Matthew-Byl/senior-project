#ifndef _CL_KERNEL
#define _CL_KERNEL

#include "CLFunction.h"

class CLKernel : public CLFunction<void>
{
public:
	CLKernel( std::string function,
			  std::string kernel,
			  const CLContext context = CLContext() )
		: CLFunction( function, kernel, context )
		{ }

	virtual void run();
	void run( int dim1 );
	void run( int dim1, int dim2 );
	void run( int dim1, int dim2, int dim3 );
	void run( std::vector<int> &dimensions );
};

// For running pre-existing kernels in 1-3 dimensions.
void CLKernel::run( std::vector<int> &dimensions )
{
	copyBuffersToDevice();

	cl::Kernel kernel = generateKernel( myKernel, myFunction );
	enqueueKernel( kernel, dimensions );

	copyBuffersFromDevice();
}

void CLKernel::run()
{
	std::vector<int> dimensions;
	dimensions.push_back( 1 );
	run( dimensions );
}

void CLKernel::run( int dim1 )
{
	std::vector<int> dimensions;
	dimensions.push_back( dim1 );
	run( dimensions );
}

void CLKernel::run( int dim1, int dim2 )
{
	std::vector<int> dimensions;
	dimensions.push_back( dim1 );
	run( dimensions );
}

void CLKernel::run( int dim1, int dim2, int dim3 )
{
	std::vector<int> dimensions;
	dimensions.push_back( dim1 );
	run( dimensions );
}

#endif
