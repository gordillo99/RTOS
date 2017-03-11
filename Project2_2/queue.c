#include "queue.h"

/*
 *  Checks if queue is full
 */
volatile int isFull(volatile int *QCount) {
    return *QCount == MAXTHREAD - 1;
}

/*
 *  Checks if queue is empty, READY QUEUE SHOULD NEVER BE EMPTY
 */
volatile int isEmpty(volatile int *QCount) {
    return *QCount == 0;
}

/*
 *  Insert into the queue sorted by priority
 */
void enqueue(volatile PD **p, volatile PD **Queue, volatile int *QCount) {
    if(isFull(QCount)) {
        return NULL;
    }

    int i = (*QCount) - 1;
    volatile PD *new = *p;
    volatile PD *temp = Queue[i];

    while(i >= 0) {
        Queue[i+1] = Queue[i];
        i--;
        temp = Queue[i];
    }

    Queue[i+1] = *p;
    (*QCount)++;
}

/*
 *  Return the first element of the Ready Queue
 */
volatile PD *dequeue(volatile PD **Queue, volatile int *QCount) {

   if(isEmpty(QCount)) {
	   return NULL;
   }

   volatile PD *result = (Queue[(*QCount)-1]);
   (*QCount)--;

   return result;
}

void enqueuePeriodic(volatile PD **p, volatile PD **Queue, volatile int *QCount) {
	if(isFull(QCount)) {
		return;
	}

	int i = (*QCount) - 1;

	volatile PD *new = *p;

	volatile PD *temp = Queue[i];

	while(i >= 0 && (new->countdown >= temp->countdown)) {
		Queue[i+1] = Queue[i];
		i--;
		temp = Queue[i];
	}

	Queue[i+1] = *p;
	(*QCount)++;
}

volatile PD *peek(volatile PD **Queue, volatile int *QCount) {
	if(isEmpty(QCount)) {
		return NULL;
	}
	volatile PD *result = (Queue[(*QCount)-1]);
	return result;
}