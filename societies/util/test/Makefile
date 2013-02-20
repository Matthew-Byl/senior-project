CC = clang++
CFLAGS = -std=c++11 -g -Wall
CL_TEST_INCLUDE = -I ../../../cl_test -I /opt/AMDAPP/include
OCL_LIBS = ../../../cl_test/libCLTest.a -L /opt/AMDAPP/lib/x86_64 -lOpenCL
OCL_CC = $(CC) $(CFLAGS) $(CL_TEST_INCLUDE)

all: choose_thread_tester max_min_tester

clean: choose_thread_tester_clean max_min_tester_clean
	rm -f *.out

choose_thread_tester: choose_thread_tester.cl choose_thread_tester.cpp
	$(OCL_CC) -o choose_thread_tester choose_thread_tester.cpp $(OCL_LIBS)

choose_thread_tester.cl: ../choose_thread.cl choose_thread_tester.cl.in
	cpp choose_thread_tester.cl.in -I ../../mwc64x/cl/ > choose_thread_tester.cl

choose_thread_tester_clean:
	rm -f choose_thread_tester.cl choose_thread_tester

max_min_tester: max_min_tester.cpp max_min_tester.cl
	$(OCL_CC) -o max_min_tester max_min_tester.cpp $(OCL_LIBS)

max_min_tester.cl: ../max_min.cl max_min_tester.cl.in
	cpp max_min_tester.cl.in > max_min_tester.cl

max_min_tester_clean:
	rm -f max_min_tester.cl max_min_tester
