#include "CLFunction.h"

const char *src = 
	"int add_one( int in ) " \
	"{" \
	"   return in + 1;" \
	"}";

int main( void )
{
	CLFunction add_one( "add_one", src );
	add_one.run();
}
