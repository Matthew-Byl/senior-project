all: KernelGenerator.o CLUnitArgument.o CLContext.o CLKernel.o CLFunction.o *.cpp *.h
	ar rcs libCLTest.a KernelGenerator.o CLUnitArgument.o CLContext.o CLKernel.o CLFunction.o

%.o: %.cpp *.h
	clang++ -I /opt/AMDAPP/include -Wall -g -c $< -o $@

clean:
	rm -f *.a *.o main