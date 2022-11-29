#define _GNU_SOURCE

#include <pthread.h>
#include <sched.h>
#include <stdatomic.h>
#include <unistd.h>

#include "common.h"

// SGEMM vs DGEMM
// This is SGEMM

// Google patent??
// https://patents.google.com/patent/WO2019055593A1/en

// What is c++ intrinsics?
// https://stackoverflow.blog/2020/07/08/improving-performance-with-simd-intrinsics-in-three-use-cases/

// Need to look at what is alignment
// Aligned data is faster than non aligned data
// https://gcc.gnu.org/onlinedocs/gcc/Common-Variable-Attributes.html#Common-Variable-Attributes
float A[N * N] __attribute__((aligned(64)));
float B[N * N] __attribute__((aligned(64)));
float C[N * N] __attribute__((aligned(64)));
float fval[N * N] __attribute__((aligned(64)));


// Is there a way to check if my CPU has 256 bit wide register?
// __m256 is a 256 bit vector which is 8 32 bit floats
__m256 *Am = (__m256 *)A;
__m256 *Bm = (__m256 *)B;
__m256 *Cm = (__m256 *)C;

float Bf[N * N] __attribute__((aligned(64)));
__m256 *Bfm = (__m256 *)Bf;

#ifndef NTHREADS
    #define NTHREADS 8
#endif

// Is this how you calculate the block size
// Width of the vector / size of a single preciscion float = BLOCK SIZE??
// #define BLOCK_SIZE 16

// Global variables
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
atomic_int threads_done = 0;
atomic_int threads_ready = 0;


void matmul(int start_index, int end_index) {
    /*
    #ifdef DEBUG
        printf("Initial matrices:\n");
        print_matrix();
    #endif
    */
    // uint64_t start = nanos();
    // printf("Multiplying block %d to %d\n", start_index, end_index);
    /*
     * SIMD
     * http://ftp.cvut.cz/kernel/people/geoff/cell/ps3-linux-docs/CellProgrammingTutorial/BasicsOfSIMDProgramming.html
     * SIMD has vector types where as in c we have scalar types
     * SIMD does clipping or saturation
     */
    /*
     * Function I can use for SIMD
     * _mm256_load_ps
     * _mm256_store_ps
     * _mm256_add_ps
     * _mm256_mul_ps
     * _mm256_fmadd_ps
     * _mm256_set1_ps
     * _mm256_setzero_ps
     * _mm256_cmp_ps
     * _mm256_blendv_ps
     * _mm256_movemask_ps
     * _mm256_permute2f128_ps
     * _mm256_broadbcast_ps
     */
    for (int y = start_index; y < end_index; y += T_BLOCK_Y) {
        for (int x = 0; x < N; x += BLOCK_SIZE * T_BLOCK_X) {
            __m256 acc[T_BLOCK_Y][T_BLOCK_X] = {};
            for (int k = 0; k < N; k++) {
                for (int iy = 0; iy < T_BLOCK_Y; iy++) {
                    // Load A into a vector
                    __m256 ta = _mm256_broadcast_ss(&A[(y + iy) * N + k]);
                    for (int ix = 0; ix < T_BLOCK_X; ix++) {
                        // Perform a FMA = Multiply then add
                        acc[iy][ix] = _mm256_fmadd_ps(ta, Bfm[((x + ix * BLOCK_SIZE) * N + k * 8) / 8], acc[iy][ix]);
                    }
                }
            }

            for (int iy = 0; iy < T_BLOCK_Y; iy++) {
                for (int ix = 0; ix < T_BLOCK_X; ix++) {
                    printf("Storing %f\n", acc[iy][ix][0]);
                    Cm[((y + iy) * N + x + ix * BLOCK_SIZE) / 8] = acc[iy][ix];
                }
            }
        }
    }
/*
#ifdef DEBUG
    printf("Multiplied matrices:\n");
    print_matrix();
#endif
*/
}

// Why?; ask the C gods.
// https://stackoverflow.com/questions/11253025/pthread-create-not-working-passing-argument-3-warning
void *matmul_thread(void *n) {
    uint64_t k = (uint64_t)n;
    int start_index = (N / NTHREADS) * k;
    int end_index = (N / NTHREADS) * (k + 1);

    // Hmmm......???
    // What are cpusets?
    // https://docs.kernel.org/admin-guide/cgroup-v1/cpusets.html
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    // Ig this is basically assigning this thread to a cpu
    CPU_SET(k, &cpuset);
    // What mutex stuff do I need to do here?
    pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);

    threads_ready++;

    pthread_mutex_lock(&lock);
    pthread_mutex_unlock(&lock);

    matmul(start_index, end_index);

    threads_done++;
    return NULL;
}

int main() {
    // init_matrix();
    FILE *f = fopen("matmul_matrix", "rb");
    if (f == NULL) {
        printf("please pregenerate python /tmp/matmul file\n");
        return -1;
    }
    size_t read_a = fread(A, 1, sizeof(float) * N * N, f);
    size_t read_b = fread(B, 1, sizeof(float) * N * N, f);
    size_t read_c = fread(fval, 1, sizeof(float) * N * N, f);
    fclose(f);

    uint32_t start = nanos();
#if NTHREADS > 1
    threads_ready = 0;
    threads_done = 0;
    pthread_mutex_lock(&lock);
    pthread_t threads[NTHREADS];
    for (int i = 0; i < NTHREADS; i++) {
        // Create the threads
        // printf("Creating thread %d\n", i);
        pthread_create(&threads[i], NULL, matmul_thread, (void *)(uint64_t)i);
    }
    while (threads_ready < NTHREADS) usleep(1);  // Wait for all threads to be ready
#endif

#if NTHERADS == 1
    matmul(0, N);
#else
    pthread_mutex_unlock(&lock);
    while (threads_done != NTHREADS) usleep(1); // Wait for all the threads to be done
    // Should I do this or just wait for join to happen?

#endif

#if NTHREADS > 1
    for (int j = 0; j < NTHREADS; j++) {
        pthread_join(threads[j], NULL); // Maybe you should get the retval?
    }
#endif
    uint32_t end = nanos();
    printf("Total time %f\n", (float)(end-start));
    get_tflops(start, end, (char *)"Mutiplication:");

    for (int k = 0; k < N * N; k++) {
        if (fabsf(C[k] - fval[k]) > 1e-3) {
            printf("MISMATCH AT %d, %f != %f\n", k, C[k], fval[k]);
            return -1;
        }
    }

#if DEBUG
    print_matrix();
#endif

    return 0;
}