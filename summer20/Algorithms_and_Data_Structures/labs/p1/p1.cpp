#include "bits/stdc++.h"

using namespace std;

#define MAXN 100009

vector < pair <int, int> > v[MAXN];

bool vis[MAXN];
int  n, l, r, m, cnt = 0;

bool DFS(int x, int y){
    cnt++;
    vis[x] = 1;
    int z = v[x][y].first;
    if(z == 0){
        printf("%d\n", cnt);
        printf("%d %d %d\n", z, v[x][y].second, x);
        return 1;
    }
    for(unsigned int i = 0; i < v[z].size(); i++){
        if(vis[v[z][i].first] && v[z][i].first != 0)
            continue;
        if(DFS(z, i)){
            printf("%d %d %d\n", z, v[x][y].second, x);
            return 1;
        }
    }
    cnt--;
    return 0;
}

int main(){

    if(1 != scanf("%d", &n))
        return -1;
    while(n--){
        if(3 != scanf("%d %d %d", &l, &m, &r))
            return -1;
        v[r].push_back(make_pair(l, m));
    }

    for(unsigned int i = 0; i < v[0].size(); i++){
        if(vis[v[0][i].first])
            continue;
        if(DFS(0, i))
    	    return 0;
    }

    printf("BRAK\n");
}
