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
		{ };
	virtual ~CLKernel() { };

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
	dimensions.clear();
	dimensions.push_back( dim1 );
	dimensionsSet = true;
}

void CLKernel::setDimensions( int dim1, int dim2 )
{
	dimensions.clear();
	dimensions.push_back( dim1 );
	dimensions.push_back( dim2 );
	dimensionsSet = true;
}

void CLKernel::setDimensions( int dim1, int dim2, int dim3 )
{
	dimensions.clear();
	dimensions.push_back( dim1 );
	dimensions.push_back( dim2 );
	dimensions.push_back( dim3 );
	dimensionsSet = true;
}

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
