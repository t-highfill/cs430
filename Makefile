
CC=g++

CFLAGS=-g -Wall -Werror -std=c++14

a1 : a1.cpp geom.h graphics.h postscript.h xpm.h
	$(CC) $(CFLAGS) a1.cpp -o a1

run : a1
	./a1 input/hw1_2.ps test.xpm

debug : a1
	valgrind ./a1 input/hw1_2.ps test.xpm