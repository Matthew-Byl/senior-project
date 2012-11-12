all: CLUnit.o main.o KernelGenerator.o CLUnitArgument.o
	g++ -g *.o -o main -lOpenCL

%.o: %.cpp
	g++ -std=c++11 -Wall -g -c $< -o $@

clean:
	rm *.o