CC = clang++
CFLAGS = -std=c++11 -g -Wall -I ../config
CL_TEST_INCLUDE = -I ../../cl_test -I /opt/AMDAPP/include
OCL_LIBS = ../../cl_test/libCLTest.a -L /opt/AMDAPP/lib/x86_64 -lOpenCL
OCL_CC = $(CC) $(CFLAGS) $(CL_TEST_INCLUDE)

resource-extraction: resource-extraction.cpp resource-extraction.h ../config/config.cpp resource-extraction.cl
	$(OCL_CC) resource-extraction.cpp ../config/config.cpp -o resource-extraction $(OCL_LIBS)

resource-extraction.cl: resource-extraction.cl.in effort.cl utility.cl ../util/choose_thread.cl ../util/max_index.cl
	cpp -I ../util -I ../mwc64x/cl -I ../config resource-extraction.cl.in > resource-extraction.cl

clean:
	rm -f resource-extraction.cl