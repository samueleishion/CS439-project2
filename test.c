#include <stdio.h> 
#include <inttypes.h> 
#include <setjmp.h> 
#include "threads.h" 

void f4(void *arg) {
  int i; 
  while(1) {
    printf("thread 4: %d\n", i++); 
    thread_yield(); 
  }
}

void f3(void *arg) {
  int i; 
  while (1) {
    printf("thread 3: %d\n", i++);
    if(i==100) {
      i = 0; 
    }
    thread_yield(); 
  }
} 

void f2(void *arg) {
  int i; 
  while(1) {
    printf("thread 2: %d\n",i++); 
    if(i==10) {
      i = 0; 
    }
    thread_yield(); 
  }
}


void f1(void*arg) {
  int i = 100; 
  struct thread *t2 = thread_create(f2, NULL); 
  struct thread *t3 = thread_create(f3, NULL); 
  struct thread *t4 = thread_create(f4, NULL); 

  thread_add_runqueue(t4); 
  thread_add_runqueue(t3); 
  thread_add_runqueue(t2); 

  while(1) {
    printf("thread 1: %d\n",i++); 
    if(i==110) {
      i = 100; 
    }
    thread_yield(); 
  }
}

int main(int argc, char **argv) {
  struct thread *t1 = thread_create(f1, NULL); 
  thread_add_runqueue(t1); 
  thread_start_threading(); 
  return 0; 
} 
      
