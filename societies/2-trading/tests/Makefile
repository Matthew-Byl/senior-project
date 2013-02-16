CC = clang++
CFLAGS = -std=c++11 -g -Wall -I ../../config
CL_TEST_INCLUDE = -I ../../../cl_test -I /opt/AMDAPP/include
OCL_LIBS = ../../../cl_test/libCLTest.a -L /opt/AMDAPP/lib/x86_64 -lOpenCL
OCL_CC = $(CC) $(CFLAGS) $(CL_TEST_INCLUDE)

all: menu_test

clean: menu_test_clean

menu_test: menu_test.cl menu_test.cpp
	$(OCL_CC) menu_test.cpp ../../config/config.cpp -o menu_test $(OCL_LIBS)

menu_test.cl: menu_test.cl.in ../menu.cl
	cpp menu_test.cl.in > menu_test.cl

menu_test_clean:
	rm -f menu_test.cl