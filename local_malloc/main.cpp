#include "ClangInterface.h"
#include "CallGraph.h"
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
 
    // At this point the rewriter's buffer should be full with the rewritten
    // file contents.
//    const RewriteBuffer *RewriteBuf =
	//      TheRewriter.getRewriteBufferFor(SourceMgr.getMainFileID());
//	llvm::outs() << string(RewriteBuf->begin(), RewriteBuf->end());


	exit( 0 );
	// There is a bug with the destructor. Solve it later.
    return 0;
}
