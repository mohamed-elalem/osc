#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>

#define REQ_SIZE 1000
#define CYLINDER_CNT 5000

int fcfs(int *pos, int i_pos, int n);
int scan(int *pos, int i_pos, int n);
int cscan(int *pos, int i_pos, int n);
int* sort(int *arr, int n);
int find_bs(int *arr, int n, int targ);

int main(int argc, char* argv[]) {
    srand(time(NULL));
    int i_pos = atoi(argv[1]);
    int *pos = (int*) malloc(REQ_SIZE * sizeof(int));
    for(int i = 0; i < REQ_SIZE; i++) {
        pos[i] = abs(rand() % CYLINDER_CNT);
    }
    
    int fcfs_seeks = fcfs(pos, i_pos, REQ_SIZE);
    printf("FCFS seeks: %d\n", fcfs_seeks);
    int scan_seeks = scan(pos, i_pos, REQ_SIZE);
    printf("SCAN seeks: %d\n", scan_seeks);
    int cscan_seeks = cscan(pos, i_pos, REQ_SIZE);
    printf("CSCAN seeks: %d\n", cscan_seeks);
    return 0;
}

int fcfs(int *pos, int i_pos, int n) {
    int seeks = 0;
    
    for (int i = 0; i < n; i++) {
        seeks += abs(i_pos - pos[i]);
        i_pos = pos[i];
    }
    return seeks;
}

int scan(int *pos, int i_pos, int n) {
    int seeks = 0;
    int* spos = sort(pos, n);
    int idx = find_bs(spos, n, i_pos);
    int c_pos = i_pos;
    for(int i = idx; i < n; i++) {
        // printf("Moving head from %d to %d\n", c_pos, spos[i]);
        seeks += (spos[i] - c_pos);
        c_pos = spos[i];
    }
    c_pos = CYLINDER_CNT - 1;
    for(int i = idx - 1; i >= 0; i--) {
        // printf("Moving head from %d to %d\n", c_pos, spos[i]);
        seeks += (c_pos - spos[i]);
        c_pos = spos[i];
    }
    return seeks;
}

int cscan(int *pos, int i_pos, int n) {
    int seeks = 0;
    int* spos = sort(pos, n);
    int idx = find_bs(spos, n, i_pos);
    int c_pos = i_pos;
    for(int i = idx; i < n; i++) {
        seeks += (spos[i] - c_pos);
        c_pos = spos[i];
    }
    c_pos = 0;
    seeks += CYLINDER_CNT;
    for(int i = 0; i < idx; i++) {
        seeks += (spos[i] - c_pos);
        c_pos = spos[i];
    }
    return seeks;
}

int* sort(int *arr, int n) {
    int *sorted_arr = (int*)malloc(n * sizeof(int));
    for(int i = 0; i < n; i++) {
        sorted_arr[i] = arr[i];
    }
    for(int i = 0; i < n - 1; i++) {
        int min_elem = 1 << 30;
        int min_idx = -1;
        for(int j = i; j < n; j++) {
            if (sorted_arr[j] < min_elem) {
                min_idx = j;
                min_elem = sorted_arr[j];
            }
        }
        if(min_idx > i) {
            int tmp = sorted_arr[i];
            sorted_arr[i] = sorted_arr[min_idx];
            sorted_arr[min_idx] = tmp;
        }
    }
    return sorted_arr;
}

int find_bs(int *arr, int n, int targ) {
    int low = 0, high = n - 1;
    while(low < high) {
        int mid = low + (high - low) / 2;
        if (arr[mid] < targ) {
            low = mid + 1;
        } else {
            high = mid;
        }
    }
    return low;
}