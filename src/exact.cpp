// Exact a(n) = f(n,1) for OEIS A398786, via:
//  * exact table F22[r] = f(22,t) for all big t (t >= 22^2+22), r = t mod prim(22)
//  * exact small table for f(22,t), t < 22^2+22
//  * top-down memoized recursion above level 22 (key: t mod prim(k) if big, else t)
#include <bits/stdc++.h>
using namespace std;
typedef unsigned long long u64; typedef unsigned __int128 u128;
static u64 PRIM[64]; static vector<pair<u64,u64>> PDIV[128]; // (d, k/d) for proper divisors d
static u64 gcdu(u64 a,u64 b){while(b){u64 t=a%b;a=b;b=t;}return a;}
static bool isprime(u64 n){if(n<2)return false;for(u64 q=2;q*q<=n;q++)if(n%q==0)return false;return true;}
const int K0=22; static vector<u64> FT[K0+1]; static vector<u64> S22; // FT[k]: exact f(k,t) for big t, indexed by t mod prim(k)
static u64 small_f(int k,u64 t,map<pair<int,u64>,u64>&m){ // exact f(k,t) for k<=22 (any t)
    if(k==1)return 1; if(t>=(u64)k*k+k) return FT[k][t%PRIM[k]];
    auto key=make_pair(k,t); auto it=m.find(key); if(it!=m.end())return it->second;
    u64 s=0; if(t>=(u64)k+2 && gcdu(t,k)==1) s+=small_f(k-1,t-k,m);
    for(auto&de:PDIV[k]) if(gcdu(t,de.first)==1) s+=small_f(k-1,de.second*t,m);
    m[key]=s; return s;
}
static vector<unordered_map<u64,u64>> MEMO; static vector<char> MEMO_ON; static size_t CAP;
// f(k, v, big): if big, v = t mod prim(k) for some t >= k^2+k (bigness is hereditary); else v = t exactly.
static u64 f(int k,u64 v,bool big){
    if(!big && v>=(u64)k*k+k){ big=true; v%=PRIM[k]; }
    if(k==K0){ return big ? FT[K0][v] : S22[v]; }
    u64 key = big ? v : (u64)(-(long long)v);
    if(MEMO_ON[k]){ auto it=MEMO[k].find(key); if(it!=MEMO[k].end()) return it->second; }
    u128 s=0; u64 Mp=PRIM[k-1];
    if(big){
        if(gcdu(v,k)==1) s+=f(k-1,(v+Mp-(k%Mp))%Mp,true);           // gcd(t,k) determined by v since k | prim(k)... rad(k) | prim(k)
        for(auto&de:PDIV[k]) if(gcdu(v,de.first)==1) s+=f(k-1,(u64)(((u128)de.second*v)%Mp),true);
    } else {
        if(v>=(u64)k+2 && gcdu(v,k)==1) s+=f(k-1,v-k,false);
        for(auto&de:PDIV[k]) if(gcdu(v,de.first)==1) s+=f(k-1,de.second*v,false);   // de.second*v < k*(k^2+k): no overflow
    }
    if(s>>64){fprintf(stderr,"overflow count\n");exit(1);}
    if(MEMO_ON[k]){ if(MEMO[k].size()<CAP) MEMO[k][key]=(u64)s; else MEMO_ON[k]=0; }
    return (u64)s;
}
int main(int argc,char**argv){
    int nmin=atoi(argv[1]), nmax=atoi(argv[2]); CAP = argc>3? atoll(argv[3]) : 40000000ULL;
    PRIM[0]=1; for(int k=1;k<64;k++){ PRIM[k]=PRIM[k-1]*(isprime(k)?k:1); if(k>=48)PRIM[k]=0; }
    for(int k=1;k<128;k++) for(u64 d=1;d<(u64)k;d++) if(k%d==0) PDIV[k].push_back({d,k/d});
    // bottom-up exact big-t tables F_k on Z/prim(k), k=1..22
    FT[1].assign(1,1);
    for(int k=2;k<=K0;k++){ u64 M=PRIM[k], Mp=PRIM[k-1]; vector<u64> cur(M); vector<u64>&prev=FT[k-1];
        for(u64 r=0;r<M;r++){ u64 s=0; if(gcdu(r,k)==1) s+=prev[((r+Mp-(k%Mp))%Mp)];
            for(auto&de:PDIV[k]) if(gcdu(r,de.first)==1) s+=prev[(u64)(((u128)de.second*r)%Mp)];
            cur[r]=s; }
        FT[k].swap(cur); }
    { map<pair<int,u64>,u64> m; S22.resize((u64)K0*K0+K0); for(u64 t=0;t<S22.size();t++) S22[t]= t? small_f(K0,t,m):0;
      // consistency: for t just above threshold table must agree with direct recursion
      for(u64 t=K0*K0+K0;t<K0*K0+K0+200;t++) if(small_f(K0,t,m)!=FT[K0][t%PRIM[K0]]){fprintf(stderr,"TABLE MISMATCH t=%llu\n",t);return 1;} }
    fprintf(stderr,"tables ready\n");
    MEMO.resize(64); MEMO_ON.assign(64,1);
    for(int n=nmin;n<=nmax;n++){ for(auto&m:MEMO){m.clear();} fill(MEMO_ON.begin(),MEMO_ON.end(),1);
        auto t0=chrono::steady_clock::now(); u64 a = n<=K0 ? (n<K0? small_f(n,1,*new map<pair<int,u64>,u64>) : S22[1]) : f(n,1,false);
        // P(n)
        u128 P=1; for(int k=2;k<=n;k++) P*= (u128)(PDIV[k].size());  // d(k)-1 = #proper divisors
        double sec=chrono::duration<double>(chrono::steady_clock::now()-t0).count();
        size_t st=0; for(auto&m:MEMO) st+=m.size();
        char pb[64]; { u128 x=P; int i=63; pb[i]=0; if(!x)pb[--i]='0'; while(x){pb[--i]='0'+(int)(x%10);x/=10;} 
          printf("n=%d a(n)=%llu P(n)=%s a/P=%.4f states=%zu time=%.1fs\n",n,a,pb+i,(double)a/(double)P,st,sec); }
        fflush(stdout);
    }
}
