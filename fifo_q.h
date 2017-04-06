/*
 * fifo_q.h
 *
 *  Created on: Apr 1, 2017
 *      Author: yxa8247
 */

typedef struct {
	int cust_id;
	long long int in_time;
	long long int out_time;
}cust_record;

void QueueInit(void);
void enqueue(int,long long int);
cust_record dequeue(void);
int queue_size(void);
