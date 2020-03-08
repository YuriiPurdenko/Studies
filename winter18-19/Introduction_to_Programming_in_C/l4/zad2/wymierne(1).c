#include"wymierne.h"

//Wiktor Pilarczyk 308533 zad 2 lista

int przek(long long x,long long y)          //sprawdza czy wynik wykracza pozna inta
{
    const int incik=2147483645;
    if(x>incik || y>incik || x<-incik || y<-incik)
    {
        printf("Wynik przekracza inta\n");
        return 1;
    }
    else
        return 0;
}
long long gcd(long long x,long long y)      //wyliczam gcd
{
    long long zm=x;
    while(y!=0)
    {
        zm=x%y;
        x=y;
        y=zm;
    }
    return x;
}
int licz(wymierna x)                //odzyskuje licznik
{
    long long zm2=(long long)x;
    zm2=(zm2>>32);
    return (int)zm2;
}
int mian(wymierna x)                //odzyskuje mianownik
{
    int zm;
    long long zm2=(long long)x,zm3=1;
    for(int g=0;g<31;g++)
    {
        zm3=zm3<<1;
        zm3++;
    }
    zm2=zm2&zm3;
    zm=(int)zm2;
    if(zm<0)
        return -zm;
    else
        return zm;

}
wymierna konstruktor(int x,int y)   //tworze z licznika i mianownika
{
    long long zm,zm2;
    if(y==0)
    {
        printf("Mianownik = 0\n");
        return 0;
    }
    if(y<0)
    {
        y=-y;
        x=-x;
    }
    zm2=gcd(x,y);
    if(zm2<0)
        zm2=-zm2;
    y/=zm2;
    x/=zm2;
    zm=x;
    zm=((zm<<32)|y);
    return (wymierna)zm;

}
wymierna suma(wymierna x,wymierna y)        //wyliczanie sumy
{
    int a=licz(x),b=licz(y),c=mian(x),d=mian(y);
    if(c==0 || d==0)
    {
        printf("Mianownik = 0\n");
        return 0;
    }
    long long sum,zm=(long long)c/(gcd(c,d))*d;
    sum=(long long)a*(zm/c)+b*(zm/d);
    if(przek(sum,zm))
        return 0;
    else
        return konstruktor((int)sum,(int)zm);
}
wymierna odej(wymierna x, wymierna y)       //korzystam, z wlasnosci odejmowania
{
    return suma(x,konstruktor(-licz(y),mian(y)));
}
wymierna mnoz(wymierna x, wymierna y)       //mnozenie
{
    int a=licz(x),b=licz(y),c=mian(x),d=mian(y);
    long long il=(long long)a*b,zm=(long long)c*d,pom;
    if(c==0 || d==0)
    {
        printf("Mianownik = 0\n");
        return 0;
    }
    pom=gcd(il,zm);
    il/=pom;
    zm/=pom;
    if(przek(il,zm))
        return 0;
    else
        return konstruktor((int)il,(int)zm);

}
wymierna dziel(wymierna x,wymierna y)       //korzystam z wlasnoci dzielenia
{
  if(licz(y)!=0)
        return  (mnoz(x,konstruktor(mian(y),licz(y))));
  else
  {
        printf("Mianownik = 0\n");
        return 0;
  }
}
void wypi(wymierna x)                       //wypisuje liczbe
{
    if(mian(x)!=0)
        printf("%d/%d\n",licz(x),mian(x));      //teoretycznie mozna sprawdzic czy mian(x)==1 jesli tak to wypisac tylko licznik, nie zdazylem sie spytac
    return;
}
wymierna wczyt()                            //wczytuje liczbe
{
    int a,b,ile;
    ile=scanf("%d/%d",&a,&b);
    if(ile==2)
        return konstruktor(a,b);
    else
    {
        if(ile==1)
            return konstruktor(a,1);
        else
        {
            printf("Zle wejscie\n");
            return 0;
        }
    }
}
