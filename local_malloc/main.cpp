#include "ClangInterface.h"
#include "CallGraph.h"
#include "AllocationAST.h"
#include "RewriterAST.h"
#include <iostream>
using namespace std;

int main ( int argc, char *argv[] )
{
	assert( argc == 2 );

	ClangInterface clangInterface( argv[1] );

	CallGraph callGraph;
	AllocationASTConsumer allocationConsumer( 
		clangInterface.getRewriter(),
		callGraph,
		clangInterface.getASTContext()
		);
	clangInterface.processAST( &allocationConsumer );

	int max_alloc = callGraph.maximum_alloc( "entry" );
	cout << "Maximum allocation: " << max_alloc << endl;

	ClangInterface clangInterface2( argv[1] );
	RewriterASTConsumer rewriterConsumer( 
		clangInterface2.getRewriter(),
		callGraph,
		clangInterface2.getASTContext()
		);
	clangInterface2.processAST( &rewriterConsumer );

 
    // At this point the rewriter's buffer should be full with the rewritten
    // file contents.
	cout << clangInterface2.getRewrittenCode();

	exit( 0 );
	// There is a bug with the destructor. Solve it later.
    return 0;
}
