#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

void gen(double t[],int x); //generator ciagow
bool spr(double t[],double z[],int x);	//sprawdzam czy ciag z, jest posortowanym ciagiem t
int compare(const void * a, const void * b);
void szsort(double t[],int x);	//qsort	O(nlogn)
void mojsort(double t[],int x);	//sortowanie poprzez przez wybieranie O(n^2)
double czas(void (*fun)(double [],int),int x);	//funkcja liczaca czas dzialania f. sort. fun dla 100 zestawow o dlugosci x
//Wiktor Pilarczyk 308533 Zad 2 Lista 6

