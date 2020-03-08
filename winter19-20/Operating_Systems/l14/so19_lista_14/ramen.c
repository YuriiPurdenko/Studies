#include "csapp.h"

static __thread unsigned seed;

#define DEBUG
#ifdef DEBUG
static __unused void outc(char c) {
  Write(STDOUT_FILENO, &c, 1);
}
#else
#define outc(c)
#endif

typedef struct ramen {
  /* TODO: Put internal state & mutexes & condvars here. */
  pthread_mutex_t critic;
  pthread_cond_t leave;
  pthread_cond_t enter;
  bool leaving;
  int eating;
  int inside;
  int seats;
} ramen_t;

static void ramen_init(ramen_t *r, unsigned seats) {
  /* TODO: Initialize internal state of ramen restaurant. */
  pthread_cond_init(&r->leave, 0);
  pthread_cond_init(&r->enter, 0);
  pthread_mutex_init(&r->critic, NULL);
  r->seats = seats;
  r->eating = 0;
  r->leaving = 0;
  r->inside = 0;
}

static void ramen_destroy(ramen_t *r) {
  /* TODO: Destroy all synchronization primitives. */
  pthread_cond_destroy(&r->leave);
  pthread_cond_destroy(&r->enter);
  pthread_mutex_destroy(&r->critic);
}

static void ramen_wait(ramen_t *r) {
  /* TODO: Enter the restaurant unless it's full or people haven't left yet. */
  pthread_mutex_lock(&r->critic);
  while(r->inside == r->seats || r->leaving){
    pthread_cond_wait(&r->enter, &r->critic);
  }
  r->inside++;
  r->eating++;
  pthread_mutex_unlock(&r->critic);
}

static void ramen_finish(ramen_t *r) {
  /* TODO: leaving eating, so wait for all other to finish before leaving. */
  pthread_mutex_lock(&r->critic);
  //outc('c');  
  r->eating--;
  
  //printf("%d\n", r->eating);
  
  if(r->eating == 0){
    pthread_cond_broadcast(&r->leave);
    r->leaving = 1;
  }
  else
    while(r->eating != 0)
      pthread_cond_wait(&r->leave, &r->critic);
  
  //outc('d');
  
  assert(r->eating == 0);
  r->inside--;
  
  if(r->inside == 0){
    r->leaving = 0;
    pthread_cond_broadcast(&r->enter);
  }
  
  pthread_mutex_unlock(&r->critic);
}

void *customer(void *data) {
  ramen_t *r = data;

  seed = (unsigned)pthread_self();

  while (true) {
    /* Wait till you get hungry. */
    usleep(rand_r(&seed) % 5000 + 5000);

    /* Queue for delicious ramen. */
    outc('.');
    ramen_wait(r);

    /* It's so yummy! */
    outc('@');
    usleep(rand_r(&seed) % 1000 + 1000);

    /* Time to leave, but don't be rude or else... */
    ramen_finish(r);
    outc('_');
  }
}

#define THREADS 10
#define SEATS 5

int main(void) {
  ramen_t r;
  ramen_init(&r, SEATS);

  pthread_t tid[THREADS];

  for (int i = 0; i < THREADS; i++)
    Pthread_create(&tid[i], NULL, customer, &r);

  for (int i = 0; i < THREADS; i++)
    Pthread_join(tid[i], NULL);

  ramen_destroy(&r);
  return 0;
}
