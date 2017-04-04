#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <fifo_q.h>
#include <timer.h>

// declare variables
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
struct timespec customer_entering_time, teller_process_time1, teller_process_time2, teller_process_time3, end_time;
// these are variables that represents flags (0:False | 1:True)
int bank_closed, queue_empty;
// these are variables that represents counting values ( greater than or equal to 0)
int queue_max_time, max_depth, queue_depth = 0;
int queue_wait_time, total_customers, teller_wait_time = 0;
long long int system_time = 0;

/**
 * Generates random number from Min to Max
 */
int ranged_random(int Min, int Max) {
	int diff = Max-Min;
	return (int) (((double)(diff+1)/RAND_MAX) * rand() + Min);
}


/**
 *  every 1 - 4 minute, customer enters the bank and
 * 	start filling in the queue
 */
void* enter_customer( void* arg ) {
	// generate random number from 1 to 4 minutes
	int hr,mn,sec;
	int min = MIN;
	int max = 4 * MIN;

	while (1) {
		if (!bank_closed) { // should not run if bank is closed
			pthread_mutex_lock( &mutex );

			enqueue(total_customers, system_time);
			queue_depth = queue_size();
			if(queue_depth>max_depth) {
				max_depth = queue_depth; // used for denoting the maximum depth of the queue.
			}
			total_customers++;

			pthread_mutex_unlock( &mutex );

		    hr = 9 + system_time/3600;
			mn = (system_time%3600)/60;
			sec = (system_time%3600)%60;
			printf("Customer_%d entered bank and waiting to be served ", total_customers-1);
			printf("[%d:%d:%d]\n", hr,mn,sec);

			int nsec = ranged_random(min,max);
			customer_entering_time.tv_nsec = nsec;
			nanosleep(&customer_entering_time, &end_time);
		}
	}
	return 0;
}


void* teller1( void* arg ) {
	cust_timing customer_data;
	int min = 30*SEC;
	int max = 8 *MIN;
	int hr,mn,sec;
	float wait_time;

    while( 1 ) {
    	int nsec = ranged_random(min,max);	// get time between 30sec to 8min
		teller_process_time1.tv_nsec = nsec;
    	if (qSize > 0) {	// run only if customer waits on the line
    		pthread_mutex_lock( &mutex );

    		teller_wait_time += nsec/SEC;				// increment teller wait time
			customer_data = dequeue();					// dequeue customer from the waiting list
			customer_data.out_time = system_time;

			pthread_mutex_unlock( &mutex );

			// increment the waiting time for calculating average waiting time
			hr = 9 + system_time/3600;
			mn = (system_time%3600)/60;
			sec = (system_time%3600)%60;
			printf("Teller_1 is serving customer_%d ", customer_data.cust_id);
			printf("[%d:%d:%d]\n", hr,mn,sec);

			// get out time here
			wait_time = (customer_data.out_time - customer_data.in_time)/SEC;
			if(wait_time > queue_max_time) {
				queue_max_time = wait_time;		// replace wait time if greater
			}
			queue_wait_time += wait_time;		// increment total wait time
			printf("Customer_%d's wait_time: %5.2f\n", customer_data.cust_id, wait_time);

			/* snooze for 30 sec - 8 min in system time */
			nanosleep(&teller_process_time1, &end_time);
    	}
    }
    return 0;
}

void* teller2( void* arg ) {
	cust_timing customer_data;
	int min = 30*SEC;
	int max = 8 *MIN;
	int hr, mn, sec;
	float wait_time;

    while( 1 ) {
		int nsec = ranged_random(min,max);
		teller_process_time2.tv_nsec = nsec;
    	if (qSize > 0) {	// run only if customer waits on the line
			pthread_mutex_lock( &mutex );

			teller_wait_time += nsec/SEC;				// increment teller wait time
			customer_data = dequeue();					// dequeue customer from the waiting list
			customer_data.out_time = system_time;

			pthread_mutex_unlock( &mutex );

			// increment the waiting time for calculating average waiting time
			hr = 9 + system_time/3600;
			mn = (system_time%3600)/60;
			sec = (system_time%3600)%60;
			printf("Teller_2 is serving customer_%d ", customer_data.cust_id);
			printf("[%d:%d:%d]\n", hr,mn,sec);

			// get out time here
			wait_time = (customer_data.out_time - customer_data.in_time)/SEC;
			if(wait_time > queue_max_time) {
				queue_max_time = wait_time;		// replace wait time if greater
			}
			queue_wait_time += wait_time;		// increment total wait time
			printf("Customer_%d's wait_time: %5.2f\n", customer_data.cust_id, wait_time);

			/* snooze for 30sec - 8min in system time */
			nanosleep(&teller_process_time2, &end_time);
    	}
    }
    return 0;
}

void* teller3( void* arg ) {
	cust_timing customer_data;
	int min = 30*SEC;
	int max = 8 *MIN;
	int hr, mn, sec;
	float wait_time;

    while( 1 ) {
    	int nsec = ranged_random(min,max);
		teller_process_time3.tv_nsec = nsec;
    	if (qSize > 0) {	// run only if customer waits on the line
			pthread_mutex_lock( &mutex );

			teller_wait_time += nsec/SEC;				// increment teller wait time
			customer_data = dequeue();					// dequeue customer from the waiting list
			customer_data.out_time = system_time;

			pthread_mutex_unlock( &mutex );

			// increment the waiting time for calculating average waiting time
			hr = 9 + customer_data.out_time/3600;
			mn = (customer_data.out_time%3600)/60;
			sec = (customer_data.out_time%3600)%60;
			printf("Teller_3 is serving customer_%d ", customer_data.cust_id);
			printf("[%d:%d:%d]\n", hr,mn,sec);

			// get out time here
			wait_time = (customer_data.out_time - customer_data.in_time)/SEC;
			if(wait_time > queue_max_time) {
				queue_max_time = wait_time;		// replace wait time if greater
			}
			queue_wait_time += wait_time;		// increment total wait time
			printf("Customer_%d's wait_time: %5.2f\n", customer_data.cust_id, wait_time);

			/* snooze for 30sec - 8min in system time */
			nanosleep(&teller_process_time3, &end_time);
    	}
    }
    return 0;
}


int main( int argc, char *argv[] ) {
	int avg_teller_time, max_queue_time;
	float avg_queue_time;
	srand(time(NULL));
	printf("Current Time || 08:00 A.M. || Bank Opened\r\n");
	bank_closed = 0;										// bank is opens

	pthread_create( NULL, NULL, &general_time, NULL );		// incrememnt system_time clock for timing calculation
	pthread_create( NULL, NULL, &enter_customer, NULL );	// customer enters the bank every 1-4 min
	pthread_create( NULL, NULL, &teller1, NULL );			// execute teller process 1
    pthread_create( NULL, NULL, &teller2, NULL );			// execute teller process 2
	pthread_create( NULL, NULL, &teller3, NULL );			// execute teller process 3

	sleep( 42 );	// this represents 420 minutes == 7 hours
	//sleep( 42 );	// this represents 420 minutes == 7 hours
    //// See if all customer has been served by tellers //////
    if (!empty()) {											//
    	printf("Not all customer has been served :(\n");	//
    	printf("Left-over customer: %d\n", queue_size());	//
    }														//
    //////////////////////////////////////////////////////////
    bank_closed = 1;			// bank is closed
    printf("Current Time || 04:00 P.M. || Bank Closed\n\n");

    // now prints all results
    /* 1. The total number of customers serviced during the day. */
    printf("Total number of customer: %d\n", total_customers);

    /* 2. The average time each customer spends waiting in the queue */

    avg_queue_time = (queue_wait_time/total_customers);
    printf("Average wating time in the queue: %4.2f sec\n", avg_queue_time );

    /* 3. The average time each customer spends with the teller */
    avg_teller_time = (teller_wait_time/total_customers);
	printf("Average teller time spent: %d min %d sec\n", ((avg_teller_time%3600)/60), ((avg_teller_time%3600)%60) );

	/* 4. The average time tellers wait for customers */


    /* 5. The maximum customer wait time in the queue */
	max_queue_time = queue_max_time/SEC;
	printf("The maximum customer wait time in the queue: %d\n", max_queue_time);
    /* 6. The maximum wait time for tellers waiting for customers */

    /* 7. The maximum transaction time for the tellers */

    /* 8. The maximum depth of the customer queue */
	printf("The maximum depth of the customer queue: %d\n", max_depth);

    return EXIT_SUCCESS;
}
