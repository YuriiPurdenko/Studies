#include<stdio.h>

using namespace std;

int main(){
	int x, y;
	scanf("%d%d", &x, &y);
	if(x > y)
		for(int i = y; i <= x; i++)
			printf("%d\n", i);
	else
		for (int i = x; i <= y; i++)
			printf("%d\n", i);
}
