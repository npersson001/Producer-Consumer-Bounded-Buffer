#include "buffer.h"
#include <assert.h>
#include <stdlib.h>
#include "semaphore.h"
#include <stdio.h>

struct Buffer{
         semaphore empty;
         semaphore full;
         int size;
         int nextIn;
         int nextOut;
         char buf[];
};

// deposit a char into the given buffer
void deposit(Buffer buffer, char c){
	down(&(buffer->empty));

	// assert awaited condition - at least one empty buffer available
	assert(buffer->empty.a >= 0);

	buffer->buf[buffer->nextIn] = c;
	buffer->nextIn = (buffer->nextIn + 1) % buffer->size;
	
	// assert synchronization condition - at least one full buffer
	assert(buffer->full.a < buffer->size);	

	up(&(buffer->full));
}

//remove a char from the buffer and return that char
char remoove(Buffer buffer){
	char c;
	down(&(buffer->full));

	// assert awaited condition - at least one full buffer available
	assert(buffer->full.a >= 0);

	c = buffer->buf[buffer->nextOut];
	buffer->nextOut = (buffer->nextOut + 1) % buffer->size;
	
	// assert synchronization condition - at least one empty buffer 
	assert(buffer->empty.a < buffer->size);	

	up(&(buffer->empty));
	return c;
}

// create and place buffer on heap
Buffer createBuffer(int n){
	// make space for buffer on heap
	Buffer buffer = malloc( sizeof(struct Buffer) + (n + 1) * sizeof(char));
	
	// inialize the semaphores
	createSem(&(buffer->empty), n);
	createSem(&(buffer->full), 0);
	
	buffer->size = n;
	buffer->nextIn = 0;
	buffer->nextOut = 0;

	return buffer;
}

