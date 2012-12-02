#ifndef _CL_FUNCTION_H
#define _CL_FUNCTION_H

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>
#include "CLUnitArgument.h"

/* Encapsulates an OpenCL function that can be called like normal code. */
class CLFunction
{
public:
	CLFunction( std::string function, std::string kernel );
	void addArgument( const CLUnitArgument &argument );
	void run();
private:
	std::vector<CLUnitArgument> myArguments;
	std::string myFunction;
	std::string myKernel;
};

#endif
