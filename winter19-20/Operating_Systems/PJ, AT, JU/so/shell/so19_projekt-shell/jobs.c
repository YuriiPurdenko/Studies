#include "shell.h"

typedef struct proc {
  pid_t pid;    /* process identifier */
  int state;    /* RUNNING or STOPPED or FINISHED */
  int exitcode; /* -1 if exit status not yet received */
} proc_t;

typedef struct job {
  pid_t pgid;    /* 0 if slot is free */
  proc_t *proc;  /* array of processes running in as a job */
  int nproc;     /* number of processes */
  int state;     /* changes when live processes have same state */
  char *command; /* textual representation of command line */
} job_t;

static job_t *jobs = NULL; /* array of all jobs */
static int njobmax = 1;    /* number of slots in jobs array */
static int tty_fd = -1;    /* controlling terminal file descriptor */

/* --- I can't imagine how to write code without this helpers */
static int proc_state(int status) {
  if (WIFEXITED(status) || WIFSIGNALED(status)) return FINISHED;
  if (WIFSTOPPED(status))                       return STOPPED;
  if (WIFCONTINUED(status))                     return RUNNING;
  debug("[%d] unknow state %d; return RUNNING\n", getpid(), status);
  return RUNNING;
}

static const char *const strstate(int state, int exitcode) {
  switch (state) {
  case FINISHED: {
    if (WIFEXITED(exitcode))
      return "exited";
    return "killed";
  }
  case STOPPED:   return "stopped";
  case RUNNING:   return "running";
  default: {
    debug("[%d] process is in unknown state [%d]; return 'running'\n",
          getpid(), state);
    return "running";
  }
  }
}

static void sigchld_handler(int sig) {
  int old_errno = errno;
  pid_t pid;
  int status;
  /* TODO: Change state (FINISHED, RUNNING, STOPPED) of processes and jobs.
   * Bury all children that finished saving their status in jobs. */

  while ((pid = waitpid(-1, &status, WNOHANG | WCONTINUED | WUNTRACED)) > 0) {
    debug("[%d] caught %d\n", getpid(), pid);
    for (int i = 0; i < njobmax; ++i) {
      for (int j = 0; j < jobs[i].nproc; ++j) {
        proc_t *const proc = &jobs[i].proc[j];
        if (pid == proc->pid) {
          debug("[%d] sigchld_handler[1]: pid [%d] state [%s] exitcode [%d]\n",
                getpid(), pid, strstate(proc->state, proc->exitcode),
                proc->exitcode);
          proc->state = proc_state(status);
          proc->exitcode = status;
          debug("[%d] sigchld_handler[2]: pid [%d] state [%s] exitcode [%d]\n",
                getpid(), pid, strstate(proc->state, proc->exitcode),
                proc->exitcode);
        }
      }
    }
  }

  
  for (int i = 0; i < njobmax; ++i) {
    bool finished = true;
    bool running = false;
    int *const jstate = &jobs[i].state;
    for (int j = 0; j < jobs[i].nproc; ++j) {
      int *const state = &jobs[i].proc[j].state;
      switch (*state) {
        case FINISHED: {
          debug("[%d] proc %d in job %d is FINISHED\n", getpid(), j, i);
          break;
        }
        case STOPPED: {
          finished = false;
          debug("[%d] proc %d in job %d is STOPPED\n", getpid(), j, i);
          break;
        }
        case RUNNING: {
          debug("[%d] proc %d in job %d is RUNNING\n", getpid(), j, i);
          finished = false;
          running = true;
          break;
        } 
      }
    }
    if (finished) {
      *jstate = FINISHED;
      debug("[%d] set final state of job %d to FINISHED\n", getpid(), i);
      continue;
    }
    if (running) {
      *jstate = RUNNING;
      debug("[%d] set final state of job %d to RUNNING\n", getpid(), i);
      continue;
    }
    *jstate = STOPPED;
    debug("[%d] set final state of job %d to STOPPED\n", getpid(), i);
  }

  errno = old_errno;
}

/* When pipeline is done, its exitcode is fetched from the last process. */
static int exitcode(job_t *job) {
  return job->proc[job->nproc - 1].exitcode;
}

static int allocjob(void) {
  /* Find empty slot for background job. */
  for (int j = BG; j < njobmax; j++)
    if (jobs[j].pgid == 0)
      return j;

  /* If none found, allocate new one. */
  jobs = realloc(jobs, sizeof(job_t) * (njobmax + 1));
  return njobmax++;
}

static int allocproc(int j) {
  job_t *job = &jobs[j];
  job->proc = realloc(job->proc, sizeof(proc_t) * (job->nproc + 1));
  return job->nproc++;
}

int addjob(pid_t pgid, int bg) {
  int j = bg ? allocjob() : FG;
  job_t *job = &jobs[j];
  /* Initial state of a job. */
  job->pgid = pgid;
  job->state = RUNNING;
  job->command = NULL;
  job->proc = NULL;
  job->nproc = 0;
  return j;
}

static void deljob(job_t *job) {
  assert(job->state == FINISHED);
  free(job->command);
  free(job->proc);
  job->pgid = 0;
  job->command = NULL;
  job->proc = NULL;
  job->nproc = 0;
}

static void movejob(int from, int to) {
  assert(jobs[to].pgid == 0);
  memcpy(&jobs[to], &jobs[from], sizeof(job_t));
  memset(&jobs[from], 0, sizeof(job_t));
}

static void mkcommand(char **cmdp, char **argv) {
  if (*cmdp)
    strapp(cmdp, " | ");

  for (strapp(cmdp, *argv++); *argv; argv++) {
    strapp(cmdp, " ");
    strapp(cmdp, *argv);
  }
}

void addproc(int j, pid_t pid, char **argv) {
  assert(j < njobmax);
  job_t *job = &jobs[j];

  int p = allocproc(j);
  proc_t *proc = &job->proc[p];
  /* Initial state of a process. */
  proc->pid = pid;
  proc->state = RUNNING;
  proc->exitcode = -1;
  mkcommand(&job->command, argv);
}

/* Returns job's state.
 * If it's finished, delete it and return exitcode through statusp. */
int jobstate(int j, int *statusp) {
  assert(j < njobmax);
  job_t *job = &jobs[j];
  int state = job->state;

  /* TODO: Handle case where job has finished. */
  if (FINISHED == state) {
    debug("[%d] job %d FINISHED\n", getpid(), j);
    *statusp = exitcode(job);
    deljob(job);
  }

  return state;
}

char *jobcmd(int j) {
  assert(j < njobmax);
  job_t *job = &jobs[j];
  return job->command;
}

/* Continues a job that has been stopped. If move to foreground was requested,
 * then move the job to foreground and start monitoring it. */
bool resumejob(int j, int bg, sigset_t *mask) {
  if (j < 0) {
    for (j = njobmax - 1; j > 0 && jobs[j].state == FINISHED; j--)
      continue;
  }

  if (j >= njobmax || jobs[j].state == FINISHED)
    return false;

  /* TODO: Continue stopped job. Possibly move job to foreground slot. */
  debug("[%d] send SIGCONT to %d\n", getpid(), -jobs[j].pgid);
  Kill(-jobs[j].pgid, SIGCONT);

  msg("[%d] continue '%s'\n", j, jobcmd(j));
  if (!bg) {
    movejob(j, FG);
    int e;
    while (STOPPED == jobstate(FG, &e)) {
      /* --- thanks Franek */
      Kill(-jobs[FG].pgid, SIGCONT);
      Sigsuspend(mask);
    }
    monitorjob(mask);
  }

  return true;
}

/* Kill the job by sending it a SIGTERM. */
bool killjob(int j) {
  if (j >= njobmax || jobs[j].state == FINISHED)
    return false;
  debug("[%d] killing '%s'\n", j, jobs[j].command);

  /* TODO: I love the smell of napalm in the morning. */

  /* --- -_- */
  int pgid = jobs[j].pgid;
  debug("[%d] send SIGTERM to %d\n", getpid(), -pgid);
  Kill(-pgid, SIGTERM);

  if (STOPPED == jobs[j].state) {
    debug("[%d] is STOPPED\n", j);
    debug("[%d] send SIGCONT to %d\n", getpid(), -pgid);
    Kill(-pgid, SIGCONT);
  }

  return true;
}

/* Report state of requested background jobs. Clean up finished jobs. */
void watchjobs(int which) {
  for (int j = BG; j < njobmax; j++) {
    job_t *job = &jobs[j];

    if (0 == job->pgid)
      continue;

    /* TODO: Report job number, state, command and exit code or signal. */
    if (ALL == which || job->state == which) {
      msg("[%d] %s '%s'", j, strstate(job->state, exitcode(job)), jobcmd(j));
    }

    if (FINISHED == job->state) {
      if (WIFSIGNALED(exitcode(job))) {
        msg(" by signal %d\n", WTERMSIG(exitcode(job)));
      } else {
        msg(", status=%d\n", WEXITSTATUS(exitcode(job)));
      }
      deljob(job);
    } else {
      msg("\n");
    }
  }
}

/* Monitor job execution. If it gets stopped move it to background.
 * When a job has finished or has been stopped move shell to foreground. */
int monitorjob(sigset_t *mask) {
  int exitcode, state;

  /* TODO: Following code requires use of Tcsetpgrp of tty_fd. */

  /* --- I absolutely hate terminals :) */
  Tcsetpgrp(tty_fd, jobs[0].pgid);
  /* --- thanks Wiktor ^^
   * it fixes ./a | ./a | ./a | ./a
   * where a is 
   * ```
   * int main() { 
   *   int x; 
   *   scanf("%d", &x); 
   *   printf("%d\n", x + x); 
   *   return 0; 
   * }
   * ```
   */
  Kill(-jobs[0].pgid, SIGCONT);

#if 0
  sigset_t full, old;
  sigfillset(&full);
  Sigprocmask(SIG_SETMASK, &full, &old);
  waitpid(jobs[0].pgid, NULL, WNOHANG | WCONTINUED | WUNTRACED);
  Sigprocmask(SIG_SETMASK, &old, NULL);
#endif

  for (int loop = 1; loop;) {
    state = jobstate(FG, &exitcode);
    switch (state) {
      case FINISHED: {
        loop = 0;
        debug("[%d] monitorjob: FINISHED\n", getpid());
        break;
      }
      case STOPPED: {
        int j = allocjob();
        /* --- remove random bytes 
         * I don't remember who mentioned it
         * */
        memset(&jobs[j], 0, sizeof(job_t));
        movejob(FG, j);
        msg("[%d] suspended '%s'\n", j, jobcmd(j));
        loop = 0;
        break;
      }
      default: {
        Sigsuspend(mask);
        break;
      }
    }
  }

  Tcsetpgrp(tty_fd, getpgrp());

  return exitcode;
}

/* Called just at the beginning of shell's life. */
void initjobs(void) {
  Signal(SIGCHLD, sigchld_handler);
  jobs = calloc(sizeof(job_t), 1);

  /* Assume we're running in interactive mode, so move us to foreground.
   * Duplicate terminal fd, but do not leak it to subprocesses that execve. */
  assert(isatty(STDIN_FILENO));
  tty_fd = Dup(STDIN_FILENO);
  fcntl(tty_fd, F_SETFL, O_CLOEXEC);
  Tcsetpgrp(tty_fd, getpgrp());
}

/* Called just before the shell finishes. */
void shutdownjobs(void) {
  sigset_t mask;
  Sigprocmask(SIG_BLOCK, &sigchld_mask, &mask);

  /* TODO: Kill remaining jobs and wait for them to finish. */

  for (int i = 0; i < njobmax; ++i) {
    if (jobs[i].pgid) {
      debug("[%d] kill job %d\n", getpid(), i);
      killjob(i);
    }
    while (FINISHED != jobs[i].state) {
      debug("[%d] wait for job %d until FINISHED; state: %d\n", getpid(), i, jobs[i].state);
      Sigsuspend(&mask);
    }
  }

  watchjobs(FINISHED);
  /* --- 'ack free' said that we don't free jobs array */
  if (jobs)
    free(jobs);
  Sigprocmask(SIG_SETMASK, &mask, NULL);

  Close(tty_fd);
}
