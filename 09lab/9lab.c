#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>

#define MAX_THREADS_AMOUNT 1000
#define ITERATIONS 10000000

int threadsAmount;
int stop = 0;
int lastIteration = 0;
pthread_barrier_t barrier;

typedef struct Arg {
    double partialSum;
    int threadNumber;
} arg;

void handler(int signum) {
    stop = 1;
}

void* threadWork(void* argument) {
    int threadNumber = ((arg*)argument)->threadNumber;
    double sum = 0.0;
    int bigIteration = 0;

    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    sigaddset(&set, SIGTERM);

    pthread_barrier_wait(&barrier);
    pthread_sigmask(SIG_BLOCK, &set, NULL);
    while (1) {
        for (long long i = (long long)bigIteration * ITERATIONS * threadsAmount + threadNumber;
            i < (long long)(bigIteration + 1) * ITERATIONS * threadsAmount; i += threadsAmount) {
            if (i % 2 == 0) {
                sum += 1.0 / (2.0 * i + 1);
            }
            else {
                sum -= 1.0 / (2.0 * i + 1);
            }
        }
        bigIteration++;
//        pthread_barrier_wait(&barrier);
        pthread_sigmask(SIG_UNBLOCK, &set, NULL);
        pthread_sigmask(SIG_BLOCK, &set, NULL);
        pthread_barrier_wait(&barrier);
        if (stop) {
            break;
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

    if (pthread_barrier_init(&barrier, NULL, threadsAmount)) {
        perror("error barrier init!\n");
        exit(1);
    }

    pthread_t* threadIds = (pthread_t*)calloc(threadsAmount, sizeof(pthread_t));
    arg* args = (arg*)calloc(threadsAmount, sizeof(arg));

    if (signal(SIGINT, handler) || signal(SIGTERM, handler)) {
        perror("error setting signal handler!\n");
        return 1;
    }

    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    sigaddset(&set, SIGTERM);
    pthread_sigmask(SIG_BLOCK, &set, NULL);

    int errCode = 0;
    for (int i = 0; i < threadsAmount; i++) {
        args[i].partialSum = 0.0;
        args[i].threadNumber = i;
        if ((errCode = pthread_create(&(threadIds[i]), NULL, threadWork, (void*)&(args[i])))) {
            fprintf(stderr, "error creating thread no.%d! error code: %d\n", i + 1, errCode);
            return 1;
        }
    }

    fprintf(stdout, "Signal handler installed!\n");

    double sum = 0.0;
    for (int i = 0; i < threadsAmount; i++) {
        if ((errCode = pthread_join(threadIds[i], NULL))) {
            fprintf(stderr, "error joining %d thread! error code: %d\n", i, errCode);
            return 1;
        }
        sum += args[i].partialSum;
    }

    fprintf(stdout, "pi = %.10lf\n", sum * 4);

    pthread_barrier_destroy(&barrier);
    free(args);
    free(threadIds);

    return  0;
}
