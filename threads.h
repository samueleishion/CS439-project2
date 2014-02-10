#ifndef THREADS_H_
#define THREADS_H_

struct thread {
	uintptr_t  stack, esp, ebp, edp;
};

typedef struct Node {
	struct thread *t;
	struct Node *next;
}node;

struct thread *thread_create(void (*f)(void *arg), void *arg);
void thread_add_runqueue(node *pointer, struct thread *t);
void thread_yield(void);
void dispatch(void);
void schedule(void);
void thread_exit(void);
void thread_start_threading(void);

void queue_insert(node *pointer, struct thread *t);
void queue_remove(node *pointer, struct thread *t);
#endif // THREADS_H
