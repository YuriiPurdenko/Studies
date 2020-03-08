#include "wymierne.h"

//Wiktor Pilarczyk 308533 zad 1 lista 4
wymierna n,m,x;
int main()
{
    n=wczyt();
    m=wczyt();
    x=suma(n,m);
    wypi(x);
    x=odej(n,m);
    wypi(x);
    x=mnoz(n,m);
    wypi(x);
    x=dziel(n,m);
    wypi(x);
}
