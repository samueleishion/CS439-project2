#ifndef THREADS_H_
#define THREADS_H_

struct thread {
	int id, ran; 
	jmp_buf env; 
	uintptr_t *stack, *esp, *ebp, *edp; 
	struct thread *next; 
	void (*fs)(void *); 
	void *args; 
};

struct scheduler { 
	int size; 
	struct thread* start; 
	struct thread* last; 
	struct thread* current; 
	struct thread* previous; 
}; 

// typedef struct Node {
// 	struct thread *t;
// 	struct Node *next;
// }node;

struct thread *thread_create(void (*f)(void *arg), void *arg);
void thread_add_runqueue(struct thread *tid);
void thread_yield(void);
void dispatch(void);
void schedule(void);
void thread_exit(void);
void thread_start_threading(void);

void scheduler_insert(struct thread* t); 
void scheduler_remove(struct thread* t); 
void scheduler_advance(void); 
// void queue_insert(node *pointer, struct thread *t);
// void queue_remove(node *pointer, struct thread *t);
#endif // THREADS_H
