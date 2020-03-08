#include<stdio.h>
#include<ctype.h>
char t[10],t1[10];
int ca[11],ca1[11],wyn[11];
int wczyt(char x[],int n);
void prze(char x[],int y[]);
int dodaj(int x[],int y[],int n,int z[]);
int odej(int x[],int y[],int n,int z[]);
int mno (int x[],int y[],int n,int z[]);
void wypis(int x[]);
//Wiktor Pilarczyk 308533 3lista zad 1
int main()					//boolik sluzy do sprawdzenia czy nie przekroczono tablicy
{
	wczyt(t,10);
	wczyt(t1,10);
	prze(t,ca);
	prze(t1,ca1);
	mno(ca,ca1,10,wyn);
	wypis(wyn);
}
int wczyt(char x[],int n)		//wczytywanie liczby  do tablicy charow 
{
	int licz=0;
	char c;
	c=getchar();
	while(isspace(c))
		c=getchar();
	x[licz]=c;
	while(1)
	{	
		licz++;
		c=getchar();
		if(isspace(c) || licz>=n-1)
		{
			if(isspace(c))
				ungetc(c,stdin);
			break;
		}
		else
			x[licz]=c;
	}
	x[licz+1]='\0';
	if(isspace(c))
		return 1;
	else
		return 0;
}
void prze(char x[],int y[])		//konwertowanie tablicy charow na tablice intow
{
	int licz=0;
	while(1)
	{
		if(x[licz]!='\0')
			licz++;
		else
			break;
	}
	y[0]=licz;
	for(int g=1;g<=licz;g++)
	{
		y[licz-g+1]=x[g-1]-'0';
	}
}
int dodaj(int x[],int y[],int n,int z[])	//dodawanie liczb
{
	int przekr,zm,licz=1,boolik=0;		//boolik sluzy do sprawdzenia czy nie przekroczono tablicy
	if(x[0]>y[0])				
		zm=x[0];
	else
		zm=y[0];
	for(int g=1;g<=zm;g++)
	{
		licz++;
		if(g>n)
		{
			boolik=1;
			break;
		}
		z[g]=x[g]+y[g];
		if(z[g]>9)			//sprawdzam czy liczba w komorce jest cyfra
		{	
			if(g+1>n)
			{
				boolik=1;
				break;
			}
			przekr=z[g]/10;
			z[g]%=10;
			z[g+1]+=przekr;
		}
	}
	z[0]=licz-1;
	if(licz<n && z[licz]!=0)		
		z[0]=licz;
	z[z[0]+1]='\0';	
	return boolik;
}
int odej(int x[],int y[],int n,int z[])	//odejmowanie liczb
{
	int przekr,zm,licz=1,boolik=0, boolik2=0;		//boolik sluzy do sprawdzenia czy nie przekroczono tablicy
	if(x[0]>y[0])
		zm=x[0];
	else
		zm=y[0];
	for(int g=zm;g>1;g--)					//sprawdzam ktora liczba jest wieksza, aby odejmowac "mniejsza od wiekszej", przy zachowaniu znaku
	{
		if(x[g]>y[g])
		{
			boolik2=0;
			break;
		}
		if(x[g]<y[g])
		{
			boolik2=1;
			break;
		}
	}
	if(boolik2==0)
	{
		for(int g=1;g<=zm;g++)
		{
			licz++;
			if(g>n)
			{
				boolik=1;
				break;
			}
			z[g]+=x[g]-y[g];
			if(z[g]<0)
			{	
				if(g+1>n)
				{
					boolik=1;
					break;
				}
				z[g]+=10;
				z[g+1]-=1;
			}
		}
	}
	else
	{
		printf("-");				
		for(int g=1;g<=zm;g++)
		{
			licz++;
			if(g>n)
			{
				boolik=1;
				break;
			}
			z[g]+=y[g]-x[g];
			if(z[g]<0)
			{	
				if(g+1>n)
				{
					boolik=1;
					break;
				}
				z[g]+=10;
				z[g+1]-=1;
			}
		}
	}
	licz=1;
	for(int g=1;g<n;g++)
		if(z[g]>0)
			licz=g;
	z[0]=licz;
	z[licz+1]='\0';
	return boolik;
}
int mno(int x[],int y[],int n,int z[])		//mnozenie liczb
{
	int przekr,zm,licz=1,boolik=0;
	if(x[0]>y[0])				//mnoze liczbe wieksza przez mniejsza
	{
		for(int g=1;g<=y[0];g++)
		{
			for(int h=1;h<=x[0];h++)			//wykonuje mnozenie "pisemne"
			{
				if(h+g-1<n)
					z[h+g-1]+=(y[g]*x[h]);
				else
					break;
			}
		}
		for(int g=1;g<n;g++)	
		{
			if(z[g]>0)
				licz=g;
			if(z[g]>9)
			{
				przekr=z[g]/10;
				z[g]%=10;
				if(g+1<n)
				{
					z[g+1]+=przekr;
				}
				else
				{
					if(g+1>=n && z[g]>9)
					{
						boolik=1;
						z[g]%=10;
						break;
					}
				}
				z[g]%=10;
			}
			if(z[g]>0)
				licz=g;
		}
	}
	else
	{
		for(int g=1;g<=x[0];g++)
		{
			for(int h=1;h<=y[0];h++)
			{
				if(h+g-1<n)
					z[h+g-1]+=x[g]*y[h];
				else
					break;
			}
		}
		for(int g=1;g<n;g++)
		{
			if(z[g]>0)
				licz=g;
			if(z[g]>10)
			{
				przekr=z[g]/10;
				if(g+1<n)
				{
					z[g+1]+=przekr;
				}
				else
				{
					if(g+1>=n && z[g]>9)
					{
						boolik=1;
						z[g]%=10;
						break;
					}
				}
				z[g]%=10;
			}
			if(z[g]>0)
				licz=g;
		}
	}
	z[n]='\0';
	z[0]=licz;
	return boolik;
}
void wypis(int x[])	//wypisywanie liczby
{
	for(int g=x[0];g>0;g--)
		printf("%d",x[g]);
}

