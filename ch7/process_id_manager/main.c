#include <stdio.h>
#include "pid.c"
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

extern int x;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* simulate(void *args) {
    pthread_t thread_id = (pthread_t) args;
    while(1) {
        int is_acquire = (int) abs(rand()) % 2;
        pthread_mutex_lock(&mutex);
        if(is_acquire == 0) {
            int process_id = acquire_id();
            printf("In thread %lld, Acquired %d\n", thread_id, process_id);
        }
        else {
            int released_process_id = rand() % MAX_NUMBER_OF_PROCESSES;
            release(released_process_id);
            printf("Released %d\n", released_process_id);
        }
        pthread_mutex_unlock(&mutex);
        usleep((int) abs(rand() % 4000000) + 1000000);
    }
}

int main()
{
    init();
    srand(time(NULL));
    pthread_mutex_init(&mutex, NULL);
    int i;
    for(i = 0; i < 1000; i++) {
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, simulate, thread_id);
//        pthread_join(thread_id, NULL);
    }
    sleep(1000);
}
