#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

void* threadWork(void* arg) {
	srand(time(NULL));
	const char symbols[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789/,.-+=~`<>:";
    const int border = sizeof(symbols) - 1;

    for (int i = 0;;i++) {
		fprintf(stdout, "%c", symbols[rand() % border]);
	}
}

int main () {
    pthread_t threadId;
    int errCode = 0;

    errCode = pthread_create(&threadId, NULL, threadWork, NULL);
    if (errCode) {
        fprintf(stderr, "error creating thread! error code: %d\n", errCode);
        return 1;
    }

    sleep(2);

    errCode = pthread_cancel(threadId);
    if (errCode) {
        fprintf(stderr, "error canceling thread!\n");
        return 1;
    }

    fflush(stdout);
    fprintf(stdout, "\n\ndaughter thread canceling request sent!\n");

    return  0;
}