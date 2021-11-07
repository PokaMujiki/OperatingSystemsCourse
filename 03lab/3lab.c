#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

#define THREADS_AMOUNT 4
#define STRINGS_IN_THREAD 5
#define MAX_SYMBOLS_IN_STR 100

void* threadWork(void* arg) {
    for (int i = 0; i < STRINGS_IN_THREAD; i++) {
        fprintf(stdout, "%s", ((char**)arg)[i]);
    }
    fflush(stdout);
}

int main() {
    int errCode = 0;
    pthread_t threadIds[THREADS_AMOUNT] = {0};

    char*** stringsForThreads = (char***)calloc(THREADS_AMOUNT, sizeof(char**));
    for (int i = 0; i < THREADS_AMOUNT; i++) {
        stringsForThreads[i] = (char**)calloc(STRINGS_IN_THREAD, sizeof(char*));
        for (int j = 0; j < STRINGS_IN_THREAD; j++) {
            stringsForThreads[i][j] = (char*)calloc(MAX_SYMBOLS_IN_STR, sizeof(char));
            sprintf(stringsForThreads[i][j], "%d string for thread no. %d\n", j + 1, i + 1);
        }
    }

    for (int i = 0; i < THREADS_AMOUNT; i++) {
        if (pthread_create(&(threadIds[i]), NULL, threadWork, stringsForThreads[i])) {
            fprintf(stderr, "error creating %d thread!\n", i);
            return 1;
        }
    }

    for (int i = 0; i < THREADS_AMOUNT; i++) {
        errCode = pthread_join(threadIds[i], NULL);
        if (errCode) {
            fprintf(stderr, "error joining %d thread! error code: %d\n", i, errCode);
            return 1;
        }
    }

    fprintf(stdout, "all threads are done!\n");

    for (int i = 0; i < THREADS_AMOUNT; i++) {
        for (int j = 0; j < STRINGS_IN_THREAD; j++) {
            free(stringsForThreads[i][j]);
        }
        free(stringsForThreads[i]);
    }
    free(stringsForThreads);

    return 0;
}