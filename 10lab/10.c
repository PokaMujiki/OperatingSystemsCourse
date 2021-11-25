#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define PHILO 5
#define DELAY 30000
#define FOOD 50

pthread_mutex_t forks[PHILO];
pthread_t phils[PHILO];
pthread_mutex_t foodlock;

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

void down_fork(int fork) {
    pthread_mutex_unlock(&forks[fork]);
}

void get_fork(int id, int fork) {
    pthread_mutex_lock(&forks[fork]);
    printf("Philosopher %d took fork %d.\n", id, fork);
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
    while((f = food_on_table())) {
        printf("Philosopher %d: get dish %d.\n", id, f);
        get_fork(id, first_fork);
        get_fork(id, second_fork);

        printf("Philosopher %d: eating.\n", id);
        usleep(DELAY * (FOOD - f + 1));

        down_fork(second_fork);
        down_fork(first_fork);
    }

    printf("Philosopher %d is done eating.\n", id);
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
    return 0;
}