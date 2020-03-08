#include "csapp.h"

#define BLKSZ 4096 /* block size */
#define NBLKS 8192 /* number of blocks written to a file */
#define WRAP 64

int main(void) {
  int fd = Open("holes.bin", O_CREAT | O_TRUNC | O_WRONLY, 0644);

  int usedblks = 0;

  for (int i = 0; i < NBLKS; i++) {
    if (i % WRAP == 0)
      dprintf(STDERR_FILENO, "%04d ", i);
    if (random() % 64) {
      Lseek(fd, BLKSZ, SEEK_CUR);
      Write(STDERR_FILENO, ".", 1);
    } else {
      char blk[BLKSZ];
      for (int j = 0; j < BLKSZ; j++)
        blk[j] = random();
      Write(fd, blk, BLKSZ);
      Write(STDERR_FILENO, "O", 1);
      usedblks++;
    }
    if (i % WRAP == WRAP - 1)
      Write(STDERR_FILENO, "\n", 1);
  }

  dprintf(STDERR_FILENO, "Non-zero blocks: %d\n", usedblks);
  
  struct stat stat_data;
  Fstat(fd, &stat_data);
  Close(fd);

  dprintf(STDERR_FILENO, "st_blksize: %ld\n", stat_data.st_blksize);
  dprintf(STDERR_FILENO, "st_blocks:  %ld\n", stat_data.st_blocks);
  dprintf(STDERR_FILENO, "sie zgadza: %d * %d = %d\n", BLKSZ / 512, usedblks, BLKSZ / 512 * usedblks); 
  dprintf(STDERR_FILENO, "real size:  %ld\n", stat_data.st_blocks * 512);
  dprintf(STDERR_FILENO, "st_size:    %ld\n", stat_data.st_size);

  return 0;
}
