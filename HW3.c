#include <stdlib.h>
#include <stdio.h>
#include "buffer.h"
#include "st.h"

#define BUFFER_SIZE 256
#define LINE_SIZE 80
	
// threads 2 and 3 need two buffers each so make structs for these
struct DoubleBuffer {
	Buffer input;
	Buffer output;
};

// function for taking input
void *takeInput(void *outputBuffer){
	// type cast the buffer
	(Buffer)outputBuffer;

	// initialize variables used
	char c;

	// take input from stdin
	while(scanf("%c", &c) != EOF){
		deposit(outputBuffer, c);
	}

	// deposit EOF so next thread knows its done
	deposit(outputBuffer, EOF);

	// exit the thread
	st_thread_exit(NULL);	
}

// function for processing newline
void *processNewline(void *dubBuffer){
	// create pointer for DoubleBuffer
	struct DoubleBuffer *db = dubBuffer;	

	// initialize variables used
	char c;

	// take input and remove newlines
	while(1){
		// take input from buffer
		c = remoove(db->input);

		// replace the newline
		if(c == '\n'){
			c = ' ';
		}

		// deposit the char in buffer
		deposit(db->output, c);

		// exit if EOF is found
		if(c == EOF){
			deposit(db->output, c);
			break;
		}
	}

	// free up buffer space
	free(db->input);
	free(db);

	// exit the thread
	st_thread_exit(NULL);	
}

// function for removing double star and replacing with ^
void *processStar(void *dubBuffer){
	// create pointer for DoubleBuffer
	struct DoubleBuffer *db = dubBuffer;

	// initialize variables used
	char current;
	char previous = '\0';
	int starCollapse = 0;

	// take input and remove **
	while(1){
		// take input from buffer
		current = remoove(db->input);

		// remove the star and deposit
		if(current == '*' && previous == '*'){
			deposit(db->output, '^');
			current = '\0';
			starCollapse = 1;
		}
		else if(previous == '*'){
			if(!starCollapse){
				deposit(db->output, previous);
			}
			starCollapse = 0;
		}
		else{
			if(previous != '\0'){
				deposit(db->output, previous);
			}
			starCollapse = 0;
		}

		// stop on EOF
		if(current == EOF){
			deposit(db->output, current);
			break;
		}

		// assign previous char
		previous = current;
	}

	// free up buffer space
	free(db->input);
	free(db);

	// exit the thread
	st_thread_exit(NULL);	
}

// function for printing output
void *printOutput(void *inputBuffer){
	// type cast the buffer
	(Buffer)inputBuffer;

	char line[LINE_SIZE + 1];
	char c;
	int charPos;

	// loop to print output taken from buffer
	while(1){
		// take input from buffer
		c = remoove(inputBuffer);

		// exit if EOF is found
		if(c == EOF){
			break;
		}

		// put input in line
		if(charPos < LINE_SIZE){
			line[charPos] = c;
		}

		// increment position 
		charPos++;

		// if end of line has been reached, print
		if(charPos == LINE_SIZE){
			line[charPos] = 0;
			printf("%s\n", line);
			charPos = 0;
		}
	}

	// free up buffer space
	free(inputBuffer);

	// exit the main thread
	st_thread_exit(NULL);	
}

int main(){
	// initialize runtime library for libST
	st_init();

	// make buffers for the threads
	Buffer buffer0 = createBuffer(BUFFER_SIZE);
	Buffer buffer1 = createBuffer(BUFFER_SIZE);
	Buffer buffer2 = createBuffer(BUFFER_SIZE);

	struct DoubleBuffer *dubBuf0 = malloc(sizeof(struct DoubleBuffer));
	dubBuf0->input = buffer0;
	dubBuf0->output = buffer1;
	struct DoubleBuffer *dubBuf1 = malloc(sizeof(struct DoubleBuffer));
	dubBuf1->input = buffer1;
	dubBuf1->output = buffer2;

	// create thread for taking input
	if (st_thread_create(&takeInput, buffer0, 0 , 0) == NULL){
		perror("error creating thread for processing");
		exit(EXIT_FAILURE);
	}

	// create thread for newline to space
	if (st_thread_create(&processNewline, dubBuf0, 0 , 0) == NULL){
		perror("error creating thread for output");
		exit(EXIT_FAILURE);
	}
	
	// create thread for ** to ^
	if (st_thread_create(&processStar, dubBuf1, 0 , 0) == NULL){
		perror("error creating thread for output");
		exit(EXIT_FAILURE);
	}

	// create thread for outputing lines
	if (st_thread_create(&printOutput, buffer2, 0 , 0) == NULL){
		perror("error creating thread for output");
		exit(EXIT_FAILURE);
	}

	// exit the main thread
	st_thread_exit(NULL);	

	return 0;
}








