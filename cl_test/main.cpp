#include "CLFunction.h"
#include "CLUnitArgument.h"

#include <vector>
#include <initializer_list>
using namespace std;

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
	CLFunction<void> some_kernel( "some_kernel", src, true );

	cl_int3 intr;
	intr.s[0] = 2;
	cl_int result;

	result = add_one( 1, intr );
	printf( "Result: %d\n", result );

	some_kernel( 1, intr );
}
