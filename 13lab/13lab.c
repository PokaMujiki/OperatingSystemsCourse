#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>

#define STR_MAX_LENGTH 100
#define LINES_AMOUNT 10

typedef struct Args {
    int myTurnNumber;
    char* stringToPrint;
} args;

pthread_mutex_t mutex;
pthread_cond_t condVar;
int currentTurn = 1;

void* print(void* arg) {
    args *threadArgument = (args *) arg;

    if (pthread_mutex_lock(&mutex)) {
        perror("Error locking mutex!\n");
        exit(1);
    }

    for (int i = 0; i < LINES_AMOUNT; i++) {
        while (currentTurn != threadArgument->myTurnNumber) {
            if (pthread_cond_wait(&condVar, &mutex)) {
                perror("Error waiting condition variable!\n");
            }
        }
        printf("%s: %d\n", threadArgument->stringToPrint, i + 1);
        currentTurn = !currentTurn;

        if (pthread_cond_signal(&condVar)) {
            perror("Error signaling condition variable!\n");
        }
    }

    if (pthread_mutex_unlock(&mutex)){
        perror("Error unlocking mutex\n");
    }

    return NULL;
}

int main() {
    pthread_t threadId;
    args* secondThreadArg = (args*)calloc(1, sizeof(args));
    secondThreadArg->myTurnNumber = 0;
    secondThreadArg->stringToPrint = (char*)calloc(STR_MAX_LENGTH, sizeof(char));
    strcpy(secondThreadArg->stringToPrint, "2nd thread message");

    args* firstThreadArg = (args*)calloc(1, sizeof(args));
    firstThreadArg->myTurnNumber = 1;
    firstThreadArg->stringToPrint = (char*)calloc(STR_MAX_LENGTH, sizeof(char));
    strcpy(firstThreadArg->stringToPrint,"1st thread message");

    if (pthread_mutex_init(&mutex, NULL)) {
        perror("Error mutex init!\n");
        return 1;
    }

    if (pthread_cond_init(&condVar, NULL)) {
        perror("Error creating condition variable!\n");
        return 1;
    }

    if (pthread_create(&threadId, NULL, print, (void*)secondThreadArg)) {
        perror("Error creating thread!\n");
        return 1;
    }

    print(firstThreadArg);

    if (pthread_join(threadId, NULL)) {
        perror("Error thread join!\n");
        return 1;
    }

    if (pthread_mutex_destroy(&mutex)) {
        perror("Error mutex destroy!\n");
        return 1;
    }

    if (pthread_cond_destroy(&condVar)) {
        perror("Error destroying condVar\n");
        return 1;
    }

    free(secondThreadArg->stringToPrint);
    free(secondThreadArg);
    free(firstThreadArg->stringToPrint);
    free(firstThreadArg);

    return 0;
}