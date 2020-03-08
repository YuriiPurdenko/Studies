#include "csapp.h"

#define N 10
#define M 5

int main(void) {
	//wskaźnik nie jest rozmiaru int, tylko void*
  //int **p = malloc(N * sizeof(int));
  int **p = malloc(N * sizeof(void*));
  
  
  for (int i = 0; i < N; i++)
    p[i] = malloc(M * sizeof(int));
  
  
	//po za tym nie uwalniam pamięci, więc dodam:
	
	for (int i=0; i< N; i++)
		free(p[i]);
	
	free(p);
	
  return 0;
}
