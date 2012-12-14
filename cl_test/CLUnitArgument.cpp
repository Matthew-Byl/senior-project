#include "CLUnitArgument.h"
#include <iostream>
#include <cassert>
using namespace std;

CLUnitArgument::CLUnitArgument( 
	string name,
	size_t size,
	void *ptr,
	bool copy,
	bool isArray
	)
	: mySize( size ), myName( name ), myCopy( copy ), myIsArray( isArray )
{
	if ( copy )
		copy_data( size, ptr );
	else
		myPtr = ptr;

	myBufferInitialized = false;
}

void CLUnitArgument::copy_data(
	size_t size,
	void *ptr
	)
{
	cout << "** COPYING " << size << " ***" << endl;

	myPtr = malloc( size );
	memcpy( myPtr, ptr, size );
}

CLUnitArgument::CLUnitArgument( const CLUnitArgument &other )
	: mySize( other.mySize ), 
	  myName( other.myName ), 
	  myCopy( other.myCopy ),
	  myIsArray( other.myIsArray )
{
	if ( other.myCopy )
		copy_data( other.mySize, other.myPtr );
	else
		myPtr = other.myPtr;

	myBufferInitialized = false;
}

// Automatically generate some constructors.
/*
#define C_CTR( host, kernel )											\
	CLUnitArgument::CLUnitArgument(										\
		host val														\
		) : mySize( sizeof( host ) ),									\
			myName( #kernel ),											\
			myCopy( true ),												\
			myIsArray( false )											\
	{																	\
		copy_data( sizeof( host ), &val );								\
		myBufferInitialized = false;									\
		cout << "Constructor for " << #host << " running." << endl;		\
	}


C_CTR( cl_int, int );
C_CTR( cl_uchar, uchar );
C_CTR( cl_float3, float3 );
C_CTR( cl_int3, int3 );
*/

/*
#define C_PTR_CTR( host_ptr, host, kernel )								\
	CLUnitArgument::CLUnitArgument(										\
		host_ptr array,													\
		size_t elements													\
		) : myPtr( array ),												\
			mySize( sizeof( host ) * elements ),						\
			myName( #kernel ),											\
			myCopy( false ),											\
			myIsArray( true )											\
	{																	\
		myBufferInitialized = false;									\
		cout << "Constructor for " << #host << " array running." << endl; \
	}

C_PTR_CTR( cl_int*, cl_int, int );
*/

CLUnitArgument::~CLUnitArgument()
{
	if ( myCopy )
	{
		cout << "Freeing!" << endl << flush;
		free( myPtr );
	}
}

cl::Buffer CLUnitArgument::getBuffer( const CLContext &context )
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

void CLUnitArgument::copyToDevice( cl::CommandQueue &queue )
{
	assert( myBufferInitialized );

	queue.enqueueWriteBuffer(
		myBuffer,
		CL_TRUE,
		0,
		mySize,
		myPtr
	);
}

void CLUnitArgument::copyFromDevice( cl::CommandQueue &queue )
{
	assert( myBufferInitialized );

	// If we own the memory, nobody else can read it anyways.
//	if ( myCopy )
//		return;

	queue.enqueueReadBuffer(
		myBuffer,
		CL_TRUE,
		0,
		mySize,
		myPtr
	);

	cout << "Copying back!" << endl;
}

bool CLUnitArgument::isArray()
{
	return myIsArray;
}
