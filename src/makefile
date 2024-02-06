CC=clang
CFAGS=-I

nemo:
	rm -rf build && mkdir build
	$(CC) -std=c17 -Wall -Wextra -Werror -I mpc main.c grammar/grammar.c mpc/mpc.c -ledit -lm -o build/nemo
clean:
	rm -rf build
