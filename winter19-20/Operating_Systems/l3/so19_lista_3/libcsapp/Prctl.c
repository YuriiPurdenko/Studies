#include "csapp.h"

void Prctl(int option, long arg) {
  if (prctl(option, arg) < 0) 
    unix_error("Prctl error");
}
