/**
 * Tests the parallel max function used to find
 *  maximum gain per minute.
 */

#define _CPP_11_
#include <CLKernel.h>
#include <fstream>
#include <iostream>
using namespace std;

#define KERNEL_SOURCE "../resource-extraction.c"

void test_first_pass( string src )
{
	cout << "Testing max_gain_per_minute_first_pass()... ";

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

	cl_uchar host_floats1_tree[4] = 
		{ 0, 3, 5, 6 };
	for ( int i = 0; i < 4; i++ )
		assert( tree[i] == host_floats1_tree[i] );
	delete[] tree;
	cout << "* " << flush;

	// Array size an odd number.
	cl_float host_floats2[] = 
		{ 54.2, 9000, 223.5, 222, 43 };
	tree  = new cl_uchar[3];

    CLUnitArgument floats2( host_floats2, 8 );
    CLUnitArgument floats2_sort_tree( tree, 4 );
	max_gain_per_minute_first_pass_tester( floats2, floats2_sort_tree );
	
	cl_uchar host_floats2_tree[3] =
		{ 1, 2, 4 };

	for ( int i = 0; i < 3; i++ )
		assert( tree[i] == host_floats2_tree[i] );
	delete[] tree;
	cout << "* " << flush;
	cout << "All tests passed." << endl << flush;
}

int main ( void )
{
	cout << "max_gains_per_minute.cpp: Test the max_gains_per_minute() function and dependencies." << endl;

	// Open OpenCL kernel
	ifstream t( KERNEL_SOURCE );
	string src((std::istreambuf_iterator<char>(t)),
			   std::istreambuf_iterator<char>());

	test_first_pass( src );
}
