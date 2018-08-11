#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <time.h>

#define NUMBER_OF_STUDENTS 100

pthread_mutex_t ta_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t student_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t ta_condition;
pthread_cond_t student_condition;

enum {
    IDLE, WORK
};

int ta_state = IDLE;
int student_number = -1;

void* ta(void* args) {
    while(1) {
        if(ta_state == IDLE) {
            pthread_mutex_lock(&ta_mutex);
            pthread_cond_wait(&ta_condition, &ta_mutex);
            pthread_mutex_unlock(&ta_mutex);
        }
        pthread_mutex_lock(&ta_mutex);

        ta_state = WORK;
        long work_time = (long) abs(rand()) % 1000000 + 20000;
        printf("TA is working with student %d for %lf sec\n", student_number, (double) work_time / 1000000);

        pthread_mutex_unlock(&ta_mutex);

        usleep(work_time);

        pthread_mutex_lock(&ta_mutex);
        ta_state = IDLE;
        printf("TA is idle and waiting for students..\n");
        pthread_cond_signal(&student_condition);
        pthread_mutex_unlock(&ta_mutex);
    }
}

void* student(void* args) {
    int current_student_number = (int) args;
    while(1) {
        pthread_mutex_lock(&ta_mutex);
        if(ta_state == WORK) {
            pthread_cond_wait(&student_condition, &ta_mutex);
        }

        student_number = current_student_number;
        pthread_cond_signal(&ta_condition);
        pthread_mutex_unlock(&ta_mutex);
    }
}

int main()
{
    srand(time(NULL));
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, ta, NULL);
    int i;
    for(i = 0; i < NUMBER_OF_STUDENTS; i++) {
        pthread_create(&thread_id, NULL, student, i + 1);
    }
    while(1) {
        sleep(5);
    }
    return 0;
}
