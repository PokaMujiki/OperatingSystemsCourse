#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <pthread.h>
#include <unistd.h>
#include "list.h"

#define SLEEP_BETWEEN_SORT_SECONDS 5
#define SORTING_THREADS_AMOUNT 1
#define SHOW_LIST_WORD " "
#define STOP_WORD "STOP"

node* list;
int stop = 0;

void* threadWork(void* arg) {
    while (!stop) {
        sleep(SLEEP_BETWEEN_SORT_SECONDS);
        sortList(list);
    }
    return NULL;
}

int main() {
    pthread_t threadIds[SORTING_THREADS_AMOUNT] = {0};
    list = createList();

    for (int i = 0; i < SORTING_THREADS_AMOUNT; i++) {
        if (pthread_create(&threadIds[i], NULL, threadWork, NULL)) {
            perror("Error creating thread!\n");
            return 1;
        }
    }

    char str[ARG_MAX + 1] = {0};
    while (fgets(str, ARG_MAX + 1, stdin) != NULL) {
        str[strlen(str) - 1] = '\0';
        if (strcmp(SHOW_LIST_WORD, str) == 0) {
            printList(list);
        }
        else if (strcmp(STOP_WORD, str) != 0) {
            addFirst(list, str);
        }
        else {
            stop = 1;
            break;
        }
    }

    for (int i = 0; i < SORTING_THREADS_AMOUNT; i++) {
        if (pthread_join(threadIds[i], NULL)) {
            perror("Error thread join!\n");
            return 1;
        }
    }

    freeList(list);

    return 0;
}