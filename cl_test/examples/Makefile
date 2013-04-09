OPENCL_LIB = /opt/AMDAPP/lib/x86_64
OPENCL_INCLUDE = /opt/AMDAPP/include

vector_add: vector_add.cpp
	clang++ -std=c++11 -g -L .. -L $(OPENCL_LIB) -I $(OPENCL_INCLUDE) -I .. vector_add.cpp -o vector_add -lCLTest -lOpenCL

clean:
	rm -f vector_add