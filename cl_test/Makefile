all: CLUnit.o main.o
	g++ -g *.o -o main -lOpenCL

%.o: %.cpp
	g++ -std=c++11 -Wall -g -c $< -o $@

clean:
	rm *.o