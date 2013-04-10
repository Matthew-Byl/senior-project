#include "LocalMallocRewriter.h"
#include <iostream>
#include <fstream>
#include <cassert>
#include <cstdlib>
using namespace std;

int main ( int argc, char *argv[] )
{
	assert( argc == 2 );

	ifstream t( argv[1] );
	string src( (std::istreambuf_iterator<char>(t) ),
				std::istreambuf_iterator<char>() );

	LocalMallocRewriter rewriter( src );
	cout << rewriter.rewrite( "entry" );

	exit( 0 );
	// There is a bug with the destructor. Solve it later.
    return 0;
}
