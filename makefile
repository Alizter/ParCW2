CC=gcc
CFLAGS=-Wall -Wextra -Wconversion -pthread

main: main.o
	$(CC) $(CFLAGS) -o main main.o 

clean:
	rm *.o
