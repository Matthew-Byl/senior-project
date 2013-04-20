OPENCL_INCLUDE = /opt/AMDAPP/include

all: KernelGenerator.o CLArgument.o CLContext.o CLKernel.o CLFunction.o *.cpp *.h
	ar rcs libCLTest.a KernelGenerator.o CLArgument.o CLContext.o CLKernel.o CLFunction.o

%.o: %.cpp *.h
	clang++ -I $(OPENCL_INCLUDE) -Wall -g -c $< -o $@

clean:
	rm -f *.a *.o