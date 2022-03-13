#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/shm.h>
#include <sys/wait.h>

#define NUM_PROCESSES 5

int main() {

    int i, j, pid;
    int shmid;
    sem_t *sem_arr;

    shmid = shmget(IPC_PRIVATE, NUM_PROCESSES * sizeof(sem_t), IPC_CREAT | 0600);
    sem_arr = (sem_t *) shmat(shmid, NULL, 0);
    
    for(i=0; i<NUM_PROCESSES; i++)
    {
        if((pid = fork()) == 0) {
            sem_init(&sem_arr[i], 1, 0);
            break;
        }
    }

    sem_post(&sem_arr[0]);

    if(pid == 0) {
        sem_wait(&sem_arr[i]);

        printf("I am child %d\n", i);

        for(j = i*10; j<i*10 + 10; j++){
            printf("%d ", j);
            fflush(stdout);
            usleep(250000);
        }

        printf("\n\n");
        
        if (i + 1 < NUM_PROCESSES) {
            sem_post(&sem_arr[i+1]);
        }
    }
    else {
        for(i=0; i<NUM_PROCESSES; i++) 
            wait(NULL);

        shmdt(sem_arr);
        shmctl(shmid, IPC_RMID, 0);
    }
}

