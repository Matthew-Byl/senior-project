/**
 * Class to generate an OpenCL kernel
 *  that calls an non-kernel OpenCL function.
 *
 * @author John Kloosterman
 * @date December 2012
 */

#ifndef _KERNEL_GENERATOR_H
#define _KERNEL_GENERATOR_H

#include <string>
#include <vector>

#include "CLUnitArgument.h"

class KernelGenerator
{
public:
	KernelGenerator( 
		std::string function, 
		std::vector<CLUnitArgument> &arguments, 
		std::string returnType );
	std::string generate();
	std::string getKernelFunction();

private:
	std::string myFunction;
	std::string myReturnType;
	std::vector<CLUnitArgument> &myArguments;

	std::string intToString( int i );
};

#endif
