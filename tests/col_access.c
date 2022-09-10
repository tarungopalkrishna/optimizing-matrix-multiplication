#include<stdio.h>
#include<stdint.h>
#include<time.h>

#define N 4096

float a[N][N];
float acc;


uint64_t nanos(){
    struct timespec start;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    return (uint64_t)start.tv_sec*1000000000 + (uint64_t)start.tv_nsec;
}


void colm_access(){
    uint32_t start = nanos();
    for(uint32_t j=0; j<N; j++)
        for(uint32_t i=0; i<N; i++)
            acc = a[i][j];
    uint32_t end = nanos();
    uint32_t time = end-start;
    printf("Total time to loop is: %i\n", time);
}


int main(){
    colm_access();
}
