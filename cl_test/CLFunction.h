#ifndef _CL_FUNCTION_H
#define _CL_FUNCTION_H

#include "CLContext.h"
#include "CLUnitArgument.h"
#include "KernelGenerator.h"
#include <iostream>
#include <cassert>

/* Encapsulates an OpenCL function that can be called like normal code. */
template<class T>
class CLFunction
{
public:
	CLFunction( std::string function, 
				std::string kernel, 
				CLContext context = CLContext() )
		: myContext( context ), 
		  myFunction( function), 
		  myKernel( kernel )
		{
		}
	virtual ~CLFunction() { }

	template<class ...Arguments>
	void setArguments( Arguments... params )
		{
			// This has to be done with an array, instead of
			//  the more elegant creating the array with an
			//  initializer list, in order to be compatible with
			//  my compiler.
			CLUnitArgument args[] = { params... };
			size_t size = sizeof( args ) / sizeof( CLUnitArgument );
			myArguments = std::vector<CLUnitArgument>( args, args + size );

			generateBuffers();
		}

	virtual T run();
	T run( std::string type );

	template<class ...Arguments>
	T operator()( Arguments... params )
		{
			setArguments( params... );
			return run();
		}
	
protected:
	CLContext myContext;
	std::vector<CLUnitArgument> myArguments;
	std::vector<cl::Buffer> myBuffers;
	std::string myFunction;
	std::string myKernel;
	cl::Kernel myCLKernel;
	bool kernelBuilt = false;

	virtual void generateKernelSource( const std::string type, std::string &source, std::string &kernel_name );
	void generateBuffers();
	void copyBuffersToDevice();
	void copyBuffersFromDevice();
	cl::Kernel generateKernel( std::string src, std::string kernel_name );
	void enqueueKernel( cl::Kernel &kernel, std::vector<int> &globalDimensions, std::vector<int> &globalOffset, std::vector<int> &localDimensions );
};


template<class T>
void CLFunction<T>::generateKernelSource( const std::string type, std::string &source, std::string &kernel_name )
{
	KernelGenerator generator( myFunction, myArguments, type );
	source = myKernel + "\n\n" + generator.generate();
	kernel_name = generator.getKernelFunction();
}

template<class T>
void CLFunction<T>::generateBuffers()
{
	myBuffers.clear();

	for ( auto &it : myArguments )
	{
		myBuffers.push_back( it.getBuffer( myContext ) );
	}
}

template<class T>
void CLFunction<T>::copyBuffersToDevice()
{
	// Queue up copying those buffers.
	auto queue = myContext.getCommandQueue();
	for ( auto &it : myArguments )
	{
		it.copyToDevice( queue );
	}
}

template<class T>
cl::Kernel CLFunction<T>::generateKernel( std::string src, std::string kernel_name )
{
	if ( !kernelBuilt )
	{
//		std::cout << "Rebuilding kernel: " << kernelBuilt << std::endl;

		cl::Program program = myContext.buildProgram( src );
		myCLKernel = cl::Kernel(
			program,
			kernel_name.c_str()
		);

		kernelBuilt = true;
	}

	// Make those buffers arguments for the kernel.
	for ( unsigned i = 0; i < myBuffers.size(); i++ )
	{
//		std::cout << "Setting argument " << i << " of type " << myArguments[i].getType() << std::endl;
		myCLKernel.setArg( i, myBuffers[i] );
	}

	return myCLKernel;
}

template<class T>
void CLFunction<T>::copyBuffersFromDevice()
{
    auto queue = myContext.getCommandQueue();
    for ( auto &it : myArguments )
    {
		it.copyFromDevice( queue );
    }
}

template<class T>
void CLFunction<T>::enqueueKernel( 
	cl::Kernel &kernel, 
	std::vector<int> &globalDimensions, 
	std::vector<int> &globalOffset, 
	std::vector<int> &localDimensions )
{
	// OpenCL only allows up to 3 dimensions.
	cl::NDRange globalWorkSize;
	if ( globalDimensions.size() == 1 )
	{
		globalWorkSize = cl::NDRange( globalDimensions[0] );
	}
	else if ( globalDimensions.size() == 2 )
	{
		globalWorkSize = cl::NDRange( 
			globalDimensions[0],
			globalDimensions[1]
		);
	}
	else if ( globalDimensions.size() == 3 )
	{
		globalWorkSize = cl::NDRange( 
			globalDimensions[0],
			globalDimensions[1],
			globalDimensions[2]
		);
	}
	else
	{
		assert( false );
	}

	// Global offset
	cl::NDRange globalOffsetRange;
	if ( globalOffset.size() == 1 )
	{
		globalOffsetRange = cl::NDRange( globalOffset[0] );
	}
	else if ( globalOffset.size() == 2 )
	{
		globalOffsetRange = cl::NDRange( 
			globalOffset[0],
			globalOffset[1]
		);
	}
	else if ( globalOffset.size() == 3 )
	{
		globalOffsetRange = cl::NDRange( 
			globalOffset[0],
			globalOffset[1],
			globalOffset[2]
		);
	}
	else
	{
		globalOffsetRange = cl::NullRange;
	}

	// Local dimensions
	cl::NDRange localWorkSize;
	if ( localDimensions.size() == 1 )
	{
		localWorkSize = cl::NDRange( localDimensions[0] );
	}
	else if ( localDimensions.size() == 2 )
	{
		localWorkSize = cl::NDRange( 
			localDimensions[0],
			localDimensions[1]
		);
	}
	else if ( localDimensions.size() == 3 )
	{
		localWorkSize = cl::NDRange( 
			localDimensions[0],
			localDimensions[1],
			localDimensions[2]
		);
	}
	else
	{
		localWorkSize = cl::NullRange;
	}

	// Queue up the kernel.
	auto &queue = myContext.getCommandQueue();
	queue.enqueueNDRangeKernel(
		kernel,
		globalOffsetRange,
		globalWorkSize,
		localWorkSize,
		NULL,
		NULL
	);

	// This is needed in case the queue supports running things
	//  out of order, the next kernel depends on this one being done,
	//  and no buffers are read back synchronously to the device.
	queue.enqueueBarrier();
}

/// @todo: specialize for void, when we return nothing; i.e. run a kernel.
template<class T>
T CLFunction<T>::run( std::string type )
{
	std::string src;
	std::string kernelFunction;
	auto queue = myContext.getCommandQueue();

	generateKernelSource( type, src, kernelFunction );

	std::cout << "FULL SOURCE" << std::endl;
	std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
	std::cout << src << std::endl;
	std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;

	copyBuffersToDevice();
	cl::Kernel kernel = generateKernel( src, kernelFunction );

	T result;
	cl::Buffer resultBuffer;
	// Create the buffer for the result.
	resultBuffer = cl::Buffer(
		myContext.getContext(),
		CL_MEM_WRITE_ONLY,
		sizeof( T ),
		NULL
		);
	
	// Make it the next kernel argument.
	kernel.setArg( myArguments.size(), resultBuffer );

	std::vector<int> globalDimensions;
	globalDimensions.push_back( 1 );
	std::vector<int> localDimensions;
	std::vector<int> globalOffset;
	enqueueKernel( kernel, globalDimensions, globalOffset, localDimensions );

	// Enqueue reading the result.
	queue.enqueueReadBuffer(
		resultBuffer,
		CL_TRUE,
		0,
		sizeof( T ),
		&result,
		NULL,
		NULL );

	// Enqueue reading all the results back.
	copyBuffersFromDevice();
	
	return result;
}

template<>
void CLFunction<void>::run();

/*
 * Generate run() methods for every standard type.
 */
#define _GEN_CL_FUNCTION_RUN_P( host, kernel ) \
	template<>								 \
	host CLFunction<host>::run();

_GEN_CL_FUNCTION_RUN_P( cl_int, int )
_GEN_CL_FUNCTION_RUN_P( cl_float, float )
_GEN_CL_FUNCTION_RUN_P( cl_float3, float3 )

#endif
