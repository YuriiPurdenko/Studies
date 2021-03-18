#include <bits/stdc++.h>

using namespace std;

int MagPiec(int *tab, int n, int k);

int media(int *tab, int start, int n){
    sort(tab+start, tab + min(start+5, n));
    return tab[min(start+2 , n-1)];
}
int same;
void inline swap(int *tab, int a, int b){
    int x = tab[a];
    tab[a] = tab[b];
    tab[b] = x;
}

int podziel(int *tab, int n, int wart){
    int iter = 0;
    same = 0;
    for(int g = 0; g < n; g++){
        if(tab[g] < wart){
            swap(tab, iter, g);
            iter++;
        }
    }
    int help = iter;
    for(int g = 0; g < n; g++){
        if(tab[g] == wart){
            swap(tab, iter + same, g);
            same++;
        }
    }
    return iter;
}

int MagPiec(int *tab, int n, int k){
    if(n < 10){
        sort(tab, tab + n);
        return tab[k-1];
    }
    //printf("n i k: %d %d\n", n, k);
    int medians[(n+4)/5];  
    //printf("\n mediany: ");
    for(int g = 0; g < (n+4)/5; g++){
        //for(int h = 0; g*5+h < min(n, g*5+5); h++)
            //printf("%d ", tab[g*5+h]);
        medians[g] = media(tab, g*5, n);
        //printf("m%d \n", medians[g]);
    }
    //printf("po medianach\n");
    int mediana = MagPiec(medians, (n+4)/5, (((n+4)/5))/2);
    //printf("\nmediana median: %d\n", mediana);
    //int x;
    //scanf("%d",&x);
    //printf("po medianie median\n");
    int podzial = podziel(tab, n, mediana);
    //printf("po podziale %d %d\n", podzial, same);
    if(podzial + same  > k-1 && podzial <= k-1)
        return mediana;
    if(podzial + same <= k-1)
        return MagPiec(tab + podzial + same, n-podzial-same, k - podzial-same);
    else
        return MagPiec(tab, podzial, k);
}

int main(){
    int n, k;
    scanf("%d%d", &n, &k);
    int tab[n];
    for(int g = 0; g < n; g++)
        scanf("%d", &tab[g]);
    printf("%d", MagPiec(tab, n, k));
}