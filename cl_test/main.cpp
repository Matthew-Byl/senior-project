#include "CLFunction.h"
#include "CLUnitArgument.h"

const char *src = 
	"int add_one( int3 i ) " \
	"{" \
	"   printf(\"From OpenCL: %d\\n\", i.x ); " \
	"   return i.x + 1;" \
	"}";

int main( void )
{
	CLContext context( 0, 0 );
	CLFunction add_one( "add_one", src, context );
	cl_int3 intr;
	intr.s[0] = 1;
	CLUnitArgument arg( intr );
	cl_int result;

	add_one.addArgument( arg );
	result = add_one.run<cl_int>( "int" );

	printf( "Result: %d\n", result );
}
