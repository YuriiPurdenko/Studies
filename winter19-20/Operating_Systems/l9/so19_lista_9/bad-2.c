#include "csapp.h"

int main(int argc, char *argv[]) {
  if (argc < 2)
    app_error("Give me a string!");
  const char *s = argv[1];
  //char *p = malloc(strlen(s));
  char *p = malloc(strlen(s) + 1);
  strcpy(p, s); //nie ma dodatkowego bajtu na /0
  free(p);
  return 0;
}
