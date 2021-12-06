#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <asm-generic/errno.h>

#define PHILOSOPHER_AMOUNT 5
#define DELAY 1
#define FOOD 10000

pthread_mutex_t foodLock;
pthread_mutex_t forks[PHILOSOPHER_AMOUNT];
pthread_t philosophers[PHILOSOPHER_AMOUNT];

pthread_mutex_t getForksMutex;
pthread_cond_t condVar;


int errorCode;
void lockMutex(pthread_mutex_t *mutex) {
    if((errorCode = pthread_mutex_lock(mutex))) {
        fprintf(stderr, "pthread_mutex_lock error code: %d\n", errorCode);
        exit(-1);
    }
}

void unlockMutex(pthread_mutex_t *mutex) {
    if((errorCode = pthread_mutex_unlock(mutex))) {
        fprintf(stderr, "pthread_mutex_unlock error code: %d\n", errorCode);
        exit(-1);
    }
}

int foodOnTable() {
    static int food = FOOD;
    int myFood;

    lockMutex(&foodLock);
    if (food > 0) {
        food--;
    }
    myFood = food;
    unlockMutex(&foodLock);
    return myFood;
}

int tryToGetFork(int philosopher, int fork, char *hand) {
    int result = pthread_mutex_trylock(&forks[fork]);
    if (result != EBUSY) {
        printf("Philosopher %d: got %s fork %d\n", philosopher, hand, fork);
    }
    return result;
}

void downForks(int f1, int f2) {
    unlockMutex (&forks[f1]);
    unlockMutex (&forks[f2]);
}

void *philosopher(void *num) {
    int id;
    int leftFork, rightFork, food;

    id = (int) num;
    printf("Philosopher %d sitting down to dinner.\n", id);
    rightFork = id;
    leftFork = id + 1;

    /* Wrap around the forks. */
    if (leftFork == PHILOSOPHER_AMOUNT) {
        leftFork = 0;
    }

    while ((food = foodOnTable())) {
        printf("Philosopher %d: get dish %d.\n", id, food);

        lockMutex(&getForksMutex);
        while (1) {
            while (tryToGetFork(id, rightFork,"right") == EBUSY) {
                pthread_cond_wait(&condVar, &getForksMutex);
            }

            if (tryToGetFork(id, leftFork,"left") == EBUSY) {
                unlockMutex(&forks[rightFork]);
                pthread_cond_wait(&condVar, &getForksMutex);
            }
            else {
                break;
            }
        }
        unlockMutex(&getForksMutex);

        printf("Philosopher %d: eating.\n", id);
        usleep(DELAY);

        lockMutex(&getForksMutex);
        downForks(rightFork, leftFork);
        pthread_cond_broadcast(&condVar);
        unlockMutex(&getForksMutex);
    }
    printf("Philosopher %d is done eating.\n", id);
    return (NULL);
}

int main (int argc, char **argv) {
    int i;

    pthread_mutexattr_t mutexattr;
    pthread_mutexattr_init(&mutexattr);
    pthread_mutexattr_settype(&mutexattr, PTHREAD_MUTEX_ERRORCHECK);

    pthread_mutex_init(&foodLock, &mutexattr);
    pthread_mutex_init(&getForksMutex, &mutexattr);
    pthread_cond_init(&condVar, NULL);
    for (i = 0; i < PHILOSOPHER_AMOUNT; i++) {
        pthread_mutex_init(&forks[i], &mutexattr);
    }
    for (i = 0; i < PHILOSOPHER_AMOUNT; i++) {
        pthread_create(&philosophers[i], NULL, philosopher, (void *)i);
    }
    for (i = 0; i < PHILOSOPHER_AMOUNT; i++) {
        pthread_join(philosophers[i], NULL);
    }
    pthread_mutex_destroy(&foodLock);
    pthread_mutex_destroy(&getForksMutex);
    pthread_cond_destroy(&condVar);
    return 0;
}
