/**
 *
 * @author John Kloosterman
 * @date March 28, 2013
 */

#ifndef _REWRITER_AST_
#define _REWRITER_AST_

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Rewrite/Core/Rewriter.h"

#include "CallGraph.h"

class RewriterASTVisitor : public clang::RecursiveASTVisitor<RewriterASTVisitor>
{
public:
    RewriterASTVisitor(
		clang::Rewriter &R, 
		CallGraph &G,
		clang::ASTContext &context,
		std::string entryFunction,
		unsigned bufferSize )
		: TheRewriter(R),
		myCallGraph( G ),
		myASTContext( context ),
		myEntryFunction( entryFunction ),
		myBufferSize( bufferSize )
		{}

    bool VisitStmt( clang::Stmt *s);
    bool VisitFunctionDecl( clang::FunctionDecl *f);

private:
	bool isOpenCLKernel( clang::FunctionDecl *f );

	clang::Rewriter &TheRewriter;
	CallGraph &myCallGraph;
	clang::ASTContext &myASTContext;
	std::string myEntryFunction;
	unsigned myBufferSize;
};


// Implementation of the ASTConsumer interface for reading an AST produced
// by the Clang parser.
class RewriterASTConsumer : public clang::ASTConsumer
{
public:
	RewriterASTConsumer( 
		clang::Rewriter &R, 
		CallGraph &G, 
		clang::ASTContext &context,
		std::string entryFunction,
		unsigned bufferSize )
		: Visitor( R, G, context, entryFunction, bufferSize )
		{}

    // Override the method that gets called for each parsed top-level
    // declaration.
    virtual bool HandleTopLevelDecl( clang::DeclGroupRef DR );

private:
    RewriterASTVisitor Visitor;
};

#endif
