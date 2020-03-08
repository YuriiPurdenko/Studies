#include "kolej.h"
//Wiktor Pilarczyk 308533 Lista 6 Zad 1
void ini(Kolejka k,double t[],int maksik)
{
    k[0]=k[1]=k[2]=&t[0];
    k[3]=&t[0]+maksik;
    return;
}
void oproz(Kolejka k)
{
    k[0]=k[1];
    return;
}
void doppocz(double x, Kolejka k)
{
    if(mogedop(k))
    {
        if(k[0]==k[2])
            k[0]=k[3];
        else
            k[0]--;
        *k[0]=x;
    }
    else
        printf("Kolejka jest pelna\n");
    return;
}
void dopkon(double x, Kolejka k)
{
    if(mogedop(k))
    {
        *k[1]=x;
        if(k[1]==k[3])
            k[1]=k[2];
        else
            k[1]++;
    }
    else
        printf("Kolejka jest pelna\n");
    return;
}
double pocz(Kolejka k)
{
    double zm=*k[0];
    if(pusta(k))
    {
        printf("Kolejka jest pusta!");
        exit (EXIT_FAILURE);
        return zm;
    }
    if(k[0]==k[3])
        k[0]=k[2];
    else
        k[0]++;
    return zm;
}
double kon(Kolejka k)
{
    if(pusta(k))
    {
        printf("Kolejka jest pusta!");
        exit (EXIT_FAILURE);
        return *(k[1]);
    }
    if(k[1]==k[2])
        k[1]=k[3];
    else
        k[1]--;
    return  *(k[1]);
}
bool pusta(Kolejka k)
{
    if(k[0]==k[1])
        return 1;
    return 0;
}
bool mogedop(Kolejka k)
{
    if((k[0]==k[2] && k[1]==k[3]) || (k[1]-k[0])==-1)
        return 0;
    return 1;
}
