#include "csapp.h"

#define M 10
#define N 20

int main(void) {
  int *x = malloc(N * sizeof(int));
  memset(x, 0xf0, N * sizeof(int));
  free(x);
  int *y = malloc(M * sizeof(int));
  
  //ponizej powinno byc M zamiast N (pisze po niezalokowanej pamieci)
  //memset(y, 0x0f, N * sizeof(int));
  memset(y, 0x0f, M*sizeof(int));
  
  //drugi raz zwalniam te sama pamiec
  //free(x);
  free(y);
  return 0;
}
