CC=gcc
CFLAGS= -g -pthread -Wall -Wconversion -Wextra -lm -l pthread
##  
main: main.o
	$(CC) -o main main.o $(CFLAGS) 

clean:
	rm *.o
