#ifndef _KERNEL_GENERATOR_H
#define _KERNEL_GENERATOR_H

#include <string>
#include <vector>

#include "CLUnit.h"
#include "CLUnitArgument.h"

/*
 * A class to generate an OpenCL kernel that
 *  can test a non-kernel function.
 */

class KernelGenerator
{
public:
	KernelGenerator( std::string function, std::vector<CLUnitArgument> &arguments );
	std::string generate();

private:
	std::string myFunction;
	std::vector<CLUnitArgument> &myArguments;
};

#endif
