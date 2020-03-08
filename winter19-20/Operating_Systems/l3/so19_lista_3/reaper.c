#include "csapp.h"

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

    kill(getppid(),SIGCONT);
    pause();
  /* TODO: Something is missing here! */
  printf("(%d) Got the signal!\n", getpid());
}

static void child(void) {
  pid_t pid;
    
    setpgid(getpid(),0);
    pid = spawn(grandchild);
  /* TODO: Something is missing here! */
  printf("(%d) Grandchild (%d) spawned!\n", getpid(), pid);
}

/* Runs command "ps -o pid,ppid,pgrp,stat,cmd" using execve(2). */ 
static void ps(void) {

    char *const cmd[] = {"/bin/ps","-o","pid,ppid,pgrp,stat,cmd", NULL};
    char *const env[] = {NULL};
    execve(cmd[0],cmd,env);
  /* TODO: Something is missing here! */
}

int main(void) {
  /* set yourself a reaper */
  Prctl(PR_SET_CHILD_SUBREAPER, 1);
  printf("(%d) I'm a reaper now!\n", getpid());

  pid_t pid, pgrp;
  int status;

    pgrp = spawn(child);
    Waitpid(pgrp,NULL,0);
    pid = spawn(ps);
    Waitpid(pid,NULL,0);
    kill(-pgrp,SIGINT);
    Waitpid(-1,&status,0);
    printf("Status: %d\n",status);
  /* TODO: Something is missing here! */

  return EXIT_SUCCESS;
}
