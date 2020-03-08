#include "csapp.h"

static __unused void outc(char c) {
  Write(STDOUT_FILENO, &c, 1);
}

typedef struct {
  /* TODO: Use this structure to store barrier internal state. */
  int sum;
  int n;
  sem_t notenough; // brak wystarczająco wielu aby wystartować wyścig
  sem_t adding; // blokada przy zmianie sum
  sem_t exited; // czekamy aż wszyscy wyjdą
} barrier_t;

static barrier_t *barrier_init(int n) {
  if (n < 1)
    app_error("barrier_init requires n > 0");

  barrier_t *b = Mmap(NULL, sizeof(barrier_t), PROT_READ|PROT_WRITE,
                      MAP_ANON|MAP_SHARED, -1, 0);

  /* TODO: Initialize barrier internal state. */
  b->n = n;
  b->sum = 0;
  Sem_init(&b->notenough, 1, 0);
  Sem_init(&b->adding, 1, 1);
  Sem_init(&b->exited, 1, 0);
  return b;
}

static void barrier_wait(barrier_t *b) {
  /* TODO: Provide wait procedure implementation here. */
  Sem_wait(&b->adding);
  b->sum++;
  if (b->sum == b->n) {
    while (--(b->sum)) {
      Sem_post(&b->notenough);
      //Sem_wait(&b->exited);
    }
    for (int i = 0; i < b->n - 1; ++i) {
      Sem_wait(&b->exited);
    }
    Sem_post(&b->adding);
    return;
  }
  Sem_post(&b->adding);
  Sem_wait(&b->notenough);
  Sem_post(&b->exited);
}

static void barrier_destroy(barrier_t *b) {
  /* TODO: Provide destroy procedure implementation here. */
  Sem_destroy(&b->adding);
  Sem_destroy(&b->notenough);
  Sem_destroy(&b->exited);
  Munmap(b, sizeof(barrier_t));
}

#define K 100
#define N 50
#define P 100

static noreturn void horse(barrier_t *b) {
  int n = rand() % K + K;

  outc('+');

  for (int i = 0; i < n; i++) {
    barrier_wait(b);
    usleep(rand() % 2000 + 1000);
  }

  outc('-');

  exit(EXIT_SUCCESS);
}

/* Do not bother cleaning up after this process. Let's assume that controlling
 * terminal sends SIGINT to the process group on CTRL+C. */
int main(void) {
  barrier_t *b = barrier_init(N);

  int horses = 0;

  for (;;) {
    do {
      if (Fork() == 0)
        horse(b);
      horses++;
    } while (horses < P);

    Wait(NULL);
    horses--;
  }

  barrier_destroy(b);

  return EXIT_SUCCESS;
}
