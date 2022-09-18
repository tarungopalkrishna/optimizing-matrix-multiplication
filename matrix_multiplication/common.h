#include <immintrin.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

#ifdef DEBUG
    #define NTHREADS 2
    #define N 8
    #define BLOCK_SIZE 2
#else
    #define NTHREADS 8
    #define N 2048
    // #define N 512
    // #define BLOCK_SIZE 16
#endif

float a[N][N];
float b[N][N];
float c[N][N];


// Get the current time in nanoseconds
// This function is bugged, I'm seeing negative values with this, idk why
uint64_t nanos() {
    struct timespec start;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    return (((uint64_t)start.tv_sec * 1000000000) + (uint64_t)start.tv_nsec);
}

void get_tflops(int start_time, int end_time, char *message) {
    printf("%s\n", message);
    double gflop = (2.0 * N * N * N) * 1e-9; // This is one GFLOP
    // double time_taken = (end_time - start_time) * 1e-9;
    double time_taken = ((double)(end_time - start_time) / CLOCKS_PER_SEC);
    double elapsed_time = end_time - start_time;
    printf("start: %d, end: %d, elapsed: %f, time taken: %f\n", start_time, end_time, elapsed_time, time_taken);
    printf("%f instructions\n", (double)(2.0 * N * N * N));
    // printf("%f seconds\n", (elapsed_time * 1e-9));
    printf("%f GFLOPS/S\n", gflop / time_taken);
    // printf("%f GFLOPS/S\n", ((2.0 * N * N * N) / ((end_time - start_time) * 1e-9)));
}

void init_matrix() {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            a[i][j] = (N * i) + j + 1;
            if (i == j)
                b[i][j] = 1;
            else
                b[i][j] = 0;
        }
    }
}

void print_matrix() {
    printf("The A matrix is:\n");
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            printf("%10.6f ", a[i][j]);
        }
        printf("\n");
    }

    printf("The B matrix is:\n");
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            printf("%10.6f ", b[i][j]);
        }
        printf("\n");
    }

    printf("The final matrix is:\n");
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            printf("%10.6f ", c[i][j]);
        }
        printf("\n");
    }
}

void reset_matrix() {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            c[i][j] = 0;
        }
    }
}
