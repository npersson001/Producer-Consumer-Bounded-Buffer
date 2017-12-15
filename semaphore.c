#include "semaphore.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "st.h"

void createSem(semaphore * s, int value) {
	// set initial value	
	s->a = value;

	// create conditional variable
	s->b = st_cond_new();
	if(s->b == NULL) {
		perror("st_cond_new failed");
		exit(1);
	}

	// create mutex variable
	s->c = st_mutex_new();
	if(s->c == NULL) {
		perror("st_mutex_new failed");
		exit(1);
	}
}

void down(semaphore * s) {
	// get mutex
	st_mutex_lock(s->c);

	if(s->a == 0) {
		// release mutex to call wait
		st_mutex_unlock(s->c);

		// wait on conditional var
		st_cond_wait(s->b);

		// gain mutex again 
		st_mutex_lock(s->c);
	}
	
	// check condition
	assert(s->a > 0);

	// decrement value
	s->a = s->a - 1;

	// release mutex
	st_mutex_unlock(s->c);
}

void up(semaphore *s){
	// get mutex
	st_mutex_lock(s->c);

	// increment value
	s->a = s->a + 1;

	// release mutex
	st_mutex_unlock(s->c);

	// check condition
	assert(s->a > 0);

	// send signal to waiting processes
	st_cond_signal(s->b);
}

