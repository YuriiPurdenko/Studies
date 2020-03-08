#include"stdio.h"
typedef struct {
	int zapis;
	int ide;
	int koncz;
	int stan;
}zachowanie;
int q,t[2000010],ruch,pom=0;
const int stala=1000002;
zachowanie czyn[34][3];

int wsk,maklew,makpraw,stanik,wart;

int main(){
	scanf("%d",&q);
	for(int g=0;g<q;g++){
		scanf("%d%d%d%d",&czyn[g][0].zapis,&czyn[g][0].ide,&czyn[g][0].koncz,&czyn[g][0].stan);
		scanf("%d%d%d%d",&czyn[g][1].zapis,&czyn[g][1].ide,&czyn[g][1].koncz,&czyn[g][1].stan);
	}
	scanf("%d",&ruch);
	stanik=0;
	wsk=maklew=makpraw=stala;
	while(ruch>0){
		ruch--;
		wart=t[wsk];
		t[wsk]=czyn[stanik][wart].zapis;
		if(czyn[stanik][wart].koncz){
			printf("halted ");
			pom=1;
			break;
		}
		if(czyn[stanik][wart].ide)
			wsk++;
		else
			wsk--;
		stanik=czyn[stanik][wart].stan;
		if(wsk<maklew)
			maklew=wsk;
		if(wsk>makpraw)
			makpraw=wsk;
	}
	if(pom==0)
		printf("running ");
	printf("%d\n",wsk-stala);
	printf("%d ",maklew-stala);
	for(int g=maklew;g<=makpraw;g++)
		printf("%d",t[g]);
	printf(" %d",makpraw-stala);
}
