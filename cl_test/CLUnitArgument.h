#ifndef _CL_UNIT_ARGUMENT
#define _CL_UNIT_ARGUMENT

#include "CLContext.h"
#include <string>

/*
 * Types of arguments we'll eventually need to be able
 *  to handle:
 *   (use a factory class to generate the right argument
 *    automagically with lots of constructors)
 *  -single value types (int, float)
 *  -vector types (int4, float2), etc.
 *  -custom objects
 *  -arrays of all the above.
 */

class CLUnitArgument
{
public:
//	virtual void updateFromBuffer( cl::Buffer &buffer ) = 0;
	virtual cl::Buffer &getBuffer();
	virtual std::string getType();
	virtual void enqueue( cl::CommandQueue &queue ) { };

	// operator == or compareWith().
};

#endif
