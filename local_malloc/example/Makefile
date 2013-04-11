OPENCL_INCLUDE = /opt/AMDAPP/include
OPENCL_LIBS = /opt/AMDAPP/lib/x86_64
CL_TEST = ../../cl_test

example: example.cpp example.cl
	clang++ example.cpp -o example \
		-I .. -I $(CL_TEST) -I $(OPENCL_INCLUDE) \
		-L .. -L $(CL_TEST) -L $(OPENCL_LIBS) \
		-lLocalMallocRewriter -lCLTest -lOpenCL

example.cl: example.cl.in
	cpp -I .. example.cl.in > example.cl

clean:
	rm -f example