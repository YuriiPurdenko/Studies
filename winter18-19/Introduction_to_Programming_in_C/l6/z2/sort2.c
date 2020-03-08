#include"sort2.h"
//Wiktor Pilarczyk 308533 Zad 2 Lista 6

void gen(double z[],int x){
	time_t t;
	int zm,zm2;
	srand((unsigned) time(&t));
	for(int g=0;g<x;g++){
		zm=rand()+1;
		zm2=(rand()%zm);
		z[g]=(double)zm2/(double)(zm);  //obliczanie czesci ulamkowej
		z[g]+=(double)rand();         //obliczanie czesci calkowitej
		zm=rand();
		if(zm%2==0)
            z[g]*=-1;               //okreslanie znaku liczby
	}
}
bool spr(double t[], double z[], int x){

	bool wsk;
	bool bat[x];
	for(int g=1;g<x;g++)				//sprawdzam czy jest niemalejacy
		if(z[g-1]>z[g])
			return 0;
	for(int g=0;g<x;g++){			//sprawdzam czy kazdy element z t nalezy do z
		wsk=1;
		for(int h=0;h<x;h++){
			if(t[g]==z[h] && bat[h]==0){
				bat[h]=1;
				wsk=0;
				break;
			}
		}
		if(wsk)
			return 0;
	}
	return 1;
}
int compare(const void * a, const void * b){
	return (*(double*)a - *(double*)b);
}
void szsort(double t[],int x){
	qsort(t,x,sizeof(double),compare);
}
void mojsort(double t[],int x){			//sortowanie przez wybieranie
	double p[x],mini;
	int wsk;
	for(int g=0;g<x;g++){
		mini=t[g];
		wsk=g;
		for(int h=g;h<x;h++)		//wyszukuje najmniejszy elemend od g do x-1
			if(t[h]<mini){
				mini=t[h];
				wsk=h;
			}
		t[wsk]=t[g];				//ustawiam najmniejsze wartosci na poczatku ciagu
		t[g]=mini;
	}
}
double czas(void (*fun)(double [],int),int x){
	double t[x],mini,maks,ave,zm;	//tablica ciagu, minimalny czas,maksymalny czas, sredni czas, zmienna
	clock_t time_req;
	gen(t,x);
	time_req=clock();
	fun(t,x);
	time_req=clock()-time_req;
	mini=maks=ave=(double)time_req/CLOCKS_PER_SEC;
	for(int g=1;g<100;g++){
		gen(t,x);
		time_req=clock();
		fun(t,x);
		time_req=clock()-time_req;
		zm=(double)time_req/CLOCKS_PER_SEC;
		if(zm>maks)
			maks=zm;
		if(zm<mini)
			mini=zm;
		ave+=zm;
	}
	ave/=100;
	printf("Minimalny czas: %lf\nMaksymalny czas: %lf\nSredni czas: %lf\n",mini,maks,ave);
}
