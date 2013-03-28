#ifndef _CLANG_INTERFACE_H
#define _CLANG_INTERFACE_H

#include <string>

#include "clang/Frontend/CompilerInstance.h"
#include "clang/Rewrite/Core/Rewriter.h"

class ClangInterface {
public:
	ClangInterface( std::string fileName );

	void processAST( clang::ASTConsumer *astConsumer );
	std::string getRewrittenCode();

	clang::CompilerInstance &getCompilerInstance() {
		return myCompilerInstance;
	}
	clang::Rewriter &getRewriter() {
		return myRewriter;
	}
	clang::ASTContext &getASTContext() {
		return myCompilerInstance.getASTContext();
	}

private:
	clang::CompilerInstance myCompilerInstance;
	clang::Rewriter myRewriter;
	clang::TargetOptions TO;
	clang::CompilerInvocation invocation;
};

#endif
