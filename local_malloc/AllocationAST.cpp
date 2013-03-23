#include "AllocationAST.h"

#include <iostream>
#include <string>

using namespace clang;
using namespace std;

bool AllocationASTVisitor::VisitStmt( Stmt *s )
{
	if ( isa<CallExpr>(s) )
	{
		CallExpr *call = cast<CallExpr>(s);
		
		FunctionDecl *callee = call->getDirectCallee();
		DeclarationName DeclName = callee->getNameInfo().getName();
		string FuncName = DeclName.getAsString();
		
		if ( FuncName == "local_malloc" || FuncName == "local_free" )
		{
			if ( call->getNumArgs() == 2 )
			{
				Expr *size_arg = call->getArg( 0 );
				llvm::APSInt value;
				
				if ( !size_arg->EvaluateAsInt( value, myASTContext ) )
				{
						cout << "Malloc argument not something foldable to an int!" << endl;
						exit( 1 );
					}
				
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

bool AllocationASTVisitor::VisitFunctionDecl( FunctionDecl *f ) 
{
	// Only function definitions (with bodies), not declarations.
	if ( f->hasBody() ) 
	{
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

bool AllocationASTConsumer::HandleTopLevelDecl( DeclGroupRef DR ) 
{
	for (DeclGroupRef::iterator b = DR.begin(), e = DR.end();
		 b != e; ++b)
		// Traverse the declaration using our AST visitor.
		Visitor.TraverseDecl(*b);
	return true;
}
