//Wiktor Pilarczyk
//308533
//PGAW
#include <stdio.h>

long long tab[] = {1, 1, 1, 0};
long long res[] = {1, 0};
int t, n, m;

int fib(int x, int m){
	tab[0] = tab[1] = tab[2] = res[0] = 1;
	tab[3] = res[1] = 0;
	x--;
	while(x){ 
		if(x & 1){
			int y = ((tab[2] * res[0]) % m + (tab[3] * res[1]) % m) % m;
			res[0] = ((tab[0] * res[0]) % m + (tab[1] * res[1]) % m) % m;
			res[1] = y;
		}
		x >>= 1;		
		int a = ((tab[0] * tab[0]) % m + (tab[1] * tab[2]) % m) % m;
		int b = ((tab[0] * tab[1]) % m + (tab[1] * tab[3]) % m) % m;
		int c = ((tab[2] * tab[0]) % m + (tab[3] * tab[2]) % m) % m;
		int d = ((tab[2] * tab[1]) % m + (tab[3] * tab[3]) % m) % m;
		tab[0] = a;
		tab[2] = b;
		tab[1] = c;
		tab[3] = d;
	}
	return res[0];
}


int main(){
    scanf("%d", &t);
    while(t--){
        scanf("%d%d", &n, &m);
        printf("%d", fib(n, m));
    }
}
