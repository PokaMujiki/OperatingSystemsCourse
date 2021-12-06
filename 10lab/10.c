#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

#define PHILO 5
#define DELAY 30000
#define FOOD_START 1000000
#define MAX_EAT_SLEEP 30000
#define MIN_EAT_SLEEP 3000
#define MAX_THINKING_SLEEP 10000
#define MIN_THINKING_SLEEP 0
#define FOOD_START 1000000

int eaten[PHILO] = {0};
pthread_mutex_t forks[PHILO];
pthread_t phils[PHILO];
pthread_mutex_t foodlock;


int food_on_table () {
    static int food = FOOD_START + 1;
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
        usleep(MIN_EAT_SLEEP + (rand() % (MAX_EAT_SLEEP - MIN_EAT_SLEEP + 1)));
        eaten[id]++;

        down_fork(second_fork);
        down_fork(first_fork);
        usleep(MIN_THINKING_SLEEP + (rand() % (MAX_THINKING_SLEEP - MIN_THINKING_SLEEP + 1)));
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

    for (int i = 0; i < PHILO; i++) {
        printf("Philosopher %d ate %d or %.2lf%% of all food\n",
            i, eaten[i], (double)eaten[i] / FOOD_START * 100);
    }

    return 0;
}