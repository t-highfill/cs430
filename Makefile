
CC=g++

CFLAGS=-g -Wall -Werror -std=c++14

a1 : a1.cpp geom.h graphics.h postscript.h xpm.h linear.h
	$(CC) $(CFLAGS) a1.cpp -o a1

RUN_ARGS=input/hw1_8.ps test.xpm

run : a1
	./a1 $(RUN_ARGS)

debug : a1
	valgrind ./a1 $(RUN_ARGS)
