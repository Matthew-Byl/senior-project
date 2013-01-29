#include "CLContext.h"
#include <iostream>
using namespace std;

CLContext::CLContext()
{
	initialize( 1, 0 );
}

CLContext::CLContext( int platform, int device )
{
	initialize( platform, device );
}

void CLContext::initialize( int platform, int device )
{
	vector<cl::Platform> platforms;
	cl::Platform::get( &platforms );

<<<<<<< local
    platforms[platform].getDevices( CL_DEVICE_TYPE_CPU, &myDevices );
=======
    platforms[platform].getDevices( CL_DEVICE_TYPE_ALL, &myDevices );
>>>>>>> other
	
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

cl::Program CLContext::buildProgram( string &src ) const
{
//	cout << "*** rebuilding! ***" << endl;

	cl::Program::Sources sources(
        1,
        std::pair<const char *, int>( src.c_str(), (int) src.length() )
        );

	cl::Program program( myContext, sources );
    try {
        program.build( myDevices );
    } catch ( cl::Error err ) {		
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

	// @TODO: this should be myDevices[device].
/*
		std::cout << "Build Status: "
                  << program.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(myDevices[0])
                  << std::endl;
		std::cout << "Build Options:\t"
                  << program.getBuildInfo<CL_PROGRAM_BUILD_OPTIONS>(myDevices[0])
                  << std::endl;
		std::cout << "Build Log:\t "
                  << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(myDevices[0])
                  << std::endl;
*/

	return program;
}
