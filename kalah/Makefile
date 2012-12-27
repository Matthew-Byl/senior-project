kalah: main.o board.o simple_players.o
	clang++ main.o board.o simple_players.o -Wall -g -o kalah

%.o: %.cpp *.h
	clang++ -std=c++11 -Wall -I ../cl_test/ -g -c $< -o $@

%.o: %.c *.h
	clang -Wall -g -c $< -o $@

clean:
	rm -f kalah *.o