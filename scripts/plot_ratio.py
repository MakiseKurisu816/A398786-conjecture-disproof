#!/usr/bin/env python3
"""Plot a(n)/P(n): exact for n<=40 (OEIS + data/exact_a26_a40.txt), MC estimates for 41..100 (data/estimates)."""
import re, os, sys
os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..'))
def P(n):
    p = 1
    for k in range(2, n+1): p *= sum(1 for d in range(1, k) if k % d == 0)
    return p
oeis = [1,1,1,2,6,9,25,59,194,480,846,1623,5046,9244,42039,143655,245396,427073,985248,4805090,19234681,47271270,63010185,223465561,928167756]
exact = {i+1: v for i, v in enumerate(oeis)}
for line in open('data/exact_a26_a40.txt'):
    if line[0].isdigit(): n, v = line.split()[:2]; exact[int(n)] = int(v)
est = {}
for line in open('data/estimates/mc_n41_100_seed2024.log'):
    m = re.match(r'n=(\d+) est=([\d.e+]+)', line)
    if m: est[int(m[1])] = float(m[2])
try:
    import matplotlib; matplotlib.use('Agg'); import matplotlib.pyplot as plt
except ImportError:
    sys.exit("matplotlib not installed: pip install matplotlib")
xs1 = sorted(n for n in exact if n >= 4); ys1 = [exact[n]/P(n) for n in xs1]
xs2 = sorted(est); ys2 = [est[n]/P(n) for n in xs2]
fig, ax = plt.subplots(figsize=(9, 4.2), dpi=160)
ax.axhline(1, color='k', lw=0.8, ls='--')
ax.plot(xs1, ys1, 'o-', ms=3, lw=1, label='exact (n ≤ 40)')
ax.plot(xs2, ys2, 's-', ms=3, lw=1, color='tab:orange', label='unbiased estimate (41–100, s.e. ≤ 0.3%)')
ax.plot([90], [est[90]/P(90)], 'o', ms=9, mfc='none', mec='red', mew=2, label='n = 90: first a(n) < P(n)  (proved)')
ax.set_yscale('log'); ax.set_xlabel('n'); ax.set_ylabel('a(n) / P(n)')
ax.set_title('OEIS A398786: a(n) / ∏(d(k)−1)   — conjecture a(n) ≥ P(n) fails first at n = 90')
ax.legend(fontsize=8); ax.grid(alpha=.3, which='both')
fig.tight_layout(); fig.savefig('ratio_plot.png'); print('wrote ratio_plot.png')
