
// https://stackoverflow.com/questions/5582211/what-does-define-gnu-source-imply
#define _GNU_SOURCE

#include <pthread.h>
#include <sched.h>
#include <stdatomic.h>
#include <unistd.h>

#include "common.h"

#ifndef NTHREADS
    #define NTHREADS 8
#endif

// #define BLOCK_SIZE 16

#define T_BLOCK_X 8
#define T_BLOCK_Y 2

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
    for (int i = start_index; i < end_index; i+=T_BLOCK_X) { // This is the X tiling
        for (int j = 0; j < N; j += T_BLOCK_Y) {
            for (int ii = 0; ii < T_BLOCK_X; ii++) {
                for (int jj = 0; jj < N; jj++) {
                    int acc;
                    for (int k = 0; k < N; k++) {
                        // Convert this is the acutal representation
                        acc += a[(jj + j)][k] * b[(ii + i)][k];
                        // c[i][j] += a[i][k] * b[k][j];
                    }
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
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
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
    init_matrix();
    struct timespec start, end;

    // int start_time = nanos();
    time_t start_time_t = time(NULL);

    clock_gettime(CLOCK_REALTIME, &start);
#if NTHREADS > 1
    threads_ready = 0;
    threads_done = 0;
    pthread_mutex_lock(&lock);
    pthread_t threads[NTHREADS];
    for (int i = 0; i < NTHREADS; i++) {
        // Create the threads
        printf("Creating thread %d\n", i);
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
    clock_gettime(CLOCK_REALTIME, &end);
    time_t end_time_t = time(NULL);
    double time_spent = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1000000000.0;
    // int end_time = nanos();
    // printf("start:%d, end:%d\n", end_time, start_time);
    printf("start_t:%lu, end_t:%lu\n", end_time_t, start_time_t);
    // printf("Time taken: %lu seconds\n", end_time_t - start_time_t);
    printf("Time taken: %f seconds\n", time_spent);
    printf("%f GFLOPS/S\n", ((2.0 * N * N * N) / ((time_spent) * 1e9)));
    // printf("Total time %f\n", (float)(end_time-start_time));
    // get_tflops(start_time_t, end_time_t, (char *)"Mutiplication:");

#if DEBUG
    print_matrix();
#endif

    return 0;
}
