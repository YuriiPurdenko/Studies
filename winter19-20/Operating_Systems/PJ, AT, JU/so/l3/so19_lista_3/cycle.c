#include "csapp.h"

static void signal_handler(int signum, siginfo_t *info, void *data) {
  if (signum == SIGINT) {
    safe_printf("(%d) Screw you guys... I'm going home!\n", getpid());
    _exit(0);
  }
}

static void play(pid_t next, const sigset_t *set) {
  for (;;) {
    printf("(%d) Waiting for a ball!\n", getpid());
    /* TODO: Something is missing here! */
    usleep(500 * 1000);
    Kill(next, SIGUSR1);
    printf("(%d) Passing ball to (%d)!\n", getpid(), next);
    Sigsuspend(set);
  }
}

int main(int argc, char *argv[]) {
  if (argc != 2)
    app_error("Usage: %s [CHILDREN]", argv[0]);

  int children = atoi(argv[1]);

  if (children < 4 || children > 20)
    app_error("Give number of children in range from 4 to 20!");

  /* Register signal handler for SIGUSR1 */
  struct sigaction action = { .sa_sigaction = signal_handler };
  Sigaction(SIGINT, &action, NULL);
  Sigaction(SIGUSR1, &action, NULL);

  /* TODO: Something is missing here! */
  
  sigset_t set;
  sigset_t blocked_set;
  Sigprocmask(0, NULL, &set);
  Sigprocmask(0, NULL, &blocked_set);

  sigaddset(&blocked_set, SIGUSR1);
  Sigprocmask(SIG_BLOCK, &blocked_set, NULL);

  pid_t pid[children + 1];
  pid[0] = getpid();

  for (int i = 1; i <= children; ++i) {
    pid[i] = fork();
    if (pid[i] == 0) {
      Sigsuspend(&set);
      play(pid[i - 1], &set);
    }
  }

  play(pid[children], &set);

  return EXIT_SUCCESS;
}
