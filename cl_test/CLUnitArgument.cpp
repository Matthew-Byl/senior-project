#include "CLUnitArgument.h"
#include <iostream>
using namespace std;

CLUnitArgument::CLUnitArgument( 
	string name,
	size_t size,
	void *ptr
	)
	: mySize( size ), myName( name )
{
	myPtr = malloc( size );
	memcpy( myPtr, ptr, size );
	myBufferInitialized = false;
}

// Automatically generate some constructors.
#define C_CTR( host, kernel )												\
	CLUnitArgument::CLUnitArgument(								\
		host val																\
		) : CLUnitArgument( #kernel, sizeof( host ), &val )	\
	{ }

C_CTR( cl_int, int );
C_CTR( cl_uchar, uchar );
C_CTR( cl_float3, float3 );
C_CTR( cl_int3, int3 );

CLUnitArgument::~CLUnitArgument() {
	free( myPtr );
}

cl::Buffer &CLUnitArgument::getBuffer( CLContext &context )
{
	if ( !myBufferInitialized )
	{
		myBuffer = cl::Buffer(
			context.getContext(),
			CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR,
			mySize,
			myPtr
			);
		myBufferInitialized = true;
	}

	return myBuffer;
}

std::string CLUnitArgument::getType()
{
	return myName;
}

void CLUnitArgument::enqueue( cl::CommandQueue &queue )
{
//	cout << "Enqueuing " << myValue << "... " << endl;

	queue.enqueueWriteBuffer(
		myBuffer,
		CL_TRUE,
		0,
		mySize,
		myPtr
	);
}
