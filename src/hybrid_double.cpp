// Hybrid bounds for a(n)=f(n,1) (OEIS A398786).
//  UPPER (any n): exact top-down from (n,1) to level k0=n-D (exact t, u128), then f(k0,t) <= U_k0(t mod M), M=19#,
//                 U computed in double with upward rounding (rigorous).  U_k checkpointed to disk.
//  LOWER (k0<=28): f(k0,t) >= L_k0(t mod M) where L_22 = exact big-t table F22 and the A-move at level 23 is dropped
//                 (prime 23 is the only prime <=28 not dividing M). Downward rounding. Small t at the cut: exact recursion.
#include <bits/stdc++.h>
using namespace std;
typedef unsigned long long u64; typedef unsigned __int128 u128;
static const u64 M=9699690ULL; static u64 PRIM[64];
static u64 gcdu(u64 a,u64 b){while(b){u64 t=a%b;a=b;b=t;}return a;}
static bool isprime(u64 n){if(n<2)return false;for(u64 q=2;q*q<=n;q++)if(n%q==0)return false;return true;}
static inline double up(double x){return nextafter(x,INFINITY);} static inline double dn(double x){return nextafter(x,-INFINITY);}
struct H{ size_t operator()(const u128&x)const{ u64 a=(u64)x,b=(u64)(x>>64); return a*0x9E3779B97F4A7C15ULL ^ (b+0x632BE59BD9B4E019ULL)*0xC2B2AE3D27D4EB4FULL; } };
static vector<vector<pair<u64,u64>>> PDIV(200);
static string ckname(int k){ return "U_"+to_string(k)+".bin"; }
static bool loadU(int k,vector<double>&U){ FILE*f=fopen(ckname(k).c_str(),"rb"); if(!f)return false; U.resize(M); size_t r=fread(U.data(),8,M,f); fclose(f); return r==M; }
static void saveU(int k,vector<double>&U){ FILE*f=fopen(ckname(k).c_str(),"wb"); fwrite(U.data(),8,M,f); fclose(f); }
static void stepU(int k,vector<double>&U,vector<double>&nU){ u64 kM=gcdu(k,M);
    for(u64 r=0;r<M;r++){ double s=0; if(gcdu(r,kM)==1) s=up(s+U[(r+M-(k%M))%M]);
        for(auto&de:PDIV[k]){ u64 dM=gcdu(de.first,M); if(dM==1||gcdu(r,dM)==1) s=up(s+U[(u64)(((u128)de.second*r)%M)]); } nU[r]=s; } U.swap(nU); }
// exact tables FT[k] (big t) for k<=22, u64
static vector<u64> FT[23];
static void buildFT(){ FT[1].assign(1,1); for(int k=2;k<=22;k++){ u64 Mk=PRIM[k],Mp=PRIM[k-1]; vector<u64>cur(Mk); auto&prev=FT[k-1];
    for(u64 r=0;r<Mk;r++){ u64 s=0; if(gcdu(r,k)==1) s+=prev[(r+Mp-(k%Mp))%Mp]; for(auto&de:PDIV[k]) if(gcdu(r,de.first)==1) s+=prev[(u64)(((u128)de.second*r)%Mp)]; cur[r]=s; } FT[k].swap(cur);} }
static map<pair<int,u64>,double> smallmemo; static vector<double> Ltab[29];
static double exact_small(int k,u64 t){ // exact f(k,t) for k<=22; for 23<=k<=28 a LOWER bound (uses Ltab for big children)
    if(k==1) return 1; if(t>=(u64)k*k+k){ if(k<=22) return (double)FT[k][t%PRIM[k]]; return Ltab[k][t%M]; }
    auto key=make_pair(k,t); auto it=smallmemo.find(key); if(it!=smallmemo.end()) return it->second;
    double s=0; if(t>=(u64)k+2&&gcdu(t,k)==1) s=dn(s+exact_small(k-1,t-k));
    for(auto&de:PDIV[k]) if(gcdu(t,de.first)==1) s=dn(s+exact_small(k-1,de.second*t)); smallmemo[key]=s; return s; }
int main(int argc,char**argv){
    string mode=argv[1]; int D=atoi(argv[2]), nmin=atoi(argv[3]), nmax=atoi(argv[4]);
    PRIM[0]=1; for(int k=1;k<64;k++){PRIM[k]=PRIM[k-1]*(isprime(k)?k:1); if(k>=48)PRIM[k]=0;}
    for(int k=1;k<200;k++) for(u64 d=1;d<(u64)k;d++) if(k%d==0) PDIV[k].push_back({d,k/d});
    auto topdown=[&](int n,int k0,unordered_map<u128,u64,H>&cur){ unordered_map<u128,u64,H> nxt; cur.clear(); cur[(u128)1]=1;
        for(int j=n;j>k0+1;j--){ nxt.clear(); nxt.reserve(cur.size()*3);
            for(auto&kv:cur){ u128 t=kv.first; u64 m=kv.second;
                if(t>=(u128)(j+2)&&gcdu((u64)(t%j),j)==1) nxt[t-j]+=m;
                for(auto&de:PDIV[j]) if(gcdu((u64)(t%de.first),de.first)==1) nxt[t*de.second]+=m; }
            cur.swap(nxt);} };
    // stream children at level k0 of all states at level k0+1 (no materialization); returns number of parents
    auto stream_last=[&](int k0,unordered_map<u128,u64,H>&cur,const function<void(u128,u64)>&cb){ int j=k0+1;
        for(auto&kv:cur){ u128 t=kv.first; u64 m=kv.second;
            if(t>=(u128)(j+2)&&gcdu((u64)(t%j),j)==1) cb(t-j,m);
            for(auto&de:PDIV[j]) if(gcdu((u64)(t%de.first),de.first)==1) cb(t*de.second,m); } };
    if(mode=="upper"){
        vector<double> U,nU(M); int k=0; for(int c=nmax-D;c>=1;c--) if(loadU(c,U)){k=c;break;} if(!k){U.assign(M,1.0);k=1;}
        for(k=k+1;k<=nmax-D;k++){ stepU(k,U,nU); if(k%5==0&&!ifstream(ckname(k))) saveU(k,U);
            int n=k+D; if(n<nmin) continue; unordered_map<u128,u64,H> cur; topdown(n,k,cur);
            double B=0; stream_last(k,cur,[&](u128 t,u64 m){ B=up(B+up((double)m*U[(u64)(t%M)])); });
            printf("n=%d k0=%d states=%zu upper=%.17g\n",n,k,cur.size(),B); fflush(stdout); }
    } else { // lower: k0 = n-D must be in [22,28]
        buildFT(); Ltab[22].resize(M); for(u64 r=0;r<M;r++) Ltab[22][r]=(double)FT[22][r];
        for(int k=23;k<=28;k++){ Ltab[k].resize(M); auto&prev=Ltab[k-1];
            for(u64 r=0;r<M;r++){ double s=0; if(k!=23 && gcdu(r,k)==1) s=dn(s+prev[(r+M-(k%M))%M]);  // drop A-move at 23 (unknown r mod 23)
                for(auto&de:PDIV[k]) if(gcdu(r,de.first)==1) s=dn(s+prev[(u64)(((u128)de.second*r)%M)]); Ltab[k][r]=s; } }
        for(int n=nmin;n<=nmax;n++){ int k0=n-D; if(k0<22||k0>28){printf("n=%d skip\n",n);continue;}
            unordered_map<u128,u64,H> cur; topdown(n,k0,cur); double B=0;
            stream_last(k0,cur,[&](u128 t,u64 m){ double v = (t>=(u128)(k0*k0+k0)) ? Ltab[k0][(u64)(t%M)] : exact_small(k0,(u64)t); B=dn(B+dn((double)m*v)); });
            printf("n=%d k0=%d states=%zu lower=%.17g\n",n,k0,cur.size(),B); fflush(stdout); }
    }
}
