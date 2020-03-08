#include"kolejka2.h"
//Wiktor Pilarczyk 308533 Zad 2 Lista 7

int main()
{
    printf("sortowanie pustych kolejek\n");
    Kolejka kol=init();
    drukkol(kol);
    kol=sortquick(kol);
    drukkol(kol);
    kol=sortwstaw(kol);
    drukkol(kol);

    printf("\nsortowanie jednoelementowej kolejki\n");
    kol=dodpocz(1,kol);
    drukkol(kol);
    kol=sortquick(kol);
    drukkol(kol);
    kol=sortwstaw(kol);
    drukkol(kol);

    printf("\nsortowanie wieloelementowych kolejek\n");
    kol=dodpocz(2,kol);
    kol=dodpocz(3,kol);
    kol=dodpocz(4,kol);
    kol=dodpocz(5,kol);
    kol=dodpocz(6,kol);

    Kolejka kol2=init();
    kol2=dodpocz(7,kol2);
    kol2=dodpocz(8,kol2);
    kol2=dodpocz(9,kol2);
    kol2=dodpocz(10,kol2);
    kol2=dodpocz(11,kol2);
    kol2=dodpocz(12,kol2);

    drukkol(kol);
    drukkol(kol2);

    kol=sortquick(kol);
    drukkol(kol);
    kol2=sortwstaw(kol2);
    drukkol(kol2);

    printf("\nkopiowanie kolejki\n");
    Kolejka kol3=kopiuj(kol2);
    drukkol(kol3);
    kol3=dodkon(0,kol3);
    drukkol(kol3);              //nie jest to po prostu przepisanie glowy
    drukkol(kol2);

    printf("\nwstawianie wartosci na odpowiednie miesjce\n");
    kol2=wstaw(7,kol2);
    kol2=wstaw(10,kol2);
    kol2=wstaw(714,kol2);
    drukkol(kol2);

    printf("\nlaczenie list\n");
    drukkol(kol2);
    drukkol(kol);
    kol2=lacz(kol2,kol);
    drukkol(kol2);
}
