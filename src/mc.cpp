// Knuth unbiased estimator for a(n)=f(n,1): random descent from (n,1) to level 22, weight = prod(#valid children) * f(22,t) exact.
// t represented exactly while small (< j^2+j), then by residues mod each prime <= n plus t mod prim(22).
#include <bits/stdc++.h>
using namespace std;
typedef unsigned long long u64; typedef unsigned __int128 u128;
static u64 PRIM[64]; static vector<vector<pair<u64,u64>>> PDIV(200); static vector<int> PR; // primes <= nmax
static u64 gcdu(u64 a,u64 b){while(b){u64 t=a%b;a=b;b=t;}return a;}
static bool isprime(u64 n){if(n<2)return false;for(u64 q=2;q*q<=n;q++)if(n%q==0)return false;return true;}
static vector<u64> FT[23]; static map<pair<int,u64>,u64> sm;
static void buildFT(){ FT[1].assign(1,1); for(int k=2;k<=22;k++){ u64 Mk=PRIM[k],Mp=PRIM[k-1]; vector<u64>cur(Mk); auto&prev=FT[k-1];
    for(u64 r=0;r<Mk;r++){ u64 s=0; if(gcdu(r,k)==1) s+=prev[(r+Mp-(k%Mp))%Mp]; for(auto&de:PDIV[k]) if(gcdu(r,de.first)==1) s+=prev[(u64)(((u128)de.second*r)%Mp)]; cur[r]=s; } FT[k].swap(cur);} }
static u64 f22(int k,u64 t){ if(k==1)return 1; if(t>=(u64)k*k+k) return FT[k][t%PRIM[k]]; auto key=make_pair(k,t); auto it=sm.find(key); if(it!=sm.end())return it->second;
    u64 s=0; if(t>=(u64)k+2&&gcdu(t,k)==1) s+=f22(k-1,t-k); for(auto&de:PDIV[k]) if(gcdu(t,de.first)==1) s+=f22(k-1,de.second*t); sm[key]=s; return s; }
int main(int argc,char**argv){
    int nmin=atoi(argv[1]), nmax=atoi(argv[2]); long long S=atoll(argv[3]); u64 seed=argc>4?atoll(argv[4]):12345;
    PRIM[0]=1; for(int k=1;k<64;k++){PRIM[k]=PRIM[k-1]*(isprime(k)?k:1); if(k>=48)PRIM[k]=0;}
    for(int k=1;k<200;k++) for(u64 d=1;d<(u64)k;d++) if(k%d==0) PDIV[k].push_back({d,k/d});
    for(int p=2;p<=nmax;p++) if(isprime(p)) PR.push_back(p);
    // prime factor lists
    vector<vector<int>> PF(200); for(int j=2;j<200;j++) for(size_t i=0;i<PR.size();i++) if(j%PR[i]==0) PF[j].push_back(i);
    buildFT(); const u64 M22=PRIM[22];
    // proxy h(k, r) = U_k(r mod 30030) (upper-bound-style count, tracks small-prime divisibility), k=1..nmax
    const u64 MH=30030; vector<vector<double>> Hh(nmax+1, vector<double>(MH,1.0));
    for(int k=2;k<=nmax;k++){ u64 kM=gcdu(k,MH); for(u64 r=0;r<MH;r++){ double x=0; if(gcdu(r,kM)==1) x+=Hh[k-1][(r+MH-(k%MH))%MH];
        for(auto&de:PDIV[k]){ u64 dM=gcdu(de.first,MH); if(dM==1||gcdu(r,dM)==1) x+=Hh[k-1][(de.second*r)%MH]; } Hh[k][r]=x; } }
    mt19937_64 rng(seed);
    for(int n=nmin;n<=nmax;n++){
        long double sum=0, sum2=0; int np=PR.size(); vector<u64> res(np); u64 rH=1;
        for(long long s=0;s<S;s++){
            bool big=false; u64 t=1, r22=0; long double w=1; rH=1;
            for(int j=n;j>22;j--){
                // enumerate valid children: encode as (type, idx)
                int ch[64]; int c=0;
                auto coprime=[&](int m)->bool{ if(!big) return gcdu(t,m)==1; for(int i:PF[m]) if(res[i]==0) return false; return true; };
                double hw[64]; double hs=0;
                if((big||t>=(u64)j+2) && coprime(j)){ ch[c]=-1; hw[c]=Hh[j-1][(rH+MH-(j%MH))%MH]; hs+=hw[c]; c++; }
                for(int i=0;i<(int)PDIV[j].size();i++) if(coprime(PDIV[j][i].first)){ ch[c]=i; hw[c]=Hh[j-1][(PDIV[j][i].second*rH)%MH]; hs+=hw[c]; c++; }
                double u=hs*(double)(rng()>>11)*0x1.0p-53; int q=0; while(q<c-1 && u>=hw[q]){ u-=hw[q]; q++; }
                int pick=ch[q]; w*=hs/hw[q];
                rH = pick<0 ? (rH+MH-(j%MH))%MH : (PDIV[j][pick].second*rH)%MH;
                if(!big){ if(pick<0) t-=j; else t*=PDIV[j][pick].second;
                    if(t>=(u64)(j-1)*(j-1)+(j-1)){ big=true; for(int i=0;i<np;i++) res[i]=t%PR[i]; r22=t%M22; } }
                else { if(pick<0){ for(int i=0;i<np;i++) res[i]=(res[i]+PR[i]-(j%PR[i]))%PR[i]; r22=(r22+M22-(j%M22))%M22; }
                       else { u64 e=PDIV[j][pick].second; for(int i=0;i<np;i++) res[i]=(res[i]*e)%PR[i]; r22=(u64)(((u128)r22*e)%M22); } }
            }
            long double v = big ? (long double)FT[22][r22] : (long double)f22(22,t);
            long double x=w*v; sum+=x; sum2+=x*x;
        }
        long double mean=sum/S, var=(sum2/S-mean*mean)/S; 
        printf("n=%d est=%.6Le se=%.3Le relse=%.4Lf\n",n,mean,sqrtl(var),sqrtl(var)/mean); fflush(stdout);
    }
}
