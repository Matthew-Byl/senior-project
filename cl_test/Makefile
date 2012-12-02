all: CLUnit.o main.o KernelGenerator.o CLFunction.o CLUnitArgument.o
	g++ -g -L /opt/AMDAPP/lib/x86_64 *.o -o main -lOpenCL

%.o: %.cpp
	clang -std=c++11 -I /opt/AMDAPP/include -Wall -g -c $< -o $@

clean:
	rm *.o