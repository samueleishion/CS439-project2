#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include "threads.h"
#include <malloc.h> 
#include <stdint.h>

struct thread *thread_create(void (*f)(void *arg), void *arg) {

	struct thread* st = (struct thread*)memalign(8, 4096);
	__asm __volatile("mov %%rsp, %%rax" : "=a" (st->esp) : );
	__asm __volatile("mov %%rbp, %%rax" : "=a" (st->ebp) : );
	__asm __volatile("mov %%rsp, %%rax" : "=a" (st->edp) : );
	st->stack=(uintptr_t)malloc(sizeof(uintptr_t));

	fprintf(stdout,"SP: %016" PRIxPTR ", ",st->esp); 
	fprintf(stdout,"BP: %016" PRIxPTR "\n",st->ebp); 

	return st;
}

void thread_add_runqueue(struct thread *t) {
	int a = 0; 
	a++; 
}
void thread_yield(void) {
	int a = 0; 
	a++; 
}

void dispatch(void) {
	int a = 0; 
	a++; 
}
void schedule(void) {
	int a = 0; 
	a++; 
}
void thread_exit(void) {
	int a = 0; 
	a++; 
}
void thread_start_threading(void) {
	int a = 0; 
	a++; 
}