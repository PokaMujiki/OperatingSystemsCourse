#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <pthread.h>
#include <unistd.h>

#define SLEEP_DURATION_SECONDS 5
#define SHOW_LIST_WORD " "
#define STOP_WORD "STOP"

typedef struct ListNode {
    struct ListNode* next;
    char* string;
} node;

pthread_mutex_t mutex;
node* list;
int stop = 0;

void mutexLockErrorCheck() {
    if (pthread_mutex_lock(&mutex)) {
        perror("Error mutex locking!\n");
        exit(1);
    }
}

void mutexUnlockErrorCheck() {
    if (pthread_mutex_unlock(&mutex)) {
        perror("Error mutex unlocking!\n");
        exit(1);
    }
}

void printList() {
    node* element = list;
    printf("---list---\n");
    while (element != NULL) {
            printf("%s\n", element->string);
            element = element->next;
    }
    printf("---end of list---\n");
}

node *createNode(char* str) {
    node* newNode = (node*)calloc(1, sizeof(node));
    newNode->next = NULL;
    char* newString = (char*) calloc(strlen(str), sizeof(char));
    strcpy(newString, str);
    newNode->string = newString;
    return newNode;
}

void addFirst(node* element) {
    element->next = list;
    list = element;
}

void freeList() {
    node* head = list;
    if (head != NULL) {
        while (head->next != NULL) {
            node* tmp = head;
            head = head->next;
            free(tmp->string);
            free(tmp);
        }
        free(head);
    }
}

void sortList() {
    if (list == NULL) {
        return;
    }

    for (node* i = list; i->next != NULL; i = i->next) {
        for (node* j = i->next; j != NULL; j = j->next) {
            if (strcmp(i->string, j->string) > 0) {
                char* tmp = i->string;
                i->string = j->string;
                j->string = tmp;
            }
        }
    }
}

void* threadWork(void* arg) {
    while (!stop) {
        sleep(SLEEP_DURATION_SECONDS);
        mutexLockErrorCheck();
        sortList();
        mutexUnlockErrorCheck();
    }
}

int main() {
    pthread_t threadId;

    pthread_mutexattr_t attr;
    if (pthread_mutexattr_init(&attr)) {
        perror("Error mutex attribute init!\n");
        return 1;
    }

    if (pthread_mutex_init(&mutex, &attr)) {
        perror("Error mutex init!\n");
        return 1;
    }

    if (pthread_create(&threadId, NULL, threadWork, NULL)) {
        perror("Error creating thread!\n");
        return 1;
    }

    char str[ARG_MAX + 1] = {0};
    while (fgets(str, ARG_MAX + 1, stdin) != NULL) {
        str[strlen(str) - 1] = '\0';

        if (strcmp(SHOW_LIST_WORD, str) == 0) {
            mutexLockErrorCheck();
            printList();
            mutexUnlockErrorCheck();
        }
        else if (strcmp(STOP_WORD, str) != 0) {
            node* newElement = createNode(str);
            mutexLockErrorCheck();
            addFirst(newElement);
            mutexUnlockErrorCheck();
        }
        else {
            stop = 1;
            break;
        }
    }

    if (pthread_join(threadId, NULL)) {
        perror("Error thread join!\n");
        return 1;
    }

    if (pthread_mutex_destroy(&mutex)) {
        perror("Error mutex destroy!\n");
        return 1;
    }

    freeList();

    return 0;
}