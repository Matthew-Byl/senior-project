/**
 * Abstraction of a cl::Context. CLFunctions and
 *  CLKernels can take custom contexts, if you want
 *  to run on a platform and device other than the default.
 *
 * @author John Kloosterman
 * @date December 2012
 */

#ifndef _CL_CONTEXT_H
#define _CL_CONTEXT_H

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>

class CLContext
{
public:
	CLContext();
	CLContext( int platform, int device );
	cl::Context &getContext();
	cl::CommandQueue &getCommandQueue();
	cl::Program buildProgram( std::string &src, std::string &compiler_flags ) const;
private:
	void initialize( int platform, int device );
	cl::Context myContext;
	cl::CommandQueue myCommandQueue;
	std::vector<cl::Device> myDevices;
	bool myDebug;
};

#endif
