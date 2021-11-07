#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

void printThreadCancelMessage(void* message) {
    fprintf(stdout, "\n%s\n", (const char*)message);
    fflush(stdout);
}

void* threadWork(void* arg) {
    pthread_cleanup_push(printThreadCancelMessage, (void*)"\ngoodbye");
	srand(time(NULL));
	const char symbols[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789/,.-+=~`<>:";
    const int border = sizeof(symbols) - 1;

    while (1) {
		fprintf(stdout, "%c", symbols[rand() % border]);
	}

    pthread_cleanup_pop(0);
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

    pthread_exit(NULL);
    // как внутри устроены pthread_cleanup_push и pthread_cleanup_pop
    
    return  0;
}