all: KernelGenerator.o CLUnitArgument.o CLContext.o CLKernel.o CLFunction.o main.o *.cpp *.h
	clang++ -g -L /opt/AMDAPP/lib/x86_64 *.o -o main -lOpenCL
	ar rcs libCLTest.a KernelGenerator.o CLUnitArgument.o CLContext.o CLKernel.o CLFunction.o

%.o: %.cpp *.h
	clang++ -std=c++11 -I /opt/AMDAPP/include -Wall -g -c $< -o $@