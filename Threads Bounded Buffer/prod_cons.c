#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/time.h>

#define BUFF_SIZE 2
#define OUTPUT_FILE "Consumer.txt"

char ** buffer;
int itemAvailable = 0;
int spaceAvailable = BUFF_SIZE;
int producingIndex = 0;
int consumingIndex = 0;
pthread_mutex_t bufferMutex =  PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t producersCondVar = PTHREAD_COND_INITIALIZER;
pthread_cond_t consumerCondVar = PTHREAD_COND_INITIALIZER;



void* producer_red(void* arg) {
    FILE * fout;

    /* open file */
    char fileName[20];
    sprintf (fileName, "%s_%s.%s","Producer", "RED", "txt");
    fout = fopen(fileName, "w");

    struct timeval t1;
    for(int i=0; i<1000; i++) {
        pthread_mutex_lock(&bufferMutex);
        while(spaceAvailable<1) {
            pthread_cond_wait(&producersCondVar, &bufferMutex);
        }
        gettimeofday(&t1, NULL);
        sprintf(buffer[producingIndex], "RED <%f>", (double)t1.tv_sec*1000000+t1.tv_usec);
        fprintf(fout, "%s\n", buffer[producingIndex]);
        producingIndex = (producingIndex + 1) % BUFF_SIZE;
        itemAvailable++;
        spaceAvailable--;
        pthread_mutex_unlock(&bufferMutex);
        pthread_cond_signal(&consumerCondVar);
    }
}

void* producer_white(void* arg) {
    FILE * fout;

    /* open file */
    char fileName[20];
    sprintf (fileName, "%s_%s.%s","Producer", "WHITE", "txt");
    fout = fopen(fileName, "w");

    struct timeval t1;
    for(int i=0; i<1000; i++) {
        pthread_mutex_lock(&bufferMutex);
        while(spaceAvailable<1) {
            pthread_cond_wait(&producersCondVar, &bufferMutex);
        }
        gettimeofday(&t1, NULL);
        sprintf(buffer[producingIndex], "WHITE <%f>", (double)t1.tv_sec*1000000+t1.tv_usec);
        fprintf(fout, "%s\n", buffer[producingIndex]);
        producingIndex = (producingIndex + 1) % BUFF_SIZE;
        itemAvailable++;
        spaceAvailable--;
        pthread_mutex_unlock(&bufferMutex);
        pthread_cond_signal(&consumerCondVar);
    }
}

void* producer_black(void* arg) {
    FILE * fout;

    /* open file */
    char fileName[20];
    sprintf (fileName, "%s_%s.%s","Producer", "BLACK", "txt");
    fout = fopen(fileName, "w");

    struct timeval t1;
    for(int i=0; i<1000; i++) {
        pthread_mutex_lock(&bufferMutex);
        while(spaceAvailable<1) {
            pthread_cond_wait(&producersCondVar, &bufferMutex);
        }
        gettimeofday(&t1, NULL);
        sprintf(buffer[producingIndex], "BLACK <%f>", (double)t1.tv_sec*1000000+t1.tv_usec);
        fprintf(fout, "%s\n", buffer[producingIndex]);
        producingIndex = (producingIndex + 1) % BUFF_SIZE;
        itemAvailable++;
        spaceAvailable--;
        pthread_mutex_unlock(&bufferMutex);
        pthread_cond_signal(&consumerCondVar);
    }
}

void* consumer(void* arg) {
    FILE * fout;

    /* open file */
    fout = fopen(OUTPUT_FILE, "w");

    struct timeval t1;
    for(int i=0; i<3000; i++) {
        pthread_mutex_lock(&bufferMutex);
        while(itemAvailable<1) {
            pthread_cond_wait(&consumerCondVar, &bufferMutex);
        }
        gettimeofday(&t1, NULL);
        fprintf(fout, "%s\n", buffer[consumingIndex]);
        itemAvailable--;
        spaceAvailable++;
        consumingIndex = (consumingIndex + 1) % BUFF_SIZE;
        pthread_mutex_unlock(&bufferMutex);
        pthread_cond_signal(&producersCondVar);
    }
}


int main() {
    buffer = (char**) malloc(sizeof(char**)*BUFF_SIZE);
    buffer[0] = (char *) malloc(sizeof(char)* 50);
    buffer[1] = (char *) malloc(sizeof(char)* 50);
    pthread_t thread_tid[4];
    pthread_create(&thread_tid[0],NULL,producer_red,NULL);
    pthread_create(&thread_tid[1],NULL,producer_white,NULL);
    pthread_create(&thread_tid[2],NULL,producer_black,NULL);
    pthread_create(&thread_tid[3],NULL,consumer,NULL);
    for(int i=0; i<4; i++) {
        pthread_join(thread_tid[i],NULL);
    }

    return 0;
}
