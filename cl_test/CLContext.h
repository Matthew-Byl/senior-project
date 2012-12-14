#ifndef _CL_CONTEXT_H
#define _CL_CONTEXT_H

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>

/* Convenience class for an OpenCL context, because
   we need lots of interdependent objects to do anything.
*/

class CLContext
{
public:
	CLContext();
	CLContext( int platform, int device );
	cl::Context getContext() const;
	cl::CommandQueue getCommandQueue() const;
	cl::Program buildProgram( std::string &src ) const;
private:
	cl::Context myContext;
	cl::CommandQueue myCommandQueue;
	std::vector<cl::Device> myDevices;
};

#endif
