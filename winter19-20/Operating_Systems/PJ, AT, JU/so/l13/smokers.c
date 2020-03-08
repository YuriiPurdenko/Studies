#include "csapp.h"

static __unused void outc(char c) {
  Write(STDOUT_FILENO, &c, 1);
}

static __thread unsigned seed;

static sem_t tobacco;
static sem_t matches;
static sem_t paper;
static sem_t doneSmoking;

/* TODO: If you need any extra global variables, then define them here. */

static sem_t tobaccoSmoker;
static sem_t matchesSmoker;
static sem_t paperSmoker;

static sem_t watcherMutex;

volatile bool tobbaccoOnTable;
volatile bool matchesOnTable;
volatile bool paperOnTable;

static void *agent(void *arg) {
  seed = pthread_self();

  while (true) {
    Sem_wait(&doneSmoking);

    int choice = rand_r(&seed) % 3;
    if (choice == 0) {
      Sem_post(&tobacco);
      Sem_post(&paper);
    } else if (choice == 1) {
      Sem_post(&tobacco);
      Sem_post(&matches);
    } else {
      Sem_post(&paper);
      Sem_post(&matches);
    }
  }

  return NULL;
}

/* TODO: If you need extra threads, then define their main procedures here. */

static void tryToWakeupSmoker(void) {
  if (tobbaccoOnTable && matchesOnTable) {
    Sem_post(&paperSmoker);
  } else if (paperOnTable && matchesOnTable) {
    Sem_post(&tobaccoSmoker);
  } else if (tobbaccoOnTable && paperOnTable) {
    Sem_post(&matchesSmoker);
  } else
    return;
  paperOnTable = tobbaccoOnTable = matchesOnTable = false;
}

static void *watcherTobacco(void *arg) {
  while (true) {
    Sem_wait(&tobacco);
    Sem_wait(&watcherMutex);
    tobbaccoOnTable = true;
    tryToWakeupSmoker();
    Sem_post(&watcherMutex);
  }
  return NULL;
}

static void *watcherMatches(void *arg) {
  while (true) {
    Sem_wait(&matches);
    Sem_wait(&watcherMutex);
    matchesOnTable = true;
    tryToWakeupSmoker();
    Sem_post(&watcherMutex);
  }
  return NULL;
}

static void *watcherPaper(void *arg) {
  while (true) {
    Sem_wait(&paper);
    Sem_wait(&watcherMutex);
    paperOnTable = true;
    tryToWakeupSmoker();
    Sem_post(&watcherMutex);
  }
  return NULL;
}

static void randsleep(void) {
  usleep(rand_r(&seed) % 1000 + 1000);
}

static void make_and_smoke(char smoker) {
  randsleep();
  Sem_post(&doneSmoking);
  outc(smoker);
  randsleep();
}

static void *smokerWithMatches(void *arg) {
  seed = pthread_self();

  while (true) {
    /* TODO: wait for paper and tobacco */
    Sem_wait(&matchesSmoker);
    make_and_smoke('M');
  }

  return NULL;
}

static void *smokerWithTobacco(void *arg) {
  seed = pthread_self();

  while (true) {
    /* TODO: wait for paper and matches */
    Sem_wait(&tobaccoSmoker);
    make_and_smoke('T');
  }

  return NULL;
}

static void *smokerWithPaper(void *arg) {
  seed = pthread_self();
 
  while (true) {
    /* TODO: wait for tobacco and matches */
    Sem_wait(&paperSmoker);
    make_and_smoke('P');
  }

  return NULL;
}

int main(void) {
  Sem_init(&tobacco, 0, 0);
  Sem_init(&matches, 0, 0);
  Sem_init(&paper, 0, 0);
  Sem_init(&doneSmoking, 0, 1);

  /* TODO: Initialize your global variables here. */
  Sem_init(&tobaccoSmoker, 0, 0);
  Sem_init(&matchesSmoker, 0, 0);
  Sem_init(&paperSmoker, 0, 0);
  Sem_init(&watcherMutex, 0, 1);

  tobbaccoOnTable = false;
  matchesOnTable = false;
  paperOnTable = false;

  pthread_t agentThread;
  Pthread_create(&agentThread, NULL, agent, NULL);

  pthread_t smokerPaperThread, smokerMatchesThread, smokerTobaccoThread;
  Pthread_create(&smokerPaperThread, NULL, smokerWithPaper, NULL);
  Pthread_create(&smokerMatchesThread, NULL, smokerWithMatches, NULL);
  Pthread_create(&smokerTobaccoThread, NULL, smokerWithTobacco, NULL);

  pthread_t watcherPaperThread, watcherMatchesThread, watcherTobaccoThread;
  Pthread_create(&watcherPaperThread, NULL, watcherPaper, NULL);
  Pthread_create(&watcherMatchesThread, NULL, watcherMatches, NULL);
  Pthread_create(&watcherTobaccoThread, NULL, watcherTobacco, NULL);

  Pthread_join(agentThread, NULL);
  Pthread_join(smokerPaperThread, NULL);
  Pthread_join(smokerMatchesThread, NULL);
  Pthread_join(smokerTobaccoThread, NULL);

  Pthread_join(watcherPaperThread, NULL);
  Pthread_join(watcherMatchesThread, NULL);
  Pthread_join(watcherTobaccoThread, NULL);

  return 0;
}
