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
				const CLContext context = CLContext(0, 0 ) )
		: myContext( context ), myFunction( function), myKernel( kernel ) 
		{
		}
/*
	CLFunction( std::string function, std::string kernel )
		: myFunction( function), myKernel( kernel ) 
		{
			myContext = CLContext( 0, 0 );
		}
*/
//	void addArgument( CLUnitArgument argument );

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
};


template<>
const int CLFunction<int>::run()
{
	return run( "int" );
}

template<class T>
const T CLFunction<T>::run( std::string type )
{
    KernelGenerator generator( myFunction, myArguments, type );
	std::string src( myKernel + "\n\n" + generator.generate() );

	std::cout << "FULL SOURCE" << std::endl;
	std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
	std::cout << src << std::endl;
	std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;

	cl::Program program = myContext.buildProgram( src );
	cl::Kernel kernel(
        program,
        generator.getKernelFunction().c_str()
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

	// Create the buffer for the result.
	T result;
	cl::Buffer resultBuffer(
		myContext.getContext(),
		CL_MEM_WRITE_ONLY,
		sizeof( T ),
		NULL
		);
	// Make it the next kernel argument.
	kernel.setArg( myArguments.size(), resultBuffer );

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

	// Enqueue reading the result.
	queue.enqueueReadBuffer(
		resultBuffer,
		CL_TRUE,
		0,
		sizeof( T ),
		&result,
		NULL,
		NULL );

	return result;
}

#endif
