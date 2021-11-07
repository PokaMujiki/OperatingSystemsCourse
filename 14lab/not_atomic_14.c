#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>

#define LINES_AMOUNT 10

typedef unsigned int* semaphore;
semaphore s[2];

int semaphore_init(semaphore* sem, void* ignored, unsigned initValue) {
    *sem = (unsigned*)malloc(sizeof(unsigned));
    if (*sem == NULL) {
        return 1;
    } 
    **sem = initValue;
    return 0;
}

void semaphore_post(semaphore* sem) {
    (**sem)++;
} 

void semaphore_wait(semaphore* sem) {
    while ((**sem) == 0) {
        struct timespec t;
        t.tv_nsec = 1000000;
        t.tv_sec = 0;
        nanosleep(&t, &t);
    }
    (**sem)--;
}

void semaphore_destroy(semaphore* sem) {
    free(*sem);
}

void* print(void* arg) {
    char* strToPrint = (char*)arg;

    for (int i = 0; i < LINES_AMOUNT; i++) {
        semaphore_wait(&s[0]);
        fprintf(stdout, "%s no. %d\n", strToPrint, i + 1);
        semaphore_post(&s[1]);
    }

    return NULL;
}

int main() {
    pthread_t threadId;

    for (int i = 0; i < 2; i++) {
        if (semaphore_init(&s[i], NULL, i)) {
            perror("Error init semaphore\n");
            return 1;
        }
    }

    if (pthread_create(&threadId, NULL, print, "2nd thread message")) {
        perror("Error creating thread!\n");
        return 1;
    }

    for (int i = 0; i < LINES_AMOUNT; i++) {
        semaphore_wait(&s[1]);
        fprintf(stdout, "1st thread message no. %d\n", i + 1);
        semaphore_post(&s[0]);
    }

    if (pthread_join(threadId, NULL)) {
        perror("Error thread join!\n");
        return 1;
    }

    for (int i = 0; i < 2; i++) {
        semaphore_destroy(&s[i]);
    }

    return 0;
}