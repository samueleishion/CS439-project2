all: thread test

thread: main.c threads.c threads.h
	gcc -Wall threads.c main.c -o main
	@./main

test: main.c 
	gcc -Wall main.c -o main
	@./main

clean: 
	rm -f *.o main *~
