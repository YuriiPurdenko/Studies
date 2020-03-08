#include<stdio.h>
int licz=0,m,n,boolik=0,liczglo=0;
char t[209],zm;
void init();
void wczyt();
void wypiszspa();
void wypiszslo();
//Wiktor Pilarczyk 308533 Lista 2 Zad 2
int main(int argc, char *argv[])
{
	if(argc<=1)			//sprawdza i wczytuje argumenty wywolania programu
	{	
		printf("Brak argumentow\n");
		return 1;
	}
	sscanf(argv[1],"%d",&m);	
	sscanf(argv[2],"%d",&n);
	if(m>=n)
	{
		printf("Niepoprawne argumenty\n");
		return 1;
	
	}
	while(1)
	{
		wczyt();
		if(licz>n-m+1)		//sprawdza poprawna dlugosc slowa			
		{
			printf("\nPodano za dlugie slowo\n");	
			break;
		}
		else			
		{
			liczglo+=licz;
			if(licz==liczglo)	//sluzy do wypisania pierwszego slowa
			{
				wypiszspa();
				wypiszslo();
			}
			else
			{
				liczglo++;		//'dodaje' spacje
				if(liczglo<=n-m+1)	//sprawdzam czy moge dopisac slowo do ostatniego wiersza
				{
					putchar(' ');
					wypiszslo();
				}
				else			//jesli nie zaczynam od nastepnego wiersza
				{		
					putchar('\n');
					wypiszspa();
					wypiszslo();
					liczglo=licz;
				}
			}
		}
		if(boolik==1)		//jesli byl koniec wejscia, konczy program
			break;
	}
}
void wczyt()	//wczytuje slowa 
{
	licz=0;
	while(1)
	{
		zm=getchar();
		if(zm==EOF)			//sprawdza czy to juz koniec wejscia
		{
			boolik=1;
			break;
		}
		if(zm=='\n' || zm==' ')		//sprawdza czy koniec slowa
			break;
		t[licz]=zm;
		licz++;
		if(licz>n-m+1)			//sprawdza czy slowo nie jest za dlugie
			break;
	}
}
void wypiszspa()	//wypisuje m-1 spacji
{
	for(int g=1;g<m;g++)	
		putchar(' ');
}
void wypiszslo()
{
	for(int g=0;g<licz;g++)	//wypisuje dane slowo 
		putchar(t[g]);
}
