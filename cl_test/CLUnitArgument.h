#ifndef _CL_UNIT_ARGUMENT
#define _CL_UNIT_ARGUMENT

#include "CLIncludes.h"
#include "CLContext.h"
#include <string>

#define CTR( host, kernel )									\
	CLUnitArgument( host val )								\
		: CLUnitArgument( #kernel, val ) { }

#define PTR_CTR( host, kernel )							\
	CLUnitArgument( host array, size_t elements )		\
		: CLUnitArgument( #kernel, array, elements ) { }

class CLUnitArgument
{
public:
	CLUnitArgument( 
		std::string name, 
		size_t size, 
		void *ptr, 
		bool copy = true,
		bool isArray = false );
	CLUnitArgument( const CLUnitArgument &other );

	// These have to be macros because the host-side names
	//  are different from the GPU names. Maybe templates
	//  would be more elegant?
	CTR( cl_int, int );
	CTR( cl_uchar, uchar );
	CTR( cl_float3, float3 );
	CTR( cl_int3, int3 );
	PTR_CTR( cl_int*, int );

	// Make some template constructors for objects
	//  like the raytracer lights and world? Maybe that
	//  could simplify this macro stuff too.

	// Value constructor
	template<class T>
	CLUnitArgument( std::string name, T value );

	// Array constructor
	template<class T>
	CLUnitArgument( std::string name, T *array, size_t elements );

	~CLUnitArgument();

	cl::Buffer getBuffer( const CLContext &context );
	std::string getType();
	void copyToDevice( cl::CommandQueue &queue );
	void copyFromDevice( cl::CommandQueue &queue );

	bool isArray();
	
private:
	void copy_data( size_t size, void *ptr );

	bool myBufferInitialized;
	void *myPtr;
	size_t mySize;
    std::string myName;
	cl::Buffer myBuffer;
	bool myCopy;
	bool myIsArray;
};

template<class T>
CLUnitArgument::CLUnitArgument( std::string name, T value )
	: CLUnitArgument( name, sizeof( T ), &value )
{

}

template<class T>
CLUnitArgument::CLUnitArgument( std::string name, T *array, size_t elements )
	: CLUnitArgument( name, sizeof( T ) * elements, array, false, true )
{
	
}

// Allows us to make copies of arguments with their data
//  without copying the memory buffer and still not
//  leaking huge chunks of memory.
//
// We don't use this yet, and it probably doesn't work.
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
