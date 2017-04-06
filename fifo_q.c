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

/*
 * Linked node structure
 * that contains cust_timing structure
 */
struct node{
	cust_record data;
    struct node *ptr;
}*head,*tail,*temp;


/**
 * Initialize the queue with null pointer
 */
void QueueInit(void){
	head = tail = NULL;
}


/**
 * Put new data into the queue
 * if tail is empty, put new data into the tail
 * and equalize with pointer of the head
 */
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


/**
 * This function removes top of the queue
 * and returns the removed data
 * if head is null pointer, return null data
 * if second node in the queue is not pointing at null
 * then copy the data into return variable and pop head
 * if neither, copy the data into return variable and re-initialize the queue
 */
cust_record dequeue(void){
	cust_record pop;
	temp = head;

	if (temp == NULL) {
		// case 0: error
		// the queue is empty! failing to dequeue
		pop.cust_id=NULL;
		pop.in_time=NULL;
		pop.out_time=NULL;
		return pop;
	} else if (temp->ptr != NULL) {
		// case 1: qSize >= 2
		temp = temp->ptr;
		pop = head->data;
		free(head);
		head = temp;
	} else {
		// case 2: qSize == 1
		pop = head->data;
		free(head);
		QueueInit();
	}
	qSize--;
	return pop;

}


/**
 * Returns size of the queue
 */
int queue_size(){
    return qSize;
}
