// thread-performance-test-2020.c
// 
// This program runs some threads to measure performance.
// It runs from one up to MAX_PARALLEL_THREADS in parallel,
// performing ITERATIONS_PER_THREAD floating-point operations in each thread.
//
// Thomas Padron-McCarthy (thomas.padron-mccarthy@oru.se)
// Mon Apr 13 11:11:13 CEST 2020

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <assert.h>

#define MAX_PARALLEL_THREADS 20
#define ITERATIONS_PER_THREAD (1000 * 1000 * 1000)

// These values are chosen to work with +, -, * and /
#define STARTING_VALUE 4711.0
#define FACTOR 1.0000000001

static void *thread_body(void *arg) {
    double* resultp = (double*)arg;
    double result = STARTING_VALUE;
    for (int i = 0; i < ITERATIONS_PER_THREAD; i++) {
        result /= FACTOR; // Here we can try +, -, * and /
    }
    *resultp = result;
    return NULL;
} // thread_body

void run_N_threads(int N) {
    pthread_t threads[N];
    double results[N];

    printf("Starting %d thread(s)...\n", N);

    struct timeval before;
    gettimeofday(&before, NULL);

    for (int i = 0; i < N; ++i) {
	if (pthread_create(&threads[i], NULL, thread_body, (void*)&results[i]) != 0) {
	    printf("Error: Couldn't create thread %d.\n", i);
	    exit(EXIT_FAILURE);
	}
    }

    if (N == 1)
        printf("The 1 thread is running. Waiting for it to finish...\n");
    else
        printf("The %d threads are running. Waiting for them to finish...\n", N);

    for (int i = 0; i < N; ++i) {
	if (pthread_join(threads[i], NULL) != 0) {
	    printf("Error: Couldn't join thread %d.\n", i);
	    exit(EXIT_FAILURE);
	}
	// printf("results[%d] = %f\n", i, results[i]);
    }

    struct timeval after;
    gettimeofday(&after, NULL);

    if (N == 1)
        printf("The 1 thread is done.\n");
    else
        printf("The %d threads are done.\n", N);

    double elapsed_milliseconds = (after.tv_sec - before.tv_sec) * 1e6 + (after.tv_usec - before.tv_usec);
    double seconds = elapsed_milliseconds / 1e6;

    /*
      printf("Elapsed: (%ld s + %ld us) = %f s\n",
      (long int)(after.tv_sec - before.tv_sec),
      (long int)(after.tv_usec - before.tv_usec),
      seconds);
    */
    printf("Elapsed time: %.2f s\n", seconds);

    double result_sum = 0;

    /*
      -- Check that you don't get overflow! Calculating with NaNs might nog take the same time.
      for (int i = 0; i < N; ++i)
      printf("Result %d: %f\n", i, results[i]);
    */

    for (int i = 0; i < N; ++i)
	result_sum += results[i];
    double operations = (double)N * ITERATIONS_PER_THREAD;

    double mflops = operations / seconds / 1e6;
    printf("%d thread(s) done: %.2f Mflops (%.2f per thread)\n", N, mflops, mflops / N);
} // run_N_threads

int main(void) {
    printf("thread-performance-test-2020: Running from 1 up to %d threads\n", MAX_PARALLEL_THREADS);
    for (int i = 1; i <= MAX_PARALLEL_THREADS; ++i)
	run_N_threads(i);
    printf("thread-performance-test-2020: Done.\n");

    return EXIT_SUCCESS;
} // main