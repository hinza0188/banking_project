#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <fifo_q.h>
#include <timer.h>

// declare variables
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int available_teller = 3;
int max_depth, queue_depth, total_customers, max_transaction_time, t_max_wait, queue_wait_time = 0;
int bank_flag, teller_waiting_time, teller_working_time, queue_max_time;
long long int t1p, t1d, t2p, t2d, t3p, t3d, system_time = 0;

/**
 * Generates random number from Min to Max
 */
int ranged_random(int Min, int Max) {
	int diff = Max-Min;
	return (int) (((double)(diff+1)/RAND_MAX) * rand() + Min);
}


/**
 *  Every 1 - 4 minute, customer enters the bank
 *  and start filling in the queue
 */
void* enter_customer( void* arg ) {
	struct timespec customer_entering_time, end_time;
	int hr,mn,sec, nsec;
	int min = MIN;
	int max = 4 * MIN;

	while (1) {
		if (!bank_flag) { 							// Run only if bank is not closed
			pthread_mutex_lock( &mutex );			// Lock mutex to prevent race condition
			enqueue(total_customers, system_time);	// Put customer into the line
			queue_depth = queue_size();				// Get the current line length
			if(queue_depth>max_depth) {
				max_depth = queue_depth; 			// Used for denoting the maximum depth of the queue.
			}
			total_customers++;
			pthread_mutex_unlock( &mutex );			// Unlock mutex release other thread to work on it

			/* Turn nano-second numbers into human readable system time */
		    hr = 9 + system_time/3600;
			mn = (system_time%3600)/60;
			sec = (system_time%3600)%60;
			/* Print current queue depth for clarity */
			printf(
				"Customer_%d entered bank @ [%d:%d:%d] | Line Size: %d\n",
				total_customers-1,hr,mn,sec,queue_depth
			);

			nsec = ranged_random(min,max);			// Generates random time from 1 - 4 minutes
			customer_entering_time.tv_nsec = nsec;	// Apply the value into timespec structure
			nanosleep(&customer_entering_time, &end_time);
		}
	}
	return 0;
}


/**
 * Teller waits for customers to for help them banking transactions.
 * Detailed actions are commented line by line
 * Shares structure with the other two tellers as well
 */
void* teller1( void* arg ) {
	struct timespec teller_process_time, end_time;
	cust_record customer_data;
	int min = 30*SEC;
	int max = 8 *MIN;
	int hr, mn, sec, wait_time, teller_wait, nsec;

    while( 1 ) {
    	nsec = ranged_random(min,max);				// Get time between 30sec to 8min
		teller_process_time.tv_nsec = nsec;			// Set the generated number to transaction time

		pthread_mutex_lock( &mutex );				// Locks mutex to prevent race condition all changing variables
    	if (queue_size() > 0) {						// Run only if customer waits on the line
    		printf(
				"Available number of teller: %d\n",	// Print number of teller available
				available_teller
			);
    		available_teller--;						// Decrement available teller
    		nsec = nsec/SEC;						// Turn the system time into human readable time
			t1p = t1d;								// Teller_1's start working time -> previous work's done time
			t1d = system_time;						// Teller_1's done time -> current system time
			teller_wait = (t1d - t1p);				// Teller_1's total wait time in human readble time
			customer_data = dequeue();				// Dequeue customer from the waiting list
			customer_data.out_time = system_time;	// Record the time when customer got served

    		if(teller_wait > t_max_wait) {
    			t_max_wait = teller_wait;			// Put Max waiting time if current wait time is greater
    			printf(
    				"New teller wait time record: %d min %d sec\n",
    				((teller_wait%3600)/60), ((teller_wait%3600)%60)
    			);
    		}

    		if(nsec > max_transaction_time){
    			max_transaction_time = nsec;		// Put Max transaction time if current transaction time is greater
    			printf(
    				"New teller transaction time record: %d min %d sec\n",
    				((nsec%3600)/60), ((nsec%3600)%60)
    			);
    		};

    		teller_waiting_time += teller_wait;		// Increment teller's wait time
    		teller_working_time += nsec;			// Increment teller's work time
			pthread_mutex_unlock( &mutex );			// Unlock the mutex to release other tellers to run

			/* Do rest of the work that does not conflict with other tellers   **
			** Increment the waiting time for calculating average waiting time */
			hr = 9 + system_time/3600;
			mn = (system_time%3600)/60;
			sec = (system_time%3600)%60;
			printf(
				"Teller_1 starts serving customer_%d @ [%d:%d:%d]\n",
				customer_data.cust_id, hr, mn, sec
			);

			/* Calculate the time when customer got served */
			wait_time = (customer_data.out_time - customer_data.in_time);
			if(wait_time > queue_max_time) {
				queue_max_time = wait_time;			// Replace wait time if greater
				printf("New customer wait time record: %d min %d sec\n", ((wait_time%3600)/60), ((wait_time%3600)%60) );
			}
			queue_wait_time += wait_time;			// Increment total wait time
			printf(
				"Customer_%d's wait_time: %d sec\n\n",
				customer_data.cust_id, wait_time
			);

			/* snooze for 30 sec - 8 min in system time */
			nanosleep(&teller_process_time, &end_time);
			available_teller++;						// Increment back to recover availability
    	} else  {
    		pthread_mutex_unlock( &mutex );			// Unlocks the mutex if no customer is waiting in the line
    	}
    }
    return 0;
}


/**
 * Look Teller_1 for detailed comments
 * The structure is exactly same
 * including global variablese for calculations
 */
void* teller2( void* arg ) {
	struct timespec teller_process_time, end_time;
	cust_record customer_data;
	int min = 30*SEC;
	int max = 8 *MIN;
	int hr, mn, sec, wait_time, teller_wait, nsec;

    while( 1 ) {
		nsec = ranged_random(min,max);
		teller_process_time.tv_nsec = nsec;
		pthread_mutex_lock( &mutex );
    	if (queue_size() > 0) {
			printf(
				"Available number of teller: %d\n",
				available_teller
			);
			available_teller--;
			nsec = nsec/SEC;
			t2p = t2d;
			t2d = system_time;
			teller_wait = (t2d - t2p);
			if(teller_wait > t_max_wait) {
				t_max_wait = teller_wait;
				printf(
					"New teller wait time record: %d min %d sec\n",
					((teller_wait%3600)/60), ((teller_wait%3600)%60)
				);
			}
			if(nsec > max_transaction_time){
				max_transaction_time = nsec;
				printf(
					"New teller transaction time record: %d min %d sec\n",
					((nsec%3600)/60), ((nsec%3600)%60)
				);
			};

			teller_waiting_time += teller_wait;
			teller_working_time += nsec;
			customer_data = dequeue();
			customer_data.out_time = system_time;
			pthread_mutex_unlock( &mutex );

			hr = 9 + system_time/3600;
			mn = (system_time%3600)/60;
			sec = (system_time%3600)%60;
			printf(
				"Teller_2 starts serving customer_%d @ [%d:%d:%d]\n",
				customer_data.cust_id, hr, mn, sec
			);

			wait_time = (customer_data.out_time - customer_data.in_time);
			if(wait_time > queue_max_time) {
				queue_max_time = wait_time;
				printf(
					"New customer wait time record: %d min %d sec\n",
					((wait_time%3600)/60), ((wait_time%3600)%60)
				);
			}
			queue_wait_time += wait_time;
			printf(
				"Customer_%d's wait_time: %d sec\n\n",
				customer_data.cust_id, wait_time
			);

			nanosleep(&teller_process_time, &end_time);
			available_teller++;
    	}
    	else {
    		pthread_mutex_unlock( &mutex );
    	}
    }
    return 0;
}


/**
 * Look Teller_1 for detailed comments
 * The structure is exactly same
 * including global variablese for calculations
 */
void* teller3( void* arg ) {
	struct timespec teller_process_time, end_time;
	cust_record customer_data;
	int min = 30*SEC;
	int max = 8 *MIN;
	int hr, mn, sec, wait_time, teller_wait, nsec;

    while( 1 ) {
    	nsec = ranged_random(min,max);
		teller_process_time.tv_nsec = nsec;
		pthread_mutex_lock( &mutex );
    	if (queue_size() > 0) {
			printf(
				"Available number of teller: %d\n",
				available_teller
			);
			available_teller--;
			nsec = nsec/SEC;
			t3p = t3d;
			t3d = system_time;
			teller_wait = (t3d - t3p);
			if(teller_wait > t_max_wait) {
				t_max_wait = teller_wait;
				printf(
					"New teller wait time record: %d min %d sec\n",
					((teller_wait%3600)/60), ((teller_wait%3600)%60)
				);
			}

			if(nsec > max_transaction_time){
				max_transaction_time = nsec;
				printf(
					"New teller transaction time record: %d min %d sec\n",
					((nsec%3600)/60), ((nsec%3600)%60)
				);
			};

			teller_waiting_time += teller_wait;

			teller_working_time += nsec;
			customer_data = dequeue();
			customer_data.out_time = system_time;
			pthread_mutex_unlock( &mutex );

			hr = 9 + customer_data.out_time/3600;
			mn = (customer_data.out_time%3600)/60;
			sec = (customer_data.out_time%3600)%60;
			printf(
				"Teller_3 starts serving customer_%d @ [%d:%d:%d]\n",
				customer_data.cust_id, hr, mn, sec
			);

			wait_time = (customer_data.out_time - customer_data.in_time);
			if(wait_time > queue_max_time) {
				queue_max_time = wait_time;
				printf(
					"New customer wait time record: %d min %d sec\n",
					((wait_time%3600)/60), ((wait_time%3600)%60)
				);
			}
			queue_wait_time += wait_time;
			printf(
				"Customer_%d's wait_time: %d sec\n\n",
				customer_data.cust_id, wait_time
			);

			nanosleep(&teller_process_time, &end_time);
			available_teller++;
    	} else {
    		pthread_mutex_unlock( &mutex );
    	}
    }
    return 0;
}


/**
 * Run all threads for 42 seconds in real time,
 * which is equivalent to 7 hours in system time.
 * Calculates all types of project questions from the simulations
 */
int main( int argc, char *argv[] ) {
	int avg_teller_work, avg_teller_wait, avg_queue_time;
	srand(time(NULL));
	QueueInit();											// Initialize Queue with null pointers
	printf("Current Time || 08:00 A.M. || Bank Opened\r\n");
	bank_flag = 0;											// bank is now opens

	pthread_create( NULL, NULL, &general_time, NULL );		// incrememnt system_time clock for timing calculation
	pthread_create( NULL, NULL, &enter_customer, NULL );	// customer enters the bank every 1-4 min
	pthread_create( NULL, NULL, &teller1, NULL );			// execute teller process 1
    pthread_create( NULL, NULL, &teller2, NULL );			// execute teller process 2
	pthread_create( NULL, NULL, &teller3, NULL );			// execute teller process 3

	sleep( 42 );	// this represents 420 minutes == 7 hours
    bank_flag = 1;											// bank is now closed
    printf("Current Time || 04:00 P.M. || Bank Closed\n\n");

    //////////////////////* now prints all results *////////////////////////////////
    /* 1. The total number of customers serviced during the day. */
    printf(
    	"1. The total number of customer serviced during the day: <%d> customers\n",
    	total_customers
    );

    /* 2. The average time each customer spends waiting in the queue */
    avg_queue_time = (queue_wait_time/total_customers);
    printf(
    	"2. The average time each customer spends waiting in the queue: %d sec\n",
    	avg_queue_time
    );

    /* 3. The average time each customer spends with the teller */
    avg_teller_work = (teller_working_time/total_customers);
	printf(
		"3. The average time each customer spends with the teller: %d min %d sec\n",
		((avg_teller_work%3600)/60), ((avg_teller_work%3600)%60)
	);

	/* 4. The average time tellers wait for customers */
	avg_teller_wait = (teller_waiting_time/total_customers);
	printf(
		"4. The average time tellers wait for customers: %d min %d sec\n",
		(avg_teller_wait%3600)/60, (avg_teller_wait%3600)%60
	);

    /* 5. The maximum customer wait time in the queue */
	printf(
		"5. The maximum customer wait time in the queue: %d min %d sec\n",
		((queue_max_time%3600)/60), ((queue_max_time%3600)%60)
	);

    /* 6. The maximum wait time for tellers waiting for customers */
	printf(
		"6. The maximum wait time for tellers waiting for customers: %d min %d sec\n",
		((t_max_wait%3600)/60), ((t_max_wait%3600)%60)
	);

    /* 7. The maximum transaction time for the tellers */
	printf(
		"7. The maximum transaction time for the tellers: %d min %d sec\n",
		((max_transaction_time%3600)/60), ((max_transaction_time%3600)%60)
	);

    /* 8. The maximum depth of the customer queue */
	printf("8. The maximum depth of the customer queue: %d\n", max_depth);

    return EXIT_SUCCESS;
}
