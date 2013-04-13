#include <LocalMallocRewriter.h>
#include <CLKernel.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
using namespace std;

int main ( int argc, char *argv[] )
{
	// Read kernel source from file
	ifstream t( "example.cl" );
	string src( (std::istreambuf_iterator<char>(t) ),
				std::istreambuf_iterator<char>() );

	// Rewrite the code t make local_malloc work.
	LocalMallocRewriter rewriter( src );
	string rewritten_src = rewriter.rewrite( "function_sum" );

	// Run the kernel.
	CLKernel function_sum( "function_sum", rewritten_src );
	cl_int range_start = 20;
	cl_int host_sum;
	CLUnitArgument sum( &host_sum, 1 );

	function_sum.setGlobalDimensions( 256 );
	function_sum.setLocalDimensions( 256 );
	
	vector<CLUnitArgument> args;
	args.push_back( range_start );
	args.push_back( sum );
	function_sum( args );

	cout << "Sum was " << host_sum << endl;

    return 0;
}
