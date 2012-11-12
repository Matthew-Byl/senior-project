#include "CLUnit.h"
#include "CLUnitValueArgument.h"

const char *src = 
	"__kernel void someFunction( void ) "		\
	"{  int i; i = 0; i++; } ";

int main( void )
{
	CLUnit test( "someFunction", src );
	test.test();
}
