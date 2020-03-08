#include "csapp.h"

static __unused void outc(char c) {
  Write(STDOUT_FILENO, &c, 1);
}

#define N 100
#define M 100

static struct {
  /* TODO: Put semaphores and shared variables here. */
  sem_t bowl;
  sem_t cooked;
  sem_t cauldron_empty;
  int meal;
} *shared = NULL;


static void savage(void) {
  for (;;) {
    /* TODO Take a meal or wait for it to be prepared. */
    //safe_printf("(%d) Waiting for bowl\n", getpid());
    Sem_wait(&shared->bowl);
    //safe_printf("(%d) Took bowl\n", getpid());
    if(shared->meal == 0){
      Sem_post(&shared->cauldron_empty);
      //safe_printf("(%d) Hungry\n", getpid());
      Sem_wait(&shared->cooked);
      assert(shared->meal == M);
    }
    safe_printf("(%d) Eating\n", getpid());
    shared->meal--;
    Sem_post(&shared->bowl);

    /* Sleep and digest. */
    usleep(rand() % 1000 + 1000);
  }

  exit(EXIT_SUCCESS);
}

static void cook(void) {
  for (;;) {
    /* TODO Cook is asleep as long as there are meals.
     * If woken up they cook exactly M meals. */
    Sem_wait(&shared->cauldron_empty);
    //safe_printf("(%d) Cooking\n", getpid());
    shared->meal = M;
    Sem_post(&shared->cooked);
  }
}

/* Do not bother cleaning up after this process. Let's assume that controlling
 * terminal sends SIGINT to the process group on CTRL+C. */
int main(void) {
  shared = Mmap(NULL, getpagesize(), PROT_READ|PROT_WRITE, MAP_ANON|MAP_SHARED,
                -1, 0);

  /* TODO: Initialize semaphores and other shared state. */
  Sem_init(&shared->bowl, 1, 1);
  Sem_init(&shared->cooked, 1, 0);
  Sem_init(&shared->cauldron_empty, 1, 0);
  shared->meal = M;
  for (int i = 0; i < N; i++)
    if (Fork() == 0)
      savage();

  cook();

  return EXIT_SUCCESS;
}
