#include "CLFunction.h"
#include "CLKernel.h"

#include <vector>
#include <iostream>
using namespace std;

/*
const char *src = 
	" __kernel void some_kernel( __global int *i, __global int3 *j ) { printf( \"From kernel: %d\", *i + (*j).x ); } \n" \
	"int add_one( int i, int3 j ) " \
	"{" \
	"   printf(\"From OpenCL: %d %d\\n\", i, j.x ); " \
	"   return i + j.x;" \
	"}";

int main( void )
{
	CLFunction<int> add_one( "add_one", src );
	CLKernel some_kernel( "some_kernel", src );

	cl_int3 intr;
	intr.s[0] = 2;
	cl_int result;

	result = add_one( 1, intr );
	printf( "Result: %d\n", result );

	some_kernel( 1, intr );
}
*/

const char *src = 
	"__kernel void fill_numbers( __global int *i ) " \
	"{"												 \
	"   int idx = get_global_id( 0 ); "				 \
	"   i[idx] = idx; "								 \
	"}";

int main( void )
{
	CLKernel fill_numbers( "fill_numbers", src );
	int numbers[100];
	CLUnitArgument numbers_arg( "int *", sizeof( numbers ), numbers, false );

	fill_numbers.setDimensions( 100 );
	fill_numbers( numbers_arg );

	for ( int i = 0; i < 100; i++ )
	{
		cout << i << " ";
	}

	cout << endl;
}
	
