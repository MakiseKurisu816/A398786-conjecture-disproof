// Exact-integer certificates for OEIS A398786.
//   mode upper n D : a(n) <= sum_{states at level k0=n-D} mult * U_k0(t mod M), M=19#, U exact in u128, sum in 256-bit.
//   mode lower n D : a(n) >= sum mult * L_k0(t)  with k0=n-D in [22,28]; L exact u64 (drops A-move at level 23 only).
#include <bits/stdc++.h>
using namespace std;
typedef unsigned long long u64; typedef unsigned __int128 u128;
static const u64 M=9699690ULL; static u64 PRIM[64];
static u64 gcdu(u64 a,u64 b){while(b){u64 t=a%b;a=b;b=t;}return a;}
static bool isprime(u64 n){if(n<2)return false;for(u64 q=2;q*q<=n;q++)if(n%q==0)return false;return true;}
struct H{ size_t operator()(const u128&x)const{ u64 a=(u64)x,b=(u64)(x>>64); return a*0x9E3779B97F4A7C15ULL ^ (b+0x632BE59BD9B4E019ULL)*0xC2B2AE3D27D4EB4FULL; } };
struct U256{ u128 lo=0,hi=0; void add(u128 x){ u128 n=lo+x; if(n<lo) hi++; lo=n; } };
static string dec(U256 v){ // base 10 via repeated division by 10^18
    if(!v.lo&&!v.hi) return "0"; string s; const u128 B=1000000000000000000ULL;
    while(v.lo||v.hi){ // divide (hi,lo) by B
        u128 r=0; u128 q_hi=0,q_lo=0; // schoolbook on 64-bit limbs
        u64 limbs[4]={(u64)v.lo,(u64)(v.lo>>64),(u64)v.hi,(u64)(v.hi>>64)}; u64 ql[4];
        for(int i=3;i>=0;i--){ u128 cur=(r<<64)|limbs[i]; ql[i]=(u64)(cur/B); r=cur%B; }
        q_lo=((u128)ql[1]<<64)|ql[0]; q_hi=((u128)ql[3]<<64)|ql[2];
        char buf[32]; snprintf(buf,32,"%018llu",(u64)r); s=string(buf)+s; v.lo=q_lo; v.hi=q_hi; }
    size_t p=s.find_first_not_of('0'); return s.substr(p); }
static string dec128(u128 x){ U256 v; v.lo=x; return dec(v); }
static vector<vector<pair<u64,u64>>> PDIV(200);
int main(int argc,char**argv){
    string mode=argv[1]; int n=atoi(argv[2]), D=atoi(argv[3]); int k0=n-D;
    PRIM[0]=1; for(int k=1;k<64;k++){PRIM[k]=PRIM[k-1]*(isprime(k)?k:1); if(k>=48)PRIM[k]=0;}
    for(int k=1;k<200;k++) for(u64 d=1;d<(u64)k;d++) if(k%d==0) PDIV[k].push_back({d,k/d});
    // top-down exact states to level k0+1
    unordered_map<u128,u64,H> cur,nxt; cur[(u128)1]=1;
    for(int j=n;j>k0+1;j--){ nxt.clear(); nxt.reserve(cur.size()*3);
        for(auto&kv:cur){ u128 t=kv.first; u64 m=kv.second;
            if(t>=(u128)(j+2)&&gcdu((u64)(t%j),j)==1){ u64&x=nxt[t-j]; if(x>~m){fprintf(stderr,"mult overflow\n");return 1;} x+=m; }
            for(auto&de:PDIV[j]) if(gcdu((u64)(t%de.first),de.first)==1){ u64&x=nxt[t*de.second]; if(x>~m){fprintf(stderr,"mult overflow\n");return 1;} x+=m; } }
        cur.swap(nxt); }
    fprintf(stderr,"states at level %d: %zu\n",k0+1,cur.size());
    U256 S; int j=k0+1;
    if(mode=="upper"){
        vector<u128> U(M,1),nU(M);
        for(int k=2;k<=k0;k++){ u64 kM=gcdu(k,M);
            for(u64 r=0;r<M;r++){ u128 s=0; if(gcdu(r,kM)==1) s+=U[(r+M-(k%M))%M];
                for(auto&de:PDIV[k]){ u64 dM=gcdu(de.first,M); if(dM==1||gcdu(r,dM)==1){ u128 v=U[(u64)(((u128)de.second*r)%M)]; if(s+v<s){fprintf(stderr,"U overflow\n");return 1;} s+=v; } } nU[r]=s; }
            U.swap(nU); }
        auto leaf=[&](u128 t,u64 m){ u128 v=U[(u64)(t%M)]; // m*v with overflow check into 256
            u128 a=(u128)(u64)v*m, b=(u128)(u64)(v>>64)*m; // v = vhi*2^64+vlo
            U256 term; term.lo=a; term.hi=0; U256 bb; bb.lo=b<<64; bb.hi=b>>64; // b*2^64
            S.add(term.lo); S.add(bb.lo); S.hi+=bb.hi; };
        for(auto&kv:cur){ u128 t=kv.first; u64 m=kv.second;
            if(t>=(u128)(j+2)&&gcdu((u64)(t%j),j)==1) leaf(t-j,m);
            for(auto&de:PDIV[j]) if(gcdu((u64)(t%de.first),de.first)==1) leaf(t*de.second,m); }
        printf("UPPER n=%d k0=%d D=%d M=%llu states=%zu bound=%s\n",n,k0,D,M,cur.size(),dec(S).c_str());
    } else {
        if(k0<22||k0>28){fprintf(stderr,"lower needs 22<=k0<=28\n");return 1;}
        // exact FT[k] for k<=22 (u64), then L[23..28] on Z/M (u64, exact except A-move at 23 dropped)
        vector<u64> FT[23]; FT[1].assign(1,1);
        for(int k=2;k<=22;k++){ u64 Mk=PRIM[k],Mp=PRIM[k-1]; FT[k].assign(Mk,0); auto&prev=FT[k-1];
            for(u64 r=0;r<Mk;r++){ u64 s=0; if(gcdu(r,k)==1) s+=prev[(r+Mp-(k%Mp))%Mp]; for(auto&de:PDIV[k]) if(gcdu(r,de.first)==1) s+=prev[(u64)(((u128)de.second*r)%Mp)]; FT[k][r]=s; } }
        vector<vector<u64>> L(29); L[22]=FT[22];
        for(int k=23;k<=k0;k++){ L[k].assign(M,0); auto&prev=L[k-1];
            for(u64 r=0;r<M;r++){ u128 s=0; if(k!=23&&gcdu(r,k)==1) s+=prev[(r+M-(k%M))%M];
                for(auto&de:PDIV[k]) if(gcdu(r,de.first)==1) s+=prev[(u64)(((u128)de.second*r)%M)]; if(s>>64){fprintf(stderr,"L overflow\n");return 1;} L[k][r]=(u64)s; } }
        unordered_map<u64,u64> sm[29];
        function<u64(int,u64)> lowsmall=[&](int k,u64 t)->u64{ if(k==1)return 1; if(t>=(u64)k*k+k) return k<=22?FT[k][t%PRIM[k]]:L[k][t%M];
            auto it=sm[k].find(t); if(it!=sm[k].end())return it->second; u128 s=0; if(t>=(u64)k+2&&gcdu(t,k)==1) s+=lowsmall(k-1,t-k);
            for(auto&de:PDIV[k]) if(gcdu(t,de.first)==1) s+=lowsmall(k-1,de.second*t); if(s>>64){fprintf(stderr,"ovf\n");exit(1);} sm[k][t]=(u64)s; return (u64)s; };
        auto leaf=[&](u128 t,u64 m){ u64 v = t>=(u128)(k0*k0+k0) ? L[k0][(u64)(t%M)] : lowsmall(k0,(u64)t); S.add((u128)v*m); };
        for(auto&kv:cur){ u128 t=kv.first; u64 m=kv.second;
            if(t>=(u128)(j+2)&&gcdu((u64)(t%j),j)==1) leaf(t-j,m);
            for(auto&de:PDIV[j]) if(gcdu((u64)(t%de.first),de.first)==1) leaf(t*de.second,m); }
        printf("LOWER n=%d k0=%d D=%d M=%llu states=%zu bound=%s\n",n,k0,D,M,cur.size(),dec(S).c_str());
    }
}
