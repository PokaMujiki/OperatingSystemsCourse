#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <semaphore.h>

#define LINES_AMOUNT 10

sem_t s[2];

void* print(void* arg) {
    char* strToPrint = (char*)arg;

    for (int i = 0; i < LINES_AMOUNT; i++) {
        sem_wait(&s[0]);
        fprintf(stdout, "%s no. %d\n", strToPrint, i + 1);
        sem_post(&s[1]);
    }

    return NULL;
}

int main() {
    pthread_t threadId;

    for (int i = 0; i < 2; i++) {
        if (sem_init(&s[i], 0, i)) {
            perror("Error init semaphore\n");
            return 1;
        }
    }

    if (pthread_create(&threadId, NULL, print, "2nd thread message")) {
        perror("Error creating thread!\n");
        return 1;
    }

    for (int i = 0; i < LINES_AMOUNT; i++) {
        sem_wait(&s[1]);
        fprintf(stdout, "1st thread message no. %d\n", i + 1);
        sem_post(&s[0]);
    }


    if (pthread_join(threadId, NULL)) {
        perror("Error thread join!\n");
        return 1;
    }


    for (int i = 0; i < 2; i++) {
        sem_destroy(&s[i]);
    }

    return 0;
}