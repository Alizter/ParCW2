CC = mpicc
CFLAGS = -g -pthread -lm -lrt -std=gnu99 -Wall -Wextra #-Wconversion 

main_dep = main.o parcw2.o error.o

.PHONY: all test clean main


main : $(main_dep)
	$(CC) -o main $(main_dep) $(CFLAGS)

clean :
	-rm *.o 2> /dev/null
	
test : test.o $(main_dep)
	$(CC) -o test test.o $(main_dep) $(CFLAGS)
	./test

all : main test
