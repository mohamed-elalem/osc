#ifndef PID_H
#define PID_H

#include <stdlib.h>
#include <string.h>

#define MAX_NUMBER_OF_PROCESSES 7000
#define LSB(x) (x & -x)
#define SIZE_OF_BYTE 8

const int ALLOC_SUCCESS = -1;
const int ALLOC_FAILED = -2;
const int NO_ID_AVAILABLE = -3;
const int SUCCESSFUL_RELEASE = -4;
const int step = sizeof(unsigned long) * SIZE_OF_BYTE;

void * available_ids;

int init() {
    int size = (int) MAX_NUMBER_OF_PROCESSES / step * (step + 1);
    printf("%d\n", size);
    available_ids = (void*) malloc(MAX_NUMBER_OF_PROCESSES / step * (step + 1) * sizeof(void));
    if (available_ids == NULL) {
        return ALLOC_FAILED;
    }
    memset(available_ids, 0, sizeof(available_ids));
    return ALLOC_SUCCESS;
}

int get_bit_index(unsigned long bytes) {
    int index = -1;
    while(bytes != 0) {
        bytes >>= 1;
        index++;
    }
    return index;
}

int acquire_id() {
    void * iterator = available_ids;
    for(iterator = available_ids; iterator < available_ids + MAX_NUMBER_OF_PROCESSES; iterator += step) {
        unsigned long bytes = *((unsigned long*) iterator);
        unsigned long next = LSB(~bytes);
        if(next > 0) {
            int index = get_bit_index(next);
            int pid = index + (iterator - available_ids);
            if(pid < MAX_NUMBER_OF_PROCESSES) {
                *((unsigned long*)iterator) |= (1L << index);
                return pid;
            }
            else {
                return NO_ID_AVAILABLE;
            }
        }
    }
    return NO_ID_AVAILABLE;
}

int release(int index) {
    *((unsigned long *)available_ids + (index / step) * step) &= ~(1L << (index % step));
    return SUCCESSFUL_RELEASE;
}

#endif
