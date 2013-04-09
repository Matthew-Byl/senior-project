/**
 * vector_add is a simple example program that uses the clTest
 *  framework to add two arrays of floats using OpenCL.
 */

#include <CLKernel.h>
#include <cassert>
#include <iostream>
#include <fstream>
using namespace std;

int main ( void )
{
	// Declare the input arrays and initialize them.
	const int ARRAY_SIZE = 16384;
	cl_float host_a[ARRAY_SIZE];
	cl_float host_b[ARRAY_SIZE];

	for ( int i = 0; i < ARRAY_SIZE; i++ )
	{
		host_a[i] = i;
		host_b[i] = i;
	}

	// Declare the output array.
	cl_float host_result[ARRAY_SIZE];

	// Load the source code of the OpenCL kernel into a string.
	ifstream t( "vector_add.cl" );
    string src((std::istreambuf_iterator<char>(t)),
               std::istreambuf_iterator<char>());

	// Create a CLKernel for the OpenCL kernel.
	CLKernel vector_add( "vector_add", src );

	// Set the global dimensions (i.e. how many instances of
	//  the kernel will be run). We do not set a local (workgroup)
	//  dimension, which means the runtime will choose an optimal
	//  size for us.
	vector_add.setGlobalDimensions( ARRAY_SIZE );

	// Create CLUnitArguments that encapsulate the data in the input
	//  and output arrays.
	//
	// The framework has templates for cl_float, so it is able to infer
	//  the size of the data in the arrays automatically.
	// The default is to copy data both to and from the device. In this
	//  case, copying it from the device is unnecessary.
	CLUnitArgument a( host_a, ARRAY_SIZE );
	CLUnitArgument b( host_b, ARRAY_SIZE );

	// This CLUnitArgument could have been declared like the other two,
	//  but this is how to declare CLUnitArguments for arbitrary
	//  types.
	CLUnitArgument result( 
		"float",                          // type on device side
		host_result,                      // pointer to data
		sizeof( cl_float ) * ARRAY_SIZE,  // size of data
		false,                            // copy to device
		true                              // copy from device
		);
		
	// Call the kernel.
	vector<CLUnitArgument> args;
	args.push_back( a );
	args.push_back( b );
	args.push_back( result );
	vector_add( args );

	// Output results.
	for ( int i = 0; i < ARRAY_SIZE; i++ )
	{
		cout << host_a[i] << " + " << host_b[i] << " = " << host_result[i] << endl;
		assert( host_a[i] + host_b[i] == host_result[i] );
	}

	return 0;
}
