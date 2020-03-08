/* WARNING: This code is buggy! */
#include "csapp.h"

/* Global shared variable */
static volatile long cnt = 0; /* Counter */
// static _Atomic long cnt = 0;
// static pthread_mutex_t cnt_lock = PTHREAD_MUTEX_INITIALIZER;

/* Thread routine */
static void *thread(void *vargp) {
  long i, niters = *((long *)vargp);

  for (i = 0; i < niters; i++) {
    // pthread_mutex_lock(&cnt_lock);
    cnt++;
    // pthread_mutex_unlock(&cnt_lock);
  }

  /* Ten ++ najpierw wczytuje zmienną, potem ją powiększa, a następnie zapisuje,
    Jeśli pomiędzy odczyten i zapisem, drugi wątek wykonał ++, to jego wynik
    zostanie nadpisany. Jeżeli niters jest duże (ok. 10^5), to szansa na to, że
    zdarzy się to przynajmniej raz, jest duża */

  return NULL;
}

int main(int argc, char **argv) {
  /* Check input argument */
  if (argc != 2)
    app_error("usage: %s <niters>\n", argv[0]);

  long niters = atoi(argv[1]);
  pthread_t tid1, tid2;

  /* Create threads and wait for them to finish */
  
  Pthread_create(&tid1, NULL, thread, &niters);
  Pthread_create(&tid2, NULL, thread, &niters);
  Pthread_join(tid1, NULL);
  Pthread_join(tid2, NULL);

  /*FIX: don't run threads concurrently */
  /*
  Pthread_create(&tid1, NULL, thread, &niters);
  Pthread_join(tid1, NULL);
  Pthread_create(&tid2, NULL, thread, &niters);
  Pthread_join(tid2, NULL);
  */

  /* Check result */
  if (cnt != (2 * niters)) {
    printf("BOOM! cnt=%ld\n", (long)cnt);
    return EXIT_FAILURE;
  }

  printf("OK cnt=%ld\n", (long)cnt);
  return EXIT_SUCCESS;
}

