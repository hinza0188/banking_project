/*
 * fifo_q.h
 *
 *  Created on: Apr 1, 2017
 *      Author: yxa8247
 */

void QueueInit(void);
void enqueue(int);
int dequeue(void);
int empty(void);
int queue_size(void);

extern int qSize;
