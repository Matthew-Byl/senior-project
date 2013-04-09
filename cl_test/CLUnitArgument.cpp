/**
 * CLUnitArgument encapsulates data passed to CLKernel 
 *  and CLFunction.
 * 
 * @author John Kloosterman
 * @date December 2012
 */

/*
 * The cl::Buffer object is initialized only right before
 *  it is needed. The reason: it is important to be able
 *  to initialize a CLUnitArgument without passing in a
 *  CLContext so that type coersion can work (e.g. passing
 *  a cl_int to a CLFunction automatically creates the correct
 *  CLUnitArgument). cl::Buffers need a cl::Context for initialization.
 *
 * However, this causes unexpected behaviour if there are copies of a
 *  CLUnitArgument being made before the buffer is initialized.
 *  Each copy will then create their own cl::Buffer. This breaks code
 *  like this:
 *
 *   cl_float array[1024];
 *   CLUnitArgument arrayArg( "float", array, 1024, false, false );
 *   someKernel( arrayArg ); // stores data into arrayArg, doesn't copy 
 *                           // data back to host.
 *   someOtherKernel( arrayArg ); // reads data someKernel() stored.
 *
 * This is because C++ made a copy of arrayArg when its cl::Buffer was
 *  not initialized, so each copy created its own buffer and the cl::Buffer
 *  passed to someKernel() and someOtherKernel() was not the same one.
 * The solution is to call CLUnitArgument::makePersistent() on the buffer
 *  first.
 */

#include "CLUnitArgument.h"
#include <iostream>
#include <cassert>
using namespace std;

CLUnitArgument::CLUnitArgument( 
	string name,
	size_t size,
	void *ptr,
	bool copy,
	bool isArray,
	bool copyTo,
	bool copyBack
	)
{
	initialize(
		name,
		size,
		ptr,
		copy,
		isArray,
		copyTo,
		copyBack
		);
}

void CLUnitArgument::initialize(
	string name,
	size_t size,
	void *ptr,
	bool copy,
	bool isArray,
	bool copyTo,
	bool copyBack
	)
{
	mySize = size;
	myName = name;
	myCopy = copy;
	myIsArray = isArray;
	myCopyTo = copyTo;
	myCopyBack = copyBack;

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
	myPtr = malloc( size );
	memcpy( myPtr, ptr, size );
}

CLUnitArgument::CLUnitArgument( const CLUnitArgument &other )
	: myBufferInitialized( other.myBufferInitialized ),
	  mySize( other.mySize ), 
	  myName( other.myName ), 
	  myCopy( other.myCopy ),
	  myIsArray( other.myIsArray ),
	  myCopyTo( other.myCopyTo ),
	  myCopyBack( other.myCopyBack )
{
	if ( other.myCopy )
	{
		copy_data( other.mySize, other.myPtr );
		myBufferInitialized = false;
	}
	else
	{
		myPtr = other.myPtr;
		myBuffer = other.myBuffer;
	}
}

CLUnitArgument::~CLUnitArgument()
{
	if ( myCopy )
	{
		free( myPtr );
	}
}

cl::Buffer *CLUnitArgument::getBuffer( CLContext &context )
{
	if ( !myBufferInitialized )
	{
		myBuffer = cl::Buffer(
			context.getContext(),
			// If we don't supply memory, allocate some for us.
			( myPtr == NULL ) ? CL_MEM_HOST_NO_ACCESS : CL_MEM_USE_HOST_PTR,
			mySize,
			myPtr
			);

		myBufferInitialized = true;
	}

	return &myBuffer;
}

std::string CLUnitArgument::getType()
{
	return myName;
}

void CLUnitArgument::copyToDevice( cl::CommandQueue &queue )
{
	assert( myBufferInitialized );

	if ( !myCopyTo )
		return;

	queue.enqueueWriteBuffer(
		myBuffer,
		CL_TRUE,
		0,
		mySize,
		myPtr
	);
}

/**
 * See note in CLUnitArgument.h
 */
void CLUnitArgument::makePersistent( CLContext &context )
{
	myBuffer = cl::Buffer(
		context.getContext(),
		// If we don't supply memory, allocate some for us.
		( myPtr == NULL ) ? CL_MEM_ALLOC_HOST_PTR : CL_MEM_USE_HOST_PTR,
		mySize,
		myPtr
		);
	
	myBufferInitialized = true;
}

void CLUnitArgument::copyFromDevice( cl::CommandQueue &queue )
{
	assert( myBufferInitialized );

	// If we own the memory, nobody else can read it anyways.
	if ( myCopy )
		return;
	if ( !myCopyBack )
		return;

	queue.enqueueReadBuffer(
		myBuffer,
		CL_TRUE,
		0,
		mySize,
		myPtr
	);
}

bool CLUnitArgument::isArray()
{
	return myIsArray;
}
