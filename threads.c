#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include "threads.h"
#include <malloc.h>

struct thread *thread_create(void (*f)(void *arg), void *arg) {

	struct thread* st = (struct thread*)memalign(8, 4096);
	
	__asm __volatile("mov %%rsp, %%rax" : "=a" (st->esp) : );
	__asm __volatile("mov %%rbp, %%rax" : "=a" (st->ebp) : );
	__asm __volatile("mov %%rsp, %%rax" : "=a" (st->edp) : );

	st->stack=(uintptr_t)malloc(sizeof(uintptr_t));
	return st;
}
