#include <stdio.h>
#include <semaphore.h>
#include <sys/shm.h>

int nproc, count, mutex_shmid, barrier_shmid;
sem_t *mutex, *barrier;

void init_barrier(int numproc) {
  nproc = numproc;
  count = 0;
  mutex_shmid = shmget(IPC_PRIVATE, sizeof(sem_t), IPC_CREAT | 0600);
  barrier_shmid = shmget(IPC_PRIVATE, sizeof(sem_t), IPC_CREAT | 0600);
  
  mutex = (sem_t *) shmat(mutex_shmid, NULL, 0);
  sem_init(mutex, 1, 1);
  barrier = (sem_t *) shmat(barrier_shmid, NULL, 0);
  sem_init(barrier, 1, 0);
}

void reach_barrier() {
  sem_wait(mutex);
  printf("inside2");
  count++;
  printf("inside");
  sem_post(mutex);
  printf("test");
  if (count == nproc) {
    sem_post(barrier);
  }
  else {
    sem_wait(barrier);
    sem_post(barrier);
  }
}

void destroy_barrier(int my_pid) {
    if(my_pid != 0) {
      sem_destroy(barrier);
      sem_destroy(mutex);
      shmdt(mutex);
      shmdt(barrier);
      shmctl(mutex_shmid, IPC_RMID, 0);
      shmctl(barrier_shmid, IPC_RMID, 0);
        // Destroy the semaphores and detach
        // and free any shared memory. Notice
        // that we explicity check that it is
        // the parent doing it.
    }
}

