/**
 * CLUnitArgument encapsulates data to send to
 *  a CLKernel or a CLFunction.
 *
 * There are tons of constructors so that type coersion
 *  will allow for calls like:
 *
 *  cl_int i;
 *  cl_float j;
 *  CLKernel a_kernel( ... );
 *  a_kernel( i, j ); // this will only work in C++11
 *
 * @author John Kloosterman
 * @date December 2012
 */

#ifndef _CL_UNIT_ARGUMENT
#define _CL_UNIT_ARGUMENT

#include "CLIncludes.h"
#include "CLContext.h"
#include <string>

#include <iostream>

// Don't worry about ___3 types for the time being.
//  and half* types, which are also typedefed with something
//  else that makes it impossible for C++ to tell their types apart.

/**
 * Macro to define constructors for a OpenCL host type,
 *  its vector types, and arrays of both of those.
 */
#define CONSTRUCTORS( type )					\
  CTR( cl_##type, type );						\
  PTR_CTR( cl_##type, type );					\
  CTR( cl_##type##2, type##2 );					\
  PTR_CTR( cl_##type##2, type##2 );				\
  CTR( cl_##type##4, type##4 );					\
  PTR_CTR( cl_##type##4, type##4 );				\
  CTR( cl_##type##8, type##8 );					\
  PTR_CTR( cl_##type##8, type##8 );				\
  CTR( cl_##type##16, type##16 );				\
  PTR_CTR( cl_##type##16, type##16 );			

#define CTR( host, kernel )									\
	CLUnitArgument( host val )								\
	{ initialize( #kernel, sizeof( host ), &val ); }

#define PTR_CTR( host, kernel )										\
	CLUnitArgument( host *array, size_t elements, bool copyTo = true, bool copyBack = true ) \
	{ initialize( #kernel, sizeof( host ) * elements, array, false, true, copyTo, copyBack ); }

class CLUnitArgument
{
public:
	CLUnitArgument( 
		std::string name, 
		size_t size, 
		void *ptr, 
		bool copy = true,
		bool isArray = false,
		bool copyTo = true,
		bool copyBack = true );
	CLUnitArgument( const CLUnitArgument &other );

	// Add constructors for the value types and
	//  arrays for all the OpenCL types.
	CONSTRUCTORS( int );
	CONSTRUCTORS( uint );
	CONSTRUCTORS( long );
	CONSTRUCTORS( ulong );
	CONSTRUCTORS( short );
	CONSTRUCTORS( ushort );
	CONSTRUCTORS( char );
	CONSTRUCTORS( uchar );
	CONSTRUCTORS( float );
	CONSTRUCTORS( double );

	// Templates for user-defined types.
	// Value constructor
	template<class T>
	CLUnitArgument( std::string name, T value );

	// Array constructor
	template<class T>
	CLUnitArgument( std::string name, T *array, size_t elements, bool copyTo = true, bool copyBack = true );

	~CLUnitArgument();

	void makePersistent( CLContext &context );

	cl::Buffer *getBuffer( CLContext &context );
	std::string getType();
	void copyToDevice( cl::CommandQueue &queue );
	void copyFromDevice( cl::CommandQueue &queue );

	bool isArray();
	
private:
	void copy_data( size_t size, void *ptr );
	void initialize(
		std::string name, 
		size_t size, 
		void *ptr, 
		bool copy = true,
		bool isArray = false,
		bool copyTo = true,
		bool copyBack = true
		);

	bool myBufferInitialized;
	void *myPtr;
	size_t mySize;
    std::string myName;
	cl::Buffer myBuffer;
	bool myCopy;
	bool myIsArray;
	bool myCopyTo;
	bool myCopyBack;
};

template<class T>
CLUnitArgument::CLUnitArgument( std::string name, T value )
{
	initialize( name, sizeof( T ), &value );
}

template<class T>
CLUnitArgument::CLUnitArgument( std::string name, T *array, size_t elements, bool copyTo, bool copyBack )
{
	initialize( name, sizeof( T ) * elements, array, false, true, copyTo, copyBack );
}

#endif
