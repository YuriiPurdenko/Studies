#include"kolejka2.h"
//Wiktor Pilarczyk 308533 Zad 2 Lista 7

Kolejka init(){
    return NULL;
}
Kolejka dodpocz(double x,Kolejka k){
    Wezel *nowy=malloc(sizeof(Wezel));
    if(nowy==NULL){
        printf("Brak pamieci");
        exit(1);
    }
    if(k==NULL){
        k=nowy;
        nowy->info=x;
        nowy->nast=nowy;
    }
    else{
        nowy->nast=k->nast;
        nowy->info=x;
        k->nast=nowy;
    }
    return k;
}
Kolejka dodkon(double x,Kolejka k){
    Wezel *nowy=malloc(sizeof(Wezel));
    if(nowy==NULL){
        printf("Brak pamieci");
        exit(1);
    }
    if(k==NULL){
        k=nowy;
        nowy->info=x;
        nowy->nast=nowy;
    }
    else{
        nowy->nast=k->nast;
        k->nast=nowy;
        k=nowy;
        nowy->info=x;
    }
    return k;
}
double wartpocz(Kolejka k){
    if(k==NULL)
        return NAN;
    return (k->nast)->info;
}
Kolejka usun(Kolejka k,double *a){
    if(k==NULL)
    {
        *a=NAN;
        return k;
    }
    *a=wartpocz(k);
    if(k==k->nast){
        free(k);
        return NULL;
    }
    Wezel *x =k->nast;
    k->nast=x->nast;
    free(x);
    return k;
}
bool pusta(Kolejka k){
    if(k==NULL)
        return 1;
    else
        return 0;
}
int dlug(Kolejka k){
    if(k==NULL)
        return 0;
    Wezel *x=k->nast;
    int a=1;
    x=x->nast;
    while(x!=k->nast){
        x=x->nast;
        a++;
    }
    return a;
}
void drukkol(Kolejka k){
    if(k==NULL){
        printf("Kolejka pusta\n");
        return;
    }
    Wezel *x=k->nast;
    printf("Kolejka: ");
    printf("%lf ",x->info);
    x=x->nast;
    while(x!=k->nast){
        printf("%lf ",x->info);
        x=x->nast;
    }
    printf("\n");
    return;
}
Kolejka lacz(Kolejka x,Kolejka y){      //lacze kolejke x z y, tak aby ostatni element x wskazywal na poczatkowy y i ostatni y wskazywal na poczatkowy x
    if(x==NULL)
        return y;
    if(y==NULL)
        return x;
    Kolejka a=x->nast;
    x->nast=y->nast;
    y->nast=a;
    return y;
}
Kolejka kopiuj(Kolejka k){              //Kopiowanie kolejki polega na zainicjalizowaniu nowej, a pozniej po koleji dodawaniu do niej elementow z podanej kolejki
    if(k==NULL){
        return k;
    }
    Kolejka zm=init();
    Wezel *x=k->nast;
    zm=dodkon(x->info,zm);
    x=x->nast;
    while(x!=k->nast){
        zm=dodkon(x->info,zm);
        x=x->nast;
    }
    return zm;
}
Kolejka wstaw(double x,Kolejka k){      //sprawdzam czy jest pusta lub czy element jest wiekszy badz rowny od najwiekszego elementu, jesli nie to szukam pierwszego elementu, od ktorego jest on mniejszy badz rowny i wstawiam go prze dnim
    if(k==NULL)
        return dodkon(x,k);
    if(x>=k->info){
        return dodkon(x,k);;
    }
    Wezel *y=k;
    while(x>((y->nast)->info))
        y=y->nast;
    Wezel *nowy=malloc(sizeof(Wezel));
    if(nowy==NULL){
        printf("Brak pamieci");
        exit(1);
    }
    nowy->info=x;
    nowy->nast=y->nast;
    y->nast=nowy;
    return k;
}
Kolejka sortwstaw(Kolejka k){
    if(k==NULL)                   //sprawdzam czy kolejka jest pusta
        return k;
    Kolejka x=init();             //tworze nowa do ktorej zapisze posortowana kolejke
    double mini;
    while(!pusta(k)){             //usuwam element z kolejki k i wstawiam w odpowiednie miejsca do x
        k=usun(k,&mini);
        x=wstaw(mini,x);
    }
    return x;
}
Kolejka sortquick(Kolejka k){
    if(k==NULL)                 //jesli kolejka sortowana jest pusta zwracam ja
        return k;
    Kolejka a=init();           //inicjalizuje dwie kolejki, ktore bede sortowal rekurencyjnie
    Kolejka b=init();
    double zm,x;
    k=usun(k,&zm);              //wybieram piwota
    while(!pusta(k)){
        k=usun(k,&x);
        if(x>zm)                //wszystkie elementy wieksze dodaje do kolejki b
            b=dodkon(x,b);
        else                    //zas mniejsze/rowne do a (i usuwam je z kolejki k)
            a=dodkon(x,a);
    }
    a=sortquick(a);             //sortuje a i b
    b=sortquick(b);
    k=dodkon(zm,k);             //lacze najpierw a  z piwotem, a pozniej sume tego z b
    k=lacz(a,k);
    k=lacz(k,b);
    return k;
}
/*  FUNKCJE  MNIEJ CZYTELNe

Kolejka sortwstaw(Kolejka k){
    if(k==NULL)                   //sprawdzam czy kolejka jest pusta
        return k;
    Kolejka x=init();             //tworze nowa do ktorej zapisze posortowana kolejke
    Wezel *y=k,*z=k;
    double mini;
    while(!pusta(k)){             //dopoki kolejka nie jest pusta wybieram najmniejszy element i wstawiam do nowej kolejki
        y=z=k;
        mini=((y->nast)->info);   //ustawiam wartosc  elementu jako najmniejszego
        y=y->nast;
        while(y!=k){              //przechodze cala kolejke w poszukiwaniu najmniejszego elemnetu oraz zapisu wskaznik przed tym elementem
            if(mini>((y->nast)->info)){
                mini=((y->nast)->info);
                z=y;
            }
            y=y->nast;
        }
        if(k==k->nast)             //jesli jest to ostatni element w kolejce ustawiam k jako pusta
            k=NULL;
        x=dodkon(mini,x);          //dodaje najmniejszy element do nowej kolejki
        y=z->nast;                 //y jest wskaznikiem na element, ktory usunalem, dzieki temu bede mogl zwolnic pamiec
        z->nast=y->nast;           //wykluczam element usuniety z listy
        if(y==k)                   //jesli usunietym elementem byl element ostatni zmieniam glowe na poprzedni
            k=z;
        free(y);                    //zwalniam pamiec
    }
    return x;
    }
    Kolejka sortquick(Kolejka k){
    if(k==NULL)                 //jesli kolejka sortowana jest pusta zwracam ja
        return k;
    Kolejka a=init();           //inicjalizuje dwie kolejki, ktore bede sortowal rekurencyjnie
    Kolejka b=init();
    double zm,x;
    k=usun(k,&zm);              //wybieram piwota
    while(!pusta(k)){
        k=usun(k,&x);
        if(x>zm)                //wszystkie elementy wieksze dodaje do kolejki b
            b=dodkon(x,b);
        else                    //zas mniejsze/rowne do a (i usuwam je z kolejki k)
            a=dodkon(x,a);
    }
    a=sortquick(a);             //sortuje kolejki a  i b
    b=sortquick(b);
    while(!pusta(a)){           //dopoki kolejka a nie jest pusta dodadaje je na poczatek kolejki k i usuwam z a
        a=usun(a,&x);
        k=dodkon(x,k);
    }
    free(a);                    //zwalaniam pamiec kolejki a
    k=dodkon(zm,k);
    while(!pusta(b)){           //tak samo z kolejka b
        b=usun(b,&x);
        k=dodkon(x,k);
    }
    free(b);
    return k;
    }
*/
