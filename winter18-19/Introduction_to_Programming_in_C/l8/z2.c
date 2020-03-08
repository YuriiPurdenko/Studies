#include"stdio.h"
#include"string.h"
#include"stdbool.h"
int n,licz,zlicz,wsk,maxwsk;
char sl[10005][36];
int rak[10005];
bool pol[10005][10005];
int bylem[10005];

int main(){
	scanf("%d",&n);
	for(int g=1;g<=n;g++)
		scanf("%s",&sl[g][0]);
	scanf("%s",&sl[0][0]);
	scanf("%s",&sl[n+1][0]);
	licz=(int)strlen(sl[0]);
	for(int g=0;g<=n+1;g++){
		for(int h=g+1;h<=n+1;h++){
			zlicz=0;
			for(int j=0;j<licz;j++){
				if(sl[g][j]!=sl[h][j])
					zlicz++;
			}
			if(zlicz<=1)
			{
				pol[h][g]=1;
				pol[g][h]=1;
			}
		}
	}
	rak[0]=0;
	bylem[0]=1;
	maxwsk=1;
	while(maxwsk>wsk){
		for(int g=0;g<=n+1;g++)
			if(bylem[g]==0 && pol[g][rak[wsk]]==1){
				rak[maxwsk]=g;
				maxwsk++;
				bylem[g]=bylem[rak[wsk]]+1;
			}
		wsk++;
	}
	if(bylem[n+1]>0)
		printf("%d",bylem[n+1]-1);
	else
		printf("-1");
}
