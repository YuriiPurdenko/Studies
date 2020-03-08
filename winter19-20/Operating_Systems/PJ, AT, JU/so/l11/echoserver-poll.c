#include "csapp.h"
#include "rio.h"

#define LISTENQ 10

typedef struct client {
  size_t id;
  rio_t rio;
} client_t;

static size_t clientid = 1;
static client_t **clients;
static struct pollfd *fds;
static nfds_t nfds = 0;
static nfds_t maxfds = 0;

static sig_atomic_t quit = false;
static size_t nbytes = 0; /* Counts total bytes received by server */

static void initclients(int listenfd) {
  nfds = 1;
  maxfds = 1;
  fds = Malloc(sizeof(struct pollfd));
  fds[0].fd = listenfd;
  fds[0].events = POLLIN;
  /*I've added this line */
  fds[0].revents = 0;
}

static void addclient(int connfd, const char *hostname, const char *port) {
  printf("[%ld] Connected to %s:%s\n", clientid, hostname, port);

  /* Should expand space to accommodate for new client ? */
  if (maxfds == nfds) {
    maxfds++;
    fds = Realloc(fds, sizeof(struct pollfd) * maxfds);
    clients = Realloc(clients, sizeof(client_t *) * maxfds);
    clients[maxfds - 1] = Malloc(sizeof(client_t));
  }

  int i = nfds++;
  fds[i].fd = connfd;
  fds[i].events = POLLIN; /* Wait only for input events! */
  fds[i].revents = 0;

  client_t *c = clients[i];
  c->id = clientid++;
  rio_readinitb(&c->rio, connfd);
}

static void delclient(int i) {
  assert(i > 0);
  Close(fds[i].fd);
  printf("[%ld] Disconnected!\n", clients[i]->id);

  fprintf(stderr, "closed %d - pos %d\n", fds[i].fd, i);

  if (i < nfds - 1) {
    
    fprintf(stderr, "swap %d with %ld\n", i, nfds-1);

    fprintf(stderr, "1: %d %d %d vs %d %d %d\n", fds[i].fd, fds[i].events,
            fds[i].revents, fds[nfds - 1].fd, fds[nfds - 1].events,
            fds[nfds - 1].revents);

    fds[i] = fds[nfds - 1];

    fprintf(stderr, "1: %d %d %d vs %d %d %d\n", fds[i].fd, fds[i].events,
            fds[i].revents, fds[nfds - 1].fd, fds[nfds - 1].events,
            fds[nfds - 1].revents);

    /* Swap slots so that prefix of array comprises only active clients. */
    client_t *tmp = clients[i];
    clients[i] = clients[nfds - 1];
    clients[nfds - 1] = tmp;

 
  }

  nfds--;
}

static int clientread(int i) {

  fprintf(stderr, "clientread [%ld] (%d) - pos %d\n", clients[i]->id, fds[i].fd,
          i);

  char buf[MAXLINE];
  int n = Rio_readlineb(&clients[i]->rio, buf, MAXLINE);

  if (n > 0) {
    Rio_writen(fds[i].fd, buf, n);
    nbytes += n;
    printf("[%ld] Received %d (%ld total) from client\n",
           clients[i]->id, n, nbytes);
  }

  return n;
}

static void sigint_handler(int sig) {
  safe_printf("Server received quit request!\n");
  quit = true;
}

int main(int argc, char **argv) {
  if (argc != 2)
    app_error("usage: %s <port>\n", argv[0]);

  Signal(SIGINT, sigint_handler);

  int listenfd = Open_listenfd(argv[1], LISTENQ);
  initclients(listenfd);

  while (!quit) {
    int nready = Poll(fds, nfds, 500);
    if (nready == 0)
      continue;

    /* TODO: If listening descriptor ready, add new client to the pool. */

    fprintf(stderr, "\ndoing a round\n");
    for (int i = 1; i < nfds; i++) {
      fprintf(stderr, "%d.  [%ld]: fd: %d\n", i, clients[i]->id, fds[i].fd);
    }

    if (fds[0].revents) {

      struct sockaddr SA;
      socklen_t addrlen = sizeof(struct sockaddr);
      char client_hostname[MAXLINE], client_port[MAXLINE];

      int connfd = Accept(fds[0].fd, &SA, &addrlen);

      fprintf(stderr, "(%d), created\n", connfd);

      SA.sa_family = 2;
      Getnameinfo(&SA, addrlen, client_hostname, MAXLINE, client_port, MAXLINE,
                  0);

      addclient(connfd, client_hostname, client_port);
      nready--;
      fds[0].revents = 0;
    }

    /* TODO: Echo a text line from each ready connected descriptor.
     * Delete a client when end-of-file condition was detected on socket. */
    int i = 1;
    while (nready > 0) {

      fprintf(stderr, "\ncheck %d (%d)\n", i, nready);
      for (int i = 1; i < nfds; i++) {
        fprintf(stderr, "%d.  [%ld]: fd: %d - %d\n", i, clients[i]->id,
                fds[i].fd, fds[i].revents);
      }

      /* poll could report not only POLLIN, but also POLLHUP, POLLERR or
       * POLLNVAL*/
      if (fds[i].revents)
        nready--;

      int move = 1; /* increment iterator by this much */
      if (fds[i].revents & POLLIN) {
        int nRead = clientread(i);

        if (nRead == 0) /*EOF */ {
          delclient(i);
          move = 0; /*delclient could have swaped elements in the array */
        }
      }

      // fds[i].revents = 0;
      i += move;
    }
  }

  printf("Server received %ld total bytes.\n", nbytes);
  return EXIT_SUCCESS;
}
