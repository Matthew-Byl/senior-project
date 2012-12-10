raytracer: ui.cpp raytracer.cl objects.h
	clang++ ui.cpp -std=c++11 ../cl_test/libCLTest.a -g -I ../cl_test -I /opt/AMDAPP/include -L /opt/AMDAPP/lib/x86_64 `pkg-config --cflags gtk+-2.0` `pkg-config --libs gtk+-2.0` -o raytracer -lOpenCL
	./raytracer

raytracer.cl: raytracer.cl.in objects.h
	cpp raytracer.cl.in > raytracer.cl