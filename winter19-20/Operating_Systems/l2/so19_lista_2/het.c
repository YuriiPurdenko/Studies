#include "csapp.h"

static int ndselect(int n) {
    for(int g = 0; g < n; g++){          //uruchamianie podprocesu na kazdym z pol
        //printf("%d Forkuje\n",getpid());
        int rc = Fork();
        if(rc == 0){
            //printf("\nPole %d Syna %d\n",g,getpid());  
            return g;        
        }
        if(rc > 0)
            Waitpid(rc,NULL,0);
    }
  exit(0);
}

static int conflict(int x1, int y1, int x2, int y2) {
  return x1 == x2
    || y1 == y2
    || x1 + y1 == x2 + y2
    || x1 - y1 == x2 - y2;
}

static void print_line_sep(int size) {
  for (int i = 0; i < size; ++i) 
    printf("+---");
  printf("+\n");
}

static void print_board(int size, int board[size]) {
  for (int i = 0; i < size; ++i) {
    print_line_sep(size);
    for (int j = 0; j < size; ++j)
      printf("|%s", board[i] == j ? " Q " : "   ");
    printf("|\n");
  }
  print_line_sep(size);
  printf("\n");
}

int main(int argc, char **argv) {
  if (argc != 2)
    app_error("Usage: %s [SIZE]", argv[0]);

  int size = atoi(argv[1]);

  if (size < 3 || size > 9)
    app_error("Give board size in range from 4 to 9!");

  int board[size];
    int k = 0;
    while(1){
        //printf("Pid: %d k: %d\n",getpid(),k);
        if(k == size)
            break;
        board[k] = ndselect(size);
        for(int g = 0; g < k; g++){           
            if(conflict(g, board[g], k, board[k])){
                //printf("Zabijam PID: %d K: %d Plansza: %d \n",getpid(),k,board[k]);             
               exit(0);
            }
        }
        k++;
    }
  print_board(size, board);

  return 0;
}
