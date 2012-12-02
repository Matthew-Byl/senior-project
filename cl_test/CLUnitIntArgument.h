#ifndef _CL_UNIT_INT_ARGUMENT
#define _CL_UNIT_INT_ARGUMENT

// TODO: make a big header file.
#include "CLFunction.h"
#include "CLUnitArgument.h"

class CLUnitIntArgument : public CLUnitArgument
{
public:
	CLUnitIntArgument( CLContext &context, cl_int value );
	cl::Buffer &getBuffer();
	std::string getType();
	
private:
	cl_int myValue;
	cl::Buffer myBuffer;
};

#endif
