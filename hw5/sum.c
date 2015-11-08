#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <errno.h>

#define n 3
#define m 3

void input(int* m1, int* m2) {
  printf("Input matrix #1\n");
  int i, j;
  for (i = 0; i < n; i++)
    for (j = 0; j < m; j++)
      scanf("%d", &m1[i*n + m]);
    
  printf("Input matrix #2\n");
  for (i = 0; i < n; i++)
    for (j = 0; j < m; j++)
      scanf("%d", &m2[i*n + m]);
  return;
}

void sum(int* m1, int* m2, int* res) {
  int i, j;
  for (i = 0; i < n; i++) 
    for (j = 0; j < m; j++) 
      res[i*n + m] = m1[i*n + m] + m2[i*n + m];
  return;
}

void output(int* matrix){
  int i, j;
  printf("Sum result is: \n");
  for (i = 0; i < n; i++) {
    for (j = 0; j < m; j++)
      printf("%d ", matrix[i*n + m]);
    printf("\n");
  }
  return;
}

int* reserve_memory(int key, char pathname[], int num) {
  int shmid;
  int * arr;
  if ((key = ftok(pathname, num)) < 0){
    perror("Can't generate key\n");
    exit(-1);
  }
  if ((shmid = shmget(key, n*m*sizeof(int), 0666|IPC_CREAT|IPC_EXCL)) < 0) {
    if (errno != EEXIST){
      perror("Can't create shared memory\n");
      exit(-1);
    } 
    else {   
      if ((shmid = shmget(key, n*m*sizeof(int), 0)) < 0){
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

void sem_d(struct sembuf * mybuf, int semid, int num) {
  mybuf->sem_op = -1;
  mybuf->sem_flg = 0;
  mybuf->sem_num = num;
  if(semop(semid, mybuf, 1) < 0){
    perror("Can\'t wait for condition\n");
    exit(-1);
  }
}

void sem_a(struct sembuf * mybuf, int semid, int num) {
  mybuf->sem_op = 1;
  mybuf->sem_flg = 0;
  mybuf->sem_num = num;
  if(semop(semid, mybuf, 1) < 0){
    perror("Can\'t wait for condition\n");
    exit(-1);
  }
}

int main() {
  
  char pathname[] = "sum.c";
  int key1, key2, key3, key, semid;
  //shared memory
  int * m1 = reserve_memory(key1, pathname, 0);
  int * m2 = reserve_memory(key2, pathname, 1);
  int * result = reserve_memory(key3, pathname, 2);
  //semaphore array
  struct sembuf mybuf; 
  if((key = ftok(pathname,0)) < 0){
    perror("Can\'t generate key\n");
    exit(-1);
  }
  if((semid = semget(key, 2, 0666 | IPC_CREAT)) < 0){
    perror("Can\'t get semid\n");
    exit(-1);
  }
  
  int pid = fork();
  if (pid < 0) {
    perror("Can't fork\n");
    exit(-1);
  }
  else if (pid == 0) {
    while (1) {
      sem_d(&mybuf, semid, 0); //waiting for matrices
      input(m1, m2);
      sem_a(&mybuf, semid, 0); //opening matrices
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
	sem_d(&mybuf, semid, 1); //waiting for result
        sem_d(&mybuf, semid, 0); //waiting for matrices
        sum(m1, m2, result);
	sem_a(&mybuf, semid, 0); //opening matrices
	sem_a(&mybuf, semid, 1); //opening result
      }
    }
    else {
      while (1) {
	sem_d(&mybuf, semid, 1); //waiting for result
	output(result);
	sem_a(&mybuf, semid, 1); //opening result
      }
    }
  }
  
  free_memory(m1);
  free_memory(m2);
  free_memory(result);
}
