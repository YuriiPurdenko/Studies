#include "csapp.h"

static char buf[256];

#define LINE1 49
#define LINE2 33
#define LINE3 78

static void do_read(int fd) {
  pid_t pid = Fork();

  if (pid == 0) {
    printf("(fork) [%d] lseek: %lu\n", getpid(), Lseek(fd, 0, SEEK_CUR));
    ssize_t rc = Read(fd, buf, 255);
    printf("(fork) [%d] %lu: %s\n", getpid(), rc, buf);
    printf("(fork) [%d] lseek: %lu\n", getpid(), Lseek(fd, 0, SEEK_CUR));
    Close(fd);
  } else {
    printf("[%d] lseek: %lu\n", getpid(), Lseek(fd, 0, SEEK_CUR));
    int status;
    Waitpid(pid, &status, 0);
    printf("[%d] lseek: %lu\n", getpid(), Lseek(fd, 0, SEEK_CUR));
    ssize_t rc = Read(fd, buf, 255);
    printf("[%d] %lu: %s\n", getpid(), rc, buf);
    Close(fd);
  }

  exit(0);
}

static void do_close(int fd) {
  pid_t pid = Fork();

  if (pid != 0) {
    Close(fd);
    printf("[%d] closed %d\n", getpid(), fd);
  }

  if (pid == 0) {
    printf("(fork) [%d] %ld\n", getpid(), Read(fd, buf, 1));
    Close(fd);
  }

  exit(0);
}

int main(int argc, char **argv) {
  if (argc != 2)
    app_error("Usage: %s [read|close]", argv[0]);

  int fd = Open("test.txt", O_RDONLY, 0);

  if (!strcmp(argv[1], "read"))
    do_read(fd);
  if (!strcmp(argv[1], "close"))
    do_close(fd);
  app_error("Unknown variant '%s'", argv[1]);
}
