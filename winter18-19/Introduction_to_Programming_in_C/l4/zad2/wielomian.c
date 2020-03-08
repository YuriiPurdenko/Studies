#include"wielomian.h"

//Wiktor Pilarczyk 308533 Zadanie 2 lista 4
const int wielwiel = 1001;      //stopień wielomianu musi być mniejszy od tej liczby jest to tez indeks, w ktorym przechowywany jest rozmiaw wielomianu
void dodajwiel(wielomian x, wielomian y,wielomian z) //dodawanie wielomianow
{
    inicial(z);
    int zm=maksik(licz(x[wielwiel]),licz(y[wielwiel]));
    for(int g=0;g<=zm;g++)
        z[g]=suma(x[g],y[g]);
    z[wielwiel]=aktual(z);
    return;
}
void odejwiel(wielomian x, wielomian y,wielomian z) //odejmowanie wielomianow
{
    inicial(z);
    int zm=maksik(licz(x[wielwiel]),licz(y[wielwiel]));
    for(int g=0;g<=zm;g++)
        z[g]=odej(x[g],y[g]);
    z[wielwiel]=aktual(z);
    return;
}
void mnozwiel(wielomian x, wielomian y,wielomian z) //mnozenie wielomianow
{
    inicial(z);
    int zm1=licz(x[wielwiel]),zm2=licz(y[wielwiel]);
    if(zm1+zm2>=wielwiel)
    {
        printf("Iloczyn wielomianów przekroczy zakres\n");
        return;
    }
    for(int g=0;g<=zm1;g++)                         //wymnazanie miedzy soba czynnikow
        for(int h=0;h<=zm2;h++)
            z[g+h]=suma(z[g+h],mnoz(x[g],y[h]));
    z[wielwiel]=konstruktor(zm1+zm2,1);
}
void wczytwiel(wielomian x)                       //wczytuje wielomian, najpierw podawany jest jego stopień(zapisywany w ostaniej komorce), sprawdzane jest czy jest on poprawny.
{                                           //Nastepnie wczytuje wspolczynniki zaczynajac od x^0.
    wymierna y;                             //nie bylo podanej specyfikacji wejscia, teoretycznie mozna wczytywac wspolczynniki dopoki na wejsciu nie pojawi sie EOF
    int z;
    printf("Podaj stopien wielomianu ");
    y=wczyt();
    if(mian(y)!=1)
    {
        printf("Stopien wielomianu nie jest calkowity!\n");
        return;
    }
    z=licz(y);
    if(z>wielwiel || z<0)
    {
        printf("Stopien wielomianu jest niepoprawny\n");
        return;
    }
    printf("Podaj argumenty: ");
    x[wielwiel]=y;
    for(int g=0;g<=z;g++)
        x[g]=wczyt();
    for(int g=z+1;g<wielwiel;g++)
        x[g]=konstruktor(0,1);
    return;
}
void wypiszwiel(wielomian x)    //Wypisuje wielomian, zaczynajac od jego stopnia, a pozniej jego czynniki z potegami x zaczynajac od 0
{
    int zm=licz(x[wielwiel]);
        printf("Wielomian stopnia %d: ",zm);
    for(int g=0;g<=zm;g++)
    {
        if(mian(x[g])==1)
            printf("%dx^%d ",licz(x[g]),g);
        else
            printf("%d/%dx^%d ",licz(x[g]),mian(x[g]),g);

    }
    printf("\n");
    return;
}
int maksik(int x, int y)    //funkcja obliczajaca maksimum
{
    if(x>y)
        return x;
    else
        return y;
}
wymierna aktual(wielomian x)       //aktualizuje wielkosc wielomianu, gdzyz przy dodawaniu i odejmowaniu wspolczynniki moga sie wyzerowac
{                                  //Stopien wielomianu umozliwia sprawdzenie, czy da sie wymnozyc wielomiany(nie przekraczaja wielkosci tablicy)
    int zm=0;
    for(int g=wielwiel-1;g>=0;g--)
    {
        if(licz(x[g])!=0)
        {
            zm=g;
            break;
        }
    }
    return konstruktor(zm,1);
}
void inicial(wielomian x)       //inicjalizuje wartosci czynnikow na zero
{
    for(int g=0;g<wielwiel;g++)
        x[g]=konstruktor(0,1);
    return;
}
void wartosc(wymierna x,wielomian y)    //oblicza wartosc wielomianu w podanym punkcie
{
    int zm=licz(y[wielwiel]);
    wymierna wyn=konstruktor(0,1),z=x;
    wyn=suma(wyn,y[0]);
    for(int g=1;g<=zm;g++)
    {
        wyn=suma(wyn,mnoz(z,y[g]));
        z=mnoz(z,x);
    }
    printf("Wartosc w podanym punkcie wynosi: ");
    wypi(wyn);

}
