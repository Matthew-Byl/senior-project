CXX = clang++ -g
CFLAGS = -fno-rtti

LLVM_SRC_PATH = /home/john/clang-dev/llvm-3.2.src
LLVM_BUILD_PATH = /home/john/clang-dev/llvm-build

LLVM_BIN_PATH = $(LLVM_BUILD_PATH)/Debug+Asserts/bin
LLVM_LIBS=all
LLVM_CONFIG_COMMAND = $(LLVM_BIN_PATH)/llvm-config --cxxflags --ldflags \
                                        --libs $(LLVM_LIBS)
CLANG_BUILD_FLAGS = -I$(LLVM_SRC_PATH)/tools/clang/include \
                                      -I$(LLVM_BUILD_PATH)/tools/clang/include

CLANGLIBS = \
  -lclangFrontendTool -lclangFrontend -lclangDriver \
  -lclangSerialization -lclangCodeGen -lclangParse \
  -lclangSema -lclangStaticAnalyzerFrontend \
  -lclangStaticAnalyzerCheckers -lclangStaticAnalyzerCore \
  -lclangAnalysis -lclangARCMigrate -lclangRewriteCore -lclangRewriteFrontend \
  -lclangEdit -lclangAST -lclangLex -lclangBasic -lclangTooling

all: functions


functions: CallGraph.cpp CallGraph.h AllocationAST.cpp AllocationAST.h ClangInterface.cpp ClangInterface.h RewriterAST.cpp RewriterAST.h main.cpp
	$(CXX) CallGraph.cpp AllocationAST.cpp RewriterAST.cpp ClangInterface.cpp main.cpp $(CFLAGS) -o functions \
              $(CLANG_BUILD_FLAGS) $(CLANGLIBS) `$(LLVM_CONFIG_COMMAND)` -ldl
clean:
	rm -rf *.o functions