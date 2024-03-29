CXX = clang++ -g
CFLAGS = -fno-rtti

LLVM_SRC_PATH = /home/john/clang-dev/llvm-3.2.src
LLVM_BUILD_PATH = /home/john/clang-dev/llvm-build

LLVM_BIN_PATH = $(LLVM_BUILD_PATH)/Debug+Asserts/bin
LLVM_LIBS=all
LLVM_CONFIG_INCLUDES_COMMAND = $(LLVM_BIN_PATH)/llvm-config --cxxflags
LLVM_CONFIG_LIBS_COMMAND = $(LLVM_BIN_PATH)/llvm-config --ldflags --libs $(LLVM_LIBS)
CLANG_BUILD_FLAGS = -I$(LLVM_SRC_PATH)/tools/clang/include -I$(LLVM_BUILD_PATH)/tools/clang/include

CLANGLIBS = \
  -lclangFrontendTool -lclangFrontend -lclangDriver \
  -lclangSerialization -lclangCodeGen -lclangParse \
  -lclangSema -lclangStaticAnalyzerFrontend \
  -lclangStaticAnalyzerCheckers -lclangStaticAnalyzerCore \
  -lclangAnalysis -lclangARCMigrate -lclangRewriteCore -lclangRewriteFrontend \
  -lclangEdit -lclangAST -lclangLex -lclangBasic -lclangTooling

OBJS = CallGraph.o AllocationAST.o RewriterAST.o ClangInterface.o LocalMallocRewriter.o MallocHeader.o

all: rewriter libLocalMallocRewriter.so

rewriter: $(OBJS) main.o
	$(CXX) $(OBJS) main.o \
	    $(CFLAGS) -o rewriter \
        $(CLANG_BUILD_FLAGS) $(CLANGLIBS) `$(LLVM_CONFIG_LIBS_COMMAND)` -ldl

libLocalMallocRewriter.so: $(OBJS)
	g++ -Wall -shared $(OBJS) $(CFLAGS) -o libLocalMallocRewriter.so \
		$(CLANG_BUILD_FLAGS) $(CLANGLIBS) `$(LLVM_CONFIG_LIBS_COMMAND)` -ldl

MallocHeader.cpp: local_malloc.h
	./embed_src.sh local_malloc.h local_malloc_header > MallocHeader.cpp

%.o: %.cpp *.h
	clang++ $(CFLAGS) $(CLANG_BUILD_FLAGS) `$(LLVM_CONFIG_INCLUDES_COMMAND)` -g -c $< -o $@

clean:
	rm -rf *.o rewriter libLocalMallocRewriter.so MallocHeader.cpp