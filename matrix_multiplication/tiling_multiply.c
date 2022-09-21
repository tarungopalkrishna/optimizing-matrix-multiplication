#include <stdio.h>
#include <time.h>
#include <assert.h>
#include "common.h"

#ifndef BLOCK_SIZE
    #define BLOCK_SIZE 8
#endif

void matmul() {
    // Tiling matrix multiplication
    assert(N % BLOCK_SIZE == 0);
    long int start_time = nanos();
    for (int i = 0; i < N; i += BLOCK_SIZE) {
        for (int j = 0; j < N; j += BLOCK_SIZE) {
            for (int k = 0; k < N; k += BLOCK_SIZE) {
                for (int ii = 0; ii < BLOCK_SIZE; ++ii) {
                    for (int kk = 0; kk < BLOCK_SIZE; ++kk) {
                        for (int jj = 0; jj < BLOCK_SIZE; ++jj) {
                            // printf("%d %d %d, c[%d][%d] += a[%d][%d] * b[%d][%d]\n", ii, jj, kk, i + ii, j + jj, i + ii, k + kk, k + kk, j + jj);
                            c[i + ii][j + jj] += a[i + ii][k + kk] * b[k + kk][j + jj];
                        }
                    }
                }
            }
        }
    }
    long int end_time = nanos();
    get_tflops(start_time, end_time, (char *)"Tiling matrix multiplication");
}

int main(){
    load_sample_matrix();
    matmul();
    print_matrix_n(PRINT_SIZE_N);
    check_result();
    return 0;
}