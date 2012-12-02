#include "CLUnitArgument.h"

cl::Buffer &CLUnitArgument::getBuffer()
{
	return * new cl::Buffer();
}

std::string CLUnitArgument::getType()
{
	return "Override me!";
}
