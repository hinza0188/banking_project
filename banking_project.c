#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int count = 0;
struct timespec tim1,tim2;

void* function1( void* arg ) {
    int tmp = 0;
    tim1.tv_sec = 0;
    tim1.tv_nsec = 100000000;

    while( 1 ) {
        pthread_mutex_lock( &mutex );
        tmp = count++;
        pthread_mutex_unlock( &mutex );
        printf( "Count is %d\n", tmp );

        /* snooze for 1 second */
        nanosleep(&tim1,NULL);
    }

    return 0;
}

void* function2( void* arg ) {
    int tmp = 0;

    while( 1 ) {
        pthread_mutex_lock( &mutex );
        tmp = count--;
        pthread_mutex_unlock( &mutex );
        printf( "** Count is %d\n", tmp );

        /* snooze for 2 seconds */
        sleep( 2 );
    }

    return 0;
}

int main( int argc, char *argv[] ) {
	printf("Testing! :D\r\n");
    pthread_create( NULL, NULL, &function1, NULL );
    pthread_create( NULL, NULL, &function2, NULL );

    /* Let the threads run for 60 seconds. */
    sleep( 42 );

    return EXIT_SUCCESS;
}
