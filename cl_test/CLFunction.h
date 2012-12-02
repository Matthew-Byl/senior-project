#ifndef _CL_FUNCTION_H
#define _CL_FUNCTION_H

#include "CLContext.h"
#include "CLUnitArgument.h"
#include "KernelGenerator.h"

/* Encapsulates an OpenCL function that can be called like normal code. */
class CLFunction
{
public:
	CLFunction( std::string function, std::string kernel, CLContext &context );
	void addArgument( CLUnitArgument &argument );

	template<class T>
	T run( std::string type );
private:
	CLContext &myContext;
	std::vector<CLUnitArgument*> myArguments;
	std::string myFunction;
	std::string myKernel;
};

template<class T>
T CLFunction::run( std::string type )
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
	for ( auto it : myArguments )
	{
		buffers.push_back( it->getBuffer() );
	}

	// Make those buffers arguments for the kernel.
	for ( int i = 0; i < buffers.size(); i++ )
	{
		kernel.setArg( i, buffers[i] );
	}

	// Queue up copying those buffers.
	auto queue = myContext.getCommandQueue();
	for ( auto it : myArguments )
	{
		it->enqueue( queue );
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
