#include "CLFunction.h"
#include "CLUnitIntArgument.h"

const char *src = 
	"int add_one( int i ) " \
	"{" \
	"   printf(\"From OpenCL: %d\\n\", i ); " \
	"   return i + 1;" \
	"}";

int main( void )
{
	CLContext context( 0, 0 );
	CLFunction add_one( "add_one", src, context );
	CLUnitIntArgument arg( context, 1 );
	cl_int result;

	add_one.addArgument( arg );
	result = add_one.run<cl_int>( "int" );

	printf( "Result: %d\n", result );
}
