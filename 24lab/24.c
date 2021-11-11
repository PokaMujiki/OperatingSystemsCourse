#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>

#define DETAIL_A_PRODUCTION_TIME_SECONDS 1
#define DETAIL_B_PRODUCTION_TIME_SECONDS 2
#define DETAIL_C_PRODUCTION_TIME_SECONDS 3

sem_t detailA, detailB, detailC, module;

_Noreturn void* produceA(void* arg) {
    while(1) {
        sleep(DETAIL_A_PRODUCTION_TIME_SECONDS);
        sem_post(&detailA);
        int available;
        sem_getvalue(&detailA, &available);
        printf("Detail A created! Available: %d\n", available);
    }
}

_Noreturn void* produceB(void* arg) {
    while(1) {
        sleep(DETAIL_B_PRODUCTION_TIME_SECONDS);
        sem_post(&detailB);
        int available;
        sem_getvalue(&detailB, &available);
        printf("Detail B created! Available: %d\n", available);
    }
}

_Noreturn void* produceC(void* arg) {
    while(1) {
        sleep(DETAIL_C_PRODUCTION_TIME_SECONDS);
        sem_post(&detailC);
        int available;
        sem_getvalue(&detailC, &available);
        printf("Detail C created! Available: %d\n", available);
    }
}

_Noreturn void* produceModule(void* arg) {
    while(1) {
        sem_wait(&detailA);
        sem_wait(&detailB);
        sem_post(&module);
        int available;
        sem_getvalue(&module, &available);
        printf("Module created! Available: %d\n", available);
    }
}

_Noreturn void* produceWidget() {
    long long available = 0;
    while(1) {
        sem_wait(&module);
        sem_wait(&detailC);
        available++;
        printf("Widget created! Available: %lld\n", available);
    }
}

int main(){
    pthread_t threadIds[4];
    sem_t* s[4] = {&detailA, &detailB, &detailC, &module};
    void* threadWorks[4] = {produceA, produceB, produceC, produceModule};

    for (int i = 0; i < 4; i++) {
        if (sem_init(s[i], 0, 0)) {
            perror("Error init semaphore\n");
            return 1;
        }
    }

    for (int i = 0; i < 4; i++) {
        if (pthread_create(&(threadIds[i]), NULL, threadWorks[i],NULL)) {
            perror("Error creating thread!\n");
            return 1;
        }
    }

    produceWidget();
}