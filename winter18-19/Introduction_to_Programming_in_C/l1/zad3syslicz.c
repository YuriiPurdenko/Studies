#include"stdio.h"
long double zm=1,x;
int p,d,licz,licz2;
int spr;
//Wiktor Pilarczyk 308533 lista 1 zad 3
int main()
{
	scanf("%Lf%d%d",&x,&p,&d);
	while(zm<x)
	{
		zm*=p;
		licz++;
	}
	if(licz==0)
		putchar('0');
	zm/=p;	
	while(licz--)
	{
		licz2=0;
		while(x>=zm)
		{
			licz2++;
			x-=zm;
		}
		if(licz2<10)
			printf("%d",licz2);
		else
			printf("%c",'A'+licz2-10);
		zm/=p;
	}
	if(d>0)
		putchar(',');
	while(d--)
	{
		licz2=0;
		while(x>=zm)
		{
			licz2++;
			x-=zm;
		}
		if(licz2<10)
			printf("%d",licz2);
		else
			printf("%c",'A'+licz2-10);
		zm/=p;	
	}
}
