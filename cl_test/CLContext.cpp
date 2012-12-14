#include "CLContext.h"
#include <iostream>
using namespace std;

CLContext::CLContext()
	: CLContext( 0, 0 )
{

}

CLContext::CLContext( int platform, int device )
{
	vector<cl::Platform> platforms;
	cl::Platform::get( &platforms );

    platforms[0].getDevices( CL_DEVICE_TYPE_ALL, &myDevices );
	
	myContext = cl::Context( myDevices, NULL, NULL, NULL );
	myCommandQueue = cl::CommandQueue( myContext, myDevices[0], 0 );
}

cl::Context CLContext::getContext() const
{
	return myContext;
}

cl::CommandQueue CLContext::getCommandQueue() const
{
	return myCommandQueue;
}

cl::Program CLContext::buildProgram( string &src ) const
{
	cl::Program::Sources sources(
        1,
        std::pair<const char *, int>( src.c_str(), src.length() + 1 )
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

	return program;
}
