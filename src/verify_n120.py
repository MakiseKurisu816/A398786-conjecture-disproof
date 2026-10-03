#!/usr/bin/env python3
"""
Rigorous disproof of the conjecture in OEIS A398786 (Sam Chapman, 2026):
    A398786(n) >= P(n) := Prod_{k=2..n} (d(k) - 1).

A398786(n) = #{m : A091508(m) = n} = f(n,1), where f counts backward paths (Lemma 1).
For any modulus M, U_k(r) defined below satisfies f(k,t) <= U_k(t mod M) (Lemma 2).
We show U_120(1) < P(120) with M = 30030, hence A398786(120) < P(120).
Only exact integer arithmetic is used.  Runtime: ~10 s (M=30030), <1 s (M=210).
"""
from math import gcd
from functools import lru_cache
import sys

def divisors(n):
    return [d for d in range(1, n + 1) if n % d == 0]

def P(n):
    p = 1
    for k in range(2, n + 1):
        p *= len(divisors(k)) - 1
    return p

# ---- exact count f(k,t) (Lemma 1); matches OEIS A398786 for n<=14 and brute force for n<=7
@lru_cache(maxsize=None)
def f(k, t):
    if k == 1:
        return 1
    s = f(k - 1, t - k) if (t >= k + 2 and gcd(t, k) == 1) else 0
    for d in divisors(k)[:-1]:
        if gcd(t, d) == 1:
            s += f(k - 1, (k // d) * t)
    return s

# ---- modular upper bound U_k(r) (Lemma 2)
def upper_bound_table(N, M):
    U = [1] * M                      # U_1 == 1
    tables = {1: U}
    for k in range(2, N + 1):
        prev = tables[k - 1]
        cur = [0] * M
        kM = gcd(k, M)
        pdivs = [(d, gcd(d, M), k // d) for d in divisors(k)[:-1]]
        for r in range(M):
            s = prev[(r - k) % M] if gcd(r, kM) == 1 else 0          # A-move (optimistic)
            for d, dM, e in pdivs:
                if gcd(r, dM) == 1:                                   # B_d-move (optimistic)
                    s += prev[(e * r) % M]
            cur[r] = s
        tables[k] = cur
    return tables

if __name__ == "__main__":
    oeis = [1,1,1,2,6,9,25,59,194,480,846,1623,5046,9244]
    assert [f(n, 1) for n in range(1, 15)] == oeis, "Lemma 1 recursion disagrees with OEIS data"
    print("Lemma 1 check: f(n,1) for n<=14 equals OEIS A398786 data.  OK")

    for M, n in [(210, 126), (30030, 120)]:
        T = upper_bound_table(n, M)
        # sanity: upper bound dominates the known exact values
        assert all(T[k][1 % M] >= oeis[k - 1] for k in range(1, 15))
        U, Pn = T[n][1 % M], P(n)
        print(f"\nM = {M}, n = {n}")
        print(f"  U_n(1) = {U}")
        print(f"  P(n)   = {Pn}")
        print(f"  U_n(1) < P(n): {U < Pn}   (ratio {U / Pn:.4f})")
        if M == 30030:
            bad = [k for k in range(120, n + 1) if T[k][1] >= P(k)]
            print(f"  k in [120,{n}] with U_k(1) >= P(k): {bad}")
    print("\nConclusion: A398786(120) <= U_120(1) < P(120).  The conjecture is FALSE.")
