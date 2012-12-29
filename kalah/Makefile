opencl_player: opencl_player.cl opencl_player.cpp board.o
	clang++ -std=c++11 -Wall -D MAKE_MAIN -g -I /opt/AMDAPP/include -I ../cl_test/ -L/opt/AMDAPP/lib/x86_64 opencl_player.cpp board.o ../cl_test/libCLTest.a -o opencl_player_test -lOpenCL

kalah: main.o board.o simple_players.o opencl_player.o opencl_player.cl
	clang++ main.o board.o simple_players.o opencl_player.o ../cl_test/libCLTest.a -Wall -g -o kalah

%.o: %.cpp *.h
	clang++ -std=c++11 -Wall -I ../cl_test/ -g -c $< -o $@

%.o: %.c *.h
	clang -Wall -g -c $< -o $@

opencl_player.cl: opencl_player.cl.in board.c
	cpp opencl_player.cl.in > opencl_player.cl

clean:
	rm -f kalah opencl_player.cl *.o