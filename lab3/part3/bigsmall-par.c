#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include "barrier.h"
#include "config.h"

#define NUM_PROCESSES 8

int main()
{

    int vect[VECT_SIZE];
    int pid, largest_shmid, smallest_shmid;
    int *largest, *smallest;

    float per_process_raw = (float) VECT_SIZE / NUM_PROCESSES;
    int per_process = (int) per_process_raw;

    clock_t start, end;
    double time_taken;

    if (per_process_raw != (float) per_process)
    {
        printf("Vector size of %d is not divisible by %d processes.\n", VECT_SIZE, NUM_PROCESSES);
        exit(-1);
    }
    // construct shared memory regions for both arrays
    largest_shmid = shmget(IPC_PRIVATE, NUM_PROCESSES * sizeof(int), IPC_CREAT | 0600);
    smallest_shmid = shmget(IPC_PRIVATE, NUM_PROCESSES * sizeof(int), IPC_CREAT | 0600);

    //attach shared memory regions
    largest = (int *) shmat(largest_shmid, NULL, 0);
    smallest = (int *) shmat(smallest_shmid, NULL, 0);

    srand(24601);
    int i;

    for (i = 0; i < VECT_SIZE; i++)
    {
        vect[i] = rand();
    }

    // initialize the barrier
    init_barrier(NUM_PROCESSES + 1);

    for (i = 0; i < NUM_PROCESSES; i++)
    {
        pid = fork();

        if (pid == 0)
            break;
    }

    int j;
    int big = -INT_MAX;
    int small = INT_MAX;

    if (pid == 0)
    {
        int start = i * per_process;
        int end = i * per_process + per_process;

        for (j = start; j < end; j++)
        {
            if (vect[j] > big)
                big = vect[j];

            if (vect[j] < small)
                small = vect[j];
        }
        largest[i] = big;
        smallest[i] = small;
        // wait for all child processes to be done
        reach_barrier();
    }
    else
    {
        start = clock();
        // check if the child processes are done
        reach_barrier();
        for (j = 0; j < NUM_PROCESSES; j++)
        {
            if (largest[j] > big)
                big = largest[j];

            if (smallest[j] < small)
                small = smallest[j];
        }
        end = clock();
        time_taken = ((double)end - start) / CLOCKS_PER_SEC;

        printf("\nNumber of items: %d\n", VECT_SIZE);
        printf("Smallest element is %d\n", small);
        printf("Largest element is %d\n", big);
        printf("Time taken is %3.2fs\n\n", time_taken);

        // Clean up process table
        for (j = 0; j < NUM_PROCESSES; j++)
            wait(NULL);
        // detach and destroy shared memory regions
        destroy_barrier(pid);
        shmdt(largest);
        shmdt(smallest);
        shmctl(largest_shmid, IPC_RMID, 0);
        shmctl(smallest_shmid, IPC_RMID, 0);
    }
}
