all: opencl_player_test kalah

opencl_player_test: opencl_player.cl opencl_player.cpp board.o tree_array.o simple_players.o
	clang++ -std=c++11 -Wall -D MAKE_MAIN -g -I /opt/AMDAPP/include -I ../cl_test/ -L/opt/AMDAPP/lib/x86_64 opencl_player.cpp tree_array.o board.o simple_players.o ../cl_test/libCLTest.a -o opencl_player_test -lOpenCL -lprofiler

kalah: main.o board.o simple_players.o opencl_player.o tree_array.o opencl_player.cl
	clang++ main.o board.o simple_players.o opencl_player.o tree_array.o ../cl_test/libCLTest.a -L /opt/AMDAPP/lib/x86_64 -Wall -g -o kalah -lOpenCL

muyskens_kalah: main.cpp board.o simple_players.o
	g++ main.cpp board.o simple_players.o -o muyskens_kalah

%.o: %.cpp *.h
	clang++ -std=c++11 -Wall -I /opt/AMDAPP/include -I ../cl_test/ -g -c $< -o $@

%.o: %.c *.h
	clang -Wall -g -c $< -o $@

opencl_player.cl: opencl_player.cl.in board.c
	cpp opencl_player.cl.in > opencl_player.cl

clean:
	rm -f kalah opencl_player.cl *.o opencl_player_test