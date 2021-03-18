#include<bits/stdc++.h>
using namespace std;

double res;
pair <int, int> ans1, ans2, ans3;

bool compy(pair <int,int> p1, pair <int,int> p2){
    if(p1.second != p2.second)
        return  p1.second < p2.second;
    return p1.first < p2.first;
}


bool compx(pair <int,int> p1, pair <int,int> p2){
    if(p1.first != p2.first)
        return  p1.first < p2.first;
    return p1.second < p2.second;
}


double dist(pair <int,int> p1, pair <int,int> p2){
    return sqrt((long long)(p1.first - p2.first) * (long long)(p1.first - p2.first) + (long long)(p1.second - p2.second) * (long long)(p1.second - p2.second));
}


double Triangle(pair <int,int> p1, pair <int,int> p2, pair <int,int> p3){
    return dist(p1, p2) + dist(p1,p3) + dist(p2,p3);
}


void betterres(pair <int,int> p1, pair <int,int> p2, pair <int,int> p3){
    double val = Triangle(p1, p2, p3);
    if(val < res){
        res = val;
        ans1 = p1;
        ans2 = p2;
        ans3 = p3;
        return;
    }
    return;
}


void rec(int n, pair <int,int> vx[], pair <int, int> vy[]){
    if(n < 12){
        for(int it = 0; it < n; it++){
            for(int g = it+1; g < n; g++){
                if(2 * dist(vx[it], vx[g]) >= res)
                    continue;
                for(int h = g+1; h < n; h++)
                    betterres(vx[it], vx[g], vx[h]);
            }
        }
        return;
    }

    int mid = n>>1;
    pair <int, int> p = vx[mid];
    pair <int, int> l[mid];
    pair <int, int> r[n-mid];
    int il = 0, ir = 0;
    for(int it = 0; it < n; it++){
        if(compx(vy[it], p))
            l[il++] = vy[it];
        else
            r[ir++] = vy[it];   
    }

    rec(mid, vx, l);
    rec(n-mid, vx+mid, r);
    
    pair <int, int> m[n];
    int im = 0;
    int cnt = 0;

    for(int it = 0; it < n; it++){
        pair <int, int> p2 = vy[it];
        
        if(2 * abs(p.first - p2.first) > res)
            continue;

        while(cnt < im && 2 * (p2.second - m[cnt].second)  > res)
            cnt++;

        for(int g = cnt; g < im; g++){
            for(int h = g+1; h < im; h++){
                betterres(p2, m[g], m[h]);
            }
        }
        m[im++] = p2;
    }
    return;
}


int main(){
    int n;
    vector <pair <int, int> > vx;
    vector <pair <int, int> > vy;
    scanf("%d", &n);
    for(int it = 0; it < n; it++){
        int a, b;
        scanf("%d%d",&a, &b);
        vx.push_back(make_pair(a,b));
    }
    sort(vx.begin(), vx.end(), compx);
    vy = vx;
    sort(vy.begin(), vy.end(), compy);
    ans1 = vx[0];
    ans2 = vx[1];
    ans3 = vx[2];
    res = Triangle(ans1, ans2, ans3);

    rec(n, &vx[0], &vy[0]);

    printf("%d %d\n%d %d\n%d %d\n", ans1.first, ans1.second, ans2.first, ans2.second, ans3.first, ans3.second);
}