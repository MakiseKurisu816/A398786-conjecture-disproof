# X / Twitter thread

Six posts, each under 280 characters. Suggested image for post 1: `ratio_plot.png`.

**1/**
A conjecture posted to the OEIS last month (A398786, Sep 2026): for a gcd-driven recursion, the number a(n) of starting values first hitting 1 at step n satisfies a(n) ≥ ∏(d(k)−1). The ratio climbs from 1 to 15 over 25 terms.
It's false. Smallest counterexample: n = 90. 🧵

**2/**
Rigorous (exact-integer certificates, reproducible):
• n ≤ 44: holds
• n = 90: a(90) < P(90), ratio ≤ 0.866
• n = 120 and all of 120–300: false
For 45–89 an unbiased estimator (s.e. ≤ 0.2%) confirms it holds; low point n = 84, ratio 1.19.

**3/**
Why: at primes the "add" move almost always exists (factor ≈ 2 > d(p)−1 = 1). At highly composite k, values divisible by small primes lose several branches at once, and ∏(d(k)−1) outgrows the true branching. First dip below 1 is exactly 90 = 2·3²·5, then 96, 98, 99, 100…

**4/**
Method: the forward map is deterministic ⇒ a(n) = leaves of a backward tree. For large t the count depends only on t mod primorial(k). Upper bound: drop size constraints, track t mod 19#. Lower bound: start from the exact k=22 table, drop only undecidable moves. Top levels enumerated exactly (10⁷ states, 128/256-bit ints).

**5/**
Bonus: 15 new terms a(26)…a(40); a(40) = 9273038181155701.
A rigorous lower bound on 45–89 needs ~100× the memory or a new idea — that stretch is statistical evidence (every point > 100σ from the threshold), stated as such in the report.

**6/**
Code, certificate logs, full report with proofs of both lemmas: https://github.com/MakiseKurisu816/A398786-conjecture-disproof
`make && make quick` reproduces the core certificates in ~2 min.
AI-assisted (Claude) for the search, proof strategy and code; every certificate re-run and checked by me. Corrections welcome — and so is anyone who closes the 45–89 gap.
