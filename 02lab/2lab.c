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
    int errCode = 0;

    errCode = pthread_create(&threadId, NULL, threadWork, NULL);
    if (errCode) {
        fprintf(stderr, "error creating thread! error code: %d\n", errCode);
        return 1;
    }

    errCode = pthread_join(threadId, NULL);
    if (errCode) {
        fprintf(stderr, "error joining thread! error code: %d\n", errCode);
        return 1;        
    }
    fflush(stdout);

    printText("1st thread");

    return  0;
}


/*
01 - threads scheduling
02 - pthread_join 
04 - pthread_cancel 
*/