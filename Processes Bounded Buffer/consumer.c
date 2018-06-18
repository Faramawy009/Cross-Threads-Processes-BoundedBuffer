#include <bits/time.h>
#include <pthread.h>
#include <sys/time.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/shm.h>

//
// Created by elfar009 on 3/3/18.
//
#define BUFF_SIZE 2
#define OUTPUT_FILE "Consumer.txt"

struct shared_memory_sync
{
    char buffer[2][100];
    int itemAvailable;
    int spaceAvailable;
    int producingIndex;
    int consumingIndex;
    pthread_mutex_t bufferMutex;
    pthread_cond_t producersCondVar;
    pthread_cond_t consumerCondVar;
};




int main(int argc, char** argv) {

    int id;         /* shared memory identifier */
    struct shared_memory_sync *ptr;       /* pointer to shared memory */

    /* A key for the shared memory segment has been passed to this program
        as its first parameter. We use it to get the segment id of the
        segment that the parent process created. The size parameter is set
        to zero, and the flag to IPC_ALLOC, indicating that the segment is
        not being created here, it already exists
    */

    id = shmget (atoi(argv[1]), 0, 0);
    if (id == -1)
    {
        perror ("child shmget failed");
        exit (1);
    }
#ifdef DEBUG
    printf ("child Got shmem id = %d\n", id);
#endif

    /* Now attach this segment into the address space. Again, the 1023 is a
        flag consisting of all 1s, and the NULL pointer means we don't care
        where in the address space the segment is attached
    */
    ptr = shmat (id, (void *) NULL, 1023);
    if (ptr == (void *) -1)
    {
        perror ("child shmat failed");
        exit (2);
    }
#ifdef DEBUG
    printf ("child Got ptr = %p\n", ptr);
#endif


    FILE * fout;


    /* open file */
    fout = fopen(OUTPUT_FILE, "w");

    struct timeval t1;
    for(int i=0; i<3000; i++) {
        pthread_mutex_lock(&(ptr->bufferMutex));
        while(ptr->itemAvailable<1) {
            pthread_cond_wait(&(ptr->consumerCondVar), &(ptr->bufferMutex));
        }
        gettimeofday(&t1, NULL);
        fprintf(fout, "%s\n", ptr->buffer[ptr->consumingIndex]);
        ptr->itemAvailable--;
        ptr->spaceAvailable++;
        ptr->consumingIndex = (ptr->consumingIndex + 1) % BUFF_SIZE;
        pthread_mutex_unlock(&(ptr->bufferMutex));
        pthread_cond_signal(&(ptr->producersCondVar));
    }



    /* Done with the program, so detach the shared segment and terminate */

    shmdt ( (void *) ptr);
    return 0;
}
