/**
 * Tests the parallel max function used to find
 *  maximum gain per minute.
 */

#define _CPP_11_
#include <CLKernel.h>
#include <fstream>
#include <iostream>
using namespace std;

#define KERNEL_SOURCE "max_index_tester.cl"

void test_first_pass( string src )
{
	cout << "Testing max_index_first_pass()... ";

	CLKernel max_index_first_pass_tester( "max_index_first_pass_tester", src );
	max_index_first_pass_tester.setGlobalDimensions( 8, 1 );
	max_index_first_pass_tester.setLocalDimensions( 8, 1 );
	cl_uchar *tree;

	// Power of two
	cl_float host_floats1[] = 
		{ 24, 6.333, 2.1, 9.76, 2.11, 44.2224, 11000, 23 };
	tree = new cl_uchar[4];

    CLUnitArgument floats1( host_floats1, 8 );
    CLUnitArgument floats1_sort_tree( tree, 4 );
	max_index_first_pass_tester( floats1, floats1_sort_tree );

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

    CLUnitArgument floats2( host_floats2, 5 );
    CLUnitArgument floats2_sort_tree( tree, 3 );
	max_index_first_pass_tester.setGlobalDimensions( 5, 1 );
	max_index_first_pass_tester.setLocalDimensions( 5, 1 );
	max_index_first_pass_tester( floats2, floats2_sort_tree );
	
	cl_uchar host_floats2_tree[3] =
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

	CLKernel max_index_tester( "max_index_tester", src );
	cl_uchar host_max;
	CLUnitArgument max( &host_max, 1 );

	// Nice power of two
	cl_float host_floats1[] = 
		{ 24, 6.333, 2.1, 9.76, 2.11, 44.2224, 11000, 23 };
	cl_uchar host_floats1_tree[4];

    CLUnitArgument floats1( host_floats1, 8 );
    CLUnitArgument floats1_tree( host_floats1_tree, 4 );
	max_index_tester.setGlobalDimensions( 8, 1 );
	max_index_tester.setLocalDimensions( 8, 1 );
	max_index_tester( floats1, floats1_tree, max );

	cl_uchar floats1_correct_tree[4] = { 6, 3, 6, 6 };
	for ( int i = 0; i < 4; i++ )
	{
		assert( host_floats1_tree[i] == floats1_correct_tree[i] );
	}
	assert( host_max == 6 );
	cout << "* ";

	// Odd number
	cl_float host_floats2[] = 
		{ 54.2, 43, 223.5, 222, 9000 };
	cl_uchar host_floats2_tree[3];

    CLUnitArgument floats2( host_floats2, 5 );
    CLUnitArgument floats2_sort_tree( host_floats2_tree, 3 );
	max_index_tester.setGlobalDimensions( 5, 1 );
	max_index_tester.setLocalDimensions( 5, 1 );
	max_index_tester( floats2, floats2_sort_tree, max );
	
	cl_uchar floats2_correct_tree[3] =
		{ 4, 2, 4 };

	for ( int i = 0; i < 3; i++ )
	{
		assert( host_floats2_tree[i] == floats2_correct_tree[i] );
	}
	assert( host_max == 4 );
	cout << "* ";

	cout << "All tests passed." << endl << flush;
}

int main ( void )
{
	cout << "max_index_tester.cpp: Test the max_index() function and dependencies." << endl;

	// Open OpenCL kernel
	ifstream t( KERNEL_SOURCE );
	string src((std::istreambuf_iterator<char>(t)),
			   std::istreambuf_iterator<char>());

	test_first_pass( src );
	test_function( src );
}
