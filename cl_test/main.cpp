#include "CLFunction.h"
#include "CLUnitArgument.h"

#include <vector>
#include <initializer_list>
using namespace std;

const char *src = 
	"int add_one( int i, int3 j ) " \
	"{" \
	"   printf(\"From OpenCL: %d %d\\n\", i, j.x ); " \
	"   return i + j.x;" \
	"}";

#define c_vec( ... ) \
	CLUnitArgument args[] = { ... };
	

int main( void )
{
	CLContext context( 0, 0 );
	CLFunction add_one( "add_one", src, context );
	cl_int3 intr;
	intr.s[0] = 2;
//	CLUnitArgument arg( intr );
	cl_int result;

//	add_one.addArgument( 1 );
//	add_one.addArgument( intr );

//	vector<CLUnitArgument> args( { 1, intr } );
//	args.push_back( 1 );
//	args.push_back( intr );

//	CLUnitArgument args[] = { 1, intr };
//	cout << "Size: " << ( sizeof( args ) / sizeof( CLUnitArgument ) ) << endl;

	add_one.setArgs( 1, intr );
	result = add_one.run<cl_int>( "int" );

	printf( "Result: %d\n", result );
}
