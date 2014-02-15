#include <stdio.h>
#include <inttypes.h>
#include <setjmp.h> 
#include "threads.h"

void f3(void *arg)
{
    printf("F3\n");
    int i;
    while (1) {
        printf("thread 3: %d\n", i++);
        thread_yield();
    }
}

void f2(void *arg)
{
    printf("F2\n");
    int i = 0;
    while(1) {
        printf("thread 2: %d\n",i++);
        if (i == 10) {
            i = 0;
        }
        thread_yield();
    }
}

void f1(void *arg)
{
    printf("F1\n");
    int i = 100;
    struct thread *t2 = thread_create(2, f2, NULL);
    thread_add_runqueue(t2);
    struct thread *t3 = thread_create(3, f3, NULL);
    thread_add_runqueue(t3);
    while(1) {
        printf("thread 1: %d\n", i++);
        if (i == 110) {
            i = 100;
        }
        thread_yield();
    }
}

int main(int argc, char **argv)
{
    printf("MAIN\n");
    struct thread *t1 = thread_create(1, f1, NULL);
    thread_add_runqueue(t1);
    thread_start_threading();
    printf("\nexited\n");
    return 0;
}
