#include "apue.h"

#include <fcntl.h>

#define BUFFSIZE 4096

int main(int argc, char* argv[]) {
  int n;
  char buf[BUFFSIZE];

  if (argc < 2)
    err_sys("path to file is required");

  int fd = open(argv[1], O_RDONLY);

  if (fd == -1)
    err_sys("open error");

  while ((n = read(fd, buf, BUFFSIZE)) > 0)
    if (write(STDOUT_FILENO, buf, n) != n)
      err_sys("write error");

  if (n < 0)
    err_sys("read error");

  exit(0);
}
