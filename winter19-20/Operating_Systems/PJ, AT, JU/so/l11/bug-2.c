/* WARNING: This code is buggy! */
#include "csapp.h"
#define N 4

int Indices[N];

static void *thread(void *vargp) {
  int myid = *((int *)vargp);
  printf("Hello from thread %d\n", myid);
  return NULL;
}

int main(void) {
  pthread_t tid[N];
  int i;

  
  for (i = 0; i < N; i++)
    Pthread_create(&tid[i], NULL, thread, &i);

  /* threads are given a pointer to a variable, whose value is
    changing */
  /* FIX: */

  /*
  for (i=0; i<N; i++){
    Indices[i] = i;
    Pthread_create(&tid[i], NULL, thread, &Indices[i]);
  }
  */

  for (i = 0; i < N; i++)
    Pthread_join(tid[i], NULL);

  return EXIT_SUCCESS;
}
