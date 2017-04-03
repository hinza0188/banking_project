#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <fifo_q.h>

// define time in nano-second => used in nanosleep();
#define MIN (100000000);		// (actual)100ms == (system)1-minute
#define SEC (100000000/60);	// (actual)1.67ms == (system)1-second
#define HR  (100000000*60);	// (actual)6second == (system)1-hour

// declare variables
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
struct timespec customer_entering_time, teller_process_time1, teller_process_time2, teller_process_time3, end_time;
// these are variables that represents flags (0:False | 1:True)
int bank_closed, queue_empty;
// these are variables that represents counting values ( greater than or equal to 0)
int total_customers = 0;
double time_waiting = 0;

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
	int min = MIN;
	int max = 4 * MIN;

	while (1) {
		if (!bank_closed) { // should not run if bank is closed
			int nsec = ranged_random(min,max);
			customer_entering_time.tv_sec = 0;
			customer_entering_time.tv_nsec = nsec;
			enqueue(total_customers);
			total_customers++;
			printf("Customer_%d has come and waiting to be served\n", total_customers);
			nanosleep(&customer_entering_time, &end_time);
		}
	}
	return 0;
}


void* teller1( void* arg ) {
	int customer_id;
	double wtime;
	int min = 30*SEC;
	int max = 8 *MIN;

    while( 1 ) {
    	if (qSize > 0) {	// run only if customer waits on the line
    		int nsec = ranged_random(min,max);
			teller_process_time1.tv_sec = 0;
			teller_process_time1.tv_nsec = nsec;
    		pthread_mutex_lock( &mutex );
			customer_id = dequeue();					// dequeue customer from the waiting list
			// increment the waiting time for calculating average waiting time
			wtime = nsec/SEC;
			time_waiting += wtime;
			printf("Teller_1 is serving customer_%d\n", customer_id);
			pthread_mutex_unlock( &mutex );
			/* snooze for 30 sec - 8 min in system time */
			nanosleep(&teller_process_time1, &end_time);
    	}
    }
    return 0;
}

void* teller2( void* arg ) {
	int customer_id;
	double wtime;
	int min = 30*SEC;
	int max = 8 *MIN;
	// generate random number from 30sec to 8 minutes

    while( 1 ) {
    	if (qSize > 0) {	// run only if customer waits on the line
			int nsec = ranged_random(min,max);
			teller_process_time2.tv_sec = 0;
			teller_process_time2.tv_nsec = nsec;
			pthread_mutex_lock( &mutex );
			customer_id = dequeue();					// dequeue customer from the waiting list
			// increment the waiting time for calculating average waiting time
			wtime = nsec/SEC;
			time_waiting += wtime;
			printf("Teller_2 is serving customer_%d\n", customer_id);
			pthread_mutex_unlock( &mutex );
			/* snooze for 30sec - 8min in system time */
			nanosleep(&teller_process_time2, &end_time);
    	}
    }
    return 0;
}

void* teller3( void* arg ) {
	int customer_id;
	double wtime;
	int min = 30*SEC;
	int max = 8 *MIN;
	// generate random number from 30sec to 8 minutes

    while( 1 ) {
    	if (qSize > 0) {	// run only if customer waits on the line
			int nsec = ranged_random(min,max);
			teller_process_time3.tv_sec = 0;
			teller_process_time3.tv_nsec = nsec;
			pthread_mutex_lock( &mutex );
			customer_id = dequeue();					// dequeue customer from the waiting list
			// increment the waiting time for calculating average waiting time
			wtime = nsec/SEC;
			time_waiting += wtime;
			printf("Teller_3 is serving customer_%d\n", customer_id);
			pthread_mutex_unlock( &mutex );
			/* snooze for 30sec - 8min in system time */
			nanosleep(&teller_process_time3, &end_time);
    	}
    }
    return 0;
}

int main( int argc, char *argv[] ) {
	int avg_time;
	//QueueInit();
	srand(time(NULL));
	printf("Current Time: 08:00 A.M. || Bank Opened\r\n");
	bank_closed = 0;			// bank is opens
	pthread_create( NULL, NULL, &enter_customer, NULL );	// customer enters the bank every 1-4 min
	pthread_create( NULL, NULL, &teller1, NULL );			// execute teller process 1
    pthread_create( NULL, NULL, &teller2, NULL );			// execute teller process 2
	pthread_create( NULL, NULL, &teller3, NULL );			// execute teller process 3

    /* Let the threads run for 60 seconds. */

    sleep( 42 );	// this represents 420 minutes == 7 hours
    //// See if all customer has been served by tellers //////
    if (!empty()) {											//
    	printf("Not all customer has been served :(\n");	//
    	printf("Left-over customer: %d\n", queue_size());	//
    }														//
    //////////////////////////////////////////////////////////
    bank_closed = 1;			// bank is closed
    printf("Current Time: 04:00 P.M. || Bank Closed\r\n");

    // now prints all results
    avg_time = (time_waiting/total_customers); // get total avg time in seconds
    printf("total customer: %d || avg_time: %d min %d sec\n", total_customers, ((avg_time%3600)/60), ((avg_time%3600)%60));

    return EXIT_SUCCESS;
}
