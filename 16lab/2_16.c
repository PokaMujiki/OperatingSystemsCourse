#include <stdio.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/stat.h>

#define LINES_AMOUNT 10
#define FIRST_SEM_NAME "/sem1_myname"
#define SECOND_SEM_NAME "/sem2_myname"

int main() {
    sem_t *firstSem = sem_open(FIRST_SEM_NAME, O_CREAT, 0660, 0);
    sem_t *secondSem = sem_open(SECOND_SEM_NAME, O_CREAT, 0660, 1);
    if (firstSem == SEM_FAILED || secondSem == SEM_FAILED) {
        perror("[2nd proc]sem_open error");
        return 1;
    }

    for (int i = 0; i < LINES_AMOUNT; i++) {
        sem_wait(firstSem);
        fprintf(stdout, "2nd proc message no. %d\n", i + 1);
        sem_post(secondSem);
    }

    sem_close(firstSem);
    sem_close(secondSem);

    sem_unlink(FIRST_SEM_NAME);
    sem_unlink(SECOND_SEM_NAME);

    return 0;
}