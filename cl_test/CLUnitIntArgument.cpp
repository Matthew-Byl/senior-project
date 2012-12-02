#include "CLUnitIntArgument.h"

CLUnitIntArgument::CLUnitIntArgument( CLContext &context, cl_int value )
	: myValue( value )
{
	cl::Context ctx = context.getContext();

	myBuffer = cl::Buffer(
		ctx,
		CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
		sizeof( cl_int ),
		&myValue
		);
}

cl::Buffer &CLUnitIntArgument::getBuffer()
{
	return myBuffer;
}

std::string CLUnitIntArgument::getType()
{
	return "int";
}
