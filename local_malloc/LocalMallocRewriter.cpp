#include "LocalMallocRewriter.h"
#include "ClangInterface.h"
#include "CallGraph.h"
#include "AllocationAST.h"
#include "RewriterAST.h"
#include <cstdio>
#include <iostream>
#include <fstream>
using namespace std;

LocalMallocRewriter::LocalMallocRewriter( string src )
	: mySrc( src )
{

}

string LocalMallocRewriter::rewrite( string entry )
{
	// Put the source code in a temp file.
	char *tmpname = strdup( "/tmp/tmpfileXXXXXX" );
	mkstemp( tmpname );
	ofstream temp_file( tmpname );
	temp_file << mySrc;
	temp_file.close();

	ClangInterface clangInterface( tmpname );

	// Find the maximum allocation
	CallGraph callGraph;
	AllocationASTConsumer allocationConsumer( 
		clangInterface.getRewriter(),
		callGraph,
		clangInterface.getASTContext()
		);
	clangInterface.processAST( &allocationConsumer );

	int max_alloc = callGraph.maximum_alloc( entry );
	cout << "Maximum allocation: " << max_alloc << endl;

	ClangInterface clangInterface2( tmpname );
	RewriterASTConsumer rewriterConsumer( 
		clangInterface2.getRewriter(),
		callGraph,
		clangInterface2.getASTContext(),
		entry,
		max_alloc
		);
	clangInterface2.processAST( &rewriterConsumer );
 
    // At this point the rewriter's buffer should be full with the rewritten
    // file contents.
	free( tmpname );
	return clangInterface2.getRewrittenCode();

	// We might crash...
}
