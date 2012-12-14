#include "CLKernel.h"

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
