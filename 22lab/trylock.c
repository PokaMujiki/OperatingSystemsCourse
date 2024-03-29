#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

#define PHILO 5
#define MAX_EAT_SLEEP 30000
#define MIN_EAT_SLEEP 3000
#define MAX_THINK_SLEEP 10000
#define MIN_THINK_SLEEP 0
#define FOOD_START 50
#define MIN_FAIL_GET_FORK_SLEEP 1000
#define MAX_FAIL_GET_FORK_SLEEP 2000

int eaten[PHILO] = {0};
pthread_mutex_t forks[PHILO];
pthread_t phils[PHILO];
pthread_mutex_t foodLock;

void mutexInitErrorCheck(pthread_mutex_t* m, const char* errorMessage) {
    if (pthread_mutex_init(m, NULL)) {
        perror(errorMessage);
        exit(1);
    }
}

void mutexLockErrorCheck(pthread_mutex_t* m, const char* errorMessage) {
    if (pthread_mutex_lock(m)) {
        perror(errorMessage);
        exit(1);
    }
}

void mutexUnlockErrorCheck(pthread_mutex_t* m, const char* errorMessage) {
    if (pthread_mutex_unlock(m)) {
        perror(errorMessage);
        exit(1);
    }
}

int foodOnTable () {
    static int food = FOOD_START + 1;
    int myFood;

    mutexLockErrorCheck(&foodLock, "foodLock mutex lock");
    if (food > 0) {
        food--;
    }
    myFood = food;
    mutexUnlockErrorCheck(&foodLock, "foodLock mutex unlock");
    return myFood;
}

int tryGetForks(int philoId, int firstFork, int secondFork) {
    int statusFirst = EBUSY;
    int statusSecond = 0;

    while (statusFirst == EBUSY) {
        if ((statusFirst = pthread_mutex_trylock(&forks[firstFork])) == EBUSY) {
            usleep(MIN_FAIL_GET_FORK_SLEEP + rand() % (MAX_FAIL_GET_FORK_SLEEP - MIN_FAIL_GET_FORK_SLEEP + 1));
        }
        else if (statusFirst != 0) {
            perror("one of forks mutex try lock");
            exit(1);
        }
    }

    if ((statusSecond = pthread_mutex_trylock(&forks[secondFork])) == 0) {
        printf("Philosopher %d got forks %d and %d\n", philoId, firstFork, secondFork);
        return 1;        
    }
    else if (statusSecond == EBUSY) {
        mutexUnlockErrorCheck(&forks[firstFork], "one of forks mutex unlock"); 
        usleep(MIN_FAIL_GET_FORK_SLEEP + rand() % (MAX_FAIL_GET_FORK_SLEEP - MIN_FAIL_GET_FORK_SLEEP + 1));
    }
    else {
        perror("one of forks mutex try lock");
        exit(1); 
    }

    return 0;
} 

void getForks(int philoId, int firstFork, int secondFork) {
    int gotForks = 0;
    while (!gotForks) {
        gotForks = tryGetForks(philoId, firstFork, secondFork);
    }
}

void downForks(int firstFork, int secondFork) {
    mutexUnlockErrorCheck(&forks[firstFork], "one of forks mutex unlock");
    mutexUnlockErrorCheck(&forks[secondFork], "one of forks mutex unlock");
}

void* philosopher(void *num) {
    int f;
    int id = (int)num;
    int firstFork = id % PHILO;
    int secondFork = (id + 1) % PHILO;

    printf("Philosopher %d sitting down to dinner.\n", id);
    while((f = foodOnTable())) {
        printf("Philosopher %d get dish %d.\n", id, f);
        getForks(id, firstFork, secondFork);

        printf("Philosopher %d eating.\n", id);
        usleep(MIN_EAT_SLEEP + (rand() % (MAX_EAT_SLEEP - MIN_EAT_SLEEP + 1)));
        printf("Philosopher %d done with %d dish.\n", id, f);
        eaten[id]++;

        downForks(firstFork, secondFork);
        usleep(MIN_THINK_SLEEP + (rand() % (MAX_THINK_SLEEP - MIN_THINK_SLEEP + 1)));
    }
    printf("Philosopher %d is done eating.\n", id);
}

int main () {
    srand(time(NULL));
    mutexInitErrorCheck(&foodLock, "food lock mutex init");
    for (int i = 0; i < PHILO; i++) {
        mutexInitErrorCheck(&forks[i], "one of forks mutex init");
    }
    for (int i = 0; i < PHILO; i++) {
        if (pthread_create(&phils[i], NULL, philosopher, (void *)i)) {
            perror("creating thread");
            exit(1);
        }
    }
    for (int i = 0; i < PHILO; i++) {
        pthread_join(phils[i], NULL);
    }

    for (int i = 0; i < PHILO; i++) {
        printf("Philosopher %d ate %d or %.2lf%% of all food\n",
            i, eaten[i], (double)eaten[i] / FOOD_START * 100);
    }

    return 0;
}