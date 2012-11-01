/**
 * Unit testing class for OpenCL
 *
 * @author John Kloosterman
 * @date November 1, 2012
 */

#include "CLUnit.h"

CLUnit::CLUnit( std::string kernel, std::string src )
{
	// Check to make sure non-empty?

	myKernel = kernel;
	mySrc = src;
}

void CLUnit::test()
{

}
