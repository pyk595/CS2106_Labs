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
    int *a;
    int *b;

    shmid = shmget(IPC_PRIVATE, 2 * sizeof(int), IPC_CREAT | 0600);
    a = (int *) shmat(shmid, NULL, 0);
    b = (int *) shmat(shmid, NULL, 0);

    *a = 11234;
    printf("b value: %d", *b);
    printf("a[1] addr: %p\n", &a[1]);
    printf("a addr: %p\n", a);

    shmdt(a);
    shmdt(b);
    shmctl(shmid, IPC_RMID, 0);
}

