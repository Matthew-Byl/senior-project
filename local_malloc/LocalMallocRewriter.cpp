#include "LocalMallocRewriter.h"
#include "ClangInterface.h"
#include "CallGraph.h"
#include "AllocationAST.h"
#include "RewriterAST.h"
#include <cstdio>
#include <unistd.h>
#include <iostream>
#include <fstream>
using namespace std;

LocalMallocRewriter::LocalMallocRewriter( string src )
	: mySrc( src )
{
	// Put the source code in a temp file.
	myTempFileName = strdup( "/tmp/tmpfileXXXXXX" );
	mkstemp( myTempFileName );
	ofstream temp_file( myTempFileName );
	temp_file << mySrc;
	temp_file.close();
}

LocalMallocRewriter::~LocalMallocRewriter()
{
	unlink( myTempFileName );
	free( myTempFileName );
}

string LocalMallocRewriter::rewrite( string entry )
{
	ClangInterface clangInterface( myTempFileName );

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

	ClangInterface clangInterface2( myTempFileName );
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
	return clangInterface2.getRewrittenCode();
}
