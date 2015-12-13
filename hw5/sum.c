#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <semaphore.h>
#include <errno.h>
#include <time.h>
#include <string.h>

#define n 3
#define m 3

void input(int* m1, int* m2) {
  printf("Input matrix #1\n");
  int i, j;
  for (i = 0; i < n; i++)
    for (j = 0; j < m; j++)
      //scanf("%d", &m1[i*m + j]);
      m1[i*m + j] = 1;
    
  printf("Input matrix #2\n");
  for (i = 0; i < n; i++)
    for (j = 0; j < m; j++)
      //scanf("%d", &m2[i*m + j]);
      m2[i*m + j] = 1;
  return;
}

void sum(int* m1, int* m2, int* res) {
  int i, j;
  for (i = 0; i < n; i++) 
    for (j = 0; j < m; j++) 
      res[i*m + j] = m1[i*m + j] + m2[i*m + j];
  return;
}

void output(int* matrix){
  int i, j;
  printf("Sum result is: \n");
  for (i = 0; i < n; i++) {
    for (j = 0; j < m; j++)
      printf("%d ", matrix[i*m + j]);
    printf("\n");
  }
  printf("\n");
  return;
}

int* reserve_memory(char pathname[], int size, int num) {
  int shmid, key;
  int * arr;
  if ((key = ftok(pathname, num)) < 0){
    perror("Can't generate key\n");
    exit(-1);
  }
  if ((shmid = shmget(key, size*sizeof(int), 0666|IPC_CREAT|IPC_EXCL)) < 0) {
    if (errno != EEXIST){
      perror("Can't create shared memory\n");
      exit(-1);
    } 
    else {   
      if ((shmid = shmget(key, size*sizeof(int), 0)) < 0){
	perror("Can't find shared memory\n");
	exit(-1);
      }
    }
  }
  if ((arr = (int *)shmat(shmid, NULL, 0)) == (int *)(-1)){
    perror("Can't attach shared memory\n");
    exit(-1);
  }
  return arr;
}

void free_memory(int * arr) {
  if (shmdt(arr) < 0){ 
    perror("Can't detach shared memory\n");
    exit(-1);
  }
  return;
}

int main() {
  char pathname[] = "sum.c";
  int key, semid;
  //shared memory
  int * m1 = reserve_memory(pathname, n*m, 0);
  int * m2 = reserve_memory(pathname, n*m, 1);
  int * result = reserve_memory(pathname, n*m, 2);
  int * new_matrices = reserve_memory(pathname, 1, 3);
  int * new_result = reserve_memory(pathname, 1, 4);
  
  //mutexes
  sem_t * mutex1 = (sem_t*) reserve_memory(pathname, 1, 5);
  sem_t * mutex2 = (sem_t*) reserve_memory(pathname, 1, 6);
  sem_init(mutex1, 1, 1); //create shared semaphore
  sem_init(mutex2, 1, 1); //create shared semaphore
  *new_matrices = 0;
  *new_result = 0;
  
  int pid = fork();
  if (pid < 0) {
    perror("Can't fork\n");
    exit(-1);
  }
  else if (pid == 0) {
    while (1) {
      sem_wait(mutex1); //waiting for matrices  //!blocking! while < 1
      if (!*new_result && !*new_matrices) {
	input(m1, m2);
	*new_matrices = 1;
      }
      sem_post(mutex1); //opening matrices
    }
  }
  else {
    pid = fork();
    if (pid < 0) {
      perror("Can't fork\n");
      exit(-1);
    }
    else if (pid == 0) {
      while (1) {
	sem_wait(mutex2); //waiting for result
        sem_wait(mutex1); //waiting for matrices
	if (*new_matrices && !*new_result) {
          sum(m1, m2, result);
	  *new_matrices = 0;
	  *new_result = 1;
	}
	sem_post(mutex1); //opening matrices
	sem_post(mutex2); //opening result
      }
    }
    else {
      while (1) {
	sem_wait(mutex2); //waiting for result
	if (*new_result && !*new_matrices) {
	  output(result);
	  *new_result = 0;
	}
	sem_post(mutex2); //opening result
      }
    }
  }
  
  sem_destroy(mutex1);
  sem_destroy(mutex2);
  free_memory(m1);
  free_memory(m2);
  free_memory(result);
  return 0;
}
