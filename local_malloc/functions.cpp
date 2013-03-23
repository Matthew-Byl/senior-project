//-------------------------------------------------------------------------
//
// rewritersample.cpp: Source-to-source transformation sample with Clang,
// using Rewriter - the code rewriting interface.
//
// Eli Bendersky (eliben@gmail.com)
// This code is in the public domain
//
#include <cstdio>
#include <iostream>
#include <string>
#include <sstream>

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/FileManager.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Basic/TargetOptions.h"
#include "clang/Basic/TargetInfo.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Parse/ParseAST.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Rewrite/Frontend/Rewriters.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/ADT/APSInt.h"

#include "CallGraph.h"
#include "AllocationAST.h"

using namespace clang;
using namespace std;

ASTContext *ast_context;

int main(int argc, char *argv[])
{
    if (argc != 2) {
		llvm::errs() << "Usage: rewritersample <filename>\n";
        return 1;
    }

    // CompilerInstance will hold the instance of the Clang compiler for us,
    // managing the various objects needed to run the compiler.
    CompilerInstance TheCompInst;
    TheCompInst.createDiagnostics(0, 0);

	CallGraph TheCallGraph;

    // Initialize target info with the default triple for our platform.
    TargetOptions TO;
    TO.Triple = llvm::sys::getDefaultTargetTriple();
    TargetInfo *TI = TargetInfo::CreateTargetInfo(
        TheCompInst.getDiagnostics(), TO);
    TheCompInst.setTarget(TI);

	CompilerInvocation invocation;
	const char * const options[] = {
		"-x", "cl",
		"-I", "libclc/generic/include",
		"-include", "clc/clc.h",
		"-I", "/usr/include/clang/3.0/include"
	};
	CompilerInvocation::CreateFromArgs(
		invocation,
		options,
		options + 8,
		TheCompInst.getDiagnostics()
		);	 
	TheCompInst.setInvocation( &invocation );

    TheCompInst.createFileManager();
    FileManager &FileMgr = TheCompInst.getFileManager();
    TheCompInst.createSourceManager(FileMgr);
    SourceManager &SourceMgr = TheCompInst.getSourceManager();
    TheCompInst.createPreprocessor();
    TheCompInst.createASTContext();

    // A Rewriter helps us manage the code rewriting task.
    Rewriter TheRewriter;
    TheRewriter.setSourceMgr(SourceMgr, TheCompInst.getLangOpts());

    // Set the main file handled by the source manager to the input file.
    const FileEntry *FileIn = FileMgr.getFile(argv[1]);
    SourceMgr.createMainFileID(FileIn);
    TheCompInst.getDiagnosticClient().BeginSourceFile(
        TheCompInst.getLangOpts(),
        &TheCompInst.getPreprocessor());

    // Create an AST consumer instance which is going to get called by
    // ParseAST.
	AllocationASTConsumer TheConsumer( 
		TheRewriter, 
		TheCallGraph, 
		TheCompInst.getASTContext()
		);

    // Parse the file to AST, registering our consumer as the AST consumer.
    ParseAST(
		TheCompInst.getPreprocessor(), 
		&TheConsumer,
		TheCompInst.getASTContext()
		);

    // At this point the rewriter's buffer should be full with the rewritten
    // file contents.
    const RewriteBuffer *RewriteBuf =
        TheRewriter.getRewriteBufferFor(SourceMgr.getMainFileID());
//	llvm::outs() << string(RewriteBuf->begin(), RewriteBuf->end());

	int max_alloc = TheCallGraph.maximum_alloc( "entry" );
	cout << "Maximum allocation: " << max_alloc << endl;

	exit( 0 );
    return 0;
}
