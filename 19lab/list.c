#include "list.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

void mutexLockErrorCheck(pthread_mutex_t* mutex) {
    if (pthread_mutex_lock(mutex)) {
        perror("Error mutex locking!\n");
        exit(1);
    }
}

void mutexUnlockErrorCheck(pthread_mutex_t* mutex) {
    if (pthread_mutex_unlock(mutex)) {
        perror("Error mutex unlocking!\n");
        exit(1);
    }
}

node* createNode(char* str) {
    node* newNode = (node*)calloc(1, sizeof(node));
    char* newString = NULL;
    if (str != NULL) {
        newString = (char*)calloc(strlen(str), sizeof(char));
        strcpy(newString, str);
    }

    newNode->next = NULL;
    newNode->string = newString;
    if (pthread_mutex_init(&newNode->mutex, NULL)) {
        perror("Error mutex init!\n");
        exit(1);
    }
    return newNode;
}

node* createList() {
    return createNode(NULL);
}

void printList(node* list) {
    printf("---list---\n");
    if (list != NULL && list->next != NULL) {
        for (node* cur = list->next; cur != NULL; cur = cur->next) {
            mutexLockErrorCheck(&cur->mutex);
            printf("%s\n", cur->string);
            mutexUnlockErrorCheck(&cur->mutex);
        }
    }
    printf("---end of list---\n");
}

void addFirst(node* list, char* str) { 
    node* newNode = createNode(str);
    mutexLockErrorCheck(&list->mutex);
    newNode->next = list->next;
    list->next = newNode;
    mutexUnlockErrorCheck(&list->mutex);
}

void freeList(node* list) {
    if (list != NULL) {
        while (list->next != NULL) {
            node* tmp = list;
            list = list->next;
            free(tmp->string);
            pthread_mutex_destroy(&tmp->mutex);
            free(tmp);
        }
        pthread_mutex_destroy(&list->mutex);
        free(list);
    }
}

void sortList(node* list) {
    if (list == NULL || list->next == NULL) {
        return; 
    }

    int swapped = 1;
    int nowSwapped = 0;
    node* last = NULL;

    while (swapped) {
        swapped = 0;
        node* cur = list->next;
        mutexLockErrorCheck(&cur->mutex);
        for (; cur->next != last; cur = cur->next) {
            nowSwapped = 0;
            mutexLockErrorCheck(&cur->next->mutex);
            if (strcmp(cur->string, cur->next->string) > 0) {
                char* tmp = cur->string;
                cur->string = cur->next->string;
                cur->next->string = tmp;
                swapped = 1;
                nowSwapped = 1;
            }
            mutexUnlockErrorCheck(&cur->mutex);
            if (nowSwapped) {
                mutexUnlockErrorCheck(&cur->next->mutex);
                sleep(SLEEP_WHILE_SORTING_SECONDS);
                mutexLockErrorCheck(&cur->next->mutex);
            }
        }
        mutexUnlockErrorCheck(&cur->mutex);
        last = cur;
    }
}