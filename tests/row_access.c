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

void rowm_access(){
    uint64_t start = nanos();
    for(uint32_t i=0; i<N; i++)
        for(uint32_t j=0; j<N; j++)
            acc = a[i][j];
    uint64_t end = nanos();
    uint32_t time = (uint32_t)(end-start);
    printf("Total time to loop is: %i\n", time);
}

int main(){
    rowm_access();
}
