#ifndef _CLUNIT_H
#define _CLUNIT_H

#include <iostream>
#include <vector>

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>

class CLUnit
{
public:
	CLUnit( std::string function, std::string src );
	CLUnit( std::string function, std::istream src );

	void test();

private:
	/* Be careful to abstract anything that would be
	   useful to overload into its own function. */
	bool isVerbose = true;

	std::string myFunction;
	std::string mySrc;

/*
    std::vector<BufferInfo> myBuffers;
	void addBuffer();
*/

	void testForDevice( cl::CommandQueue queue );
/*	bool compareBuffers( bufferInfo 1, bufferInfo 2 ); */
};

#endif
