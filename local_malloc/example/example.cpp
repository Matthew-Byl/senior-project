#include <LocalMallocRewriter.h>
#include <CLKernel.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
using namespace std;

int main ( int argc, char *argv[] )
{
	ifstream t( "example.cl" );
	string src( (std::istreambuf_iterator<char>(t) ),
				std::istreambuf_iterator<char>() );

	LocalMallocRewriter rewriter( src );
	cout << rewriter.rewrite( "function_sum" );

    return 0;
}
