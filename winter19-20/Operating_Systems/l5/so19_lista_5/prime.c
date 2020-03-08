#include "csapp.h"
#include "pipeio.h"

static bool ReadNum(pipe_t p, long *valp) {
  return Read(p.read, valp, sizeof(long)) == sizeof(long);
}

static bool WriteNum(pipe_t p, long val) {
  return Write(p.write, &val, sizeof(long)) == sizeof(long);
}

static noreturn void generator(pipe_t out, long maxprime) {
  for (long n = 2; n <= maxprime; n++)
    WriteNum(out, n);
  exit(EXIT_SUCCESS);
}

static void filter(pipe_t in, pipe_t out, long prime) {
  long num;
  while (ReadNum(in, &num)) {
    if (num % prime != 0)
      WriteNum(out, num);
  }
}

static noreturn void filter_chain(pipe_t in) {
  long prime;

    static int count = 0;
    pipe_t fil_pipe = MakePipe();
    if(ReadNum(in, &prime) == 0){
        safe_printf("\n%d\n", count);
        CloseReadEnd(in);
        exit(EXIT_SUCCESS);
    }
    safe_printf("%ld ", prime);
    if(Fork()){
        CloseReadEnd(fil_pipe);
        filter(in, fil_pipe, prime);        
        CloseReadEnd(in);   
        CloseWriteEnd(fil_pipe);
        Wait(NULL);
    }
    else{
        count++;
        CloseReadEnd(in);
        CloseWriteEnd(fil_pipe);
        filter_chain(fil_pipe);
    }
  /* TODO: Something is missing here! */

  exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {
  if (argc != 2)
    app_error("Usage: %s [MAXPRIME]", argv[0]);

  long maxprime = atol(argv[1]);

  if (maxprime < 2 || maxprime > 10000)
    app_error("Give maximum prime number in range from 2 to 10000!");

  /* Spawn generator. */
  pipe_t gen_pipe = MakePipe();
  if (Fork()) { /* parent */
    CloseWriteEnd(gen_pipe);
  } else { /* child */
    CloseReadEnd(gen_pipe);
    generator(gen_pipe, maxprime);
  }

  /* Spawn filter chain. */
  if (Fork()) { /* parent */
    CloseReadEnd(gen_pipe);
  } else { /* child */
    filter_chain(gen_pipe);
  }

  for (int i = 0; i < 2; i++)
    Wait(NULL);

  return 0;
}
