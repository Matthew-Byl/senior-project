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

using namespace clang;
using namespace std;

ASTContext *ast_context;

// By implementing RecursiveASTVisitor, we can specify which AST nodes
// we're interested in by overriding relevant methods.
class MyASTVisitor : public RecursiveASTVisitor<MyASTVisitor>
{
public:
    MyASTVisitor(Rewriter &R, CallGraph &G)
		: TheRewriter(R),
		  myCallGraph( G )
		{}

    bool VisitStmt(Stmt *s) {
        if (isa<CallExpr>(s)) {
			CallExpr *call = cast<CallExpr>(s);
			
			FunctionDecl *callee = call->getDirectCallee();
            DeclarationName DeclName = callee->getNameInfo().getName();
            string FuncName = DeclName.getAsString();

			if ( FuncName == "local_malloc" || FuncName == "local_free" )
			{
				if ( call->getNumArgs() == 2 )
				{
					Expr *size_arg = call->getArg( 0 );
					if ( isa<IntegerLiteral>(size_arg) )
					{
						llvm::APSInt value;

						size_arg->EvaluateAsInt( value, *ast_context );

						if ( FuncName == "local_malloc" )
						{
							cout << "*** Malloc of size " << value.toString(10) << endl;
							myCallGraph.malloc( (int)value.getLimitedValue() );
						}
						else
						{
							cout << "*** Free of size " << value.toString(10) << endl;
							myCallGraph.free( (int)value.getLimitedValue() );
						}
						
					}
					else
					{
						cout << "Malloc/free: first argument not an integer literal." << endl;
					}
				}
				else
				{
					cout << "Malloc/free: wrong # of arguments." << endl;
				}
			}
			else
			{
				cout << "*** Calling function " << FuncName << endl;
				myCallGraph.call( FuncName );
			}
        }

        return true;
    }

    bool VisitFunctionDecl(FunctionDecl *f) {
        // Only function definitions (with bodies), not declarations.
        if (f->hasBody()) {
            Stmt *FuncBody = f->getBody();

            // Type name as string
            QualType QT = f->getResultType();
            string TypeStr = QT.getAsString();

            // Function name
            DeclarationName DeclName = f->getNameInfo().getName();
            string FuncName = DeclName.getAsString();
			myCallGraph.enter_function( FuncName );

			cout << "Begin function " << FuncName << endl;
        }

        return true;
    }

private:
    void AddBraces(Stmt *s);

    Rewriter &TheRewriter;
	CallGraph &myCallGraph;
};


// Implementation of the ASTConsumer interface for reading an AST produced
// by the Clang parser.
class MyASTConsumer : public ASTConsumer
{
public:
    MyASTConsumer(Rewriter &R, CallGraph &G)
		: Visitor(R, G)
		{}

    // Override the method that gets called for each parsed top-level
    // declaration.
    virtual bool HandleTopLevelDecl(DeclGroupRef DR) {
        for (DeclGroupRef::iterator b = DR.begin(), e = DR.end();
             b != e; ++b)
            // Traverse the declaration using our AST visitor.
            Visitor.TraverseDecl(*b);
        return true;
    }

private:
    MyASTVisitor Visitor;
};


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
		"-x",
		"cl",
		"-I",
		"libclc/generic/include",
		"-include",
		"clc/clc.h",
		"-I",
		"/usr/include/clang/3.0/include"
	};
	CompilerInvocation::CreateFromArgs(
		invocation,
		options,
		options+8,
		TheCompInst.getDiagnostics()
		);	 
	TheCompInst.setInvocation( &invocation );

    TheCompInst.createFileManager();
    FileManager &FileMgr = TheCompInst.getFileManager();
    TheCompInst.createSourceManager(FileMgr);
    SourceManager &SourceMgr = TheCompInst.getSourceManager();
    TheCompInst.createPreprocessor();
    TheCompInst.createASTContext();

    ast_context = &(TheCompInst.getASTContext());

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
    MyASTConsumer TheConsumer(TheRewriter, TheCallGraph);

    // Parse the file to AST, registering our consumer as the AST consumer.
    ParseAST(TheCompInst.getPreprocessor(), &TheConsumer,
             TheCompInst.getASTContext());

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
