#include <readline/readline.h>
#include <readline/history.h>

#include "shell.h"

sigset_t sigchld_mask;

static sigjmp_buf loop_env;

static void sigint_handler(int sig) {
  siglongjmp(loop_env, sig);
}

/* Consume all tokens related to redirection operators.
 * Put opened file descriptors into inputp & output respectively. */
static int do_redir(token_t *token, int ntokens, int *inputp, int *outputp) {
  token_t mode = NULL; /* T_INPUT, T_OUTPUT or NULL */
  int n = 0;           /* number of tokens after redirections are removed */

  for (int i = 0; i < ntokens; i++) {
    /* TODO: Handle tokens and open files as requested. */
    mode = token[i];

    switch ((intptr_t)mode) {
      case (intptr_t)T_INPUT: { 
        if (*inputp >= 0) {
          debug("[%d] close old fd %d\n", getpid(), *inputp);
          Close(*inputp);
        }
        debug("[%d] open new input fd '%s'\n", getpid(), token[i + 1]);
        *inputp = open(token[i + 1], O_RDONLY);
        if (-1 == *inputp) {
          msg("shell: open failed '%s': %m", token[i + 1]);
        }
        break;
      }
      case (intptr_t)T_OUTPUT: {
        if (*outputp >= 0) {
          debug("[%d] close old fd %d\n", getpid(), *outputp);
          Close(*outputp);
        }
        debug("[%d] open new output fd '%s'\n", getpid(), token[i + 1]);
        *outputp = open(token[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (-1 == *outputp) {
          msg("shell: open failed '%s': %m", token[i + 1]);
        }
        break;
      }
      case (intptr_t)T_APPEND: {
        if (*outputp >= 0) {
          debug("[%d] close old fd %d\n", getpid(), *outputp);
          Close(*outputp);
        }
        debug("[%d] open new output fd '%s'\n", getpid(), token[i + 1]);
        *outputp = open(token[i + 1], O_WRONLY | O_CREAT | O_APPEND, 0644);
        if (-1 == *outputp) {
          msg("shell: open failed '%s': %m", token[i + 1]);
        }
        break;
      }
      default: {
        token[n] = token[i];
        ++n;
        continue;
        break;
      }
    }

    token[i] = token[i + 1] = NULL;
    ++i;
  }

  token[n] = NULL;
  return n;
}

/* Execute internal command within shell's process or execute external command
 * in a subprocess. External command can be run in the background. */
static int do_job(token_t *token, int ntokens, bool bg) {
  int input = -1, output = -1;
  int exitcode = 0;

  ntokens = do_redir(token, ntokens, &input, &output);

  /* --- XXX this is bug if we want to start command in background */
  if ((exitcode = builtin_command(token)) >= 0)
    return exitcode;

  sigset_t mask;
  Sigprocmask(SIG_BLOCK, &sigchld_mask, &mask);

  /* TODO: Start a subprocess, create a job and monitor it. */
  pid_t pid = Fork();

  if (0 == pid) {
    debug("[%d] I am child process\n", getpid());
    if (-1 != input) {
      Dup2(input, STDIN_FILENO);
      Close(input);
      debug("[%d] input redirected (%d -> %d)\n", getpid(), input,
            STDIN_FILENO);
    }
    if (-1 != output) {
      Dup2(output, STDOUT_FILENO);
      Close(output);
      debug("[%d] output redirected (%d -> %d)\n", getpid(), output,
            STDOUT_FILENO);
    }

    Signal(SIGCHLD, SIG_DFL);
    Signal(SIGTSTP, SIG_DFL);
    Signal(SIGTTIN, SIG_DFL);
    Signal(SIGTTOU, SIG_DFL);
    Sigprocmask(SIG_SETMASK, &mask, NULL);
    debug("[%d] sginals restored\n", getpid());

    setpgid(0, 0);

    /* --- if not XXX we should call builtin_command here */
    //if ((exitcode = builtin_command(token)) >= 0)
      //exit(exitcode);
    
    debug("[%d] shields up, weapons armed - going live\n", getpid());
    external_command(token);
  }

  if (-1 != input) {
    debug("[%d] close child input %d", getpid(), input);
    Close(input);
  }
  if (-1 != output) {
    debug("[%d] close child output %d", getpid(), output);
    Close(output);
  }

  /* --- thanks Michał Syposz 
   * raw function because errors are okay
   * */
  setpgid(pid, pid);
  debug("[%d] set pgid: %d %d\n", getpid(), pid, pid);

  int job = addjob(pid, bg);
  addproc(job, pid, token);

  if (bg) {
    msg("[%d] running '%s' %d\n", job, jobcmd(job), pid);
  } else {
    debug("[%d] foreground\n", getpid());
    exitcode = monitorjob(&mask);
  }

  Sigprocmask(SIG_SETMASK, &mask, NULL);
  return exitcode;
}

/* Start internal or external command in a subprocess that belongs to pipeline.
 * All subprocesses in pipeline must belong to the same process group. */
static pid_t do_stage(pid_t pgid, sigset_t *mask, int input, int output,
                      token_t *token, int ntokens) {
  ntokens = do_redir(token, ntokens, &input, &output);

  /* TODO: Start a subprocess and make sure it's moved to a process group. */
  pid_t pid = Fork();

  if (0 == pid) {
    if (-1 != input) {
      Dup2(input, STDIN_FILENO);
      Close(input);
      debug("[%d] do_stage: input redirected (%d -> %d)\n", getpid(), input,
            STDIN_FILENO);
    }
    if (-1 != output) {
      Dup2(output, STDOUT_FILENO);
      Close(output);
      debug("[%d] do_stage: output redirected (%d -> %d)\n", getpid(), output,
            STDOUT_FILENO);
    }

    /* --- I see here race condition 
     * what if first process died before start of the last?
     * maybe starting everything in subshell would be better?
     * then set pgid as pid of subshell and wait in it for al childs? 
     */
    setpgid(0, pgid);

    Signal(SIGCHLD, SIG_DFL);
    Signal(SIGTSTP, SIG_DFL);
    Signal(SIGTTIN, SIG_DFL);
    Signal(SIGTTOU, SIG_DFL);
    Sigprocmask(SIG_SETMASK, mask, NULL);
    debug("[%d] sginals restored\n", getpid());
  
    int exitcode = 0;
    if ((exitcode = builtin_command(token)) >= 0)
      exit(exitcode);

    debug("[%d] do_stage: shields up, weapons armed - going live\n", getpid());
    external_command(token);
  }

  setpgid(pid, pgid);

  return pid;
}

static void mkpipe(int *readp, int *writep) {
  int fds[2];
  Pipe(fds);
  *readp = fds[0];
  *writep = fds[1];
}

/* Pipeline execution creates a multiprocess job. Both internal and external
 * commands are executed in subprocesses. */
static int do_pipeline(token_t *token, int ntokens, bool bg) {
  pid_t pid, pgid = 0;
  int job = -1;
  int exitcode = 0;

  int input = -1, output = -1, next_input = -1;

  /* --- it's annoying; let's remove it
   * otherwise change 
   * `if (n + p < ntokens)` into `if (0 != n && n + p < ntokens)`
   * mkpipe(&next_input, &output);
   */

  sigset_t mask;
  Sigprocmask(SIG_BLOCK, &sigchld_mask, &mask);

  /* TODO: Start pipeline subprocesses, create a job and monitor it.
   * Remember to close unused pipe ends! */
  for (int n = 0, p = 0; n < ntokens; n += p + 1) {
    /* --- my favourite loop :) */
    for (p = 0; NULL != token[n + p] && T_PIPE != token[n + p]; ++p);
    /* --- T_PIPE -> NULL 
     *       NULL -> NULL
     */
    token[n + p] = NULL;

    /* --- end of pipeline -- print to STDOUT */
    if (n + p < ntokens) {
      mkpipe(&next_input, &output);
    }

    /* --- I don't think it's necessary
     * debug("[%d] do_pipeline: fcntl(%d) magic\n", getpid(), next_input);
     * int mode = fcntl(next_input, F_GETFD);
     * fcntl(next_input, mode | O_CLOEXEC);
     */
    pid = do_stage(pgid, &mask, input, output, token + n, p);
    /* fcntl(next_input, mode); */

    if (0 == pgid) {
      debug("[%d] do_pipeline: %d is first process in pipeline -- set group\n",
            getpid(), pid);
      pgid = pid;
      job = addjob(pgid, bg);
    }

    if (output >= 0) {
      debug("[%d] do_pipeline: close output fd %d\n", getpid(), output);
      Close(output);
    }
    if (input >= 0) {
      debug("[%d] do_pipeline: close input fd %d\n", getpid(), input);
      Close(input);
    }

    input = next_input;
    output = -1;

    debug("[%d] do_pipeline: add job %d to pipeline\n", getpid(), job);
    addproc(job, pid, token + n);
    debug("[%d] do_pipeline: job command '%s'\n", getpid(), jobcmd(job));
  }


  if (!bg) {
    debug("[%d] foreground\n", getpid());
    exitcode = monitorjob(&mask);
  } else {
    msg("[%d] running '%s' %d\n", job, jobcmd(job), pgid);
  }

  Sigprocmask(SIG_SETMASK, &mask, NULL);
  return exitcode;
}

static bool is_pipeline(token_t *token, int ntokens) {
  for (int i = 0; i < ntokens; i++)
    if (token[i] == T_PIPE)
      return true;
  return false;
}

static void eval(char *cmdline) {
  bool bg = false;
  int ntokens;
  token_t *token = tokenize(cmdline, &ntokens);

  if (ntokens > 0 && token[ntokens - 1] == T_BGJOB) {
    token[--ntokens] = NULL;
    bg = true;
  }

  if (ntokens > 0) {
    if (is_pipeline(token, ntokens)) {
      do_pipeline(token, ntokens, bg);
    } else {
      do_job(token, ntokens, bg);
    }
  }

  free(token);
}

int main(int argc, char *argv[]) {
  rl_initialize();

  sigemptyset(&sigchld_mask);
  sigaddset(&sigchld_mask, SIGCHLD);

  initjobs();

  Signal(SIGINT, sigint_handler);
  Signal(SIGTSTP, SIG_IGN);
  Signal(SIGTTIN, SIG_IGN);
  Signal(SIGTTOU, SIG_IGN);

#if DEBUG > 0
  msg("[%d] shell (DEBUG built on " __DATE__ " " __TIME__ ")\n\n", getpid());
#else
  msg("[%d] shell (built on " __DATE__ " " __TIME__ ")\n\n", getpid());
#endif
  msg(""
      "              ,\n"
      "             >++\n"
      "            +++++\n"
      "           +[-<---\n"
      "          --->],[>+\n"
      "         +++++++[-<-\n"
      "        ----->]<<[->+\n"
      "       +++++++++<]>>]<\n"
      "      [->+>+>>>>>>>+<<<\n"
      "     <<<<<<]>>>>++++++++\n"
      "    [-<++++>]>++++++[-<++\n"
      "   +++++>]+>>>++[-<+++++>]\n"
      "  <<<<<<[-[>.<-]<[-<+>>+<]<\n"
      " [->+<]>>>>>[-<.>>+<]>[-<+>]\n"
      ">.<<++<<<-<->]>>>>>>>-[-<<<<<\n"
      "            <.>>>\n"
      "            >>>]<\n"
      "            <<<<.\n\n");
  msg("--- Wesołych Świąt\n");
  msg("--- i szczęśliwego Nowego Roku!\n");
  msg("--- życzy PJ\n\n");
  msg("--- UWAGA! są pewne 'problemy', które łatwo rozwiązać, ale nie wiem\n"
      "    czy podane fixy są 'legalne'.\n"
      "    1. zmiana readline na libedit; w przypadku wielokrotnego zrzucania\n"
      "       w tło procesów wykorzystujących pager tracimy kontrolę nad\n"
      "       wyświetlanymi znakami; taki sam problem tycyz się wzorcówki\n"
      "    2. polecenia wbudowane nie są dobrze obsługiwane w do_job -- aby\n"
      "       to naprawić należy przenieść wywołanie funkcji za forka, ale\n"
      "       jest to zmiana poza TODO powoduje to jednak inny problem\n"
      "       możemy zmusić fg do głupiego zachowania i zawiesić nim shella\n"
      "       odpalając na odpowiednim poleceniu wysłanym w tło\n"
      "    3. podczas wychodzenia z programu powinniśmy zawołać shutdownjobs\n"
      "    4. w shutdownjobs warto zawołać free(jobs) co nie wydaje się takie\n"
      "       oczywiste\n"
      "    NOTKA! używałem libreadline w wersji 8; polecenia były wykonywane\n"
      "           z użyciem bash w wersji 5; glibc 2.29; emulator terminala\n"
      "           yakuake; kernel 5.3.15; gcc 9.2; system 64 bitowy\n"
      "           szczególnie chciałbym zwrócić uwagę na libreadline, które\n"
      "           potrafiło spowodować wyciek pamięci:\n"
      "           /lib/x86_64-linux-gnu/libreadline.so.8+0x3e3f8\n"
      "\n");

  char *line;
  while (true) {
    if (!sigsetjmp(loop_env, 1)) {
      char cwdbuf[4096];
      memset(cwdbuf, 0, 4096);
      if (NULL == getcwd(cwdbuf, 4096)) {
        line = readline("# ");
      }
      else {
        int _n = (int)strlen(cwdbuf);
        cwdbuf[_n] = ':';
        cwdbuf[_n + 1] = ' ';
        line = readline(cwdbuf);
      }
    } else {
      msg("\n");
      continue;
    }

    if (line == NULL)
      break;

    if (strlen(line)) {
      add_history(line);
      eval(line);
    }
    free(line);
    watchjobs(FINISHED);
  }

  msg("\n");
  shutdownjobs();

  return 0;
}
