#include "csapp.h"

static __unused void outc(char c) {
  Write(STDOUT_FILENO, &c, 1);
}

static __thread unsigned seed;

static sem_t tobacco;	//1
static sem_t matches;	//2
static sem_t paper;	//3
static sem_t doneSmoking;
static sem_t smoker_with_matches;
static sem_t smoker_with_tobacoo;
static sem_t smoker_with_paper;
static sem_t watcher;


int matches_on_table = false;
int paper_on_table = false;
int tobacco_on_table = false;
/* TODO: If you need any extra global variables, then define them here. */

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
/*
static void *supervisor(void *arg){
  while(true){
    sem_getvalue(&paper, &p);
    sem_getvalue(&tobacco, &t);
    sem_getvalue(&matches, &m);
    if(p & t){
      Sem_wait(&paper);
      Sem_wait(&tobacco);
      Sem_post(&smoker_with_matches);
    }
    else if(p & m){
      Sem_wait(&paper);
      Sem_wait(&matches);
      Sem_post(&smoker_with_tobacoo);
    }
    else if(t & m){
      Sem_wait(&tobacco);
      Sem_wait(&matches);
      Sem_post(&smoker_with_paper);
    }
  }
  return NULL;
}
*/

static void waker(void){
  if(paper_on_table & tobacco_on_table){
      Sem_post(&smoker_with_matches);
      paper_on_table = false;
      tobacco_on_table = false;
    }
    else if(paper_on_table & matches_on_table){
      Sem_post(&smoker_with_tobacoo);
      paper_on_table = false;
      matches_on_table = false;
    }
    else if(matches_on_table & tobacco_on_table){
      Sem_post(&smoker_with_paper);
      matches_on_table = false;
      tobacco_on_table = false;
    }
}


static void *paperwatcher(void *arg){
  while(true){
    Sem_wait(&paper);
    Sem_wait(&watcher);
    paper_on_table = true;
    waker();
    Sem_post(&watcher);
  }
  return NULL;
}

static void *matcheswatcher(void *arg){
  while(true){
    Sem_wait(&matches);
    Sem_wait(&watcher);
    matches_on_table = true;
    waker();
    Sem_post(&watcher);
  }
  return NULL;
}

static void *tobaccowatcher(void *arg){
  while(true){
    Sem_wait(&tobacco);
    Sem_wait(&watcher);
    tobacco_on_table = true;
    waker();
    Sem_post(&watcher);
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
		Sem_wait(&smoker_with_matches);
    make_and_smoke('M');
  }

  return NULL;
}

static void *smokerWithTobacco(void *arg) {
  seed = pthread_self();

  while (true) {
    /* TODO: wait for paper and matches */
		Sem_wait(&smoker_with_tobacoo);
    make_and_smoke('T');
  }

  return NULL;
}

static void *smokerWithPaper(void *arg) {
  seed = pthread_self();
 
  while (true) {
    /* TODO: wait for tobacco and matches */
		Sem_wait(&smoker_with_paper);
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
  Sem_init(&smoker_with_matches, 0, 0);
  Sem_init(&smoker_with_tobacoo, 0, 0);
  Sem_init(&smoker_with_paper, 0, 0);
  Sem_init(&watcher, 0, 1);

  pthread_t paperwatcherThread;
  pthread_t matcheswatcherThread;
  pthread_t tobaccowatcherThread;
  Pthread_create(&paperwatcherThread, NULL, paperwatcher, NULL);
  Pthread_create(&matcheswatcherThread, NULL, matcheswatcher, NULL);
  Pthread_create(&tobaccowatcherThread, NULL, tobaccowatcher, NULL);
  
  pthread_t agentThread;
  Pthread_create(&agentThread, NULL, agent, NULL);
  
  pthread_t smokerPaperThread, smokerMatchesThread, smokerTobaccoThread;
  Pthread_create(&smokerPaperThread, NULL, smokerWithPaper, NULL);
  Pthread_create(&smokerMatchesThread, NULL, smokerWithMatches, NULL);
  Pthread_create(&smokerTobaccoThread, NULL, smokerWithTobacco, NULL);

  Pthread_join(agentThread, NULL);
  Pthread_join(paperwatcherThread, NULL);
  Pthread_join(matcheswatcherThread, NULL);
  Pthread_join(tobaccowatcherThread, NULL);
  Pthread_join(smokerPaperThread, NULL);
  Pthread_join(smokerMatchesThread, NULL);
  Pthread_join(smokerTobaccoThread, NULL);

  return 0;
}
