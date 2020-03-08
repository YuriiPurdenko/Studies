#include<stdio.h>
#include<stdbool.h>
int n,t[10000];
bool nastperm(int x,int y[]);
//Wiktor Pilarczyk lista 5 zad 1 308533
int main ()
{
	scanf("%d",&n);
	for(int g=1;g<=n;g++)
        t[g]=g;
    do
    {
       for(int h=1;h<=n;h++)
            printf("%d ",t[h]);
        printf("\n");
    }while(nastperm(n,t));
    return 0;
}
bool nastperm(int x,int y[])
{
    int zm;
    if(x==1)
        return false;
    if(y[1]==x)
    {
        for(int g=1;g<n;g++)
        {
            y[g]=y[g+1];
        }
        if(nastperm(x-1,y))
            y[x]=x;
        else
            return false;
    }
    else
    {
        for(int g=2;g<=x;g++)
            if(y[g]==x)
            {
                zm=y[g-1];
                y[g-1]=x;
                y[g]=zm;
                break;
            }
    }
    return true;
}
