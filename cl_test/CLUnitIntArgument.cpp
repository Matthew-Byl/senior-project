#include "CLUnitIntArgument.h"
#include <iostream>
using namespace std;

CLUnitIntArgument::CLUnitIntArgument( CLContext &context, cl_int value )
	: myValue( value )
{
	myBuffer = cl::Buffer(
		context.getContext(),
		CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR,
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

void CLUnitIntArgument::enqueue( cl::CommandQueue &queue )
{
	cout << "Enqueuing " << myValue << "... " << endl;

	queue.enqueueWriteBuffer(
		myBuffer,
		CL_TRUE,
		0,
		sizeof( cl_int ),
		&myValue
	);
}
