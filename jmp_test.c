#include <stdio.h> 
#include <stdlib.h>
#include <setjmp.h> 

static jmp_buf buf; 

void second(void) {
	printf("second function\n"); 
	longjmp(buf,1); 
}

void first(void) {
	printf("first function \n"); 
	second(); 
}

int main() {
	if(!setjmp(buf)) {
		printf("first time main is called\n"); 
		first(); 
	} else {
		printf("second time main is called\n"); 
		printf("main\n"); 
	}
	return 0; 
}
