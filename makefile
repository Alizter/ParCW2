CC=gcc
CFLAGS= -g -pthread -Wall -Wconversion -Wextra -lm -l pthread -lrt -std=gnu99

.PHONY: all test clean main

main : main.o parcw.o parcw2.o
	$(CC) -o main main.o parcw.o parcw2.o $(CFLAGS) 

clean :
	rm main.o test.o *.o 2> /dev/null
	
test : test.o parcw.o
	$(CC) -o test test.o parcw.o parcw2.o $(CFLAGS)
	./test

all : main test
