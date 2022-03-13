#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/shm.h>
#include <sys/wait.h>

#define NUM_PROCESSES 5

int main() {

    int i, j, pid, shmid;
    sem_t *semArr[NUM_PROCESSES];

    shmid = shmget(IPC_PRIVATE, NUM_PROCESSES * sizeof(sem_t), IPC_CREAT | 0600);

    for (int k = 0; k < NUM_PROCESSES; k++) {
      semArr[k] = (sem_t *) shmat(shmid, NULL, 0);
      sem_init(semArr[k], 1, 0);
    }
    
    sem_post(semArr[0]);
    for(i=0; i<NUM_PROCESSES; i++)
    {
        if((pid = fork()) == 0) {
            break;
        }
    }

    if(pid == 0) {
      sem_wait(semArr[i]);
        printf("I am child %d\n", i);

        for(j = i*10; j<i*10 + 10; j++){
            printf("%d ", j);
            fflush(stdout);
            usleep(250000);
        }

        printf("\n\n");
        sem_post(semArr[i+1]);
    }
    else {
        for(i=0; i<NUM_PROCESSES; i++) {
          wait(NULL);
          sem_destroy(semArr[i]);
        }
        shmctl(shmid, IPC_RMID, 0);
    }

}

