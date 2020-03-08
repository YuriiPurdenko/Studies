#include"drzewo.h"
//Wiktor Pilarczyk 308533 Z1 L8

NaWezel czytaj(){
    int gora=0;
    NaWezel stos[100];
    double zm;
    char c=' ';
    while(c!='='){
        c=getchar();
        if(isspace(c))
            continue;
        if(c=='=' || c=='*' || c=='+' || c=='-' || c=='/'){
                if(c=='=')
                    break;
                else{
                    if(gora<2){
                        printf("Zle wejscie\n");
                        exit(1);
                    }
                    stos[gora-2]=lacz(znak(c),stos[gora-2],stos[gora-1]);
                    gora--;
                }
        }
        else{
            ungetc(c,stdin);
            if(scanf("%lf",&zm)==1){
                stos[gora]=tworz(zm);
                gora++;
            }
        }
    }
    return stos[gora-1];
}
NaWezel tworz(double x){
    NaWezel nowe=malloc(sizeof(Wezel));
    nowe->arg=x;
    nowe->op='a';
    nowe->lewy=NULL;
    nowe->prawy=NULL;
    nowe->licz=1;
    return nowe;
}
NaWezel znak(char x){
    NaWezel nowe=malloc(sizeof(Wezel));
    nowe->arg=NAN;
    nowe->op=x;
    nowe->lewy=NULL;
    nowe->prawy=NULL;
    nowe->licz=0;
    return nowe;
}
NaWezel lacz(NaWezel x, NaWezel l, NaWezel p){
    x->lewy=l;
    x->prawy=p;
    return x;
}
void wypisz(NaWezel x, char ost, bool lewe){
    if(x->licz==1){
        printf("%lf",x->arg);
    }
    else{
        if(prior(ost)>prior(x->op) || (przem(ost)==0 && przem(x->op)==0 && lewe==0)){
            printf("(");
            wypisz(x->lewy,x->op,1);
            printf("%c",x->op);
            wypisz(x->prawy,x->op,0);
            printf(")");
        }
        else{
            wypisz(x->lewy,x->op,1);
            printf("%c",x->op);
            wypisz(x->prawy,x->op,0);
        }
    }
}
bool przem(char x){
    if(x=='+' || x=='*')
        return 1;
    return 0;
}
bool prior(char x){
    if(x=='*' || x=='/')
        return 1;
    return 0;
}
