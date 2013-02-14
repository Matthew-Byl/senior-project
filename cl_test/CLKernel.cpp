#include "CLKernel.h"

CLKernel::CLKernel( std::string function,
					std::string kernel,
					std::string compilerFlags,
					const CLContext context )
	: CLFunction( function, kernel, context ),
	  globalDimensionsSet( false ),
	  globalOffsetSet( false ),
	  localDimensionsSet( false ),
	  myCompilerFlags( compilerFlags )
{ 
	myCLKernel = generateKernel( myKernel, myFunction, compilerFlags );
};


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

void CLKernel::setLocalArgument( int arg, size_t size )
{
	myCLKernel.setArg( arg, cl::__local( size ) );
}

void CLKernel::run()
{
	// Be nicer, later.
	assert( globalDimensionsSet );

	copyBuffersToDevice();

    for ( unsigned i = 0; i < myBuffers.size(); i++ )
    {
        myCLKernel.setArg( i, *myBuffers[i] );
    }
	enqueueKernel( myCLKernel, globalDimensions, globalOffset, localDimensions );

	copyBuffersFromDevice();
}
