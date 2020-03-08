#include<stdio.h>
#include<stdbool.h>
long long n,t[90][1009],odp[1009];	//x1,y1,x2,y2,x3,y3,vxy,vy
bool koliz(long long a,long long b);
bool zawie(long long a,long long b);
bool czyzaw(long long a,long long b);
bool przecina(long long a1,long long a2,long long b1,long long b2,long long c1,long long c2,long long d1,long long d2);
bool lezpom(long long x1,long long y1,long long x2, long long y2,long long x3,long long y3);
long long mini(long long x,long long y);
int main (){
	//trojkaty sie zawieraja - 3
	// trojkaty maja kolizje i nie zawieraja -2
	scanf("%lld",&n);
	for(int g=0;g<n;g++)
		scanf("%lld%lld%lld%lld%lld%lld%lld%lld",&t[0][g],&t[1][g],&t[2][g],&t[3][g],&t[4][g],&t[5][g],&t[6][g],&t[7][g]);
	for(int g=0;g<n;g++)
		for(int h=g+1;h<n;h++){
			if(koliz(g,h)){
				odp[g]=2;
				odp[h]=2;
			}
			if(zawie(g,h)){
				odp[g]=3;
				odp[h]=3;
			}
	}
	for(int g=0;g<n;g++)
	{
		if(odp[g]==3)
			printf("0 ");
		else
			if(odp[g]==2)
				printf("3 ");
			else
				printf("1 ");
	}
}
bool zawie(long long a,long long b){
	if(przecina(t[0][a],t[1][a],t[2][a],t[3][a],t[0][b],t[1][b],t[2][b],t[3][b]))
		return 1;
	if(przecina(t[0][a],t[1][a],t[2][a],t[3][a],t[4][b],t[5][b],t[2][b],t[3][b]))
		return 1;
	if(przecina(t[0][a],t[1][a],t[2][a],t[3][a],t[0][b],t[1][b],t[4][b],t[5][b]))
		return 1;
	if(przecina(t[0][a],t[1][a],t[4][a],t[5][a],t[0][b],t[1][b],t[2][b],t[3][b]))
		return 1;
	if(przecina(t[0][a],t[1][a],t[4][a],t[5][a],t[4][b],t[5][b],t[2][b],t[3][b]))
		return 1;
	if(przecina(t[0][a],t[1][a],t[4][a],t[5][a],t[0][b],t[1][b],t[4][b],t[5][b]))
		return 1;
	if(przecina(t[4][a],t[5][a],t[2][a],t[3][a],t[0][b],t[1][b],t[2][b],t[3][b]))
		return 1;
	if(przecina(t[4][a],t[5][a],t[2][a],t[3][a],t[4][b],t[5][b],t[2][b],t[3][b]))
		return 1;
	if(przecina(t[4][a],t[5][a],t[2][a],t[3][a],t[0][b],t[1][b],t[4][b],t[5][b]))
		return 1;
	if(czyzaw(a,b))
		return 1;
	if(czyzaw(b,a))
		return 1;
	return 0;
}
bool czyzaw(long long a, long long b){
	long long a1=t[0][a],a2=t[1][a],b1=t[2][a],b2=t[3][a],c1=t[4][a],c2=t[5][a];
	long long aa1=t[0][b],aa2=t[1][b],bb1=t[2][b],bb2=t[3][b],cc1=t[4][b],cc2=t[5][b];
	long long zm1,zm2,zm3,zm4;
	zm1=(b1-a1)*(c2-a2)-(c1-a1)*(b2-a2);
	zm2=(b1-a1)*(aa2-a2)-(aa1-a1)*(b2-a2);
	zm3=(b1-a1)*(bb2-a2)-(bb1-a1)*(b2-a2);
	zm4=(b1-a1)*(cc2-a2)-(cc1-a1)*(b2-a2);
	if(!(zm1<0 && zm2<0 && zm3<0 && zm4<0) && !(zm1>0 && zm2>0 && zm3>0 && zm4>0))
		return 0;
	zm1=(c1-a1)*(b2-a2)-(b1-a1)*(c2-a2);
	zm2=(c1-a1)*(aa2-a2)-(aa1-a1)*(c2-a2);
	zm3=(c1-a1)*(bb2-a2)-(bb1-a1)*(c2-a2);
	zm4=(c1-a1)*(cc2-a2)-(cc1-a1)*(c2-a2);
	if(!(zm1<0 && zm2<0 && zm3<0 && zm4<0) && !(zm1>0 && zm2>0 && zm3>0 && zm4>0))
		return 0;
	zm1=(c1-b1)*(a2-b2)-(a1-b1)*(c2-b2);
	zm2=(c1-b1)*(aa2-b2)-(aa1-b1)*(b2-b2);
	zm3=(c1-b1)*(bb2-b2)-(bb1-b1)*(b2-b2);
	zm4=(c1-b1)*(cc2-b2)-(cc1-b1)*(b2-b2);
	if(!(zm1<0 && zm2<0 && zm3<0 && zm4<0) && !(zm1>0 && zm2>0 && zm3>0 && zm4>0))
		return 0;
	return 1;
}
bool koliz(long long a,long long b){
	//1 trojkat
	if(przecina(t[0][a],t[1][a],t[0][a]+t[6][a]-t[6][b],t[1][a]+t[7][a]-t[7][b],t[0][b],t[1][b],t[2][b],t[3][b]))
		return 1;
	if(przecina(t[0][a],t[1][a],t[0][a]+t[6][a]-t[6][b],t[1][a]+t[7][a]-t[7][b],t[4][b],t[5][b],t[2][b],t[3][b]))
		return 1;
	if(przecina(t[0][a],t[1][a],t[0][a]+t[6][a]-t[6][b],t[1][a]+t[7][a]-t[7][b],t[0][b],t[1][b],t[4][b],t[5][b]))
		return 1;
	if(przecina(t[2][a],t[3][a],t[2][a]+t[6][a]-t[6][b],t[3][a]+t[7][a]-t[7][b],t[0][b],t[1][b],t[2][b],t[3][b]))
		return 1;
	if(przecina(t[2][a],t[3][a],t[2][a]+t[6][a]-t[6][b],t[3][a]+t[7][a]-t[7][b],t[4][b],t[5][b],t[2][b],t[3][b]))
		return 1;
	if(przecina(t[2][a],t[3][a],t[2][a]+t[6][a]-t[6][b],t[3][a]+t[7][a]-t[7][b],t[0][b],t[1][b],t[4][b],t[5][b]))
		return 1;
	if(przecina(t[4][a],t[5][a],t[4][a]+t[6][a]-t[6][b],t[5][a]+t[7][a]-t[7][b],t[0][b],t[1][b],t[2][b],t[3][b]))
		return 1;
	if(przecina(t[4][a],t[5][a],t[4][a]+t[6][a]-t[6][b],t[5][a]+t[7][a]-t[7][b],t[4][b],t[5][b],t[2][b],t[3][b]))
		return 1;
	if(przecina(t[4][a],t[5][a],t[4][a]+t[6][a]-t[6][b],t[5][a]+t[7][a]-t[7][b],t[0][b],t[1][b],t[4][b],t[5][b]))
		return 1;
	//2 trojkat
	if(przecina(t[0][b],t[1][b],t[0][b]+t[6][b]-t[6][a],t[1][b]+t[7][b]-t[7][a],t[0][a],t[1][a],t[2][a],t[3][a]))
		return 1;
	if(przecina(t[0][b],t[1][b],t[0][b]+t[6][b]-t[6][a],t[1][b]+t[7][b]-t[7][a],t[4][a],t[5][a],t[2][a],t[3][a]))
		return 1;
	if(przecina(t[0][b],t[1][b],t[0][b]+t[6][b]-t[6][a],t[1][b]+t[7][b]-t[7][a],t[0][a],t[1][a],t[4][a],t[5][a]))
		return 1;
	if(przecina(t[2][b],t[3][b],t[2][b]+t[6][b]-t[6][a],t[3][b]+t[7][b]-t[7][a],t[0][a],t[1][a],t[2][a],t[3][a]))
		return 1;
	if(przecina(t[2][b],t[3][b],t[2][b]+t[6][b]-t[6][a],t[3][b]+t[7][b]-t[7][a],t[4][a],t[5][a],t[2][a],t[3][a]))
		return 1;
	if(przecina(t[2][b],t[3][b],t[2][b]+t[6][b]-t[6][a],t[3][b]+t[7][b]-t[7][a],t[0][a],t[1][a],t[4][a],t[5][a]))
		return 1;
	if(przecina(t[4][b],t[5][b],t[4][b]+t[6][b]-t[6][a],t[5][b]+t[7][b]-t[7][a],t[0][a],t[1][a],t[2][a],t[3][a]))
		return 1;
	if(przecina(t[4][b],t[5][b],t[4][b]+t[6][b]-t[6][a],t[5][b]+t[7][b]-t[7][a],t[4][a],t[5][a],t[2][a],t[3][a]))
		return 1;
	if(przecina(t[4][b],t[5][b],t[4][b]+t[6][b]-t[6][a],t[5][b]+t[7][b]-t[7][a],t[0][a],t[1][a],t[4][a],t[5][a]))
		return 1;
	return 0;
}
bool przecina(long long a1,long long a2,long long b1,long long b2,long long c1,long long c2,long long d1,long long d2){
	long long W,Wt1,Wt2;
	double zm1,zm2;
	W=(b1-a1)*(c2-d2)-(c1-d1)*(b2-a2);
	Wt1=(c1-a1)*(c2-d2)-(c1-d1)*(c2-a2);
	Wt2=(b1-a1)*(c2-a2)-(c1-a1)*(b2-a2);
	if(W==0 && Wt1==0 && Wt2==0){
		if(lezpom(a1,a2,b1,b2,c1,c2) || lezpom(a1,a2,b1,b2,d1,d2))
			return 1;
		else
			return 0;
	}
	else{
		if(W==0)
			return 0;
		else{
			zm1=Wt1;
			zm1/=W;
			zm2=Wt2;
			zm2/=W;
			if(zm1>=0 && zm1<=1 && zm2>=0 && zm2<=1)
				return 1;
			else
				return 0;
		}
	}
}
bool lezpom(long long x1,long long y1,long long x2, long long y2,long long x3,long long y3){
	if(mini(x1,x2)<=x3 && -mini(-x1,-x2)>=x3 && mini(y1,y2)<=y3 && -mini(-y1,-y2)>=y3)
		return 1;
	else
		return 0;
}
long long mini(long long x,long long y){
	if(x<y)
		return x;
	else
		return y;
}
