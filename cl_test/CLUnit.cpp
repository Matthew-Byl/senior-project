/**
 * Unit testing class for OpenCL
 *
 * @author John Kloosterman
 * @date November 1, 2012
 */

#include "CLUnit.h"
using namespace std;

CLUnit::CLUnit( string function, string src )
{
	// Check to make sure non-empty?

	myFunction = function;
	mySrc = src;
}

CLUnit::CLUnit( string function, istream instream )
{
	// Check to make sure non-empty?

	myFunction = function;

	// Is this the right syntax?
	mySrc = string( istreambuf_iterator<char>( instream ), istreambuf_iterator<char>() );
}

/*
 * Idea: run all the tests in parallell so we get
 *  done faster.
 */
void CLUnit::test()
{
	// Source doesn't depend on anything.
	// For some reason, make_pair doesn't work. I have no clue why.
	cl::Program::Sources sources( 
		1,
		std::pair<const char *, int>( mySrc.c_str(), mySrc.length() + 1 )
	);

	// Iterate through all devices on all platforms.
	vector<cl::Platform> platforms;
	cl::Platform::get( &platforms );

	for ( cl::Platform &platform : platforms )
	{
		if ( isVerbose )
		{
			string platform_name;
			platform.getInfo( CL_PLATFORM_NAME, &platform_name );
			cout << "Testing platform " << platform_name << endl << flush;
		}

		vector<cl::Device> devices;
		platform.getDevices( CL_DEVICE_TYPE_ALL, &devices );

		// Add error callback later.
		cl::Context context( devices );
		
		cl::Program program( context, sources );
		program.build( devices );
		// Show error messages, if any.
		
		
	}
}
