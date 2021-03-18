#include <stdio.h>

#define MAX_MASS 1000001
#define MAX_VAL 100000000001 //11 zer

#define coin_val(t,n) ((t)[(n)][0])
#define coin_mass(t,n) ((t)[(n)][1])

typedef struct{
    long long max_val;
    long long min_val;
    int max_from;
    int min_from;
} dynamic;

dynamic box[MAX_MASS];
int coins[100 + 3][4];
int n;
int c;


static inline void update(int summass, int tabmass, int c_val, int c_mass, int index){
    if(c_mass + tabmass > summass)
        return;
    if(box[tabmass].max_val + (long long) c_val > box[c_mass + tabmass].max_val){
        box[c_mass + tabmass].max_val = box[tabmass].max_val + (long long) c_val;
        box[c_mass + tabmass].max_from = index;
    }
    if(box[tabmass].min_val + (long long) c_val < box[c_mass + tabmass].min_val){
        box[c_mass + tabmass].min_val = box[tabmass].min_val + (long long) c_val;
        box[c_mass + tabmass].min_from = index;
    }
}

int main(){
    scanf("%d",&n);
    scanf("%d",&c);

    for(int g = 1; g <= n; g++)
        box[g].min_val = MAX_VAL;

    for(int g = 1; g <= c; g++){
        scanf("%d %d", &coins[g][0], &coins[g][1]);

        update(n, 0, coin_val(coins, g), coin_mass(coins, g), g);

        for(int h = 1; h + coin_mass(coins,g) <= n; h++)
            if(box[h].max_from != 0)
                update(n, h, coin_val(coins, g), coin_mass(coins, g), g);
    }
    if(box[n].max_from != 0){
        int val = n;
        while(val > 0){
            coins[box[val].max_from][2]++;
            val -= coin_mass(coins, box[val].max_from);
        }
        val = n;        
        while(val > 0){
            coins[box[val].min_from][3]++;
            val -= coin_mass(coins, box[val].min_from);
        }

        printf("TAK\n%lld\n", box[n].min_val);
        for(int g = 1; g <= c; g++)
            printf("%d ", coins[g][3]);
        printf("\n%lld\n", box[n].max_val);
        for(int g = 1; g <= c; g++)
            printf("%d ", coins[g][2]);   
        
    }
    else
        printf("NIE");
}
