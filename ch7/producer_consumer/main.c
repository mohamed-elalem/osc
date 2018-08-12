#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>

#define PRODUCER_COUNT 50
#define CONSUMER_COUNT 50
#define BUFFER_SIZE 200

typedef int buffer_item;

sem_t empty, full, mutex;
buffer_item buffer[BUFFER_SIZE];
int front = -1, rear = 0;


void * producer(void *args) {
    while(1) {
        int produced = rand() % 10000;
        printf("Produced %d\n", produced);
        sem_wait(&empty);
        printf("Acquired an empty location in buffer\n");
        sem_wait(&mutex);

        front = (front + 1) % BUFFER_SIZE;
        buffer[front] = produced;
        printf("Item %d is now in buffer in index %d\n", produced, front);

        sem_post(&mutex);
        printf("Announcing that the produced is now in the buffer\n");
        sem_post(&full);

        long random_time = (long) abs(rand() % 5000000 + 200000);
        usleep(random_time);
    }
}

void * consumer(void *args) {
    while(1) {
        sem_wait(&full);
        printf("Acquired a buffer item to consume\n");
        sem_wait(&mutex);

        int consumed = buffer[rear];
        printf("Item %d is consumed from buffer from index %d\n", consumed, rear);
        rear = (rear + 1) % BUFFER_SIZE;

        sem_post(&mutex);
        printf("Announcing that a buffer item is consumed\n");
        sem_post(&empty);

        long random_time = (long) abs(rand() % 50000000 + 2000000);
        usleep(random_time);
    }
}

int main()
{
    srand(time(NULL));
    sem_init(&mutex, 0, 1);
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);

    int i;
    for(i = 0; i < PRODUCER_COUNT; i++) {
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, producer, NULL);
    }

    for(i = 0; i < CONSUMER_COUNT; i++) {
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, consumer, NULL);
    }

    while(1) {
        sleep(5);
    }
    return 0;
}
