/*
 * fifo_q.h
 *
 *  Created on: Apr 1, 2017
 *      Author: yxa8247
 */

typedef struct {
	int cust_id;
	int in_time;
	int out_time;
}cust_timing;

void QueueInit(void);
void enqueue(int,int);
cust_timing dequeue(void);
int empty(void);
int queue_size(void);

extern int qSize;

