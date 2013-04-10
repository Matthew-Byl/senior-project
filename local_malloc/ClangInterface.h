#ifndef _CLANG_INTERFACE_H
#define _CLANG_INTERFACE_H

#include <string>

#include "clang/Frontend/CompilerInstance.h"
#include "clang/Rewrite/Core/Rewriter.h"

class ClangInterface {
public:
	ClangInterface( std::string fileName );
	~ClangInterface();

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
	clang::Rewriter myRewriter;
	clang::CompilerInvocation *invocation;
	clang::TargetOptions *TO;
	clang::CompilerInstance myCompilerInstance;
};

#endif
