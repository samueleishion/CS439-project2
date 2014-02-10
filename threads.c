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

void queue_thread_remove(node *pointer, struct thread *t) {
	node *start = pointer;

	while(pointer->next!=start && (pointer->next)->t != t) {
		pointer = pointer -> next;
	}
	if(pointer->next==start) {
		printf("Thread is not present in the queue");
		 return;
	}

	node *temp;
	temp = pointer -> next;
	pointer->next = temp->next;
	free(temp);
	return;
}


void thread_add_runqueue(node *pointer, struct thread *t) {
	node *start = pointer;
	while(pointer->next!=start) {
		pointer = pointer -> next;
	}

	pointer -> next = (node *)malloc(sizeof(node));
	pointer = pointer -> next;
	pointer -> t = t;
}

