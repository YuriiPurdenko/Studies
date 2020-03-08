#include<stdio.h>
long long t[1009][4];
long long rodz[1009],odw[1009],lprzec,lsklad,makssklad,n;
void unia(long long x,long long y);
long long znaj(long long x);
long long przecina(long long x,long long y);
long long mini(long long x,long long y);
long long lezpom(long long x1,long long y1,long long x2,long long y2,long long x3,long long y3);
int main()
{
	scanf("%lld",&n);
	for(long long g=1;g<=n;g++)
		scanf("%lld%lld%lld%lld",&t[g][0],&t[g][1],&t[g][2],&t[g][3]);
	for(long long g=1;g<=n;g++)
		rodz[g]=g;
	for(long long g=1;g<=n;g++)
	{
		for(long long h=g+1;h<=n;h++)
		{
			if(przecina(g,h))
			{
				unia(g,h);
				lprzec++;
			}
		}
	}
	for(long long g=1;g<=n;g++)
	{
		if(odw[znaj(rodz[g])]==0)
			lsklad++;
		odw[znaj(rodz[g])]++;
		if(odw[znaj(rodz[g])]>makssklad)
			makssklad=odw[znaj(rodz[g])];
	}
	printf("%lld %lld %lld",lprzec,lsklad,makssklad);
}
void unia(long long x, long long y)
{
	rodz[znaj(x)]=rodz[znaj(y)];
}
long long znaj(long long x)
{
	if(rodz[x]==x)
		return x;
	else
	{
		rodz[x]=znaj(rodz[x]);
		return rodz[x];
	}
}
long long przecina(long long x,long long y)
{
	long long a1=t[x][0],a2=t[x][1],b1=t[x][2],b2=t[x][3],c1=t[y][0],c2=t[y][1],d1=t[y][2],d2=t[y][3];
	long long W,Wt1,Wt2;
	double zm1,zm2;
	W=(b1-a1)*(c2-d2)-(c1-d1)*(b2-a2);
	Wt1=(c1-a1)*(c2-d2)-(c1-d1)*(c2-a2);
	Wt2=(b1-a1)*(c2-a2)-(c1-a1)*(b2-a2);
	if(W==0 && Wt1==0 && Wt2==0)
	{
		if(lezpom(a1,a2,b1,b2,c1,c2) || lezpom(a1,a2,b1,b2,d1,d2))
			return 1;
		else
			return 0;
	}
	else
	{
		if(W==0)
			return 0;
		else
		{
			zm1=Wt1;
			zm1/=W;
			zm2=Wt2;
			zm2/=W;
			if(zm1>=0 && zm1<=1 && zm2>=0 && zm2<=1)
				return 1;
			else
				return 0;
		}
	}
}
long long lezpom(long long x1,long long y1,long long x2, long long y2,long long x3,long long y3)
{
	if(mini(x1,x2)<=x3 && -mini(-x1,-x2)>=x3 && mini(y1,y2)<=y3 && -mini(-y1,-y2)>=y3)
		return 1;
	else
		return 0;	
}
long long mini(long long x,long long y)
{
	if(x<y)
		return x;
	else
		return y;		
}
