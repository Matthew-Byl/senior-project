#ifndef _CLUNIT_H
#define _CLUNIT_H

#include <vector>
#include <CL/cl.hpp>

class CLUnit
{
public:
	CLUnit( std::string kernel, std::string src );
	void test();

private:
	/* Be careful to abstract anything that would be
	   useful to overload into its own function. */

	std::string myKernel;
	std::string mySrc;

/*
    std::vector<BufferInfo> myBuffers;
	void addBuffer();
*/

	void testInContext( cl::Context ctx );
/*	bool compareBuffers( bufferInfo 1, bufferInfo 2 ); */
};

#endif
