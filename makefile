CC=gcc
CFLAGS=-Wall -Wextra #-Wconversion

main: main.o
	$(CC) -o main main.o

clean:
	rm *.o