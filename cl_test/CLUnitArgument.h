#ifndef _CL_UNIT_ARGUMENT
#define _CL_UNIT_ARGUMENT

#include "CLUnit.h"

class CLUnitArgument
{
public:
	virtual void updateFromBuffer( cl::Buffer &buffer ) = 0;
	virtual cl::Buffer getBuffer() = 0;
	virtual std::string getType() = 0;

	// operator == or compareWith().
};

#endif
