all: CLUnit.o main.o KernelGenerator.o
	g++ -g *.o -o main -lOpenCL

%.o: %.cpp
	clang -std=c++11 -Wall -g -c $< -o $@

clean:
	rm *.o