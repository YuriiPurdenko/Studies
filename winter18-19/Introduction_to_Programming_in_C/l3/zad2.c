#include <stdio.h>
#include <stdbool.h>
#define wiel 1003							
char c;
void wczyt (int x, int y, int tab [] [wiel] );				//wczytuje do podanej tablicy, tablice o rozmiarach x y
void wypisz (int x, int y, int tab [] [wiel] );				//wypisuje tablice o rozmiarach x y
void obroc (int x, int y, int z, int tab [] [wiel]); 			//obraca podana tablice, za pomoca pomocniczej o podanych rozmiarach oraz o podany kat

int t[wiel][wiel];

//Wiktor Pilarczyk zadanie 2 lista 3 308533
int main()
{
	wczyt(5,5,t);			
	wypisz(5,5,t);
	obroc(5,5,180,t);
	wypisz(5,5,t);
	obroc(5,5,90,t);
	wypisz(5,5,t);
	obroc(5,5,270,t);
	wypisz(5,5,t);
}

void wczyt (int x,int y, int tab[][wiel])
{
	for(int g=0;g<y;g++)
		for(int h=0;h<x;h++)
		{
			c=getchar();
			if(c=='\n')			//ignoruje wczytywanie spacji
				c=getchar();
			if(c=='X')			//jesli znak jest'X' zapisuje jego pozycje w tablicy
				tab[h][g]=1;
		}
}
void wypisz (int x, int y,int tab[][wiel])
{
	for(int g=0;g<y;g++)
	{	
		for(int h=0;h<x;h++)			
		{
			if(tab[h][g]==1)		
				putchar('X');
			else
				putchar(' ');
		}
		putchar('\n');
	}
}
void obroc (int x, int y,int z,int tab[][wiel],int pom[][wiel])
{
	for(int g=0;g<y;g++)
		for(int h=0;h<x;h++)
			pom[g][h]=tab[g][h];
	switch(z)
	{
		case 180:						//obraca o 180
			for(int g=y-1;g>=0;g--)
				for(int h=x-1;h>=0;h--)
					tab[y-g-1][x-h-1]=pom[g][h];
			break;
		case 270:						//obraca o 270
			for(int g=0;g<y;g++)		
				for(int h=0;h<x;h++)
					tab[h][y-g-1]=pom[g][h];
			break;
		case 90:						//obraca o 90
			for(int g=0;g<y;g++)
				for(int h=0;h<x;h++)
					tab[x-h-1][g]=pom[g][h];
			break;
	}
}
