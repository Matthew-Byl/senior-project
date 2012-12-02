#include "CLUnitIntArgument.h"

CLUnitIntArgument::CLUnitIntArgument( CLContext context, cl_int value )
	: myValue( value ),
	  myBuffer( context.getContext(),
				CL_MEM_READ_ONLY,
				sizeof( cl_int ),
				&myValue )
{

}

cl::Buffer &CLUnitIntArgument::getBuffer()
{
	return myBuffer;
}

std::string CLUnitIntArgument::getType()
{
	return "int";
}
