#include "CLContext.h"
#include <iostream>
#include <cstdlib>
using namespace std;

CLContext::CLContext()
{
	initialize( 1, 0 );
}

CLContext::CLContext( int platform, int device )
{
	initialize( platform, device );
}

/**
 * Intialize this CLContext with a given platform
 *  and device.
 *
 * @TODO: device is not actually selectable?
 *
 * @param platform, device
 *  The OpenCL platform and device numbers
 *  to use for this context.
 */
void CLContext::initialize( int platform, int device )
{
	char *cl_debug = getenv( "CL_DEBUG" );
	if ( cl_debug 
		 && strcmp( cl_debug , "1" ) == 0 )
	{
		cout << "CL_DEBUG set: using CPU and outputting compiler output." << endl;
		myDebug = true;
	}
	else
		myDebug = false;

	vector<cl::Platform> platforms;
	cl::Platform::get( &platforms );

	if ( myDebug )
		platforms[platform].getDevices( CL_DEVICE_TYPE_CPU, &myDevices );	
	else
	{
		try {
			platforms[platform].getDevices( CL_DEVICE_TYPE_GPU, &myDevices );	
		} catch ( cl::Error e ) {
			// If we got no GPU devices, get a CPU.
			cout << "No GPU, so using a CPU." << endl;
			platforms[platform].getDevices( CL_DEVICE_TYPE_CPU, &myDevices );
		}
	}

	myContext = cl::Context( myDevices, NULL, NULL, NULL );
	myCommandQueue = cl::CommandQueue( myContext, myDevices[device], 0 );
}

cl::Context &CLContext::getContext()
{
	return myContext;
}

cl::CommandQueue &CLContext::getCommandQueue()
{
	return myCommandQueue;
}

/**
 * Compile OpenCL source for this platform.
 *
 * @param src
 *  The source code to compile.
 * @param compiler_flage
 *  Flags to pass to the OpenCL compiler.
 *
 * @return
 *  A cl::Program
 */
cl::Program CLContext::buildProgram( string &src, string &compiler_flags ) const
{
//	cout << "*** rebuilding! ***" << endl;

	cl::Program::Sources sources(
        1,
        std::pair<const char *, int>( src.c_str(), (int) src.length() )
        );

	cl::Program program( myContext, sources );

    try {
		if ( myDebug )
		{
			program.build( 
				myDevices,
				( "-g -O0 " + compiler_flags ).c_str()
				);

			// In debug mode, show all status
			std::cout << "Build Status: "
					  << program.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(myDevices[0])
					  << std::endl;
			std::cout << "Build Options:\t"
					  << program.getBuildInfo<CL_PROGRAM_BUILD_OPTIONS>(myDevices[0])
					  << std::endl;
			std::cout << "Build Log:\t "
					  << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(myDevices[0])
					  << std::endl;
		}
		else
		{
			program.build( myDevices, compiler_flags.c_str() );
		}
    } 
	catch ( cl::Error err ) 
	{
		// If there was a compiler error, spit out the errors.
		std::cout << "Build Status: "
                  << program.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(myDevices[0])
                  << std::endl;
		std::cout << "Build Options:\t"
                  << program.getBuildInfo<CL_PROGRAM_BUILD_OPTIONS>(myDevices[0])
                  << std::endl;
		std::cout << "Build Log:\t "
                  << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(myDevices[0])
                  << std::endl;

		throw new exception();
    }

	return program;
}
