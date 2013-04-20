/**
 * Tests the parallel n_minimum function
 */

#define _CPP_11_
#include <CLKernel.h>
#include <fstream>
#include <iostream>
using namespace std;

#define KERNEL_SOURCE "n_minimum_tester.cl"

int main ( void )
{
	cout << "max_index_tester.cpp: Test the max_index() function and dependencies." << endl;

	// Open OpenCL kernel
	ifstream t( KERNEL_SOURCE );
	string src((std::istreambuf_iterator<char>(t)),
			   std::istreambuf_iterator<char>());

	CLKernel n_minimum_tester( "n_minimum_tester", src );

	cl_uint host_n = 6;
	cl_float host_values1[20] = {
		88.231,
		-84.102,
		-58.05,
		69.232,
		48.808,
		8.741,
		64.927,
		-2.643,
		-57.100,
		-42.691,
		73.887,
		100.676,
		63.522,
		18.538,
		42.445,
		36.100,
		-97.121,
		26.442,
		-61.280,
		-27.342
	};
	cl_float host_mins1[6];
	cl_uchar host_indices1[6];

	n_minimum_tester.setLocalArgument( 4, sizeof( cl_float ) * 6 );
	n_minimum_tester.setLocalArgument( 5, sizeof( cl_uchar ) * 6 );
	n_minimum_tester.setLocalArgument( 6, sizeof( cl_int ) * 6 );

	CLArgument values1( host_values1, 20 );
	CLArgument n( &host_n , 1 );
	CLArgument mins1( host_mins1, 6 );
	CLArgument indices1( host_indices1, 6 );

	n_minimum_tester.setGlobalDimensions( 20, 1 );
	n_minimum_tester.setLocalDimensions( 20, 1 );
	n_minimum_tester( n, values1, mins1, indices1 );

	for ( int i = 0; i < 6; i++ )
	{
		cout << host_mins1[i] << ", index " << (int) host_indices1[i] << endl;
	}
}
