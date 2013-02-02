/**
 * Tests the parallel max function used to find
 *  maximum gain per minute
 */

#define _CPP_11_
#include <CLKernel.h>
#include <fstream>
#include <iostream>
using namespace std;

#define KERNEL_SOURCE "../resource-extraction.c"

int main ( void )
{
	// Open OpenCL kernel
	ifstream t( KERNEL_SOURCE );
	string src((std::istreambuf_iterator<char>(t)),
			   std::istreambuf_iterator<char>());
	CLKernel max_gain_per_minute_first_pass_tester( "max_gain_per_minute_first_pass_tester", src );
	max_gain_per_minute_first_pass_tester.setGlobalDimensions( 8, 1 );
	max_gain_per_minute_first_pass_tester.setLocalDimensions( 8, 1 );
	cl_uchar *tree;

	// Power of two
	cl_float host_floats1[] = 
		{ 24, 6.333, 2.1, 9.76, 2.11, 44.2224, 11000, 23 };
	tree = new cl_uchar[4];

    CLUnitArgument floats1( host_floats1, 8 );
    CLUnitArgument floats1_sort_tree( tree, 4 );
	max_gain_per_minute_first_pass_tester( floats1, floats1_sort_tree );

	for ( int i = 0; i < 4; i++ )
	{
		cout << (int) tree[i] << " ";
	}
	cout << endl << flush;

	cl_uchar host_floats1_tree[4] = 
		{ 0, 3, 5, 6 };
	for ( int i = 0; i < 4; i++ )
		assert( tree[i] == host_floats1_tree[i] );

}
