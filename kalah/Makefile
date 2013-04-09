OPENCL_LIB = /opt/AMDAPP/lib/x86_64
OPENCL_INCLUDE = /opt/AMDAPP/include
CL_TEST = ../cl_test

all: opencl_player_tests kalah

opencl_player_tests: opencl_player.cl opencl_player_tests.cpp opencl_player.cpp board.o tree_array.o simple_players.o evaluate.o
	clang++ -Wall -g -I $(OPENCL_INCLUDE) -I $(CL_TEST) -L $(OPENCL_LIB) -L $(CL_TEST) \
	 evaluate.o opencl_player.cpp opencl_player_tests.cpp tree_array.o board.o simple_players.o \
	 -o opencl_player_tests -lCLTest -lOpenCL

kalah: main.o board.o simple_players.o opencl_player.o tree_array.o opencl_player.cl evaluate.o
	clang++ main.o board.o simple_players.o opencl_player.o tree_array.o evaluate.o \
		-L $(OPENCL_LIB) -L $(CL_TEST) -Wall -g -o kalah -lCLTest -lOpenCL

%.o: %.cpp *.h
	clang++ -Wall -I /opt/AMDAPP/include -I ../cl_test/ -g -c $< -o $@

%.o: %.c *.h
	clang -Wall -g -c $< -o $@

opencl_player.cl: opencl_player.cl.in board.c depths.h tree_array.c evaluate.c
	cpp opencl_player.cl.in > opencl_player.cl

clean:
	rm -f kalah opencl_player.cl *.o opencl_player_tests