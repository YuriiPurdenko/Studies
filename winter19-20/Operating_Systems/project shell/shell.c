#include <readline/readline.h>
#include <readline/history.h>

#define DEBUG 0
#include "shell.h"

sigset_t sigchld_mask;

static sigjmp_buf loop_env;

static void sigint_handler(int sig) {
  siglongjmp(loop_env, sig);
}

/* Rewrite closed file descriptors to -1,
 * to make sure we don't attempt do close them twice. */
static void MaybeClose(int *fdp) {
  if (*fdp < 0)
    return;
  Close(*fdp);
  *fdp = -1;
}

/* Consume all tokens related to redirection operators.
 * Put opened file descriptors into inputp & output respectively. */
static int do_redir(token_t *token, int ntokens, int *inputp, int *outputp) {
  token_t mode = NULL; /* T_INPUT, T_OUTPUT or NULL */
  int n = 0;           /* number of tokens after redirections are removed */

  for (int i = 0; i < ntokens; i++) {
    /* TODO: Handle tokens and open files as requested. */
    mode = token[i];
    if(mode == T_INPUT){
      token[i] = NULL;
      i++;
      MaybeClose(inputp);
      *inputp = Open(token[i], O_CREAT|O_RDONLY, 0644); 
      token[i] = NULL;
    }
    else{
      if(mode == T_OUTPUT){
        token[i] = NULL;
        i++;
        MaybeClose(outputp);
        *outputp = Open(token[i], O_CREAT|O_WRONLY|O_TRUNC, 0644); 
        token[i] = NULL;
      } 
      else{
        if(mode == T_APPEND){
          token[i] = NULL;
          i++;
          MaybeClose(outputp);
          *outputp = Open(token[i], O_CREAT|O_WRONLY|O_APPEND, 0644); 
          token[i] = NULL;
        } 
        else{
          token[n] = token[i];
          n++;
        }
      }
    }
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

  if (!bg) {
    if ((exitcode = builtin_command(token)) >= 0)
      return exitcode;
  }

  sigset_t mask;
  Sigprocmask(SIG_BLOCK, &sigchld_mask, &mask);

  /* TODO: Start a subprocess, create a job and monitor it. */
  pid_t pid = Fork();
  int job = -1;
  if(pid == 0){ //child
    Setpgid(0, 0);  //przywrocenie maski sygnalow dziecku
    Sigprocmask(SIG_SETMASK, &mask, NULL);
    Signal(SIGCHLD, SIG_DFL);
    Signal(SIGTSTP, SIG_DFL);
    Signal(SIGTTIN, SIG_DFL);
    Signal(SIGTTOU, SIG_DFL);

    if(input != -1){
      dup2(input, STDIN_FILENO);
      Close(input);
    }
    if(output != -1){
      dup2(output, STDOUT_FILENO);
      Close(output);
    }
    if ((exitcode = builtin_command(token)) >= 0)
      exit(exitcode);
    external_command(token);
  }
  //parent
  Setpgid(pid, pid);
  job = addjob(pid, bg);
  addproc(job, pid, token);

  MaybeClose(&input);
  MaybeClose(&output);

  if(!bg)
    exitcode = monitorjob(&mask);
  else
    msg("[%d] running '%s'\n",job, jobcmd(job));
  
  Sigprocmask(SIG_SETMASK, &mask, NULL);
  return exitcode;
}

/* Start internal or external command in a subprocess that belongs to pipeline.
 * All subprocesses in pipeline must belong to the same process group. */
static pid_t do_stage(pid_t pgid, sigset_t *mask, int input, int output,
                      token_t *token, int ntokens) {
  int in = input, out = output; //problem with multiple time closing the same file descriptor in do_stage and do_pipeline
  ntokens = do_redir(token, ntokens, &input, &output);

  if (ntokens == 0)
    app_error("ERROR: Command line is not well formed!");

  /* TODO: Start a subprocess and make sure it's moved to a process group. */
  pid_t pid = Fork();

  if(pid == 0){                             //child    
    Setpgid(0, pgid);                       //przywrocenie domyslnej obslugi sygnalow
    Sigprocmask(SIG_SETMASK, mask, NULL);
    Signal(SIGCHLD, SIG_DFL);
    Signal(SIGTSTP, SIG_DFL);
    Signal(SIGTTIN, SIG_DFL);
    Signal(SIGTTOU, SIG_DFL);

    if(input != -1){
      dup2(input, STDIN_FILENO);
      Close(input);
    }
    if(output != -1){
      dup2(output, STDOUT_FILENO);
      Close(output);
    }
    int exitcode = 0;
    if ((exitcode = builtin_command(token)) >= 0)
      exit(exitcode);
    external_command(token);
  }
  //parent
  if(in != input)           //zamykanie deskryptorow otwartych przez do_redir
    MaybeClose(&input);
  if(out != output)
    MaybeClose(&output);

  Setpgid(pid, pgid);   //aby nie nastopil race
  return pid;
}

static void mkpipe(int *readp, int *writep) {
  int fds[2];
  Pipe(fds);
  fcntl(fds[0], F_SETFD, FD_CLOEXEC);
  fcntl(fds[1], F_SETFD, FD_CLOEXEC);
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

  mkpipe(&next_input, &output);

  sigset_t mask;
  Sigprocmask(SIG_BLOCK, &sigchld_mask, &mask);

  /* TODO: Start pipeline subprocesses, create a job and monitor it.
   * Remember to close unused pipe ends! */
  for(int i = 0; i < ntokens; i++){
    int p = i;
    for(;token[p] != NULL && token[p] != T_PIPE && p < ntokens; p++){}  //szukanie nastepnego | lub dotarcie do konca
    
    token[p] = NULL; 

    if(p < ntokens && i != 0) //for i == 0 already created
      mkpipe(&next_input, &output);

    pid = do_stage(pgid, &mask, input, output, token + i, p - i);

    if(i == 0){ //jesli pierwszy proces w potoku to tworze job'a
      pgid = pid;
      job = addjob(pgid, bg);
    }
    
    MaybeClose(&input); //zamykam deskryptory potoku
    MaybeClose(&output);
    addproc(job, pid, token + i);
    input = next_input;
    i = p;
  }
  if(!bg)
    exitcode = monitorjob(&mask);
  else
    msg("[%d] running '%s'\n",job, jobcmd(job));
  
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

  Setpgid(0, 0);

  initjobs();

  Signal(SIGINT, sigint_handler);
  Signal(SIGTSTP, SIG_IGN);
  Signal(SIGTTIN, SIG_IGN);
  Signal(SIGTTOU, SIG_IGN);

  char *line;
  while (true) {
    if (!sigsetjmp(loop_env, 1)) {
      line = readline("# ");
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
