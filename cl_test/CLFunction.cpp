#include "CLFunction.h"
#include "KernelGenerator.h"
using namespace std;

CLFunction::CLFunction( std::string function, std::string kernel )
	: myFunction( function), myKernel( kernel )
{

}

void CLFunction::addArgument( const CLUnitArgument &argument )
{
	myArguments.push_back( argument );
}

void CLFunction::run()
{
	KernelGenerator generator( myFunction, myArguments );
	string src( myKernel + "\n" + generator.generate() );

	vector<cl::Platform> platforms;
	cl::Platform::get( &platforms );
	cl::Platform platform = platforms[0];

	vector<cl::Device> devices;
	platform.getDevices( CL_DEVICE_TYPE_ALL, &devices );

	cl::Context context( devices );

    // Source doesn't depend on anything.
    // For some reason, make_pair doesn't work. I have no clue why.
	cl::Program::Sources sources(
        1,
        std::pair<const char *, int>( src.c_str(), src.length() + 1 )
		);

	cl::Program program( context, sources );
	try {
		program.build( devices );
	} catch ( cl::Error err ) {
		std::cout << "Build Status: " 
				  << program.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(devices[0]) 
				  << std::endl;
		std::cout << "Build Options:\t" 
				  << program.getBuildInfo<CL_PROGRAM_BUILD_OPTIONS>(devices[0]) 
				  << std::endl;
		std::cout << "Build Log:\t " 
				  << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[0]) 
				  << std::endl;
		return;
	}

	cl::Device device = devices[0];
	cl::CommandQueue queue( context, device, 0 );
	cl::Kernel kernel(
		program,
		generator.getKernelFunction().c_str()
	);

}

