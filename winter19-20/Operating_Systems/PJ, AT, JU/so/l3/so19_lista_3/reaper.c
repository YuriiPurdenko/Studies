#include "csapp.h"

void ignore_signal(int signum) {
  printf("(%d) I ignored signal [%d]\n", getpid(), signum);
}

static pid_t spawn(void (*fn)(void)) {
  pid_t pid = Fork();
  if (pid == 0) {
    fn();
    printf("(%d) I'm done!\n", getpid());
    exit(EXIT_SUCCESS);
  }
  return pid;
}

static void grandchild(void) {
  printf("(%d) Waiting for signal!\n", getpid());
  pause();
  printf("(%d) Got the signal!\n", getpid());
}

static void child(void) {
  pid_t pid;
  Setpgid(0, 0);
  pid = spawn(grandchild);
  printf("(%d) Grandchild (%d) spawned!\n", getpid(), pid);
}

/* Runs command "ps -o pid,ppid,pgrp,stat,cmd" using execve(2). */ 
static void ps(void) {
  char *const argv[] = {"/bin/ps", "-o", "pid,ppid,pgrp,stat,cmd", NULL};
  char *const envp[] = {NULL};
  printf("(%d) run [%s]\n", getpid(), argv[0]);
  execve(argv[0], argv, envp); 
  perror("[EE]");
}

int main(void) {
  /* set yourself a reaper */
  Prctl(PR_SET_CHILD_SUBREAPER, 1);
  printf("(%d) I'm a reaper now!\n", getpid());

  pid_t pid, pgrp;
  int status;
  
  //Setpgid(0, 0);
  pid = spawn(child);
  Waitpid(pid, &status, 0);
  pid_t ps_pid = spawn(ps);
  int ps_status;
  Waitpid(ps_pid, &ps_status, 0);
  signal(SIGINT, ignore_signal);
  Kill(-pid, SIGINT);
  Waitpid(-1, &status, 0);
  printf("(%d) Grandchild status [%d]\n", getpid(), WTERMSIG(status));

  return EXIT_SUCCESS;
}
