#include <pthread.h>

typedef struct ListNode {
    struct ListNode* next;
    char* string;
    pthread_mutex_t mutex;
} node;

node* createList();
void printList(node* list);
void addFirst(node* list, char* str);
void freeList(node* list);
void sortList(node* list);