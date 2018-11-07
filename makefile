CC=gcc
CFLAGS= -g -pthread -Wall -Wconversion -Wextra -lm -l pthread -std=c99

.PHONY: all test clean main

main : main.o parcw.o
	$(CC) -o main main.o parcw.o $(CFLAGS) 

clean :
	rm main.o test.o *.o 2> /dev/null
	
test : test.o parcw.o
	$(CC) -o test test.o parcw.o $(CFLAGS)
	./test

all : main test
