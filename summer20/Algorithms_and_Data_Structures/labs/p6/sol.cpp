#include <bits/stdc++.h>

#define MAXKEY 202
#define MAXTAB 2009
#define POT 19777
#define MOD 104729

using namespace std; 
unsigned long long key[MAXKEY], tab[MAXTAB][MAXTAB], pot[MAXKEY];
int a, b, c, d, res = 0;
char ch[MAXTAB][MAXTAB];
int main() {
    scanf("%d %d %d %d", &a, &b, &c, &d);
    pot[0] = 1;
    for(int g = 1; g <= b; g++)
        pot[g] = (pot[g-1] * POT) % MOD;

    for(int g = 0; g < a; g++){
        scanf("%s", ch[0]);
       // printf("next\n");
        for(int h = 0; h < b; h++){
            key[g] = (key[g] * POT + (unsigned long long) (ch[0][h] - 'A' + 1)) % MOD;
          //  printf("MY VALUE %lld\n", key[g]);
        }
    }
   // printf("Drugie slowo\n");
    for(int g = 0; g < c; g++){
        scanf("%s", ch[g]);        
        //printf("Next\n");
        for(int h = 0; h < d; h++){
            if(h == 0)
                tab[g][h] = ((unsigned long long) (ch[g][h] - 'A' + 1)) % MOD;
            else{
                if(h >= b){
                   // printf("odejmuje %lld", ((unsigned long long)(ch[g][h-b] - 'A' + 1) * pot[b-1]));
                    tab[g][h] = (tab[g][h-1] - (((unsigned long long)(ch[g][h-b] - 'A' + 1) * pot[b-1])%MOD)) % MOD;
                    if(tab[g][h] < 0)
                        tab[g][h] += MOD;
                    tab[g][h] = (tab[g][h] * POT + (unsigned long long)(ch[g][h] - 'A' + 1)) % MOD;
                }
                else
                    tab[g][h] = (tab[g][h-1] * POT + (unsigned long long) (ch[g][h] - 'A' + 1)) % MOD;
            }
          //  printf("myvalue %lld\n", tab[g][h]);
        }
    }
    /*
2 2 2 3
aa
aa
aaa
aaa


    */

    for(int g = 0; g + a <= c; g++){
        for(int h = 0; h + b <= d; h++){
            bool ok = 1;
           // printf("KEYS\n");
            for(int i = 0; i < a; i++){
               // printf("%lld ", key[i]);
               // printf("%lld\n",(tab[g+i][h+b-1]));

                if(key[i] != tab[g+i][h+b-1]){
                    ok = 0;
                    break;
                }
            }
            if(ok)
                res++;
        }
    }
    printf("%d", res);
}