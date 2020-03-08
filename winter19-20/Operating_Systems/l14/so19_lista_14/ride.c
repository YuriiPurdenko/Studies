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

typedef struct ride {
  /* TODO: Put internal state & mutexes & condvars here. */
  pthread_mutex_t critic;
  pthread_cond_t loadintocart;
  pthread_cond_t cartfull;
  pthread_cond_t cartempty;
  pthread_cond_t getoutofcart;
  bool unload;
  bool load;
  int seats;
  int passengers;

} ride_t;

static void ride_init(ride_t *r, unsigned seats) {
  pthread_cond_init(&r->loadintocart, 0);
  pthread_cond_init(&r->cartfull, 0);
  pthread_cond_init(&r->cartempty, 0);
  pthread_cond_init(&r->getoutofcart, 0);
  pthread_mutex_init(&r->critic, NULL);
  r->seats = seats;
  r->passengers = 0;
  r->unload = 0;
  r->load = 0;
}

static void ride_destroy(ride_t *r) {
  /* TODO: Destroy all synchronization primitives. */
	pthread_cond_destroy(&r->loadintocart);
  pthread_cond_destroy(&r->cartfull);
  pthread_cond_destroy(&r->cartempty);
  pthread_cond_destroy(&r->getoutofcart);
  pthread_mutex_destroy(&r->critic);
}

static void cart_load(ride_t *r) {
  /* TODO: Wait for all seats to be taken and depart. */
  pthread_mutex_lock(&r->critic);
  r->load = 1;
  pthread_cond_broadcast(&r->loadintocart);
  while(r->seats != r->passengers)
    pthread_cond_wait(&r->cartfull, &r->critic);
  pthread_mutex_unlock(&r->critic);
}

static void cart_unload(ride_t *r) {
  /* TODO: Wait for all passangers to leave and be ready for next ride. */
  pthread_mutex_lock(&r->critic);
  r->unload = 1;
  pthread_cond_broadcast(&r->getoutofcart);
  assert(r->passengers != 0);
  while(r->passengers != 0)
    pthread_cond_wait(&r->cartempty, &r->critic);
  pthread_mutex_unlock(&r->critic);
}

static void passenger_board(ride_t *r) { 
  /* TODO: Wait for the cart, enter it and wait for the ride to begin. */
  pthread_mutex_lock(&r->critic);
  while(!r->load)
    pthread_cond_wait(&r->loadintocart, &r->critic);
  r->passengers++;
  if(r->passengers == r->seats){
    r->load = 0;
    pthread_cond_signal(&r->cartfull);
  }
  pthread_mutex_unlock(&r->critic);
}

static void passenger_unboard(ride_t *r) {
  /* TODO: Wait for the ride to end and leave the cart. */
  pthread_mutex_lock(&r->critic);
  while(!r->unload)
    pthread_cond_wait(&r->getoutofcart, &r->critic);
  r->passengers--;
  if(r->passengers == 0){
    r->unload = 0;
    pthread_cond_signal(&r->cartempty);
  }
  pthread_mutex_unlock(&r->critic);
}

static void *cart(void *data) {
  ride_t *r = data;

  seed = (unsigned)pthread_self();

  while (true) {
    outc('L');
    cart_load(r);

    usleep(rand_r(&seed) % 1000 + 1000);

    outc('U');
    cart_unload(r);
  }

  return NULL;
}

static void *passenger(void *data) {
  ride_t *r = data;

  seed = (unsigned)pthread_self();

  while(true) {   
    outc('-');
    passenger_board(r);

    usleep(rand_r(&seed) % 500 + 500);

    outc('!');
    passenger_unboard(r);

    outc('+');
    usleep(rand_r(&seed) % 2000 + 2000);
  }

  return NULL;
}

#define PASSENGERS 50
#define SEATS 10

int main(void) {
  ride_t r;
  ride_init(&r, SEATS);

  pthread_t cartThread;
  pthread_t passengerThread[PASSENGERS];

  pthread_create(&cartThread, NULL, cart, &r);
  for (int i = 0; i < 50; i++)
    pthread_create(&passengerThread[i], NULL, passenger, &r);

  pthread_join(cartThread, NULL);
  for (int i = 0; i < 50; i++)
    pthread_join(passengerThread[i], NULL);

  ride_destroy(&r);
  return 0;
}
