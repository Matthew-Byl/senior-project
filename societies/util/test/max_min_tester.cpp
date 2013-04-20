/**
 * Tests the parallel max function used to find
 *  maximum gain per minute.
 */

#define _CPP_11_
#include <CLKernel.h>
#include <fstream>
#include <iostream>
using namespace std;

#define KERNEL_SOURCE "max_min_tester.cl"

void test_first_pass( string src )
{
	cout << "Testing max_index_first_pass()... ";

	CLKernel max_min_first_pass_tester( "max_min_first_pass_tester", src );
	max_min_first_pass_tester.setGlobalDimensions( 8, 1 );
	max_min_first_pass_tester.setLocalDimensions( 8, 1 );
	cl_uint *tree;

	// Power of two
	cl_float host_floats1[] = 
		{ 24, 6.333, 2.1, 9.76, 2.11, 44.2224, 11000, 23 };
	tree = new cl_uint[4];

    CLArgument floats1( host_floats1, 8 );
    CLArgument floats1_sort_tree( tree, 4 );
	max_min_first_pass_tester( floats1, floats1_sort_tree );

	cl_uint host_floats1_tree[4] = 
		{ 0, 3, 5, 6 };
	for ( int i = 0; i < 4; i++ )
		assert( tree[i] == host_floats1_tree[i] );
	delete[] tree;
	cout << "* " << flush;

	// Array size an odd number.
	cl_float host_floats2[] = 
		{ 54.2, 9000, 223.5, 222, 43 };
	tree  = new cl_uint[3];

    CLArgument floats2( host_floats2, 5 );
    CLArgument floats2_sort_tree( tree, 3 );
	max_min_first_pass_tester.setGlobalDimensions( 5, 1 );
	max_min_first_pass_tester.setLocalDimensions( 5, 1 );
	max_min_first_pass_tester( floats2, floats2_sort_tree );
	
	cl_uint host_floats2_tree[3] =
		{ 1, 2, 4 };

	for ( int i = 0; i < 3; i++ )
	{
		assert( tree[i] == host_floats2_tree[i] );
	}
	delete[] tree;
	cout << "* " << flush;
	cout << "All tests passed." << endl << flush;
}

void test_function( string src )
{
	cout << "Testing max_index()... ";

	CLKernel max_tester( "max_tester", src );
	cl_uint host_max;
	CLArgument max( &host_max, 1 );

	// Nice power of two
	cl_float host_floats1[] = 
		{ 24, 6.333, 2.1, 9.76, 2.11, 44.2224, 11000, 23 };
	cl_uint host_floats1_tree[4];

    CLArgument floats1( host_floats1, 8 );
    CLArgument floats1_tree( host_floats1_tree, 4 );
	max_tester.setGlobalDimensions( 8, 1 );
	max_tester.setLocalDimensions( 8, 1 );
	max_tester( floats1, floats1_tree, max );

	cl_uint floats1_correct_tree[4] = { 6, 3, 6, 6 };
	for ( int i = 0; i < 4; i++ )
	{
		assert( host_floats1_tree[i] == floats1_correct_tree[i] );
	}
	assert( host_max == 6 );
	cout << "* ";

	// Odd number
	cl_float host_floats2[] = 
		{ 54.2, 43, 223.5, 222, 9000 };
	cl_uint host_floats2_tree[3];

    CLArgument floats2( host_floats2, 5 );
    CLArgument floats2_sort_tree( host_floats2_tree, 3 );
	max_tester.setGlobalDimensions( 5, 1 );
	max_tester.setLocalDimensions( 5, 1 );
	max_tester( floats2, floats2_sort_tree, max );
	
	cl_uint floats2_correct_tree[3] =
		{ 4, 2, 4 };

	for ( int i = 0; i < 3; i++ )
	{
		assert( host_floats2_tree[i] == floats2_correct_tree[i] );
	}
	assert( host_max == 4 );
	cout << "* ";

	cout << "All tests passed." << endl << flush;
}

void test_mask( string src )
{
	cout << "Testing masking... ";

	CLKernel mask_tester( "max_min_mask_tester", src );
	cl_uint host_min;
	CLArgument min( &host_min, 1 );
	cl_uint host_min_no_mask;
	CLArgument min_no_mask( &host_min_no_mask, 1 );

	cl_float host_values[20] = 
		{
			-27.342,
			73.887,  // masked
			-61.280,
			88.231, // masked
			-57.100,
			-564.927, // masked, attractive min
			-42.691,
			-97.121, // masked
			-2.643,
			8.741, // masked
			18.538,
			26.442, // masked
			36.100,
			42.445, // masked
			-104.102,  // actual min
			48.808, // masked
			63.522,
			-58.05, // masked
			69.232,
			100.676 // masked
		};

	CLArgument values( host_values, 20 );
	mask_tester.setGlobalDimensions( 20, 1 );
	mask_tester.setLocalDimensions( 20, 1 );
	mask_tester( values, min, min_no_mask );


	assert( host_min == 14 );
	cout << "* " << flush;
	assert( host_min_no_mask == 5 );
	cout << "* " << flush;
	
	cout << "All tests passed!" << endl << flush;
}

void test_n_max_indices( string src )
{
	cout << "Testing n_max_indices()... ";

	CLKernel n_max_indices_tester( "n_max_indices_tester", src );
	cl_uint host_results[6];
	CLArgument results( host_results, 6 );

	cl_float host_values[20] = 
		{
			-27.342,
			73.887,
			-61.280,
			88.231,
			-57.100,
			-564.927,
			-42.691,
			-97.121,
			-2.643,
			8.741,
			18.538,
			26.442,
			36.100,
			42.445,
			-104.102,
			48.808,
			63.522,
			-58.05,
			69.232,
			100.676
		};

	CLArgument values( host_values, 20 );
	n_max_indices_tester.setGlobalDimensions( 20, 1 );
	n_max_indices_tester.setLocalDimensions( 20, 1 );
	n_max_indices_tester( values, results );

	assert( host_results[0] == 19 );
	assert( host_results[1] == 3 );
	assert( host_results[2] == 1 );
	assert( host_results[3] == 18 );
	assert( host_results[4] == 16 );
	assert( host_results[5] == 15 );
	cout << "* " << flush;

	// Duplicate values; make sure they're not reused.
	for ( int i = 0; i < 20; i++ )
		host_values[i] = 5;
	n_max_indices_tester( values, results );
	
	cout << endl;
	for ( int i = 0; i < 6; i++ )
	{
		cout << host_results[i] << " ";
	}
	cout << endl;
	
	cout << "All tests passed!" << endl << flush;
}

int main ( void )
{
	cout << "max_min_tester.cpp: Test the maximum and minimum functions and dependencies." << endl;

	// Open OpenCL kernel
	ifstream t( KERNEL_SOURCE );
	string src((std::istreambuf_iterator<char>(t)),
			   std::istreambuf_iterator<char>());

	test_first_pass( src );
	test_function( src );
	test_mask( src );
	test_n_max_indices( src );
}
