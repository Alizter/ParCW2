CC=gcc
CFLAGS=-Wall -Wextra -Wconversion -pthread -g

main: main.o
	$(CC) $(CFLAGS) -o main main.o 

clean:
	rm *.o
