#ifndef _CL_FUNCTION_H
#define _CL_FUNCTION_H

#include "CLContext.h"
#include "CLUnitArgument.h"
#include "KernelGenerator.h"
#include <iostream>

/* Encapsulates an OpenCL function that can be called like normal code. */
template<class T>
class CLFunction
{
public:
	CLFunction( std::string function, 
				std::string kernel, 
				bool isKernel = false,
				const CLContext context = CLContext( 0, 0 ) )
		: myContext( context ), 
		  myFunction( function), 
		  myKernel( kernel ), 
		  myIsKernel( isKernel)
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
		}

	const T run();
	const T run( std::string type );

	template<class ...Arguments>
	T operator()( Arguments... params )
		{
			setArguments( params... );
			return run();
		}
	
private:
	const CLContext myContext;
	std::vector<CLUnitArgument> myArguments;
	std::string myFunction;
	std::string myKernel;
	bool myIsKernel;
};


/*
 * Generate run() methods for every standard type.
 */
#define _GEN_CL_FUNCTION_RUN( host, kernel ) \
	template<>								 \
	const host CLFunction<host>::run() \
	{									   \
		return run( #kernel );			   \
	}

_GEN_CL_FUNCTION_RUN( cl_int, int )
_GEN_CL_FUNCTION_RUN( cl_float, float )


/// @todo: specialize for void, when we return nothing; i.e. run a kernel.
template<class T>
const T CLFunction<T>::run( std::string type )
{
	std::string src;
	std::string kernelFunction;
	if ( myIsKernel )
	{
		src = myKernel;
		kernelFunction = myFunction;
	}
	else
	{	
		KernelGenerator generator( myFunction, myArguments, type );
		src = myKernel + "\n\n" + generator.generate();
		kernelFunction = generator.getKernelFunction();
	}

	std::cout << "FULL SOURCE" << std::endl;
	std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
	std::cout << src << std::endl;
	std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;

	cl::Program program = myContext.buildProgram( src );
	cl::Kernel kernel(
        program,
        kernelFunction.c_str()
	);

	// Assemble a vector of buffers.
	std::vector<cl::Buffer> buffers;
	for ( auto &it : myArguments )
	{
		buffers.push_back( it.getBuffer( myContext ) );
	}

	// Make those buffers arguments for the kernel.
	for ( unsigned i = 0; i < buffers.size(); i++ )
	{
		std::cout << "Setting argument " << i << " of type " << myArguments[i].getType() << std::endl;
//		printf( "%x\n", buffers[i] );
		kernel.setArg( i, buffers[i] );
	}

	// Queue up copying those buffers.
	auto queue = myContext.getCommandQueue();
	for ( auto &it : myArguments )
	{
		it.enqueue( queue );
	}

	T result;
	cl::Buffer resultBuffer;
	if ( !myIsKernel )
	{
		// Create the buffer for the result.
		resultBuffer = cl::Buffer(
			myContext.getContext(),
			CL_MEM_WRITE_ONLY,
			sizeof( T ),
			NULL
			);
		// Make it the next kernel argument.
		kernel.setArg( myArguments.size(), resultBuffer );
	}

	// Queue up the kernel.
	cl::NDRange globalWorkSize( 1 );
	queue.enqueueNDRangeKernel(
		kernel,
		cl::NullRange,
		globalWorkSize,
		cl::NullRange,
		NULL,
		NULL
	);

	if ( !myIsKernel )
	{
		// Enqueue reading the result.
		queue.enqueueReadBuffer(
			resultBuffer,
			CL_TRUE,
			0,
			sizeof( T ),
			&result,
			NULL,
			NULL );
	}

	// Enqueue reading all the results back.

	return result;
}

#endif
