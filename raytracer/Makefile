raytracer: ui.cpp raytracer.cl CLRenderer.h CLRenderer.cpp objects.h
	clang++ ui.cpp CLRenderer.cpp -std=c++11  -g -I ../cl_test -I /opt/AMDAPP/include -L /opt/AMDAPP/lib/x86_64 `pkg-config --cflags gtk+-2.0` `pkg-config --libs gtk+-2.0` -o raytracer -lOpenCL
	./raytracer

raytracer.cl: raytracer.cl.in objects.h
	cpp raytracer.cl.in > raytracer.cl