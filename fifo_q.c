/*
 * fifo_q.c
 *
 *  Created on: Apr 1, 2017
 *      Author: yxa8247
 */
#include <stdio.h>
#include <stdlib.h>
#include <fifo_q.h>

int qSize = 0;

struct node{
    cust_timing data;
    struct node *ptr;
}*head,*tail,*temp;

void QueueInit(void){
	head = tail = NULL;
}
void enqueue(int customer, long long int in_time){
	if (tail == NULL) {
		tail = (struct node *)malloc(sizeof(struct node));
		tail->ptr = NULL;
		tail->data.in_time = in_time;
		tail->data.out_time = NULL;
		tail->data.cust_id = customer;
		head = tail;
	} else {
		temp=(struct node *)malloc(sizeof(struct node));
		tail->ptr = temp;
		temp->data.cust_id = customer;
		temp->data.in_time = in_time;
		temp->data.out_time = NULL;
		temp->ptr = NULL;
		tail = temp;
	}
	qSize++;
}
cust_timing dequeue(void){
	cust_timing done;
	temp = head;

	if (temp == NULL) {
		// case 0: error
		done.cust_id=NULL;
		done.in_time=NULL;
		done.out_time=NULL;
		return done; // the queue is empty! failing to dequeue
	} else if (temp->ptr != NULL) {
		// case 1: qSize >= 2
		temp = temp->ptr;
		done = head->data;
		free(head);
		head = temp;
	} else {
		// case 2: qSize == 1
		done = head->data;
		free(head);
		head = tail = NULL;
	}
	qSize--;
	return done;

}

int empty() {
	if (head==NULL && tail==NULL){
		return 1;	// true, the queue is empty
	} else {
		return 0;	// false, the queue element still exists
	}
}

int queue_size(){
    return qSize;
}
