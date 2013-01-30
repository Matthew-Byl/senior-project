all: effort-curve utility

clean:
	rm -f effort-curve utility *.out

effort-curve: effort-curve.cpp ../effort.c
	clang++ -std=c++11 -I ../../../cl_test -I /opt/AMDAPP/include -L /opt/AMDAPP/lib/x86_64 effort-curve.cpp ../../../cl_test/libCLTest.a -lOpenCL -o effort-curve

utility: utility.cpp ../utility.c
	clang++ -std=c++11 -I ../../../cl_test -I /opt/AMDAPP/include -L /opt/AMDAPP/lib/x86_64 utility.cpp ../../../cl_test/libCLTest.a -lOpenCL -o utility