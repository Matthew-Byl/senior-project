#ifndef _CL_UNIT_ARGUMENT
#define _CL_UNIT_ARGUMENT

// TODO: make a big header file.
#include "CLIncludes.h"
#include "CLContext.h"
#include <string>

#define CTR( type ) CLUnitArgument( type val );

class CLUnitArgument
{
public:
	CLUnitArgument( std::string name, size_t size, void *ptr );
	CTR( cl_int );
	CTR( cl_uchar );
	CTR( cl_float3 );
	CTR( cl_int3 );


	~CLUnitArgument();

	cl::Buffer &getBuffer( CLContext &context );
	std::string getType();
	void enqueue( cl::CommandQueue &queue );
	
private:
	bool myBufferInitialized;
	void *myPtr;
	size_t mySize;
    std::string myName;
	cl::Buffer myBuffer;
};

#endif
