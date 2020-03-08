#include"stdio.h"
#include"stdbool.h"
int a,b,startx,starty,t[109][109],maseczka=2,suma;
char sl;
bool odw[109][109][4][1025];
void DFS(int x,int y,int kier,int maska);
int main(){
	scanf("%d%d",&a,&b);
	for(int g=1;g<=b;g++){
		for(int h=1;h<=a;h++){
			scanf("%c",&sl);
			while(sl=='\n')
				scanf("%c",&sl);
			switch(sl){
				case '#':
					t[g][h]=0;
					break;
				case '.':
					t[g][h]=1;
					break;
				case 'S':
					t[g][h]=maseczka;
					suma+=maseczka;
					maseczka*=2;
					break;
				case 'P':
					starty=g;
					startx=h;
					t[g][h]=1;
					break;
			}
		}
	}
	DFS(startx,starty,0,0);//dol
	DFS(startx,starty,1,0);//gora
	DFS(startx,starty,2,0);//lewo
	DFS(startx,starty,3,0);//prawo
	for(int g=1;g<=b;g++){
		for(int h=1;h<=a;h++){
			if(t[g][h]==0)
				printf("#");
			else{
				if((odw[h][g][0][suma]==1 || odw[h][g][1][suma]==1) || (odw[h][g][2][suma]==1 || odw[h][g][3][suma]==1))
					printf("X");
				else
					printf(".");
			}
		}
		printf("\n");
	}
}
void DFS(int x,int y,int kier,int maska){
	odw[x][y][kier][maska]=1;
	switch(kier){
		case 0:
			switch(t[y-1][x]){
				case 0:
					if(odw[x][y][1][maska]==0 && t[y+1][x]!=0)
						DFS(x,y,1,maska);
					if(odw[x][y][2][maska]==0 && t[y][x-1]!=0)
						DFS(x,y,2,maska);
					if(odw[x][y][3][maska]==0 && t[y][x+1]!=0)
						DFS(x,y,3,maska);		
					break;
				case 1:
					if(odw[x][y-1][kier][maska]==0)
						DFS(x,y-1,kier,maska);
					break;
			}
			if(t[y-1][x]>1){
				if(odw[x][y-1][0][(maska|t[y-1][x])]==0)
					DFS(x,y-1,0,(maska|t[y-1][x]));
				if(odw[x][y-1][1][(maska|t[y-1][x])]==0)
					DFS(x,y-1,1,(maska|t[y-1][x]));
				if(odw[x][y-1][2][(maska|t[y-1][x])]==0)
					DFS(x,y-1,2,(maska|t[y-1][x]));
				if(odw[x][y-1][3][(maska|t[y-1][x])]==0)
					DFS(x,y-1,3,(maska|t[y-1][x]));
			}
			break;
		
		case 1:
			switch(t[y+1][x]){
				case 0:
					if(odw[x][y][0][maska]==0 && t[y-1][x]!=0)
						DFS(x,y,0,maska);
					if(odw[x][y][2][maska]==0 && t[y][x-1]!=0)
						DFS(x,y,2,maska);
					if(odw[x][y][3][maska]==0 && t[y][x+1]!=0)
						DFS(x,y,3,maska);		
					break;
				case 1:
					if(odw[x][y+1][kier][maska]==0)
						DFS(x,y+1,kier,maska);
					break;
			}
			if(t[y+1][x]>1){
				if(odw[x][y+1][0][(maska|t[y+1][x])]==0)
					DFS(x,y+1,0,(maska|t[y+1][x]));
				if(odw[x][y+1][1][(maska|t[y+1][x])]==0)
					DFS(x,y+1,1,(maska|t[y+1][x]));
				if(odw[x][y+1][2][(maska|t[y+1][x])]==0)
					DFS(x,y+1,2,(maska|t[y+1][x]));
				if(odw[x][y+1][3][(maska|t[y+1][x])]==0)
					DFS(x,y+1,3,(maska|t[y+1][x]));
			}
			break;
		case 2:
			switch(t[y][x-1]){
				case 0:
					if(odw[x][y][0][maska]==0 && t[y-1][x]!=0)
						DFS(x,y,0,maska);
					if(odw[x][y][1][maska]==0 && t[y+1][x]!=0)
						DFS(x,y,1,maska);
					if(odw[x][y][3][maska]==0 && t[y][x+1]!=0)
						DFS(x,y,3,maska);		
					break;
				case 1:
					if(odw[x-1][y][kier][maska]==0)
						DFS(x-1,y,kier,maska);
					break;
			}
			if(t[y][x-1]>1){
				if(odw[x-1][y][0][(maska|t[y][x-1])]==0)
					DFS(x-1,y,0,(maska|t[y][x-1]));
				if(odw[x-1][y][1][(maska|t[y][x-1])]==0)
					DFS(x-1,y,1,(maska|t[y][x-1]));
				if(odw[x-1][y][2][(maska|t[y][x-1])]==0)
					DFS(x-1,y,2,(maska|t[y][x-1]));
				if(odw[x-1][y][3][(maska|t[y][x-1])]==0)
					DFS(x-1,y,3,(maska|t[y][x-1]));
			}
			break;
		case 3:
			switch(t[y][x+1]){
				case 0:
					if(odw[x][y][0][maska]==0 && t[y-1][x]!=0)
						DFS(x,y,0,maska);
					if(odw[x][y][1][maska]==0 && t[y+1][x]!=0)
						DFS(x,y,1,maska);
					if(odw[x][y][2][maska]==0 && t[y][x-1]!=0)
						DFS(x,y,2,maska);		
					break;
				case 1:
					if(odw[x+1][y][kier][maska]==0)
						DFS(x+1,y,kier,maska);
					break;
			}
			if(t[y][x+1]>1){
				if(odw[x+1][y][0][(maska|t[y][x+1])]==0)
					DFS(x+1,y,0,(maska|t[y][x+1]));
				if(odw[x+1][y][1][(maska|t[y][x+1])]==0)
					DFS(x+1,y,1,(maska|t[y][x+1]));
				if(odw[x+1][y][2][(maska|t[y][x+1])]==0)
					DFS(x+1,y,2,(maska|t[y][x+1]));
				if(odw[x+1][y][3][(maska|t[y][x+1])]==0)
					DFS(x+1,y,3,(maska|t[y][x+1]));
			}
			break;
	}
}
