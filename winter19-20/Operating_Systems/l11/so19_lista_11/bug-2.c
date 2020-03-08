/* WARNING: This code is buggy! */
#include "csapp.h"
#define N 100

static void *thread(void *vargp) {
  int myid = *((int *)vargp);
  //lepsze rozw
    //int myid =(int) (long) vargp;
  printf("Hello from thread %d\n", myid);
  //free(vargp);
  return NULL;
}

int main(void) {
  pthread_t tid[N];
  int i;

  for (i = 0; i < N; i++){
    //int* argum = (int*) malloc(sizeof(int));
    //*argum = i;
    //Pthread_create(&tid[i], NULL, thread, argum);	//i as a pointer :(
    
    //lepsze rozw
      //Pthread_create(&tid[i], NULL, thread, (void*) (long) i);
    Pthread_create(&tid[i], NULL, thread, &i);
  }
  for (i = 0; i < N; i++)
    Pthread_join(tid[i], NULL);

  return EXIT_SUCCESS;
}
