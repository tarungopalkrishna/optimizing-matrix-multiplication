#include <immintrin.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>

#ifdef DEBUG
    #define NTHREADS 2
    #define N 8
    #define BLOCK_SIZE 2
    #define T_BLOCK_X 4
    #define T_BLOCK_Y 2
    #define PRINT_SIZE_N 4
#else
    #define NTHREADS 8
    #define N 512
    #define BLOCK_SIZE 8
    #define PRINT_SIZE_N 8
    #define T_BLOCK_X 8
    #define T_BLOCK_Y 2
    // #define N 512
    // #define BLOCK_SIZE 16
#endif

float a[N][N];
float b[N][N];
float c[N][N];
float result[N][N];


// Get the current time in nanoseconds
// This function is bugged, I'm seeing negative values with this, idk why
long int nanos() {
    struct timespec start_clock_monotonic;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start_clock_monotonic);
    // long int current_time = start_clock_monotonic.tv_sec * 1000000000 + start_clock_monotonic.tv_nsec;
    // printf("Current time: %ld\n", current_time);
    // printf("%ld\n", ((start_clock_monotonic.tv_sec * 1000000000) + start_clock_monotonic.tv_nsec));
    return ((start_clock_monotonic.tv_sec * 1000000000) + start_clock_monotonic.tv_nsec);
}

void get_tflops(long int start_time, long int end_time, char *message) {
    printf("%s\n", message);
    double gflop = (2.0 * N * N * N) * 1e-9; // This is one GFLOP
    double time_taken = (end_time - start_time) * 1e-9;
    double elapsed_time = end_time - start_time;
    printf("start: %ld, end: %ld, elapsed: %f, time taken: %f\n", start_time, end_time, elapsed_time, time_taken);
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
            printf("%12.8f ", a[i][j]);
        }
        printf("\n");
    }

    printf("The B matrix is:\n");
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            printf("%12.8f ", b[i][j]);
        }
        printf("\n");
    }

    printf("The final matrix is:\n");
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            printf("%12.8f ", c[i][j]);
        }
        printf("\n");
    }
}

void print_matrix_n(long int n) {
    printf("The A matrix is:\n");
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            printf("%12.8f ", a[i][j]);
        }
        printf("\n");
    }

    printf("The B matrix is:\n");
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            printf("%12.8f ", b[i][j]);
        }
        printf("\n");
    }

    printf("The C matrix is:\n");
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            printf("%12.8f ", c[i][j]);
        }
        printf("\n");
    }

    printf("The RESULT matrix is:\n");
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            printf("%12.8f ", result[i][j]);
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

void load_sample_matrix() {
    FILE *fp = fopen("./matmul_matrix", "rb");
    if (fp == NULL) {
        printf("Error opening file\n");
        exit(0);
    }
    printf("matrix size = %u\n", N);
    size_t read_a = fread(a, 1, sizeof(float) * N * N, fp);
    size_t read_b = fread(b, 1, sizeof(float) * N * N, fp);
    size_t read_c = fread(result, 1, sizeof(float) * N * N, fp);
    printf("Read %ld bytes from A, %ld bytes from B, %ld bytes from C\n", read_a, read_b, read_c);
}

void check_result() {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            if (fabsf(c[i][j] - result[i][j]) > 1e-3) {
                printf("Error at %d, %d\n", i, j);
                printf("Expected: %f, got: %f\n", result[i][j], c[i][j]);
                exit(0);
            }
        }
    }
    printf("Result is correct\n");
}