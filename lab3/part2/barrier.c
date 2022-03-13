#include <stdio.h>
#include <semaphore.h>
#include <sys/shm.h>

int nproc, count, shmid;
sem_t *mutex, *barrier;

void init_barrier(int numproc) {
  nproc = numproc;
  count = 0;
  shmid = shmget(IPC_PRIVATE, 2 * sizeof(sem_t), IPC_CREAT | 0600);
  mutex = (sem_t *) shmat(shmid, NULL, 0);
  sem_init(mutex, 1, 1);
  barrier = (sem_t *) shmat(shmid, NULL, 0);
  sem_init(barrier, 1, 0);
}

void reach_barrier() {
  sem_wait(mutex);
  count++;
  sem_post(mutex);
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
      shmctl(shmid, IPC_RMID, 0);
        // Destroy the semaphores and detach
        // and free any shared memory. Notice
        // that we explicity check that it is
        // the parent doing it.
    }
}


