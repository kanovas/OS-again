#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define n 3
#define m 3
#define nThreads 3

void *input(), *sum(), *output();
int m1[n*m], m2[n*m], result[n*m];
sem_t mutex1, mutex2;

void *input() {
  int i, j;
  while(1) {
    sem_wait(&mutex1); //waiting for matrices
    printf("Input matrix #1\n");
    for (i = 0; i < n; i++)
      for (j = 0; j < m; j++)
        scanf("%d", &m1[i*m + j]);
    
    printf("Input matrix #2\n");
    for (i = 0; i < n; i++)
      for (j = 0; j < m; j++)
        scanf("%d", &m2[i*m + j]);
    sem_post(&mutex1); //opening matrices
  }
}

void *sum() {
  int i, j;
  while(1) {
    sem_wait(&mutex2); //waiting for result
    sem_wait(&mutex1); //waiting for matrices
    for (i = 0; i < n; i++) 
      for (j = 0; j < m; j++) 
        result[i*m + j] = m1[i*m + j] + m2[i*m + j];
    sem_post(&mutex1); //opening matrices
    sem_post(&mutex2); //opening result
  }
}

void *output(){
  int i, j;
  while(1) {
    sem_wait(&mutex2); //waiting for result
    printf("Sum result is: \n");
    for (i = 0; i < n; i++) {
      for (j = 0; j < m; j++)
        printf("%d ", result[i*m + j]);
      printf("\n");
    }
    sem_post(&mutex2); //opening result
  }
}

int main() { 
  pthread_t thread[nThreads];
  sem_init(&mutex1, 0, 1);
  sem_init(&mutex2, 0, 2);
  int res = pthread_create(&thread[0], NULL, &input, NULL);
  if (res != 0) {
    perror("Can't create thread");
    return -1;
  }
  res = pthread_create(&thread[1], NULL, &sum, NULL);
  if (res != 0) {
    perror("Can't create thread");
    return -1;
  }
  res = pthread_create(&thread[2], NULL, &output, NULL);
  if (res != 0) {
    perror("Can't create thread");
    return -1;
  }
  sem_destroy(&mutex1);
  sem_destroy(&mutex2);
  
  return 0;
}