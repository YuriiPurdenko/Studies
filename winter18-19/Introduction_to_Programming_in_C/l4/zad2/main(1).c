#include "wielomian.h"

//Wiktor Pilarczyk 308533 zad 2 lista 4

int main()
{

    printf("Podaj wartość x: ");
    wielomian a,b,c;
    wymierna x=wczyt();
    wczytwiel(a);
    wypiszwiel(a);
    wartosc(x,a);
    wczytwiel(b);
    wypiszwiel(b);
    wartosc(x,b);
    dodajwiel(a,b,c);
    wypiszwiel(c);
    wartosc(x,c);
    odejwiel(a,b,c);
    wypiszwiel(c);
    wartosc(x,c);
    mnozwiel(a,b,c);
    wypiszwiel(c);
    wartosc(x,c);
}
