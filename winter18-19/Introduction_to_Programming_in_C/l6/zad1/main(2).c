#include "kolej.h"
//Wiktor Pilarczyk 308533 Lista 6 Zad 1
int main()
{
    double tab[100];
    Kolejka x;
    ini(x,tab,10);
    //printf("%f\n",pocz(x));
    doppocz(1.2,x);
    doppocz(1.2,x);
    doppocz(1.2,x);
    doppocz(1.2,x);
    doppocz(1.2,x);
    doppocz(1.2,x);
    doppocz(1.2,x);
    doppocz(1.2,x);
    doppocz(1.2,x);
    doppocz(1.2,x);
    doppocz(1.2,x);
    doppocz(1.2,x);
    printf("%f\n",pocz(x));
    dopkon(1.5,x);
    printf("%f\n",kon(x));
    if(pusta(x))
        printf("PUSTA\n");
    dopkon(1.5,x);
    if(pusta(x))
        printf("PUSTA\n");
    oproz(x);
    if(pusta(x))
        printf("PUSTA\n");
    return 0;
}
