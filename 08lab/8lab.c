#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_THREADS_AMOUNT 1000
#define ITERATIONS_AMOUNT 1000000000
int threadsAmount;

typedef struct Arg {
    double partialSum;
    int threadNumber;
} arg;

void* threadWork(void* argument) {
    int threadNumber = ((arg*)argument)->threadNumber;
    double sum = 0.0;
    for (int i = threadNumber; i < ITERATIONS_AMOUNT; i += threadsAmount) {
        if (i % 2 == 0) {
            sum += 1.0 / (2 * i + 1);
        }
        else {
            sum -= 1.0 / (2 * i + 1);
        }
    }
    ((arg*)argument)->partialSum = sum;
    pthread_exit(NULL);
}

int main (int argv, char** argc) {
    if (argv != 2) {
        fprintf(stderr, "wrong amount of parameters!\n");
        return 1;
    }

    threadsAmount = atoi(argc[1]);
    if (threadsAmount <= 0) {
        fprintf(stderr, "threads amount must be > 0!\n");
        return 1;
    }

    if (threadsAmount > MAX_THREADS_AMOUNT) {
        fprintf(stderr, "you can't use more than %d threads!\n", MAX_THREADS_AMOUNT);
        return 1;
    }

    pthread_t* threadIds = (pthread_t*)calloc(threadsAmount, sizeof(pthread_t));
    arg* args = (arg*)calloc(threadsAmount, sizeof(arg));

    int errCode = 0;
    for (int i = 0; i < threadsAmount; i++) {
        args[i].partialSum = 0.0;
        args[i].threadNumber = i;
        errCode = pthread_create(&(threadIds[i]), NULL, threadWork, (void*)&(args[i]));
        if (errCode) {
            fprintf(stderr, "error creating thread no.%d! error code: %d\n", i + 1,errCode);
            return 1;
        }
    }

    double sum = 0.0;
    for (int i = 0; i < threadsAmount; i++) {
        errCode = pthread_join(threadIds[i], NULL);
        if (errCode) {
            fprintf(stderr, "error joining %d thread! error code: %d\n", i, errCode);
            return 1;
        }
        sum += args[i].partialSum;
    }

    fprintf(stdout, "pi = %.10lf\n", sum * 4);

    free(args);
    free(threadIds);
    
    return  0;
}