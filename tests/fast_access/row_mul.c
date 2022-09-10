#include<stdio.h>
#include<time.h>
#include<stdint.h>
#define N 512 

float a[N][N];
float b[N][N];
float c[N][N];

uint64_t nanos(){
    struct timespec start;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    return (uint64_t)start.tv_sec*1000000000 + (uint64_t)start.tv_nsec;
}

void init_matrix(){
    for(int i=0; i<N; ++i){
        for(int j=0; j<N; ++j){
            a[i][j] = (N*i)+j+1;
            if(i==j)
                b[i][j] = 1;
            else
                b[i][j] = 0;
        }
    }
}

void print_matrix(){

    printf("The A matrix is:\n");
    for(int i=0; i<N; ++i){
        for(int j=0; j<N; ++j){
            printf("%10.6f ", a[i][j]);
        }
        printf("\n");
    }

    printf("The B matrix is:\n");
    for(int i=0; i<N; ++i){
        for(int j=0; j<N; ++j){
            printf("%10.6f ", b[i][j]);
        }
        printf("\n");
    }

    printf("The final matrix is:\n");
    for(int i=0; i<N; ++i){
        for(int j=0; j<N; ++j){
            printf("%10.6f ", c[i][j]);
        }
        printf("\n");
    }
}

void matmul(){
    uint64_t start = nanos();
    for(int i=0; i<N; i++)
        for(int j=0; j<N; j++){
            float acc = 0;
            for(int k=0; k<N; k++)
                acc += a[i][k] * b[j][k];
            c[i][j] = acc;
        }
    uint64_t end = nanos();
    uint32_t time = (uint32_t)end-start;
    printf("Total time to run the function is: %i\n", time);
}



int main(){
    init_matrix();
    // print_matrix();
    matmul();
    // print_matrix();
    return 0;
}
