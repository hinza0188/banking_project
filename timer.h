/*
 * timer.h
 *
 *  Created on: Apr 3, 2017
 *      Author: yxa8247
 */

// define time in nano-second => used in nanosleep();
#define MIN (100000000);		// (actual)100ms == (system)1-minute
#define SEC (100000000/60);		// (actual)1.67ms == (system)1-second
#define HR  (100000000*60);		// (actual)6second == (system)1-hour

extern long long int system_time;

void* general_time(void);

