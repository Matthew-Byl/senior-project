#include "CLFunction.h"
#include "CLUnitIntArgument.h"

const char *src = 
	"int add_one( int i ) " \
	"{" \
	"   return i + 1;" \
	"}";

int main( void )
{
	CLContext context( 0, 0 );
	CLFunction add_one( "add_one", src, context );
	CLUnitIntArgument arg( context, 1 );

	add_one.addArgument( arg );
	add_one.run();
}
