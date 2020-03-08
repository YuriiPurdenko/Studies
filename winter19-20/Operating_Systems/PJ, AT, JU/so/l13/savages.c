#include "csapp.h"

static __unused void outc(char c) {
  Write(STDOUT_FILENO, &c, 1);
}

#define N 100
#define M 100

static struct {
  /* TODO: Put semaphores and shared variables here. */
  uint32_t stewAmount;
  sem_t stewMutex;
  sem_t cookBlock;
  sem_t savageBlock;
} *shared = NULL;


static void savage(int id) {
  srand(id);
  for (;;) {
    /* TODO Take a meal or wait for it to be prepared. */
    sem_wait(&shared->stewMutex);
    safe_printf("[%d] I'm hungry!. Give me some stew!\n", id);
    if(shared->stewAmount == 0) {
      safe_printf("[%d] Oh no. The stew is gone! COOK! WAKE UP!\n", id);
      sem_post(&shared->cookBlock);
      sem_wait(&shared->savageBlock);
    }

    shared->stewAmount--;
    safe_printf("[%d] That was delicious. Time to sleep!\n", id);
    sem_post(&shared->stewMutex);
    /* Sleep and digest. */
    usleep(rand() % 1000 + 1000);
    //sleep(rand() % 10 + 1);
  }

  exit(EXIT_SUCCESS);
}

static void cook(void) {
  for (;;) {
    /* TODO Cook is asleep as long as there are meals.
     * If woken up they cook exactly M meals. */
    sem_wait(&shared->cookBlock);
    safe_printf("Uhm, what is this? You have eaten all the stew? Fine, I'll do more. Wait a second.\n");
    shared->stewAmount = M;
    sem_post(&shared->savageBlock);
  }
}

/* Do not bother cleaning up after this process. Let's assume that controlling
 * terminal sends SIGINT to the process group on CTRL+C. */
int main(void) {
  shared = Mmap(NULL, getpagesize(), PROT_READ|PROT_WRITE, MAP_ANON|MAP_SHARED,
                -1, 0);

  /* TODO: Initialize semaphores and other shared state. */
  shared->stewAmount = M;
  sem_init(&shared->stewMutex, 1, 1);
  sem_init(&shared->cookBlock, 1, 0);
  sem_init(&shared->savageBlock, 1, 0);

  for (int i = 0; i < N; i++)
    if (Fork() == 0)
      savage(i);

  cook();

  return EXIT_SUCCESS;
}
