#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define n 3
#define m 3

void input(int m1[][m] , int m2[][m] ) {
  printf("Input matrix #1\n");
  int i, j;
  for (i = 0; i < n; i++)
    for (j = 0; j < m; j++)
      scanf("%d", &m1[i][j]);
    
  printf("Input matrix #2\n");
  for (i = 0; i < n; i++)
    for (j = 0; j < m; j++)
      scanf("%d", &m2[i][j]);
  return;
}

void sum(int m1[][m], int m2[][m], int res[][m]) {
  int i, j;
  for (i = 0; i < n; i++) 
    for (j = 0; j < m; j++) 
      res[i][j] = m1[i][j] + m2[i][j];
  return;
}

void output(int matrix[][m]){
  int i, j;
  printf("Sum result is: \n");
  for (i = 0; i < n; i++) {
    for (j = 0; j < m; j++)
      printf("%d ", matrix[i][j]);
    printf("\n");
  }
}

int main() {
  char pathname[] = "sum.c";
  int m1[n][m], m2[n][m], result[n][m];
  input(m1, m2);
  sum(m1, m2, result);
  output(result);
}
