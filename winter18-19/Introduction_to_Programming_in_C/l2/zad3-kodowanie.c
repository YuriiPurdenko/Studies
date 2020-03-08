#include<stdio.h>
int x,y,a,b,licz,boolik,wsk,licz2;
char zm,o,t[1009][1009],t2[1000009];
void wczyt();
void koder();
void dekoder();
//Wiktor Pilarczyk 308533 lista 2 zad 3
int main()
{
	wczyt();
	if(o=='K')
		koder();
	else
		dekoder();
}

void wczyt()
{
	scanf("%c%d%d",&o,&x,&y);
	while ((zm=getchar())!=EOF && zm!='\n');
}
void koder()
{
	while(1)
	{
		a=x-1;
		b=y;
		licz=0;
		for(int g=0;g<y*x;g++)
		{
			zm=getchar();
			if(zm==EOF)
			{
				boolik=1;
				licz=g;
				break;
			}
			licz++;
			t2[g]=zm;
		}
		for(int g=0;g<x;g++)
		{
			wsk=0;
			for(int h=0;h<y;h++)
			{
				if(g+wsk<licz)
					putchar(t2[g+wsk]);
				wsk+=x;
			}
		}
		if(boolik==1)
			break;
	}
}
void dekoder()
{
	while(1)
	{
		licz=0;
		for(int g=0;g<x*y;g++)
		{
			zm=getchar();
			if(zm==EOF)
			{
				boolik=1;
				licz=g;
				break;
			}
			licz++;
			t2[g]=zm;
		}
		licz2=licz;
		if(licz%x==0)
			licz/=x;
		else
		{
			licz/=x;
			licz++;
		}
		for(int g=0;g<licz;g++)
		{
			wsk=0;
			for(int h=0;h<x;h++)
			{
				if(g+wsk<licz2)
					putchar(t2[g+wsk]);
				wsk+=licz;
			}
		}
		if(boolik==1)
			break;
	}
}
