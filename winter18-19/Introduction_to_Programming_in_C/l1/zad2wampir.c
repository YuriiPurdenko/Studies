#include"stdio.h"
int w,t[15],odp=-1,licz,zm;
int p[15],b[15];
//Wiktor Pilarczyk 308533
int sprawdz(int zm2)			//sprawdza czy istnieje mozliwosc utworzenia liczby b, dla takiego a, ze b=w/a
{
	int pomoc;
	while(zm2>0)
	{
		pomoc=0;
		for(int k=0;k<licz;k++)
		{
			if(t[k]==(zm2-(zm2/10)*10) && p[k]==0 && b[k]==0)
			{
				pomoc=1;
				b[k]=1;
			}
		}	
		zm2/=10;
		if(pomoc==0)
			return 0;
	}
	return 1;
}
void rek(int x,int y, int liczba)	//kreuje liczby a rekurencyjnie
{
	for(int h=0;h<licz;h++)
	{	
		if(p[h]==0)
		{
			p[h]=1;
			if(x+1==y)
			{
				if(w%(liczba*10+t[h])==0 && ((liczba*10+t[h])%10!=0 || w%(liczba*10+t[h])%10!=0))
				{
					if(sprawdz(w/(liczba*10+t[h])))
						odp=(liczba*10+t[h]);
					for(int j=0;j<14;j++)
						p[j]=0;
				}
			}
			else
			{
				rek(x+1,y,liczba*10+t[h]);
			}
			p[h]=0;
		}
	}
	return;
}

int main()
{
	scanf("%d",&w);
	zm=1;
	while(zm<w)
	{
		licz++;	
		zm*=10;
	}
	zm=w;
	for(int g=0;g<licz;g++)
	{
		p[g]=b[g]=0;
		t[g]=zm-(zm/10)*10;
		zm/=10;	
	}
	if(licz%2==1)
	{
		printf("NIE WAMPIRZA");
		return 0;
	}
	else
	{
		for(int g=0;g<licz;g++)
		{
			p[g]=1;
			if(t[g]!=0)
				rek(1,licz/2,t[g]);
			p[g]=0;		
		}
		if(odp==-1)
			printf("NIE WAMPIRZA");
		else
			printf("%d\n%d\n%d",w,odp,w/odp);
	}
}
