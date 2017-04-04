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
int max_depth, queue_depth, tran1, tran2, tran3, total_customers = 0;
long long int teller_waiting_time, teller_working_time, queue_max_time;
long long int t1p, t1d, t2p, t2d, t3p, t3d, t_max_wait = 0;
long long int system_time, queue_wait_time= 0;

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
	int hr,mn,sec, nsec;
	int min = MIN;
	int max = 4 * MIN;

	while (1) {
		if (!bank_closed) { 						// Run only if bank is not closed
			pthread_mutex_lock( &mutex );			// Lock mutex to prevent race condition
			enqueue(total_customers, system_time);	// put customer into the line
			queue_depth = queue_size();
			if(queue_depth>max_depth) {
				max_depth = queue_depth; 			// used for denoting the maximum depth of the queue.
			}
			total_customers++;
			pthread_mutex_unlock( &mutex );			// Unlock mutex release other thread to work on it

			/* turn nano-second numbers into human readable system time */
		    hr = 9 + system_time/3600;
			mn = (system_time%3600)/60;
			sec = (system_time%3600)%60;
			/* print current queue depth for clarity */
			printf("Customer_%d entered bank and waiting to be served ", total_customers-1);
			printf("[%d:%d:%d] ", hr,mn,sec);
			printf("Line Size: %d\n", queue_depth);

			nsec = ranged_random(min,max);
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
	int hr, mn, sec, wait_time, t_wait, nsec;

    while( 1 ) {
    	nsec = ranged_random(min,max);				// get time between 30sec to 8min
		teller_process_time1.tv_nsec = nsec;
    	if (qSize > 0) {							// run only if customer waits on the line
    		nsec = nsec/SEC							// turn the system time into human readable time
    		t1p = t1d;								// teller_1's start working time -> previous work's done time
			t1d = system_time;						// teller_1's done time -> current system time
			t_wait = (t1d - t1p);					// teller_1's total wait time in human readble time

			pthread_mutex_lock( &mutex );			// Locks mutex to prevent race condition

			customer_data = dequeue();				// dequeue customer from the waiting list
			customer_data.out_time = system_time;	// record the time when customer got served

    		if(t_wait > t_max_wait) {
    			t_max_wait = t_wait;				// put Max waiting time if current wait time is greater
    			printf("New teller wait time record: %d min %d sec\n", ((t_wait%3600)/60), ((t_wait%3600)%60) );
    		}
    		teller_waiting_time += t_wait;			// increment teller's wait time
    		teller_working_time += nsec;			// increment teller's work time

			tran1++;								// increment teller's tansaction time for record
			pthread_mutex_unlock( &mutex );

			/* increment the waiting time for calculating average waiting time */
			hr = 9 + system_time/3600;
			mn = (system_time%3600)/60;
			sec = (system_time%3600)%60;
			printf("Teller_1 is serving customer_%d ", customer_data.cust_id);
			printf("@ [%d:%d:%d] ", hr,mn,sec);
			printf("for %d min %d sec\n", ((nsec%3600)/60), ((nsec%3600)%60));

			/* calculate the time when customer got served */
			wait_time = (customer_data.out_time - customer_data.in_time);
			if(wait_time > queue_max_time) {
				queue_max_time = wait_time;			// replace wait time if greater
				printf("New customer wait time record: %d min %d sec\n", ((wait_time%3600)/60), ((wait_time%3600)%60) );
			}
			queue_wait_time += wait_time;			// increment total wait time
			printf("Customer_%d's wait_time: %d sec\n", customer_data.cust_id, wait_time);

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
	int hr, mn, sec, wait_time, t_wait, nsec;

    while( 1 ) {
		nsec = ranged_random(min,max);
		teller_process_time2.tv_nsec = nsec;
    	if (qSize > 0) {							// run only if customer waits on the line
    		nsec = nsec/SEC;
    		t2p = t2d;
			t2d = system_time;
			t_wait = (t2d - t2p);
			pthread_mutex_lock( &mutex );
			if(t_wait > t_max_wait) {
				t_max_wait = t_wait;
				printf("New teller wait time record: %d min %d sec\n", ((t_wait%3600)/60), ((t_wait%3600)%60) );
			}
			teller_waiting_time += t_wait;

			teller_working_time += nsec;			// increment teller wait time
			customer_data = dequeue();				// dequeue customer from the waiting list
			customer_data.out_time = system_time;

			tran2++;
			pthread_mutex_unlock( &mutex );

			/* increment the waiting time for calculating average waiting time */
			hr = 9 + system_time/3600;
			mn = (system_time%3600)/60;
			sec = (system_time%3600)%60;
			printf("Teller_2 is serving customer_%d ", customer_data.cust_id);
			printf("@ [%d:%d:%d] ", hr,mn,sec);
			printf("for %d min %d sec\n", ((nsec%3600)/60), ((nsec%3600)%60));

			/* calculate the time when customer got served */
			wait_time = (customer_data.out_time - customer_data.in_time);
			if(wait_time > queue_max_time) {
				queue_max_time = wait_time;			// replace wait time if greater
				printf("New customer wait time record: %d min %d sec\n", ((wait_time%3600)/60), ((wait_time%3600)%60) );
			}
			queue_wait_time += wait_time;			// increment total wait time
			printf("Customer_%d's wait_time: %d sec\n", customer_data.cust_id, wait_time);

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
	int hr, mn, sec, wait_time, t_wait, nsec;

    while( 1 ) {
    	nsec = ranged_random(min,max);
		teller_process_time3.tv_nsec = nsec;
    	if (qSize > 0) {	// run only if customer waits on the line
    		nsec = nsec/SEC;
    		t3p = t3d;
			t3d = system_time;
			t_wait = (t3d - t3p);
			pthread_mutex_lock( &mutex );
			if(t_wait > t_max_wait) {
				t_max_wait = t_wait;
				printf("New teller wait time record: %d min %d sec\n", ((t_wait%3600)/60), ((t_wait%3600)%60) );
			}

			teller_waiting_time += t_wait;

			teller_working_time += nsec;				// increment teller wait time
			customer_data = dequeue();					// dequeue customer from the waiting list
			customer_data.out_time = system_time;

			tran3++;
			pthread_mutex_unlock( &mutex );

			/* increment the waiting time for calculating average waiting time */
			hr = 9 + customer_data.out_time/3600;
			mn = (customer_data.out_time%3600)/60;
			sec = (customer_data.out_time%3600)%60;
			printf("Teller_3 is serving customer_%d ", customer_data.cust_id);
			printf("@ [%d:%d:%d] ", hr,mn,sec);
			printf("for %d min %d sec\n", ((nsec%3600)/60), ((nsec%3600)%60));

			/* get out time here */
			wait_time = (customer_data.out_time - customer_data.in_time);
			if(wait_time > queue_max_time) {
				queue_max_time = wait_time;		// replace wait time if greater
				printf("New customer wait time record: %d min %d sec\n", ((wait_time%3600)/60), ((wait_time%3600)%60) );
			}
			queue_wait_time += wait_time;		// increment total wait time
			printf("Customer_%d's wait_time: %d sec\n", customer_data.cust_id, wait_time);

			/* snooze for 30sec - 8min in system time */
			nanosleep(&teller_process_time3, &end_time);
    	}
    }
    return 0;
}


int main( int argc, char *argv[] ) {
	int avg_teller_work, avg_teller_wait, avg_queue_time;
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

    //////////////////////* now prints all results */////////////////////////////////
    /* 1. The total number of customers serviced during the day. */
    printf("1. Total number of customer: %d\n", total_customers);

    /* 2. The average time each customer spends waiting in the queue */
    avg_queue_time = (queue_wait_time/total_customers);
    printf(
    	"2. Average wating time in the queue: %d sec\n",
    	avg_queue_time
    );

    /* 3. The average time each customer spends with the teller */
    avg_teller_work = (teller_working_time/total_customers);
	printf(
		"3. Average teller time spent: %d min %d sec\n",
		((avg_teller_work%3600)/60), ((avg_teller_work%3600)%60)
	);

	/* 4. The average time tellers wait for customers */
	avg_teller_wait = (teller_waiting_time/total_customers);
	printf(
		"4. Average time tellers wait for customers: %d min %d sec\n",
		(avg_teller_wait%3600)/60, (avg_teller_wait%3600)%60
	);

    /* 5. The maximum customer wait time in the queue */
	printf(
		"5. The maximum customer wait time in the queue: %lld min %lld sec\n",
		((queue_max_time%3600)/60), ((queue_max_time%3600)%60)
	);

    /* 6. The maximum wait time for tellers waiting for customers */
	printf(
		"6. The maximum wait time for tellers waiting for customers: %lld min %lld sec\n",
		((t_max_wait%3600)/60), ((t_max_wait%3600)%60)
	);

    /* 7. The maximum transaction time for the tellers */
	if (tran1>=tran2 && tran1>=tran3) {
		printf("7. The maximum transaction time: %d by Teller[1]\n", tran1);
	} else if (tran2 >= tran1 && tran2>=tran3) {
		printf("7. The maximum transaction time: %d by Teller[2]\n", tran2);
	} else if (tran3 >= tran1 && tran3>=tran2) {
		printf("7. The maximum transaction time: < %d > by Teller[3]\n", tran3);
	}

    /* 8. The maximum depth of the customer queue */
	printf("8. The maximum depth of the customer queue: %d\n", max_depth);

    return EXIT_SUCCESS;
}
