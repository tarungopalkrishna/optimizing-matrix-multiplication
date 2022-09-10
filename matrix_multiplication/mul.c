// gcc -O2 -march=native mul.c -lpthread && ./a.out
#define _GNU_SOURCE

#include <stdint.h>
#include <time.h>
#include <sched.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <immintrin.h>
#include <pthread.h>
#include <unistd.h>
#include <stdatomic.h>

#include<stdio.h>
#include<stdint.h>
#include<time.h>
#include <immintrin.h>
#include <unistd.h>
#include <stdatomic.h>


#define pf printf

#ifdef DEBUG
    #define NTHREADS 1
    #define N 4
    #define BLOCK_SIZE 2
#else
    #define NTHREADS 8
    #define N 2048
    #define BLOCK_SIZE 16
#endif


#define BLOCK 8
#define BLOCK_Y 4
#define BLOCK_X 2


/*
 * Lectures for learning
 * 1. https://ocw.mit.edu/courses/6-172-performance-engineering-of-software-systems-fall-2018/resources/lecture-1-intro-and-matrix-multiplication/
 */

/*
 * A BIG RABBIT HOLE OF WHY FLOPS AND ITD DEFINITION
 * https://stackoverflow.com/questions/329174/what-is-flop-s-and-is-it-a-good-measure-of-performance?answertab=scoredesc#tab-top
 */

/*
 * CACHING
 * https://stackoverflow.com/questions/16699247/what-is-a-cache-friendly-code
 * https://lwn.net/Articles/255364/ - How memeory works for matrices
 * https://lwn.net/Articles/256433/
 * https://stackoverflow.com/questions/11413855/why-is-transposing-a-matrix-of-512x512-much-slower-than-transposing-a-matrix-of
 */

/*
 * Matrix multiply
 * 1. Tiling
 * 2. Cache coherence
 * 3. The fundamental thing about machine learing is matrix multiply
 * 4.
 *
 * Memory order row and column major
 * In C the 2-D array is stored in row major order and its efficient to parse
 * the array in row major order than column major order
 *
 * Not much improvement from the accessing memeory via row major order:
 * 1. https://stackoverflow.com/questions/1907557/optimized-matrix-multiplication-in-c
 */

// How much is my L1 cache?
// What is aligned bits?

/*
 * Global vars
 */
float a[N][N];
float b[N][N];
float c[N][N];

float A[N*N] __attribute__ ((aligned (64)));
float B[N*N] __attribute__ ((aligned (64)));
float C[N*N] __attribute__ ((aligned (64)));
float val[N*N] __attribute__ ((aligned (64)));

__m256 *Am = (__m256*)A;
__m256 *Bm = (__m256*)B;
__m256 *Cm = (__m256*)C;

float Bf[N*N] __attribute__ ((aligned (64)));
__m256 *Bfm = (__m256*)Bf;

uint64_t nanos(){
    struct timespec start;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    return (uint64_t)start.tv_sec*1000000000 + (uint64_t)start.tv_nsec;
}

void get_tflops(uint64_t start, uint64_t end, char *message){
    printf("%s", message);
    double tflops = (2.0*N*N*N)*1e-12;
    double s = (end-start)*1e-9;
    double elapsed_time = end-start;
    printf("%f seconds\n", elapsed_time);
    printf("%f TFLOPS/S\n", tflops/s);
}

/*
void get_tflops(uint64_t start, uint64_t end){
    double tflops = (2.0*N*N*N)*1e-12;
    double s = (end-start)*1e-9;
    printf("%f TFLOPS/S\n", tflops/s);
}
*/

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



void init_matrix(){
    for(int i=0; i<N; ++i){
        for(int j=0; j<N; ++j){
            // if(i==0)
            //     a[i][j] = j;
            // else
            a[i][j] = (N*i)+j+1;
            if(i==j)
                b[i][j] = 1;
            else
                b[i][j] = 0;
        }
    }
}

void reset_matrix(){
    for(int i=0; i<N; ++i){
        for(int j=0; j<N; ++j){
            c[i][j] = 0;
        }
    }
}

void tiling_multiply(){
    /*
     * Threads - Actual worker nodes for execution
     * Block - A group of threads with shared memory
     * Grid - A group of blocks with access to global memory and constant memory
     */

    assert(N%BLOCK_SIZE == 0);
    printf("imax = %i, jmax = %i\n", (N/BLOCK_SIZE), (N/BLOCK_SIZE));
    uint64_t start = nanos();
    // for(int i=0; i<(N/BLOCK_SIZE); i+=BLOCK_SIZE){
    //     for(int j=0; j<(N/BLOCK_SIZE); j+=BLOCK_SIZE){
    for(int i=0; i<(N/BLOCK_SIZE); ++i){
        for(int j=0; j<(N/BLOCK_SIZE); ++j){
            float acc[BLOCK_SIZE][BLOCK_SIZE] = {};
            // printf("%i ", i);
            // for(int
            for(int k=0; k<N; ++k){
                for(int ki=0;ki<BLOCK_SIZE; ++ki){
                    for(int kj=0; kj<BLOCK_SIZE; ++kj){
                        // printf("%i * %i + %i\n",i, BLOCK_SIZE, ki);
                        // printf("acc[%i][%i] += a[%i][%i] * b[%i][%i]\n", ki, kj, (i*BLOCK_SIZE)+ki, k, k, (j*BLOCK_SIZE)+kj);
#ifdef DEBUG
                        printf("%i %i %i %i (%i, %i)\n", i, j, ki, kj, i+ki, j+kj);
#endif
                        acc[ki][kj] += a[(i*BLOCK_SIZE)+ki][k] * b[k][(j*BLOCK_SIZE)+kj];
                    }
                }
            }
            // c[i][j] = acc;
            // printf("--------------------------\n");
            for(int kj=0; kj<BLOCK_SIZE; ++kj){
                for(int ki=0;ki<BLOCK_SIZE; ++ki){
                        // printf("acc[%i][%i] = %f ", ki, kj, acc[ki][kj]);
                        c[(i*BLOCK_SIZE)+ki][(j*BLOCK_SIZE)+kj] = acc[ki][kj];
                }
                // printf("\n");
            }
        }
        // printf("\n");
    }
    uint64_t end = nanos();
    get_tflops(start, end, (char *)"Tiling mutiplication:");
#ifdef DEBUG
    print_matrix();
#endif
}


void tiling_transpose_multiply(){
    /*
     * Threads - Actual worker nodes for execution
     * Block - A group of threads with shared memory
     * Grid - A group of blocks with access to global memory and constant memory
     *
     * Here since the matrix b is a unit matrix the transpose is the same as the
     * the original matrix.(i.e B = Transpose(B))
     */

    assert(N%BLOCK_SIZE == 0);
    pf("imax = %i, jmax = %i\n", (N/BLOCK_SIZE), (N/BLOCK_SIZE));
    // float bt = transpose;
    uint64_t start = nanos();
    for(int i=0; i<(N/BLOCK_SIZE); ++i){
        for(int j=0; j<(N/BLOCK_SIZE); ++j){
            float acc[BLOCK_SIZE][BLOCK_SIZE] = {};
            // printf("%i ", i);
            // for(int
            for(int k=0; k<N; ++k){
                for(int ki=0;ki<BLOCK_SIZE; ++ki){
                    for(int kj=0; kj<BLOCK_SIZE; ++kj){
                        // printf("%i * %i + %i\n",i, BLOCK_SIZE, ki);
                        // printf("acc[%i][%i] += a[%i][%i] * b[%i][%i]\n", ki, kj, (i*BLOCK_SIZE)+ki, k, k, (j*BLOCK_SIZE)+kj);
#ifdef DEBUG
                        printf("%i %i %i %i (%i, %i)\n", i, j, ki, kj, i+ki, j+kj);
#endif
                        acc[ki][kj] += a[(i*BLOCK_SIZE)+ki][k] * b[(j*BLOCK_SIZE)+kj][k];
                    }
                }
            }
            // c[i][j] = acc;
            // printf("--------------------------\n");
            for(int kj=0; kj<BLOCK_SIZE; ++kj){
                for(int ki=0;ki<BLOCK_SIZE; ++ki){
                        // printf("acc[%i][%i] = %f ", ki, kj, acc[ki][kj]);
                        c[(i*BLOCK_SIZE)+ki][(j*BLOCK_SIZE)+kj] = acc[ki][kj];
                }
                // printf("\n");
            }
        }
        // printf("\n");
    }
    uint64_t end = nanos();
    get_tflops(start, end, (char *)"Tiling Transpose mutiplication:");
#ifdef DEBu
    print_matrix();
#endif
}


void transpose_multiply(){
    uint64_t start = nanos();
    // Need to transpose the matrix
    // Here the matix b is the a unit matrix
    for(int i=0; i<N; i++){
        for(int j=0; j<N; j++){
        float acc = 0;
            for(int k=0; k<N; k++){
#ifdef DEBUG
                        printf("%i, %i, %i\n", i, j, k);
#endif
                acc += a[i][k] * b[j][k];
            }
        c[i][j] = acc;
        }
    }
    uint64_t end = nanos();
    get_tflops(start, end, (char *)"Transpose mutiplication:");
#ifdef DEBUG
    print_matrix();
#endif
}




void matmul(int sy, int ey) {
  // 136.77 GFLOPS on single core numpy
  // 4.9 GHz is max boost for 5950X
  // 32 FLOPS/cycle (16 FMAs, aka 2x 8 single wide / 32 byte FMAs)
  // theoretical max is 156.8 GFLOPS, we see 150
  // multicore theo max = 2508.8 GFLOPS, we see 1501.434299

  // Bf = (y/8, k, 8)
  uint64_t start = nanos();
  for (int y = sy; y < ey; y+=BLOCK_Y) {
    for (int x = 0; x < N; x+=BLOCK*BLOCK_X) {

      __m256 acc[BLOCK_Y][BLOCK_X] = {};
      for (int k = 0; k < N; k++) {
        for (int iy = 0; iy < BLOCK_Y; iy++) {
          __m256 ta = _mm256_broadcast_ss(&A[(y+iy)*N + k]);
          for (int ix = 0; ix < BLOCK_X; ix++) {
            acc[iy][ix] = _mm256_fmadd_ps(ta, Bfm[((x+ix*BLOCK)*N + k*8)/8], acc[iy][ix]);
          }
        }
      }

      for (int iy = 0; iy < BLOCK_Y; iy++) {
        for (int ix = 0; ix < BLOCK_X; ix++) {
          Cm[((y+iy)*N + x + ix * BLOCK)/8] = acc[iy][ix];
        }
      }
    }
  }
  uint64_t end = nanos();
  get_tflops(start, end, (char *)"GEOHOTZ mutiplication:");
}


// This is the baseline multiplication
// O(n^3)
void basic_multiply(){
    // float a[][2] = {
    //     { 1, 2 },
    //     { 3, 4 },
    //
    // };
    // float b[][2] = {
    //     { 1, 2 },
    //     { 3, 4 },
    // };
    // Sometimes this matrix is not set to 0,0
    // Ig that only happens when this is in bss and not when in the
    // data segment.
    // float c[2][2];
    uint64_t start = nanos();
    for(int i =0;i<N;i++){
        for(int j=0;j<N;j++){
            float acc = 0;
            for(int k=0;k<N;k++){
                //c[i][j] = c[i][j] + (a[i][k] * b[k][j]);
                acc += a[i][k] * b[k][j];
            }
            c[i][j] = acc;
            // printf("c[%d][%d] = %f\n", i, j, c[i][j]);
        }
    }
    uint64_t end = nanos();
    get_tflops(start, end, (char *)"Basic mutiplication:");
#ifdef DEBUG
    print_matrix();
#endif
}

int main(){

#ifdef DEBUG
    init_matrix();
#endif
    basic_multiply();
#ifdef DEBUG
    reset_matrix();
#endif
    tiling_multiply();
#ifdef DEBUG
    reset_matrix();
#endif
    tiling_transpose_multiply();
#ifdef DEBUG
    reset_matrix();
#endif
    transpose_multiply();

    int k=1;

    int sy = (N/NTHREADS) * k;
    int ey = (N/NTHREADS) * (k+1);

    matmul(sy, ey);
    return 0;
}
