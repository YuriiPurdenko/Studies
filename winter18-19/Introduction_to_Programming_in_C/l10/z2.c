#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include <math.h>

float epsilon=0.0001;

typedef struct Vec2T { float x; float y; } Vec2;

typedef struct BubbleT{
  Vec2 center; /* typedef struct Vec2T { float x; float y; } Vec2; */
  float radius;
  int visibility;
  int durability;
  int reflectivity;
}	Bubble;

typedef struct BubbleCloudT{
  Bubble* bubbles;
  long xN;
  long yN;
} BubbleCloud;

unsigned int rnd_d(unsigned int* rnd) { *rnd = 69069*(*rnd) + 1234567; return *rnd; }
float rnd_f(unsigned int* rnd) { *rnd = 69069*(*rnd) + 1234567; return ( ((float) *rnd) / ((float) 0xFFFFFFFF) ); }

BubbleCloud generateBubbleCloud(long xN, long yN, long seed, int* visSeq, int* durSeq, int* refSeq);
long long symulacja(long xN, long yN, BubbleCloud x,float xO,float yO,float xd,float yd,long long ost);
bool blizej(Vec2 v,Vec2 u,float x, float y);
Vec2 czyprzecina(Bubble *wsk,float xO,float yO, float xd, float yd);
//Wiktor Pilarczyk lista 10 Zad 2
int main (int argc,char *argv[] )
{
	char s[10],v[3],d[3],r[3],rng[3];
	r[0]='r';
	r[1]='n';
	r[2]='g';
	long xN,yN,nray,zm,seed;//
	float xO,yO,xd,yd;

	scanf("%s%ld%ld",&s[0],&xN,&yN);
	long long odp;
	const long long n=xN*yN;
	int ref[n],vis[n],dur[n];
	float pom;
	scanf("%s%ld",&s[0],&seed);
	scanf("%s%s",&s[0],&v[0]);
	zm=strcmp(v,rng);
	if(zm!=0)
		for(long long g=0;g<n;g++)
			scanf("%d",&vis[g]);
	else
		for(long long g=0;g<n;g++)
			vis[g]=NULL;
	scanf("%s%s",&s[0],&d[0]);
	zm=strcmp(d,rng);
	if(zm!=0)
		for(long long g=0;g<n;g++)
			scanf("%d",&dur[g]);
	else
		for(long long g=0;g<n;g++)
			dur[g]=NULL;
	scanf("%s%s",&s[0],&r[0]);
	zm=strcmp(r,rng);
	if(zm!=0)
		for(long long g=0;g<n;g++)
			scanf("%d",&ref[g]);
	else
		for(long long g=0;g<n;g++)
			ref[g]=NULL;
	scanf("%s%f%f",&s[0],&xO,&yO);
	scanf("%s%f%f",&s[0],&xd,&yd);
	scanf("%s%ld",&s[0],&nray);

	BubbleCloud x =generateBubbleCloud(xN,yN,seed,vis,dur,ref);
	nray--;
	while(nray--)
		odp = symulacja(xN, yN, x, xO, yO, xd, yd,-1);
	printf("%lld",odp);
}

BubbleCloud generateBubbleCloud(long xN, long yN, long seed, int* visSeq, int* durSeq, int* refSeq){
  /* local decls */
  BubbleCloud bubbleCloud;
  int x, y;
  unsigned int rndval;

  /* early init */
  bubbleCloud.bubbles = NULL;
  rndval = seed;
  x = y = 0;

  /* create bubble cloud */
  bubbleCloud.xN = xN;
  bubbleCloud.yN = yN;
  bubbleCloud.bubbles = (Bubble*) malloc( xN * yN * sizeof(Bubble) );

  for (y = 0; y < yN; y++)
    for (x = 0; x < xN; x++)
    {
      bubbleCloud.bubbles[x + y * xN].center.x = (float) x;
      bubbleCloud.bubbles[x + y * xN].center.y = (float) y;
      bubbleCloud.bubbles[x + y * xN].radius   = rnd_f(&rndval) * 0.4f;

      if ( visSeq == NULL || durSeq == NULL || refSeq == NULL )
      {
        /* generate randomized data by using higher half of bits, lower half is too regular */
        bubbleCloud.bubbles[x + y * xN].visibility   = (rnd_d(&rndval) >> 5) & 0x1;
        bubbleCloud.bubbles[x + y * xN].durability   = (rnd_d(&rndval) >> 6) & 0xF;
        bubbleCloud.bubbles[x + y * xN].reflectivity = (rnd_d(&rndval) >> 7) & 0x1;
      }
      else
      {
        bubbleCloud.bubbles[x + y * xN].visibility   = visSeq[x + y * xN];
        bubbleCloud.bubbles[x + y * xN].durability   = durSeq[x + y * xN];
        bubbleCloud.bubbles[x + y * xN].reflectivity = refSeq[x + y * xN];
      }
    }

  return bubbleCloud;
}
long long symulacja(long xN, long yN, BubbleCloud x,float xO,float yO,float xd,float yd,long long ost){
	long long wyn=ost;		//numer kulki
	Vec2 punkty,najblizsze;	//punkty zderzenia
	int indeksokregu=-1;
	bool czysiezdezyly=0;
	Bubble *wsk,*okrag;
	wsk=x.bubbles;

	for (long g = 0; g < yN; g++)
      for (long  h = 0; h < xN; h++){
		  if(x.bubbles->visibility==1){
		  	punkty=czyprzecina(wsk, xO,yO,xd,yd);
			if(punkty.x-xO>epsilon && punkty.y-yO>epsilon){
				if(czysiezdezyly){
					if(blizej(punkty,najblizsze,xO,yO)){
						najblizsze=punkty;
						indeksokregu=g*yN+h;
						okrag=wsk;
					}
				}
				else{
					czysiezdezyly=1;
					najblizsze=punkty;
					indeksokregu=g*yN+h;
					okrag=wsk;
				}
			}
		}
		wsk++;
	  }
	  if(indeksokregu!=-1){
		  wyn=indeksokregu;
		  kierunek=odbicie(najblizsze,xO,yO,okrag);
		  okrag->durability--;
		  if(okrag->durability<=0)
		  	okrag->visibility=
	  }
	  if(wyn!=ost)
	  	return wyn=symulacja(xN,yN, x,najblizsze.x,najblizsze.y,kierunek.x,kierunek.y,wyn);
		else
	  	return wyn;
}
bool blizej(Vec2 v,Vec2 u,float x, float y){
	float v1=v.x,v2=v.y,u1=u.x,u2=u.y;
	if((v1-x)*(v1-x)+(v2-y)*(v2-y)<(u1-x)*(u1-x)+(u2-y)*(u2-y))
		return true;
	else
		return false;
}
Vec2 czyprzecina(Bubble *wsk,float xO,float yO, float xd, float yd){
	float C=xd*xO-yd*yO;
	C*=-1;

}
/*[4] Aby sprawdzać, czy promień trafia w bąbelek, użyj wzoru na promień
     oraz wzoru na okrąg i szukaj najmniejszego dodatniego miejsca zerowego
     funkcji kwadratowej powstałej poprzez wstawienie równania promienia
     do równania okręgu.
	 [5] Podczas odbijania promienia uważaj na to, aby promień nie trafił w obiekt,
     od którego się odbija. Może sie tak stać z powodu precyzji numerycznej. Patrz punkt [3].
	 */
