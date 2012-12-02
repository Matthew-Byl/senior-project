#ifndef _CL_FUNCTION_H
#define _CL_FUNCTION_H

#include "CLContext.cpp"
#include "CLUnitArgument.h"

/* Encapsulates an OpenCL function that can be called like normal code. */
class CLFunction
{
public:
	CLFunction( std::string function, std::string kernel, CLContext &context );
	void addArgument( const CLUnitArgument &argument );
	void run();
private:
	CLContext &myContext;
	std::vector<CLUnitArgument> myArguments;
	std::string myFunction;
	std::string myKernel;
};

#endif
