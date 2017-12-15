#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include "st.h"

// a is the semaphore value, b is the conditional variable, c is mutex lock
typedef struct{int a; st_cond_t b; st_mutex_t c;} semaphore;

void down(semaphore* s);
void up(semaphore* s);
void createSem(semaphore* s, int value);

#endif
