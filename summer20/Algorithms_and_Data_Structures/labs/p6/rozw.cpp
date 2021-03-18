#include <bits/stdc++.h>

#define MAXKEY 202
#define MAXTAB 2009
#define POT 199873
#define MOD 1000012337

using namespace std; 
long long key[MAXKEY], tab[MAXTAB][MAXTAB], pot;
int a, b, c, d, res = 0;
char ch[MAXTAB][MAXTAB];

inline long long modulo(long long x){
    long long ret = x % MOD;
    if(ret < 0)
        ret += MOD;
    return ret;
}

int main() {
    scanf("%d %d %d %d", &a, &b, &c, &d);
    pot = 1;
    for(int g = 1; g < b; g++)
        pot = modulo(pot * POT);

    for(int g = 0; g < a; g++){
        scanf("%s", ch[0]);
        for(int h = 0; h < b; h++)
            key[g] = modulo(modulo(key[g] * POT) + (long long) (ch[0][h] - 'A' + 1));
        
    }

    for(int g = 0; g < c; g++){
        scanf("%s", ch[g]);      
        for(int h = 0; h < d; h++){
            if(h == 0)
                tab[g][h] = (long long) (ch[g][h] - 'A' + 1);
            else{
                tab[g][h] = tab[g][h-1];
                if(h >= b){
                    tab[g][h] -=  modulo((long long)(ch[g][h-b] - 'A' + 1) * pot);
                    tab[g][h] = modulo(tab[g][h]);
                    tab[g][h] = modulo(tab[g][h] * POT);
                    tab[g][h] = modulo(tab[g][h] + (long long)(ch[g][h] - 'A' + 1));
                }
                else{
                    tab[g][h] = modulo(tab[g][h] * POT);
                    tab[g][h] = modulo(tab[g][h] + (long long) (ch[g][h] - 'A' + 1));
                }
            }
        }
    }
    
    for(int g = 0; g + a <= c; g++){
        for(int h = 0; h + b <= d; h++){
            bool ok = 1;
            for(int i = 0; i < a; i++){
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