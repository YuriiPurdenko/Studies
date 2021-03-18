#include <bits/stdc++.h>

#define MAXTAB 1000009

using namespace std;

typedef struct entrance{
    int x, y;
    int val;
} Enter;

bool comparatorx(Enter a, Enter b){
    if(a.x == b.x){
        return a.y < b.y;
    }
    return a.x < b.x;
}

bool comparatory(Enter a, Enter b){
    if(a.y == b.y){
        return a.x < b.x;
    }
    return a.y < b.y;
}

int k, powertwo;
vector <long long> v;
Enter tab[MAXTAB], val[MAXTAB]; 

long long findbest(int ind){
    int left = 0, right = powertwo-1, mid = powertwo / 2;
    int start = 1;
    long long best = 0;
    while(right - left > 0){
        if(mid > ind){
            right = mid;
            start *= 2;
        }
        else{
            left = mid + 1;
            best = max(best, v[start*2]);
            start = 2 * start + 1;
        }
        mid = (left + right)/2;
    }
    return best;    
}

void update(int index, long long value){
    while(value > v[index] && index > 0){
        v[index] = value;
        index /= 2;
    }
}

int main() {
    scanf("%d %d", &k, &powertwo);
    scanf("%d", &k);
    powertwo = 1;
    while(powertwo < k)
        powertwo *= 2;
    Enter read;
    for(int g = 0; g < k; g++){
        scanf("%d %d %d", &read.x, &read.y, &read.val);
        val[g] = read;
    }

    sort(val, val + k, comparatory);
    
    for(int g = 0; g < k; g++){
        tab[g] = val[g];
        tab[g].val = g;
    }

    sort(tab, tab + k, comparatorx);

    v.resize(powertwo * 2);
    for(int g = 0; g < 2 * powertwo; g++)
        v[g] = 0;
    
    for(int g = 0; g < k; g++){
        long long best = findbest(tab[g].val);
        update(tab[g].val + powertwo, val[tab[g].val].val + best);
    }

    
    int resindex = 0;
    long long result = 0;
    for(int g = 0; g < k; g++){
        if(v[g + powertwo] > result){
            result = v[g + powertwo];
            resindex = g; 
        }
    }
    printf("%lld\n", result);
    Enter actual = val[resindex];
    vector <Enter> path;
    path.push_back(actual);
    result -= actual.val;
    while(result > 0){
        for(int g = resindex; g >= 0; g--){
            if(v[g+powertwo] == result && val[g].x <= actual.x && val[g].y <= actual.y){
                resindex = g;
                actual = val[resindex];
                path.push_back(actual);
                result -= actual.val;
                break;
            }
        }
    }
    printf("%d\n", (int) path.size());
    for(int g = (int) path.size() - 1; g >= 0; g--)
        printf("%d %d\n", path[g].x, path[g].y);
}