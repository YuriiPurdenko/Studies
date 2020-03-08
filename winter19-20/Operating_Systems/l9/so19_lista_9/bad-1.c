#include "csapp.h"

#define N 10
#define M 5

int main(void) {
  //int **p = malloc(N * sizeof(int));
  int **p = malloc(N * sizeof(int*));    //za maly rozmiar jak na wskaznik
  for (int i = 0; i < N; i++)
    p[i] = malloc(M * sizeof(int));
  return 0;
}
