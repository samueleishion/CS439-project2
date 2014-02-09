all: thread test

thread: threads.c threads.h
	gcc -Wall threads.c

test: main.c 
	gcc -Wall main.c -o main
	@./main

clean: 
	rm -f *.o main *~