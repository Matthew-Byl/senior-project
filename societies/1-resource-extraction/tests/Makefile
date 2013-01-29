all: effort-curve

clean:
	rm -f effort-curve effort-curve.out

effort-curve: effort-curve.cpp ../effort.c
	clang++ -std=c++11 -I ../../../cl_test -I /opt/AMDAPP/include -L /opt/AMDAPP/lib/x86_64 effort-curve.cpp ../../../cl_test/libCLTest.a -lOpenCL -o effort-curve