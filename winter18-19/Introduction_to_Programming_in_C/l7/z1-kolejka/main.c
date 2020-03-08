#include"kolejka2.h"
//Wiktor Pilarczyk 308533

int main()
{
    double zm;
    Kolejka kol=init();
    if(pusta(kol))
        printf("\nDziala\n");
    drukkol(kol);
    printf("\nDlugosc %d\n",dlug(kol));
    kol=dodkon(1,kol);
    drukkol(kol);
    printf("\nDlugosc %d\n",dlug(kol));
    printf("\nPoczatek %lf\n",wartpocz(kol));
    kol=dodpocz(2,kol);
    drukkol(kol);
    kol=usun(kol,&zm);
    printf("\nPoczatek %lf\n",zm);
    printf("\nDlugosc %d\n",dlug(kol));
    kol=dodkon(2,kol);
    drukkol(kol);
    kol=dodkon(3,kol);
    drukkol(kol);
    kol=dodpocz(4,kol);
    drukkol(kol);
    kol=dodkon(5,kol);
    drukkol(kol);
}
