#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <signal.h>

#define NUMBER_OF_PHILOSOPHERS 5

enum {
  THINKING,
  HUNGRY,
  EATING
};

int philosopher_statistics[NUMBER_OF_PHILOSOPHERS];

int state[NUMBER_OF_PHILOSOPHERS];

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond[NUMBER_OF_PHILOSOPHERS];

void pickup_forks(int philosopher_number);
void return_forks(int philosopher_number);
void check(int philosopher_number);
void * philosopher(void *args);

void record_eating(int philosopher_number) {
    pthread_mutex_lock(&mutex);
    philosopher_statistics[philosopher_number]++;
    pthread_mutex_unlock(&mutex);
}

void term(int signum)
{
    pthread_mutex_lock(&mutex);
    for(int i = 0; i < NUMBER_OF_PHILOSOPHERS; i++) {
        printf("\nPhiosopher %d ate %d times\n", i, philosopher_statistics[i]);
    }
    exit(0);
}

int main()
{
    srand(time(NULL));

    struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_handler = term;
    sigaction(SIGQUIT, &action, NULL);

    int i;
    pthread_mutex_init(&mutex, NULL);
    for(i = 0; i < NUMBER_OF_PHILOSOPHERS; i++) {
        pthread_cond_init(&cond[i], NULL);
    }

    for(i = 0; i < NUMBER_OF_PHILOSOPHERS; i++) {
        pthread_t thread_id;
        state[i] = THINKING;
        pthread_create(&thread_id, NULL, philosopher, i);
    }

    while(1) {
        sleep(5);
    }

    return 0;
}

void * philosopher(void * args) {
    int philosopher_number = (int) args;
    long random_time;
    while(1) {
        printf("Philosopher %d is trying to pick forks\n", philosopher_number);
        pickup_forks(philosopher_number);
        random_time = (long) (abs(rand() % 1000000) + 20000);
        printf("Philosopher %d picked up forks and will eat for %0.2lf sec\n", philosopher_number, random_time / 1000000.0);
        usleep(random_time);
        printf("Philosopher %d finished eating\n", philosopher_number);
        return_forks(philosopher_number);
        record_eating(philosopher_number);
        printf("Philosopher %d returned forks\n", philosopher_number);
        random_time = (long) (abs(rand() % 1000000) + 20000);
        printf("Philosopher %d will be thinking for %0.2lf\n", philosopher_number, random_time / 1000000.0);
        usleep(random_time);
    }
}

void pickup_forks(int philosopher_number) {
    printf("Philosopher %d is now hungry\n", philosopher_number);
    state[philosopher_number] = HUNGRY;
    check(philosopher_number);
    if(state[philosopher_number] != EATING) {
        printf("Philosopher %d has to wait his turn\n", philosopher_number);
        pthread_mutex_lock(&mutex);
        pthread_cond_wait(&cond[philosopher_number], &mutex);
        pthread_mutex_unlock(&mutex);
        printf("Philosopher %d turn to eat\n", philosopher_number);
    }
    pthread_mutex_lock(&mutex);
    printf("Philosopher %d is now eating\n", philosopher_number);
    state[philosopher_number] = EATING;
    pthread_mutex_unlock(&mutex);
}

void return_forks(int philosopher_number) {
    pthread_mutex_lock(&mutex);
    state[philosopher_number] = THINKING;
    pthread_mutex_unlock(&mutex);
    check((philosopher_number + NUMBER_OF_PHILOSOPHERS - 1) % NUMBER_OF_PHILOSOPHERS);
    check((philosopher_number + 1) % NUMBER_OF_PHILOSOPHERS);
    printf("Philosopher %d returned forks and is back to thinking\n", philosopher_number);
}

void check(int philosopher_number) {
    pthread_mutex_lock(&mutex);
    if(state[philosopher_number] == HUNGRY &&
            state[(philosopher_number + NUMBER_OF_PHILOSOPHERS - 1) % NUMBER_OF_PHILOSOPHERS] != EATING &&
            state[(philosopher_number + 1) % NUMBER_OF_PHILOSOPHERS] != EATING) {
        state[philosopher_number] = EATING;
        pthread_cond_signal(&cond[philosopher_number]);
    }
    pthread_mutex_unlock(&mutex);
}


