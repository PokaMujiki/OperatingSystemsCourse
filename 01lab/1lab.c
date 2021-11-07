#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void printText(const char* threadName) {
    fprintf(stdout, "1st  line of text from %s\n"
                    "2nd  line of text from %s\n"
                    "3rd  line of text from %s\n"
                    "4th  line of text from %s\n"
                    "5th  line of text from %s\n"
                    "6th  line of text from %s\n"
                    "7th  line of text from %s\n"
                    "8th  line of text from %s\n"
                    "9th  line of text from %s\n"
                    "10th line of text from %s\n", 
                    threadName, threadName, threadName, threadName, threadName,
                    threadName, threadName, threadName, threadName, threadName);
    fflush(stdout);
}

void* threadWork(void* arg) {
    printText("2nd thread");
}

int main() {
    pthread_t threadId;

    if (pthread_create(&threadId, NULL, threadWork, NULL)) {
        fprintf(stderr, "error creating thread!\n");
        return 1;
    }

    printText("1st thread");

    return 0;
}