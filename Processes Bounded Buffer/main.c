#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <memory.h>

#define BUFF_SIZE 2

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



int main()
{
    int shmem_id;       /* shared memory identifier */
    struct shared_memory_sync *shmem_ptr;     /* pointer to shared segment */

    key_t key = 3355;   /* A key to access shared memory segments.
                           Some arbitrary integer, which will also be
                            passed to the other processes which need to
                            share memory */


    size_t size = sizeof(struct shared_memory_sync);           /* Memory size needed, in bytes */
    int flag = 1023;           /* Controls r/w permissions. 1023 = 0b11111111, all permissions */


    /* First, create a shared memory segment */
    shmem_id = shmget (key, size, flag);
    if (shmem_id == -1)
    {
        perror ("shmget failed");
        exit (1);
    }
#ifdef DEBUG
    printf ("Got shmem id = %d\n", shmem_id);
#endif

    /* Now attach the new segment into my address space.
        This will give me a (void *) pointer to the shared memory area.
        The NULL pointer indicates that we don't care where in the address
        space the new segment is attached. The return value gives us that
        location anyway.
    */

    shmem_ptr = shmat (shmem_id, (void *) NULL, 1023);
    if (shmem_ptr == (void *) -1)
    {
        perror ("shmat failed");
        exit (2);
    }
#ifdef DEBUG
    printf ("Got ptr = %p\n", shmem_ptr);
#endif



    pthread_mutexattr_t mutexAttr;
    pthread_mutexattr_init ( &mutexAttr );
    pthread_mutexattr_setpshared ( &mutexAttr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init( &(shmem_ptr->bufferMutex), &mutexAttr );

    pthread_condattr_t condAttr;
    pthread_condattr_init ( &condAttr );
    pthread_condattr_setpshared ( &condAttr, PTHREAD_PROCESS_SHARED);
    pthread_cond_init( &(shmem_ptr->producersCondVar), &condAttr );
    pthread_cond_init( &(shmem_ptr->consumerCondVar), &condAttr );

    shmem_ptr->itemAvailable = 0;
    shmem_ptr->spaceAvailable = BUFF_SIZE;
    shmem_ptr->producingIndex = 0;
    shmem_ptr->consumingIndex = 0;

    char keystr[10];
    sprintf (keystr, "%d", key);

    pid_t pids[4];
    pids[0] = fork ();
    if(pids[0]==0) {
        execl ("./producer", "producer", keystr,"WHITE", NULL);
    }



    pids[1] = fork ();
    if(pids[1]==0)
        execl ("./producer", "producer", keystr,"RED", NULL);

    pids[2] = fork ();
    if(pids[2]==0)
        execl ("./producer", "producer",keystr,"BLACK", NULL);

    pids[3] = fork ();
    if(pids[3]==0)
        execl ("./consumer", "consumer", keystr, NULL);


    for(int i=0; i<4; i++) {
        waitpid(pids[i],NULL,0);
    }

    shmdt ( (void *)  shmem_ptr);
    shmctl (shmem_id, IPC_RMID, NULL);

    return 0;
}
