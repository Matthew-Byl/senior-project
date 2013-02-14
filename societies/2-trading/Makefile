CC = clang++
CFLAGS = -std=c++11 -g -Wall -I ../config
CL_TEST_INCLUDE = -I ../../cl_test -I /opt/AMDAPP/include
OCL_LIBS = ../../cl_test/libCLTest.a -L /opt/AMDAPP/lib/x86_64 -lOpenCL
OCL_CC = $(CC) $(CFLAGS) $(CL_TEST_INCLUDE)

trading: trading.cl trading.cpp trading.h ../config/config.h
	$(OCL_CC) trading.cpp ../config/config.cpp -o trading $(OCL_LIBS)

trading.cl: trading.cl.in menu.cl valuation.cl
	cpp trading.cl.in > trading.cl

clean:
	rm -f trading trading.cl