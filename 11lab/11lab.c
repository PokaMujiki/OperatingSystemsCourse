#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>

#define LINES_AMOUNT 10

pthread_mutex_t m[3];
pthread_barrier_t b;

void mutexLockErrorCheck(int index) {
    if (pthread_mutex_lock(&m[index])) {
        perror("Error mutex locking!\n");
        exit(1);
    }
}

void mutexUnlockErrorCheck(int index) {
    if (pthread_mutex_unlock(&m[index])) {
        perror("Error mutex unlocking!\n");
        exit(1);
    }
}

void* print(void* arg) {
    char* strToPrint = (char*)arg;

    mutexLockErrorCheck(1);
    pthread_barrier_wait(&b);
    for (int i = 0; i < LINES_AMOUNT; i++) {
        mutexLockErrorCheck(2);
        mutexUnlockErrorCheck(1);

        mutexLockErrorCheck(0);
        mutexUnlockErrorCheck(2);
        fprintf(stdout, "%s no. %d\n", strToPrint, i + 1);

        mutexLockErrorCheck(1);
        mutexUnlockErrorCheck(0);
    }

    mutexUnlockErrorCheck(1);

    return NULL;
}

int main() {
    pthread_t threadId;

    pthread_mutexattr_t attr;
    if (pthread_mutexattr_init(&attr)) {
        perror("Error mutex attribute init!\n");
        return 1;
    }
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);

    for (int i = 0; i < 3; i++) {
        if (pthread_mutex_init(&m[i], &attr)) {
            perror("Error mutex init!\n");
            return 1;
        }
    }

    if (pthread_barrier_init(&b, NULL, 2)) {
        perror("Error barrier init!\n");
        return 1;
    }

    mutexLockErrorCheck(0);

    if (pthread_create(&threadId, NULL, print, "2nd thread message")) {
        perror("Error creating thread!\n");
        return 1;
    }

    pthread_barrier_wait(&b);
    for (int i = 0; i < LINES_AMOUNT; i++) {
        fprintf(stdout, "1st thread message no. %d\n", i + 1);
        mutexLockErrorCheck(1);
        mutexUnlockErrorCheck(0);

        mutexLockErrorCheck(2);
        mutexUnlockErrorCheck(1);

        mutexLockErrorCheck(0);
        mutexUnlockErrorCheck(2);
    }

    mutexUnlockErrorCheck(0);

    if (pthread_join(threadId, NULL)) {
        perror("Error thread join!\n");
        return 1;
    }


    for (int i = 0; i < 3; i++) {
        if (pthread_mutex_destroy(&m[i])) {
            perror("Error mutex destroy!\n");
            return 1;
        }
    }

    pthread_barrier_destroy(&b);

    return 0;
}