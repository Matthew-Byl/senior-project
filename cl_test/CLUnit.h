#ifndef _CLUNIT_H
#define _CLUNIT_H

#include <iostream>
#include <vector>

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>

#include "CLUnitArgument.h"

class CLUnit
{
public:
	CLUnit( std::string function, std::string src );
	CLUnit( std::string function, std::istream &src );
	~CLUnit(); // TODO: free the pointers passed in.

/*
	template<class T>
		void addArgument( std::string type_name, T arg );

	template<class T>
		void addArrayArgument( std::string type_name, T *arg, unsigned elements );
*/

	void test();

private:
	/* Be careful to abstract anything that would be
	   useful to overload into its own function. */
	bool isVerbose = true;

	std::string myFunction;
	std::string mySrc;

	std::vector<CLUnitArgument> myArguments;

/*
    std::vector<BufferInfo> myBuffers;
	void addBuffer();
*/

	void testForDevice( cl::CommandQueue queue );
/*	bool compareBuffers( bufferInfo 1, bufferInfo 2 ); */
};

/*
template<class T>
void CLUnit::addArgument( std::string type_name, T arg )
{
	// This has to be an array so it can be freed like arrays with delete[]
	//  in the destructor.
	T *ptr = new T[1];
	ptr[0] = arg;

	CLUnit::CLUnitArgument argument;
	argument.type_name = type_name;
	argument.ptr = (void *) ptr;
	argument.size = sizeof( T );

	myArguments.push_back( arg );
}

template<class T>
void CLUnit::addArrayArgument( std::string type_name, T *arg, unsigned elements )
{
	
}
*/

#endif
