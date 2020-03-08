#include"kolejka2.h"
//Wiktor Pilarczyk 308533

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
