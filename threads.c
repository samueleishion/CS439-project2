#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <setjmp.h> 
#include "threads.h"
#include <malloc/malloc.h>

#define CAST(t,e) ((t)(e)) 
#define TYPED_MALLOC(t) CAST(t*,malloc(sizeof(t))) 

#define STACK_SIZE 4096 

struct scheduler* robin; 
int id_so_far = 0; 

struct thread *thread_create(int id, void (*f)(void *arg), void *arg) { 
	uintptr_t *memptr; 
	struct thread *process; 
	if(!posix_memalign((void **)&memptr,8,STACK_SIZE)) { 
		process = TYPED_MALLOC(struct thread); 
		// @TODEL 
		// printf("\tmemptr: \t%p\n",memptr); 
		// printf("\tmemptr+stack: \t%p\n",(memptr+STACK_SIZE)-(20-sizeof(uintptr_t))); 
		process->stack = memptr; 
		process->esp = memptr+STACK_SIZE; 
		process->ebp = memptr; 
		process->id = ++id_so_far; 
		process->fs = f; 
		process->args = arg; 
		process->ran = 0; 
		// @TODEL 
		printf("Thread created (%d).\n",process->id); 
	} else { 
		process = NULL; 
		// @TODEL 
		printf("Failed to create thread.\n"); 
	} 

	return process; 
} 

void thread_add_runqueue(struct thread *tid) { 
	if(robin==NULL) {
		robin = TYPED_MALLOC(struct scheduler); 
		robin->start = NULL; 
		// @TODEL 
		printf("Started queue.\n"); 
	} 
	scheduler_insert(tid); 
}

void thread_yield(void) {
	// @TODEL 
	printf("Yielding thread %d.\n", robin->current->id);  
	setjmp(robin->current->env); 
	schedule(); 
	dispatch(); 
}

void dispatch(void) {
	// move register value into esp 
	__asm__ volatile("mov %%rax, %%rsp" : : "a" (robin->previous->esp) );
	__asm__ volatile("mov %%rax, %%rbp" : : "a" (robin->previous->ebp) );

	if(!robin->current->ran) { 
		robin->current->fs(robin->current->args); 
		robin->current->ran = 1; 
	} else {
		// move esp into register 
		__asm__ volatile("mov %%rsp, %%rax" : "=a" (robin->current->esp) : ); 
		__asm__ volatile("mov %%rbp, %%rax" : "=a" (robin->current->ebp) : );
		longjmp(robin->current->env,1); 
	} 
	// @TODEL 
	printf("Dispatching thread %d.\n",robin->current->id); 
}

void schedule(void) {
	// @TODEL 
	printf("Scheduling thread.\n"); 
	scheduler_advance(); 
}

void thread_exit(void) {
	// @TODEL 
	printf("Exiting thread.\n"); 
	scheduler_remove(robin->current); 
}

void thread_start_threading(void) {
	// @TODEL 
	printf("Starting threading.\n"); 
	// thread_yield(); 
	dispatch(); 
}

void scheduler_insert(struct thread* t) {
	if(robin->start==NULL) {
		// @TODEL 
		printf("Inserting at start of queue.\n"); 
		robin->start = t; 
		robin->start->next = robin->start; 
		robin->current = robin->start; 
		robin->last = robin->start; 
		robin->last->next = robin->start; 
		robin->previous = t; 
		robin->size = 1; 
	} else {
		robin->last->next = t; 
		robin->last = t; 
		t->next = robin->start;
		robin->size++;  
	}
} 

void scheduler_remove(struct thread* t) {
	struct thread* pointer = robin->start; 
	struct thread* check = pointer->next; 
	int loop = 0; 

	while(check!=t && loop<1) {
		pointer = check; 
		check = pointer->next; 
		if(pointer==robin->start) loop++; 
	}
	
	if(loop>=1) {
		printf("Thread is not present.\n"); 
		return; 
	} 
	// @TODEL 
	else printf("Removing thread.\n"); 

	pointer->next = check->next; 
	free(check); 
	robin->size -= 1; 
	return; 
}

void scheduler_advance(void) {
	robin->previous = robin->current; 
	robin->current = robin->current->next; 
}

/* Pointer to first thread node in run queue */
// node *start;
// node *current_node; 
// struct thread *current_thread; 
// // @TODEL 
// int queue_counter = 0; 

// struct thread *thread_create(int id, void (*f)(void *arg), void *arg) {

// 	// struct thread* st = (struct thread*)memalign(8, 4096); 
// 	struct thread *st; 
// 	uintptr_t *memptr; 
// 	if(!posix_memalign((void **)&memptr,8,4096)) { 
// 		st = (struct thread*)memptr; 
// 		__asm __volatile("mov %%rsp, %%rax" : "=a" (st->esp) : );
// 		__asm __volatile("mov %%rbp, %%rax" : "=a" (st->ebp) : );
// 		__asm __volatile("mov %%rsp, %%rax" : "=a" (st->edp) : );
// 		st->stack=(uintptr_t)malloc(sizeof(uintptr_t)); 
// 		// @TODEL 
// 		st->id = id; 
// 		printf("Successfully allocating memory for thread. \n"); 
// 	} else {
// 		st = NULL; 
// 		printf("Error allocating memory for thread. \n"); 
// 	}

// 	return st; 
// }

// void queue_thread_remove(node *pointer, struct thread *t) {
// 	node *start = pointer;

// 	while(pointer->next!=start && (pointer->next)->t != t) {
// 		pointer = pointer -> next;
// 	}
// 	if(pointer->next==start) {
// 		printf("Thread is not present in the queue");
// 		 return; 
// 	}

// 	node *temp;
// 	temp = pointer -> next;
// 	pointer->next = temp->next;
// 	free(temp);
// 	return;
// }


// void thread_add_runqueue(struct thread *t) {
// 	// @TODEL 
// 	printf("To add %d, start: %p\n",t->id,start); 

// 	if(start==NULL) {
// 	// if(queue_counter==0) {
// 		queue_counter = 1; 
// 		start = (node *)malloc(sizeof(node)); 
// 		start->next = start; 
// 		start->t = t; 
// 		// @TODEL 
// 		printf("Created start node"); 
// 	} else {
// 		queue_counter += 1; 
// 		node *pointer = start; 
// 		node *new_node = (node *)malloc(sizeof(node)); 
// 		while(pointer->next!=start) {
// 			pointer = pointer -> next;
// 		}

// 		new_node->t = t; 
// 		new_node->next = start; 
// 		pointer->next = new_node; 

// 		// pointer -> t = t;
// 		// pointer = pointer -> next;
// 		// pointer -> next = start; //(node *)malloc(sizeof(node));
// 		// @TODEL 
// 		printf("Added node"); 
// 	}
// }

// void thread_yield(void) { 
// 	__asm __volatile("mov %%rsp, %%rax" : "=a" (current_thread->esp) : );
// 	__asm __volatile("mov %%rbp, %%rax" : "=a" (current_thread->ebp) : );
// 	setjmp(current_thread->env); 
// 	// @TODEL 
// 	// printf("Queue size: %d\n",queue_counter); 
// 	schedule(); 
// 	dispatch(); 
// } 

// void thread_exit(void) { 
// 	node *to_be_removed = current_node; 
// 	current_node = current_node->next; 
// 	current_thread = current_node->t; 
// 	queue_thread_remove(to_be_removed,to_be_removed->t); 
// }

// void schedule(void) { 
// 	current_node = current_node->next; 
// 	current_thread = current_node->t; 
// }

// void dispatch(void) {
// 	// @TODEL 
// 	// printf("%d\n",current_thread->id); 
// 	__asm __volatile("mov %%rax, %%rsp" : : "a" (current_thread->esp) ); 
// 	__asm __volatile("mov %%rax, %%rbp" : : "a" (current_thread->ebp) ); 
// 	longjmp(current_thread->env,1); 
// } 

// void thread_start_threading(void) { 
// 	current_node = start; 
// 	current_thread = start->t; 
// 	thread_yield(); 
// 	// schedule(); 
// 	// dispatch(); 
// } 
