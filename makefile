CC=gcc
CFLAGS= -g -pthread -Wall -Wconversion -Wextra  
##  
main: main.o
	$(CC) $(CFLAGS) -o main main.o 

clean:
	rm *.o
