all: thread test

CFLAGS = -Wall -g 

thread: main.c threads.c threads.h 
	gcc $(CFLAGS) threads.c main.c -o main
	@./main

test: main.c 
	gcc $(CFLAGS) main.c -o main
	@./main

clean: 
	rm -f *.o main *~ *.dSYM
