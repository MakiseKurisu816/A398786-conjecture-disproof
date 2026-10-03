import sys
exact={30:91313705322,31:288850466259,32:850322925064,33:4012051390889,34:8872132697704,35:34766652126827,36:105962930618570,37:233048825328494,38:427922092082052,39:1490289805794031,40:9273038181155701}
def P(n):
    p=1
    for k in range(2,n+1): p*=sum(1 for d in range(1,k) if k%d==0)
    return p
for line in sys.stdin:
    if not line.startswith('n='): print(line,end=''); continue
    d=dict(x.split('=') for x in line.split()); n=int(d['n']); est=float(d['est']); rel=float(d['relse'])
    ex=f"  exact/P={exact[n]/P(n):.4f}  est/exact={est/exact[n]:.4f}" if n in exact else ""
    print(f"n={n:4d}  est/P={est/P(n):8.4f} ± {rel*100:.2f}%{ex}")
