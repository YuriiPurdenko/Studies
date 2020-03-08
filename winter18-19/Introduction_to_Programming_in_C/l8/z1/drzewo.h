#include"stdio.h"
#include <stdlib.h>
#include"stdbool.h"
#include"math.h"
#include<ctype.h>
//Wiktor Pilarczyk 308533 Z1 L8

typedef struct wezel{
    bool licz;
    char op;
    double arg;
    struct wezel *lewy,*prawy;
} *NaWezel, Wezel;


NaWezel czytaj();
NaWezel tworz(double x);
NaWezel znak(char x);
NaWezel lacz(NaWezel x, NaWezel l, NaWezel p);
void wypisz(NaWezel x,char ost, bool lewe);
bool przem(char x);
bool prior(char x);
