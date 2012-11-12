#ifndef _CL_UNIT_ARGUMENT
#define _CL_UNIT_ARGUMENT

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
	virtual cl::Buffer getBuffer() = 0;
	virtual std::string getType() = 0;

	// operator == or compareWith().
};

#endif
