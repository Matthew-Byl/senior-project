all: KernelGenerator.o CLFunction.o CLUnitArgument.o CLContext.o main.o *.cpp *.h
	clang++ -g -L /opt/AMDAPP/lib/x86_64 *.o -o main -lOpenCL
#	ar rcs libCLTest.a *.o

%.o: %.cpp *.h
	clang++ -std=c++11 -I /opt/AMDAPP/include -Wall -g -c $< -o $@

clean:
	rm *.o