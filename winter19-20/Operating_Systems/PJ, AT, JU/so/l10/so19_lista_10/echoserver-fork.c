#include "csapp.h"
#include "rio.h"

#define LISTENQ 10
#define MAXCLIENTS (PAGE_SIZE / sizeof(client_t))

typedef struct client {
  pid_t pid;    /* Client process id */
  size_t nread; /* Numer of bytes received so far */
} client_t;

static client_t *client = NULL;
static sig_atomic_t nclients = 0;
static size_t nread = 0; /* number of bytest received on all connections */

static client_t *findclient(pid_t pid) {
  for (int i = 0; i < MAXCLIENTS; i++)
    if (client[i].pid == pid)
      return &client[i];
  return NULL;
}

static client_t *addclient(void) {
  client_t *c = findclient(0);
  if (c) {
    c->pid = -1; /* XXX: must be filled in after fork */
    c->nread = 0;
    nclients++;
  }
  return c;
}

static void delclient(pid_t pid) {
  client_t *c = findclient(pid);
  assert(c != NULL);
  nread += c->nread;
  c->pid = 0;
  nclients--;
}

static void sigchld_handler(int sig) {
  pid_t pid;
  /* TODO: Delete clients as they die. */
  int status;
  while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
    if(status >= 0)
      delclient(pid);
  }
}

static sigjmp_buf jmpbuf;

static void sigint_handler(int sig) {
  safe_printf("Server received quit request!\n");
  /* TODO: Change control flow so that it does not return to main loop. */
  siglongjmp(jmpbuf, sig);
}

static void echo(client_t *c, int connfd) {
  size_t n;
  char buf[MAXLINE];
  rio_t rio;

  rio_readinitb(&rio, connfd);
  while ((n = Rio_readlineb(&rio, buf, MAXLINE))) {
    Rio_writen(connfd, buf, n);
    c->nread += n;
  }
}

int main(int argc, char **argv) {
  if (argc != 2)
    app_error("usage: %s <port>\n", argv[0]);

  sigset_t sig_mask;
  sigemptyset(&sig_mask);
  sigaddset(&sig_mask, SIGCHLD);

  Signal(SIGCHLD, sigchld_handler);
  Signal(SIGINT, sigint_handler);

  client =
    Mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED, -1, 0);

  int listenfd = Open_listenfd(argv[1], LISTENQ);

  /* TODO: Wait for all clients to quit and print a message with nread. */
  if (sigsetjmp(jmpbuf, 1)) {
    close(listenfd);
    sigset_t mask;
    Sigprocmask(SIG_BLOCK, &sig_mask, &mask);
    while (true) {
      if (nclients == 0)
        break;
      Sigsuspend(&mask);
    }
    safe_printf("Server received %ld bytes\n", nread);
    return 0;
  }

  while (1) {
    socklen_t clientlen = sizeof(struct sockaddr_storage);
    struct sockaddr_storage clientaddr; /* Enough space for any address */
    int connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);

    static char client_hostname[MAXLINE], client_port[MAXLINE];
    Getnameinfo((SA *)&clientaddr, clientlen, client_hostname, MAXLINE,
                client_port, MAXLINE, 0);

    sigset_t mask;
    Sigprocmask(SIG_BLOCK, &sig_mask, &mask);
    /* TODO: Start client in subprocess, close unused file descriptors. */
    client_t *c = addclient();
    pid_t pid = Fork();
    if(!pid) {
      Sigprocmask(SIG_SETMASK, &mask, NULL);
      Signal(SIGINT, SIG_IGN);
      safe_printf("[%d] Connected to %s:%s\n", getpid(), client_hostname, client_port);
      close(listenfd);
      
      echo(c, connfd);

      close(connfd);
      safe_printf("[%d] Disconnected\n", getpid());
      exit(0);
    }
    close(connfd);
    c->pid = pid;

    Sigprocmask(SIG_SETMASK, &mask, NULL);
  }
  return 0;
}
