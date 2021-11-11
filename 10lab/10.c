#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

#define PHILO 5
#define DELAY 30000
#define FOOD 50

pthread_mutex_t forks[PHILO];
pthread_t phils[PHILO];
pthread_mutex_t foodlock;
int eaten[PHILO] = {0};

int food_on_table () {
    static int food = FOOD + 1;
    int myfood;

    pthread_mutex_lock(&foodlock);
    if (food > 0) {
        food--;
    }
    myfood = food;
    pthread_mutex_unlock(&foodlock);
    return myfood;
}

void down_forks (int f1, int f2) {
    pthread_mutex_unlock(&forks[f1]);
    pthread_mutex_unlock(&forks[f2]);
}

void get_forks(int id, int first_fork, int second_fork) {
    /*if (pthread_mutex_trylock(&forks[first_fork]) == EBUSY) {
        printf("Philosopher %d failed to take fork %d.\n", id, first_fork);
        return 0;
    }
    printf("Philosopher %d took fork %d.\n", id, first_fork);
    if (pthread_mutex_trylock(&forks[second_fork]) == EBUSY) {
        printf("Philosopher %d failed to take fork %d.\n", id, second_fork);
        pthread_mutex_unlock(&forks[first_fork]);
        return 0;
    }
    printf("Philosopher %d took fork %d.\n", id, second_fork);
    return 1;*/
    pthread_mutex_lock(&forks[first_fork]);
    printf("Philosopher %d took fork %d.\n", id, first_fork);
    pthread_mutex_lock(&forks[second_fork]);
    printf("Philosopher %d took fork %d.\n", id, second_fork);
}

void* philosopher(void *num) {
    int f;
    int id = (int)num;
    int first_fork = id % PHILO;
    int second_fork = (id + 1) % PHILO;
    if (second_fork < first_fork) {
        int tmp = first_fork;
        first_fork = second_fork;
        second_fork = tmp;
    }

    printf("Philosopher %d sitting down to dinner.\n", id);
    while(f = food_on_table()) {
        printf("Philosopher %d: get dish %d.\n", id, f);
        get_forks(id, first_fork, second_fork);
        printf("Philosopher %d: eating.\n", id);
        usleep(DELAY * (FOOD - f + 1));
        eaten[id]++;
        down_forks(first_fork, second_fork);
    }

    printf ("Philosopher %d is done eating.\n", id);
    return NULL;
}

int main () {
    pthread_mutex_init (&foodlock, NULL);
    for (int i = 0; i < PHILO; i++) {
        pthread_mutex_init (&forks[i], NULL);
    }
    for (int i = 0; i < PHILO; i++) {
        pthread_create (&phils[i], NULL, philosopher, (void *)i);
    }
    for (int i = 0; i < PHILO; i++) {
        pthread_join (phils[i], NULL);
    }

    FILE* result = fopen("result.txt", "w");

    for (int i = 0; i < PHILO; i++) {
        fprintf(result, "Philosopher %d ate %.2f%% of food, %d dishes from %d\n", i, (double)eaten[i] / FOOD * 100, eaten[i], FOOD);
    }
    return 0;
}