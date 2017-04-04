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
#include <stdio.h>
#include <time.h>
#include <sys/netmgr.h>
#include <sys/neutrino.h>
#include <timer.h>

#define MY_PULSE   _PULSE_CODE_MINAVAIL

typedef union {
        struct _pulse   pulse;
} my_message_t;

void* general_time(void) {
	struct sigevent event;
	struct itimerspec itime;
	timer_t timer_id;
	int chid, rcvid;
	my_message_t msg;

   chid = ChannelCreate(0);

   event.sigev_notify = SIGEV_PULSE;
   event.sigev_coid = ConnectAttach(ND_LOCAL_NODE, 0, chid, _NTO_SIDE_CHANNEL, 0);
   event.sigev_priority = getprio(0);
   event.sigev_code = MY_PULSE;
   timer_create(CLOCK_REALTIME, &event, &timer_id);

   /* set timer to be interuupted every second */
   itime.it_value.tv_nsec = SEC;
   /* set timer to be interuupted every second */
   itime.it_interval.tv_nsec = SEC;
   timer_settime(timer_id, 0, &itime, NULL);

   for (;;) {
       rcvid = MsgReceive(chid, &msg, sizeof(msg), NULL);
       if (rcvid == 0) { /* pulse detected */
            if (msg.pulse.code == MY_PULSE) {
            	system_time++;
            }
       }
   }
   return 0;
}
