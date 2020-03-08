#include"stdio.h"
#include"stdlib.h"
#include "stdbool.h"
//Wiktor Pilarczyk 308533 Lista 6 Zad 1

typedef double *Kolejka[4];

void ini(Kolejka k,double t[],int maksik);
void oproz(Kolejka k);
void doppocz(double x, Kolejka k);
void dopkon(double x, Kolejka k);
double pocz(Kolejka k);
double kon(Kolejka k);
bool pusta(Kolejka k);
bool mogedop(Kolejka k);
