all: thread test

CFLAGS = -Wall -g 

thread: main.c threads.c threads.h 
	gcc $(CFLAGS) threads.c main.c -o main
	@./main

test: test.c threads.c threads.h
	gcc $(CFLAGS) threads.c test.c -o test
	@./test

clean: 
	rm -f *.o main *~ *.dSYM
