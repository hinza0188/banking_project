/*
 * fifo_q.c
 *
 *  Created on: Apr 1, 2017
 *      Author: yxa8247
 */
#include <stdio.h>
#include <stdlib.h>

int qSize = 0;

struct node{
    int customer;
    struct node *ptr;
}*head,*tail,*temp;

void QueueInit(void){
	head = tail = NULL;
}
void enqueue(int customer){
	if (tail == NULL) {
		tail = (struct node *)malloc(sizeof(struct node));
		tail->ptr = NULL;
		tail->customer = customer;
		head = tail;
	} else {
		temp=(struct node *)malloc(sizeof(struct node));
		tail->ptr = temp;
		temp->customer = customer;
		temp->ptr = NULL;
		tail = temp;
	}
	qSize++;
}
int dequeue(void){
	int done;
	temp = head;

	if (temp == NULL) {
		// case 0: error
		return 0; // the queue is empty! failing to dequeue
	} else if (temp->ptr != NULL) {
		// case 1: qSize >= 2
		temp = temp->ptr;
		done = head->customer;
		free(head);
		head = temp;
	} else {
		// case 2: qSize == 1
		done = head->customer;
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
