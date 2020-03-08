#include"sort2.h"
//Wiktor Pilarczyk 308533 Zad 2 Lista 6

int main()
{
	printf("\nDla funkcji qsort i dlugosci 5000:\n");
	czas(szsort,5000);
	printf("\nDla funkcji mojsort i dlugosci 5000:\n");
	czas(mojsort,5000);
	printf("\nDla funkcji qsort i dlugosci 10000:\n");
	czas(szsort,10000);
	printf("\nDla funkcji mojsort i dlugosci 10000:\n");
	czas(mojsort,10000);
	printf("\nDla funkcji qsort i dlugosci 50000:\n");
	czas(szsort,50000);
	printf("\nDla funkcji mojsort i dlugosci 50000:\n");
	czas(mojsort,50000);
	return 0;
}
