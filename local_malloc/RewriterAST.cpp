#include "RewriterAST.h"

#include <iostream>
#include <string>

using namespace clang;
using namespace std;

bool RewriterASTVisitor::VisitStmt( Stmt *s )
{
	if ( isa<CallExpr>(s) )
	{
		CallExpr *call = cast<CallExpr>(s);
		
		FunctionDecl *callee = call->getDirectCallee();
		DeclarationName DeclName = callee->getNameInfo().getName();
		string funcName = DeclName.getAsString();

		if ( funcName == "local_malloc"
			 || funcName == "local_free" )
		{
			// For all calls to local_malloc and local_free, add the
			//  local_malloc parameter.
		
			cout << "call to malloc function!" << endl;
		}
		else if ( myCallGraph.isDefined( funcName ) )
		{
			// For all functions that had bodies in last AST build that
			//  were not kernels, add a parameter for the local_malloc
			//  object.

			cout << "call to defined function: " << funcName << endl;
		}
		else
		{
			cout << "call to builtin function: " << funcName << endl;
		}
    }

	return true;
}

bool RewriterASTVisitor::isOpenCLKernel( FunctionDecl *f )
{
	if ( !f->hasAttrs() )
		return false;

	AttrVec &attributes = f->getAttrs();
	PrintingPolicy policy( TheRewriter.getLangOpts() );
	policy.Indentation = 0;

	for ( 
		AttrVec::iterator it = attributes.begin();
		it != attributes.end();
		it++
		)
	{
		string attribute_name;
		llvm::raw_string_ostream attribute_name_stream( attribute_name );
		(*it)->printPretty( attribute_name_stream, policy );

		// Flushes the stream to the string.
		attribute_name_stream.str();

		// This is how clang mungs up __kernel.
		if ( attribute_name == " __attribute__((opencl_kernel_function))" )
		{
			return true;
		}
	}

	return false;
}

bool RewriterASTVisitor::VisitFunctionDecl( FunctionDecl *f ) 
{
	DeclarationName DeclName = f->getNameInfo().getName();
	string funcName = DeclName.getAsString();

	if ( funcName == "local_malloc"
		 || funcName == "local_free" )
	{
		// For all calls to local_malloc and local_free, add the
		//  local_malloc parameter.
		
		cout << "malloc function!" << endl;
	}
	else if ( myCallGraph.isDefined( funcName ) )
	{
		// For all functions that had bodies in last AST build that
		//  were not kernels, add a parameter for the local_malloc
		//  object.

			if ( isOpenCLKernel( f ) )
			{
				cout << "Is an openCL kernel." << endl;
			}

		cout << "defined function: " << funcName << endl;
	}
	else
	{
//		cout << "builtin function: " << funcName << endl;
	}

	return true;
}

bool RewriterASTConsumer::HandleTopLevelDecl( DeclGroupRef DR ) 
{
	for (DeclGroupRef::iterator b = DR.begin(), e = DR.end();
		 b != e; ++b)
		// Traverse the declaration using our AST visitor.
		Visitor.TraverseDecl(*b);
	return true;
}
