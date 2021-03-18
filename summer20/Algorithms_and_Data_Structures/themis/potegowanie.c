//Wiktor Pilarczyk
//308533
//PGAW
#include <stdio.h>

int t;
long long a, b, m, res;

int main(){
	scanf("%d", &t);
	while(t--){
		res = 1;
		scanf("%lld%lld%lld", &a, &b, &m);
		while(b){
			if(b & 1)
				res = (res * a) % m;
			a = (a * a) % m;
			b >>= 1;
		}
		printf("%lld\n", res);
	}
	return 0;
}
