#ifndef _CL_KERNEL
#define _CL_KERNEL

#include "CLFunction.h"

class CLKernel : public CLFunction<void>
{
public:
	CLKernel( std::string function,
			  std::string kernel,
			  const CLContext context = CLContext() )
		: CLFunction( function, kernel, context ), dimensionsSet( false )
		{ }

	void setDimensions( int dim1 );
	void setDimensions( int dim1, int dim2 );
	void setDimensions( int dim1, int dim2, int dim3 );

	virtual void run();

private:
	bool dimensionsSet;
	std::vector<int> dimensions;
};

void CLKernel::setDimensions( int dim1 )
{
	std::vector<int> dim;
	dim.push_back( dim1 );
	dimensions = dim;
	dimensionsSet = true;
}

void CLKernel::setDimensions( int dim1, int dim2 )
{
	std::vector<int> dim;
	dim.push_back( dim1 );
	dim.push_back( dim2 );
	dimensions = dim;
	dimensionsSet = true;
}

void CLKernel::setDimensions( int dim1, int dim2, int dim3 )
{
	std::vector<int> dim;
	dim.push_back( dim1 );
	dim.push_back( dim2 );
	dim.push_back( dim3 );
	dimensions = dim;
	dimensionsSet = true;
}

// For running pre-existing kernels in 1-3 dimensions.
void CLKernel::run()
{
	// Be nicer, later.
	assert( dimensionsSet );

	copyBuffersToDevice();

	cl::Kernel kernel = generateKernel( myKernel, myFunction );
	enqueueKernel( kernel, dimensions );

	copyBuffersFromDevice();
}

#endif
