#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <setjmp.h> 
#include "threads.h"
#include <malloc.h>

#define CAST(t,e) ((t)(e)) 
#define TYPED_MALLOC(t) CAST(t*,malloc(sizeof(t))) 

// Static variable indicating stack size to use;
#define STACK_SIZE 4096 

struct scheduler* robin; 
int id_so_far = 0; 
jmp_buf GLOBAL_ENV; 
uintptr_t GLOBAL_SP, GLOBAL_BP; 

// Creates and initializes a thread using an 8-byte aligned memory allocation
// All stack pointer variables are populated accordingly
// If the thread creation is unsuccessful, NULL is returned
struct thread *thread_create(void (*f)(void *arg), void *arg) { 
   
   uintptr_t *memptr; 
	struct thread *process; 
	if(!posix_memalign((void **)&memptr,8,STACK_SIZE)) { 
		process = TYPED_MALLOC(struct thread); 
		process->stack = memptr; 
		process->esp = memptr+STACK_SIZE; 
		process->ebp = memptr; 
		process->id = ++id_so_far; 
		process->fs = f; 
		process->args = arg; 
		process->ran = 0; 
	} else { 
		process = NULL; 
	} 
	return process; 
} 

// Assign the thread to a scheduler struct (circular linked list)
void thread_add_runqueue(struct thread *tid) { 
	if(robin==NULL) {
		robin = TYPED_MALLOC(struct scheduler); 
		robin->start = NULL; 
	} 
	scheduler_insert(tid); 
}

// Schedules and dispatches the next process in the scheduler
void thread_yield(void) {

   printf("Thread yielding for thread (%d)\n", robin->current->id);

   // Only schedule/dispatch if process is not returning to this point;
   if(setjmp(robin->current->env) == 0) { 	   
		schedule(); 
		dispatch(); 
   }
   return;
}

// Save stack register results to the internal thread stack pointer variables
// Resume execution of the next thread scheduled to execute
void dispatch(void) {
	// move register value into esp 

	if(robin->current->id != 1) {
	__asm__ volatile("mov %%rax, %%rsp" : : "a" (robin->previous->esp) );
	__asm__ volatile("mov %%rax, %%rbp" : : "a" (robin->previous->ebp) );
   }
   
	if(!robin->current->ran) { 
		robin->current->ran = 1; 
      printf("Thread (%d) has been changed to ran.\n",robin->current->id);
		robin->current->fs(robin->current->args);
	} else {
		// move esp into register 
		__asm__ volatile("mov %%rsp, %%rax" : "=a" (robin->current->esp) : ); 
		__asm__ volatile("mov %%rbp, %%rax" : "=a" (robin->current->ebp) : );
		longjmp(robin->current->env,1); 
	} 

	thread_exit(); 
   return;
}

// Increment current and previous pointers in scheduler
void schedule(void) {

	scheduler_advance(); 
}

void thread_exit(void) {
 
   scheduler_remove(robin->current); 
   if(robin->size > 0)
      dispatch();
    
   __asm__ volatile("mov %%rsp, %%rax" : "=a" (GLOBAL_SP) : );  
   __asm__ volatile("mov %%rbp, %%rax" : "=a" (GLOBAL_BP) : ); 
   longjmp(GLOBAL_ENV,1); 
   //return;
}

void thread_start_threading(void) {
   
   //__asm__ volatile("mov %%rax, %%rsp" : : "a" (GLOBAL_SP) );  
   //__asm__ volatile("mov %%rax, %%rbp" : : "a" (GLOBAL_BP) ); 
   if(setjmp(GLOBAL_ENV)!=0)
        return; 
   
	dispatch(); 
}

void scheduler_insert(struct thread* t) {
 
	if(robin->start==NULL) {
		robin->start = t; 

      t->next = t;
//		robin->start->next = robin->start; 
//    robin->current = robin->start;
      robin->current = t;

//    robin->last = robin->start; 
      robin->last = t;

//    robin->last->next = robin->start; 
		robin->previous = t; 
		robin->size = 1; 
	} 
	else {
	  t->next = robin->current; 
	  robin->previous->next = t; 
	  robin->previous = t; 
	  robin->size++;   
	  //t->next = robin->start;
	  //robin->last->next = t; 
	  //robin->last = t; 
	  //robin->size++;  
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
 
		return; 
	} 

	pointer->next = check->next; 
   robin->current = robin->current->next;
   robin->previous->next = robin->current;
	free(check); 
	robin->size -= 1; 
	return; 
}

void scheduler_advance(void) {

	robin->previous = robin->current; 
	robin->current = robin->current->next; 
	printf("current thread is %d\n", robin->current->id);

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
