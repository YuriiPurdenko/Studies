#include"stdio.h"
#include"stdlib.h"
#include"stdbool.h"
#include"math.h"
//Wiktor Pilarczyk 308533

typedef struct wezel{
    double info;
    struct wezel *nast;
}   *Kolejka,Wezel;

Kolejka init();
Kolejka dodpocz(double x,Kolejka k);
Kolejka dodkon(double x,Kolejka k);
double wartpocz(Kolejka k);
Kolejka usun(Kolejka k,double *a);
bool pusta(Kolejka k);
int dlug(Kolejka k);
void drukkol(Kolejka k);
