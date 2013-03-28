#include "ClangInterface.h"

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
#include <cassert>

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

ClangInterface::ClangInterface( string fileName )
{
	cout << "File: " << fileName << endl;

    myCompilerInstance.createDiagnostics(0, 0);

	// Target info
    TO.Triple = llvm::sys::getDefaultTargetTriple();
    TargetInfo *TI = TargetInfo::CreateTargetInfo(
        myCompilerInstance.getDiagnostics(), TO);
    myCompilerInstance.setTarget(TI);

	// Invocation, including all the headers necessary for
	//  OpenCL support.
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
		myCompilerInstance.getDiagnostics()
		);	 
	myCompilerInstance.setInvocation( &invocation );

	// Files
    myCompilerInstance.createFileManager();
    FileManager &FileMgr = myCompilerInstance.getFileManager();
    myCompilerInstance.createSourceManager(FileMgr);
    SourceManager &SourceMgr = myCompilerInstance.getSourceManager();
    myCompilerInstance.createPreprocessor();
    myCompilerInstance.createASTContext();

    // A Rewriter helps us manage the code rewriting task.
    myRewriter.setSourceMgr(SourceMgr, myCompilerInstance.getLangOpts());

    // Set the main file handled by the source manager to the input file.
    const FileEntry *FileIn = FileMgr.getFile( fileName );
    SourceMgr.createMainFileID(FileIn);
    myCompilerInstance.getDiagnosticClient().BeginSourceFile(
        myCompilerInstance.getLangOpts(),
        &myCompilerInstance.getPreprocessor());
}

void ClangInterface::processAST( ASTConsumer *astConsumer )
{
   ParseAST(
	   myCompilerInstance.getPreprocessor(), 
	   astConsumer,
	   myCompilerInstance.getASTContext()
	   );
}

string ClangInterface::getRewrittenCode()
{
    SourceManager &SourceMgr = myRewriter.getSourceMgr();

    const RewriteBuffer &RewriteBuf = myRewriter.getEditBuffer( SourceMgr.getMainFileID() );
	return string(RewriteBuf.begin(), RewriteBuf.end());
}
