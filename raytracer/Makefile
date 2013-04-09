CL_TEST = ../cl_test
OPENCL_INCLUDE = /opt/AMDAPP/include
OPENCL_LIB = /opt/AMDAPP/lib/x86_64

raytracer: ui.cpp raytracer.cl CLRenderer.h CLRenderer.cpp objects.h
	clang++ -std=c++11 ui.cpp CLRenderer.cpp -g -L $(CL_TEST) -I $(CL_TEST) -I $(OPENCL_INCLUDE) -L $(OPENCL_LIB) `pkg-config --cflags gtk+-2.0` `pkg-config --libs gtk+-2.0` -o raytracer -lCLTest -lOpenCL

raytracer.cl: raytracer.cl.in objects.h
	cpp raytracer.cl.in > raytracer.cl

clean:
	rm -f raytracer.cl raytracer