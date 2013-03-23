/**
 * Classes to traverse a Clang AST that track how
 *  much memory is local_malloc()ed and local_free()d
 *  at one time.
 *
 * Based on code from Eli Bendersky (eliben@gmail.com)
 *  at http://eli.thegreenplace.net/2012/06/08/basic-source-to-source-transformation-with-clang/
 *
 * @author John Kloosterman
 * @date March 23, 2013
 */

#ifndef _ALLOCATION_AST_
#define _ALLOCATION_AST_

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Rewrite/Core/Rewriter.h"

#include "CallGraph.h"

class AllocationASTVisitor : public clang::RecursiveASTVisitor<AllocationASTVisitor>
{
public:
    AllocationASTVisitor(
		clang::Rewriter &R, 
		CallGraph &G,
		clang::ASTContext &context )
		: TheRewriter(R),
		myCallGraph( G ),
		myASTContext( context )
		{}

    bool VisitStmt( clang::Stmt *s);
    bool VisitFunctionDecl( clang::FunctionDecl *f);

private:
	clang::Rewriter &TheRewriter;
	CallGraph &myCallGraph;
	clang::ASTContext &myASTContext;
};


// Implementation of the ASTConsumer interface for reading an AST produced
// by the Clang parser.
class AllocationASTConsumer : public clang::ASTConsumer
{
public:
	AllocationASTConsumer( clang::Rewriter &R, CallGraph &G, clang::ASTContext &context )
		: Visitor( R, G, context )
		{}

    // Override the method that gets called for each parsed top-level
    // declaration.
    virtual bool HandleTopLevelDecl( clang::DeclGroupRef DR );

private:
    AllocationASTVisitor Visitor;
};

#endif
