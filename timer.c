/*
 * timer.c
 *
 *  Created on: Apr 3, 2017
 *      Author: yxa8247
 *
 * This function utilizes timer to increment system_time
 * by every 100ms which is in 1 minute within the system
 *
 */
#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <sys/netmgr.h>
#include <sys/neutrino.h>
#include <timer.h>

#define MY_PULSE_CODE   _PULSE_CODE_MINAVAIL

typedef union {
        struct _pulse   pulse;
} my_message_t;

void* general_time(void) {
	pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
	struct sigevent event;
	struct itimerspec itime;
	timer_t timer_id;
	int chid, rcvid;
	my_message_t msg;

   chid = ChannelCreate(0);

   event.sigev_notify = SIGEV_PULSE;
   event.sigev_coid = ConnectAttach(ND_LOCAL_NODE, 0,
                                    chid,
                                    _NTO_SIDE_CHANNEL, 0);
   event.sigev_priority = getprio(0);
   event.sigev_code = MY_PULSE_CODE;
   timer_create(CLOCK_REALTIME, &event, &timer_id);

   /* 500 million nsecs = .5 secs */
   itime.it_value.tv_nsec = SEC;
   /* 500 million nsecs = .5 secs */
   itime.it_interval.tv_nsec = SEC;
   timer_settime(timer_id, 0, &itime, NULL);

   /*
    * As of the timer_settime(), we will receive our pulse
    * in 1.5 seconds (the itime.it_value) and every 1.5
    * seconds thereafter (the itime.it_interval)
    */

   for (;;) {
       rcvid = MsgReceive(chid, &msg, sizeof(msg), NULL);
       if (rcvid == 0) { /* we got a pulse */
            if (msg.pulse.code == MY_PULSE_CODE) {
            	pthread_mutex_lock( &mutex );
            	system_time++;
            	pthread_mutex_unlock( &mutex );
            }
       }
   }
   return 0;
}
