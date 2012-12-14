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
				const CLContext context = CLContext() )
		: myContext( context ), 
		  myFunction( function), 
		  myKernel( kernel )
		{
		}

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
	const CLContext myContext;
	std::vector<CLUnitArgument> myArguments;
	std::vector<cl::Buffer> myBuffers;
	std::string myFunction;
	std::string myKernel;

	virtual void generateKernelSource( const std::string type, std::string &source, std::string &kernel_name );
	void generateBuffers();
	void copyBuffersToDevice();
	void copyBuffersFromDevice();
	cl::Kernel generateKernel( std::string src, std::string kernel_name );
	void enqueueKernel( cl::Kernel &kernel, std::vector<int> &dimensions );
};


/*
 * Generate run() methods for every standard type.
 */
#define _GEN_CL_FUNCTION_RUN( host, kernel ) \
	template<>								 \
	host CLFunction<host>::run()			 \
	{									   \
		return run( #kernel );			   \
	}

_GEN_CL_FUNCTION_RUN( cl_int, int )
_GEN_CL_FUNCTION_RUN( cl_float, float )
_GEN_CL_FUNCTION_RUN( cl_float3, float3 )

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
	cl::Program program = myContext.buildProgram( src );
	cl::Kernel kernel(
        program,
        kernel_name.c_str()
	);

	// Make those buffers arguments for the kernel.
	for ( unsigned i = 0; i < myBuffers.size(); i++ )
	{
		std::cout << "Setting argument " << i << " of type " << myArguments[i].getType() << std::endl;
		kernel.setArg( i, myBuffers[i] );
	}

	return kernel;
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
void CLFunction<T>::enqueueKernel( cl::Kernel &kernel, std::vector<int> &dimensions )
{
	// OpenCL only allows up to 3 dimensions.
	cl::NDRange globalWorkSize;
	if ( dimensions.size() == 1 )
	{
		globalWorkSize = cl::NDRange( dimensions[0] );
	}
	else if ( dimensions.size() == 2 )
	{
		globalWorkSize = cl::NDRange( 
			dimensions[0],
			dimensions[1]
		);
	}
	else if ( dimensions.size() == 3 )
	{
		globalWorkSize = cl::NDRange( 
			dimensions[0],
			dimensions[1],
			dimensions[2]
		);
	}
	else
	{
		assert( false );
	}

	// Queue up the kernel.
	auto queue = myContext.getCommandQueue();
	queue.enqueueNDRangeKernel(
		kernel,
		cl::NullRange,
		globalWorkSize,
		cl::NullRange,
		NULL,
		NULL
	);
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

	std::vector<int> dimensions;
	dimensions.push_back( 1 );
	enqueueKernel( kernel, dimensions );

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
void CLFunction<void>::run()
{
	std::string src;
	std::string kernelFunction;

	generateKernelSource( "void", src, kernelFunction );

	std::cout << "FULL SOURCE" << std::endl;
	std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
	std::cout << src << std::endl;
	std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;

	copyBuffersToDevice();
	cl::Kernel kernel = generateKernel( src, kernelFunction );

	std::vector<int> dimensions;
	dimensions.push_back( 1 );
	enqueueKernel( kernel, dimensions );

	copyBuffersFromDevice();
}

#endif
