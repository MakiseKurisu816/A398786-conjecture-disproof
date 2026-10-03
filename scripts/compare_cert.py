import sys
def P(n):
    p=1
    for k in range(2,n+1): p*=sum(1 for d in range(1,k) if k%d==0)
    return p
exact={36:105962930618570,37:233048825328494,38:427922092082052,39:1490289805794031,40:9273038181155701}
for line in sys.stdin:
    if not (line.startswith('UPPER') or line.startswith('LOWER')): print(line,end=''); continue
    kind=line.split()[0]; d=dict(x.split('=') for x in line.split()[1:]); n=int(d['n']); B=int(d['bound']); Pn=P(n)
    ok = (B<Pn) if kind=='UPPER' else (B>=Pn)
    ex = f"  [exact a(n)={exact[n]}, bound<=exact: {B<=exact[n]}]" if (n in exact and kind=='LOWER') else ""
    print(f"{kind} n={n} k0={d['k0']} states={d['states']}\n   bound={B}\n   P(n) ={Pn}\n   ratio={B/Pn:.4f}  -> {'a(n) < P(n) PROVEN' if (kind=='UPPER' and ok) else 'a(n) >= P(n) PROVEN' if (kind=='LOWER' and ok) else 'inconclusive'}{ex}")
