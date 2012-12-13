#ifndef _CL_UNIT_ARGUMENT
#define _CL_UNIT_ARGUMENT

#include "CLIncludes.h"
#include "CLContext.h"
#include <string>

#define CTR( type ) CLUnitArgument( type val );

class CLUnitArgument
{
public:
	CLUnitArgument( std::string name, size_t size, void *ptr );
	CLUnitArgument( const CLUnitArgument &other );
	CTR( cl_int );
	CTR( cl_uchar );
	CTR( cl_float3 );
	CTR( cl_int3 );

	~CLUnitArgument();

	cl::Buffer &getBuffer( const CLContext &context );
	std::string getType();
	void enqueue( cl::CommandQueue &queue );
	
private:
	void _construct( size_t size, void *ptr );

	bool myBufferInitialized;
	void *myPtr;
	size_t mySize;
    std::string myName;
	cl::Buffer myBuffer;
};

// Allows us to make copies of arguments with their data
//  without copying the memory buffer and still not
//  leaking huge chunks of memory.
class CLReferenceBuffer
{
public:
	CLReferenceBuffer( void *ptr, size_t size )
		: mySize( size )
		{ 
			myBuffer = malloc( size );
			memcpy( myBuffer, ptr, size );
			myReferences = new int( 1 );
		}
	CLReferenceBuffer( const CLReferenceBuffer &other )
		: mySize( other.mySize ), myBuffer( other.myBuffer )
		{
			(*myReferences)++;
		}
	~CLReferenceBuffer()
		{
			myReferences--;
			if ( !myReferences )
			{
				free( myBuffer );
				delete myReferences;
			}
		}

	operator void*() { return myBuffer; };
private:
	int *myReferences;
	size_t mySize;
	void *myBuffer;
};

#endif
