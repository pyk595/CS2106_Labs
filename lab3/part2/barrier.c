#include <stdio.h>
#include <semaphore.h>
#include <sys/shm.h>

int nproc, count_shmid, mutex_shmid, barrier_shmid;
int *count;
sem_t *mutex, *barrier;

void init_barrier(int numproc) {
  nproc = numproc;

  // Get required memory to this process
  count_shmid = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0600);;
  mutex_shmid = shmget(IPC_PRIVATE, sizeof(sem_t), IPC_CREAT | 0600);
  barrier_shmid = shmget(IPC_PRIVATE, sizeof(sem_t), IPC_CREAT | 0600);

  // attach shared memory count to keep track of count
  count = (int *) shmat(count_shmid, NULL, 0);
  *count = 0;

  // attach shared memory for semaphore initialisation
  mutex = (sem_t *) shmat(mutex_shmid, NULL, 0);
  barrier = (sem_t *) shmat(barrier_shmid, NULL, 0);
  
  // semaphores for mutex for count++ and barrier
  sem_init(mutex, 1, 1);
  sem_init(barrier, 1, 0);
}

void reach_barrier() {
  // prevent race condition on count
  sem_wait(mutex);
  (*count)++; // ++ takes precedence, therefore the bracket
  sem_post(mutex);

  if (*count == nproc) {
    // Release one process
    sem_post(barrier);
  }
  else {
    // We are not the last process. So we wait at the
    // semaphore until we are freed.
    sem_wait(barrier);

    // Now that we are freed, we free the next process
    sem_post(barrier);
  }
}

void destroy_barrier(int my_pid) {
  if(my_pid != 0) {
    // Destroy the semaphores and detach
    // and free any shared memory. Notice
    // that we explicity check that it is
    // the parent doing it.

    // destroy semaphores
    sem_destroy(barrier);
    sem_destroy(mutex);

    // detach and destroy shared memory
    shmdt(mutex);
    shmdt(barrier);
    shmdt(count);
    shmctl(count_shmid, IPC_RMID, 0);
    shmctl(mutex_shmid, IPC_RMID, 0);
    shmctl(barrier_shmid, IPC_RMID, 0);
  }
}

