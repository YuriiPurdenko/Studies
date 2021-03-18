//Wiktor Pilarczyk
//308533
//PGAW
#include <stdio.h>

int a, b, i = 0;
int t[] = {1, 2, 4, 5, 10, 20, 101, 202, 404, 505, 1010, 2020};

int main(){
	scanf("%d%d", &a, &b);
	while(i < 12 && a > t[i])
		i++;
    while(i < 12 && b >= t[i]){
        printf("%d ", t[i]);
        i++;
    }
}
