#include "csapp.h"

static char buf[256];

#define LINE1 49
#define LINE2 33
#define LINE3 78

static void do_read(int fd) {
        
    int rc = Fork();
    volatile int x;
    if(rc == 0){
        scanf("%d",&x);
        x = Lseek(fd,0,SEEK_CUR);
        printf("PID: %d dziecko, pozycja kursora: %p\n",getpid(),x);
        Read(fd, buf, 69);
        x = Lseek(fd,0,SEEK_CUR);
        printf("PID: %d dziecko, pozycja kursora: %p\n",getpid(),x);
        printf("%s ",buf);
    }
    if(rc < 0)
        fprintf(stderr, "fork failed\n");
    if(rc > 0){
        x = Lseek(fd,0,SEEK_CUR);
        printf("PID: %d rodzic, pozycja kursora: %p\n",getpid(),x);
        Read(fd, buf, 49);
        x = Lseek(fd,0,SEEK_CUR);
        printf("PID: %d rodzic, pozycja kursora: %p\n",getpid(),x);
        printf("%s ",buf);
        
        Waitpid(rc,NULL,WNOHANG);
    }  
  exit(0);
}
static void do_close(int fd) {
    int rc = Fork();
    if(rc == 0){
        int x;
        scanf("%d",&x);
        printf("PID: %d Dziecko skonczylo\n",getpid());
        Close(fd);
    }
    if(rc > 0){        
        printf("PID: %d Rodzic zamknal deskryptor\n",getpid());
        Close(fd);
        Waitpid(rc,NULL,WNOHANG);
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
