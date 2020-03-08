#include "wiel.h"

void czytaj(){
	char c,sl[100],stos[100];
	Kol kolej[100];
	int x,y,licz=0;
	bool wsk;
	int wskstos=0,wskkolej=0;
	c=getchar();
	while(c!=EOF && c!='='){
		if(isspace(c)){
			c=getchar();
			continue;
		}
		if(c=='+' || c=='-' || c=='*' || c=='\'' || c=='(' || c==')'){
			x=y=0;
			for(int g=0;g<licz;g++)	//tworze jednomian
			{
				if(isdigit(sl[g])){
					if(wsk){
						y*=10;
						y+=(int)(sl[g]-'0');
					}
					else{
						x*=10;
						x+=(int)(sl[g]-'0');
					}
				}
				else
					wsk=1;
			}
			if(x==0)
				x=1;
			if(y==0 && wsk==1)
				y=1;
			//printf("%d %d\n",x,y);
						//uzyskuje ONP
			kolej[wskkolej].czyjed=1;
			kolej[wskkolej].jed1=x;
			kolej[wskkolej].jed2=y;
			wskkolej++;

			if(c=='('){
				stos[wskstos]=c;
				wskstos++;
			}
			else{
				if(c==')'){
					printf("%d",wskkolej);
					while(stos[wskstos-1]!='('){
						kolej[wskkolej].czyjed=0;
						kolej[wskkolej].dzial=c;
						wskkolej++;
						wskstos--;
					}
				}
				else{
					while(wskstos>0 && prior(stos[wskstos-1])>=prior(c)){
						kolej[wskkolej].czyjed=0;
						kolej[wskkolej].dzial=stos[wskstos-1];
						wskkolej++;
						wskstos--;
					}
				}
			}
			stos[wskstos]=c;
			wskstos++;
			licz=0;
			wsk=0;
		}
		else{
			sl[licz]=c;
			licz++;
		}
		c=getchar();
	}
	for(int g=0;g<licz;g++)	//tworze jednomian
	{
		if(isdigit(sl[g])){
			if(wsk){
				y*=10;
				y+=(int)(sl[g]-'0');
			}
			else{
				x*=10;
				x+=(int)(sl[g]-'0');
			}
		}
		else
			wsk=1;
	}
	if(licz>0){
		if(x==0)
			x=1;
		if(y==0 && wsk==1)
			y=1;			//uzyskuje ONP
		kolej[wskkolej].czyjed=1;
		kolej[wskkolej].jed1=x;
		kolej[wskkolej].jed2=y;
		wskkolej++;
	}
	while(wskstos>0){
		kolej[wskkolej].czyjed=0;
		kolej[wskkolej].dzial=stos[wskstos-1];
		wskkolej++;
		wskstos--;
	}
	for(int g=0;g<wskkolej;g++)
	{
		if(kolej[g].czyjed)
			printf("%dx^%d ",kolej[g].jed1,kolej[g].jed2);
		else
			printf("%c ",kolej[g].dzial);
	}
	//printf("%d",wskkolej);
}
int prior(char x){
	if(x=='+' || x=='-')
		return 0;
	if(x=='*')
		return 1;
	else
		return 2;
}
