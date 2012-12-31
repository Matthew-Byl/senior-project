#include "CLKernel.h"

void CLKernel::setGlobalDimensions( int dim1 )
{
	globalDimensions.clear();
	globalDimensions.push_back( dim1 );
	globalDimensionsSet = true;
}

void CLKernel::setGlobalDimensions( int dim1, int dim2 )
{
	globalDimensions.clear();
	globalDimensions.push_back( dim1 );
	globalDimensions.push_back( dim2 );
	globalDimensionsSet = true;
}

void CLKernel::setGlobalDimensions( int dim1, int dim2, int dim3 )
{
	globalDimensions.clear();
	globalDimensions.push_back( dim1 );
	globalDimensions.push_back( dim2 );
	globalDimensions.push_back( dim3 );
	globalDimensionsSet = true;
}

void CLKernel::setLocalDimensions( int dim1 )
{
	localDimensions.clear();
	localDimensions.push_back( dim1 );
	localDimensionsSet = true;
}

void CLKernel::setLocalDimensions( int dim1, int dim2 )
{
	localDimensions.clear();
	localDimensions.push_back( dim1 );
	localDimensions.push_back( dim2 );
	localDimensionsSet = true;
}

void CLKernel::setLocalDimensions( int dim1, int dim2, int dim3 )
{
	localDimensions.clear();
	localDimensions.push_back( dim1 );
	localDimensions.push_back( dim2 );
	localDimensions.push_back( dim3 );
	localDimensionsSet = true;
}

void CLKernel::setGlobalOffset( int dim1 )
{
	globalOffset.clear();
	globalOffset.push_back( dim1 );
	globalOffsetSet = true;
}

void CLKernel::setGlobalOffset( int dim1, int dim2 )
{
	globalOffset.clear();
	globalOffset.push_back( dim1 );
	globalOffset.push_back( dim2 );
	globalOffsetSet = true;
}

void CLKernel::setGlobalOffset( int dim1, int dim2, int dim3 )
{
	globalOffset.clear();
	globalOffset.push_back( dim1 );
	globalOffset.push_back( dim2 );
	globalOffset.push_back( dim3 );
	globalOffsetSet = true;
}

void CLKernel::run()
{
	// Be nicer, later.
	assert( globalDimensionsSet );

	copyBuffersToDevice();

	cl::Kernel kernel = generateKernel( myKernel, myFunction );
	enqueueKernel( kernel, globalDimensions, globalOffset, localDimensions );

	copyBuffersFromDevice();
}
