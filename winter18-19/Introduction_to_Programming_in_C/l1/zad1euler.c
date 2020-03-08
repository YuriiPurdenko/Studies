#include"stdio.h"
#include"math.h"
//Wiktor Pilarczyk 308533
//Lista 1 Zad 1
int eps1=100,eps1l=5000000;
double a=1,e1,e2,eps2=100000000;
const double ee=2.71828182845904523536028747135266249;
long double al=1,e1l,e2l,eps2l=100000000;
const long double eel=2.71828182845904523536028747135266249L;
int main()
{
    e1=1;
    for(int g=1;g<eps1;g++)
    {
        e1+=a/g;
        a/=g;
    }
    e2=1;
    for(int g=1;g<=eps2;g++)
        e2*=(1+(double)(1/eps2));

    printf("DLA DOUBLI:\n");
    printf("Przyblizenie za pomoca szeregu: \n%.16f\nBlad: %.16g\n\n",e1,ee-e1);
    printf("Przyblizenie za pomoca granicy: \n%.16f\nBlad: %.16g\n\n",e2,ee-e2);
    printf("Wartosc z wikipedii \n%.16f\n\n",ee);
    printf("DLA LONG DOUBLI:\n");

    e1l=1;
    for(int g=1;g<eps1l;g++)
    {
        e1l+=al/g;
        al/=g;
    }
    e2l=1;
    for(int g=1;g<=eps2l;g++)
        e2l*=(1+(long double)(1/eps2l));

    printf("Przyblizenie za pomoca szeregu: \n%.19Lf\nBlad: %.19Lg\n\n",e1l,eel-e1l);
    printf("Przyblizenie za pomoca granicy: \n%.19Lf\nBlad: %.19Lg\n\n",e2l,eel-e2l);
    printf("Wartosc z wikipedii \n%.19Lf\n",eel);
}
