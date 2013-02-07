all: effort-curve utility marginal-utility gain-per-minute

clean:
	rm -f effort-curve utility marginal-utility gain-per-minute *.out

effort-curve: effort-curve.cpp ../effort.cl
	clang++ -std=c++11 -I ../../../cl_test -I /opt/AMDAPP/include -L /opt/AMDAPP/lib/x86_64 effort-curve.cpp ../../../cl_test/libCLTest.a -lOpenCL -o effort-curve

utility: utility.cpp ../utility.cl
	clang++ -std=c++11 -I ../../../cl_test -I /opt/AMDAPP/include -L /opt/AMDAPP/lib/x86_64 utility.cpp ../../../cl_test/libCLTest.a -lOpenCL -o utility

marginal-utility: marginal-utility.cpp ../utility.cl
	clang++ -std=c++11 -I ../../../cl_test -I /opt/AMDAPP/include -L /opt/AMDAPP/lib/x86_64 marginal-utility.cpp ../../../cl_test/libCLTest.a -lOpenCL -o marginal-utility

gain-per-minute: gain-per-minute.cpp ../utility.cl
	clang++ -std=c++11 -I ../../../cl_test -I /opt/AMDAPP/include -L /opt/AMDAPP/lib/x86_64 gain-per-minute.cpp ../../../cl_test/libCLTest.a -lOpenCL -o gain-per-minute
