#include "csapp.h"

#define M 10
#define N 20

int main(void) {
  int *x = malloc(N * sizeof(int));
  memset(x, 0xf0, N * sizeof(int));
  
  //nie moge zwalniac tej pamieci tutaj
  //free(x);
  
  int *y = malloc(M * sizeof(int));
  for (int i = 0; i < M; i++)
  //uzywam zwolnionej pamieci
    y[i] = x[i]++;
    
  //nie zwalniam pamieci (y), więc:
  free(x);
  free(y);
  return 0;
}
