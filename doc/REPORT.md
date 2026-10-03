# Disproof of the lower-bound conjecture in OEIS A398786

> **Summary.** The conjecture stated in OEIS A398786 (Sam Chapman, September 2026),
> $$a(n)\ \ge\ \prod_{k=2}^{n}\bigl(d(k)-1\bigr),$$
> **is false.** We prove $a(90) < P(90)$ (and also $a(120) < P(120)$, and $a(n) < P(n)$ for every $120 \le n \le 300$); we prove that the conjecture holds for $n \le 44$; and unbiased estimates with relative standard error $\le 0.2\%$ show that it holds for $45 \le n \le 89$. **The smallest counterexample is $n = 90$** (rigorous bracket: $45 \le n^* \le 90$).

---

## 1. Origin and statement of the conjecture

**A091508** (Benoit Cloitre, 2004). For a positive integer $m$ put $b(1) = m$ and
$$
b(k+1)=\begin{cases} b(k)+(k+1), & \gcd(k+1,\,b(k))=1,\\[2pt] b(k)/\gcd(k+1,\,b(k)), & \text{otherwise}.\end{cases}
$$
$A091508(m)$ is the least $k$ with $b(k) = 1$.

**A398786** (Sam Chapman, 2026-09-06). $a(n) = \#\{\,m : A091508(m) = n\,\}$, the number of starting values that first reach 1 at step $n$. Known terms for $n = 1..25$:

```
1, 1, 1, 2, 6, 9, 25, 59, 194, 480, 846, 1623, 5046, 9244, 42039, 143655,
245396, 427073, 985248, 4805090, 19234681, 47271270, 63010185, 223465561, 928167756
```

**Conjecture** (Formula section of A398786, still marked "Conjecture" as of 2026-10-02). With $d(k)$ the number of divisors of $k$ and $P(n) := \prod_{k=2}^{n}(d(k)-1)$, one has $a(n) \ge P(n)$ for all $n$.

It holds for $n \le 25$, and $a(n)/P(n)$ grows from $1$ at $n = 4$ to $\approx 15.2$ at $n = 25$, so the evidence looks solid.

## 2. Main theorem

**Theorem.** $a(90) < P(90)$ and $a(120) < P(120)$. More precisely,
$$
a(90)\ \le\ 9.954\times10^{40}\ <\ 1.1495\times10^{41}\ =\ P(90),\qquad
a(120)\ \le\ 1.1395\times10^{58}\ <\ 1.5530\times10^{58}\ =\ P(120).
$$
Moreover $a(n) < P(n)$ for every $120 \le n \le 300$. Hence the conjecture is false. The $n = 120$ certificate is the simplest (Section 3, pure Python, about 10 seconds); the $n = 90$ certificate is in Section 5.3. On the other hand the conjecture holds rigorously for $n \le 44$ (Section 5), and the smallest counterexample is located in Section 5.

## 3. Proof

### 3.1 Lemma 1: counting backward paths

Let $f(k,t)$ be the number of sequences $(b_1,\dots,b_k)$ with $b_k = t$, $b_j \ge 2$ for $j < k$, and every step obeying the A091508 recursion. Since the forward process is deterministic, such sequences are in bijection with their starting values $b_1$, so
$$a(n) = f(n,1).$$

Given $(k,t)$, the previous term $b_{k-1} = s$ can arise in exactly two ways:

- **Type A (addition step):** $\gcd(k,s) = 1$ and $s + k = t$, i.e. $s = t - k$. This needs $s \ge 2$ and $\gcd(k, t-k) = \gcd(k,t) = 1$, i.e. **$t \ge k+2$ and $\gcd(t,k) = 1$**.
- **Type B (division step):** $g = \gcd(k,s) > 1$ and $s/g = t$. Write $s = gt$ and $d = k/g$; then $\gcd(k, gt) = g \iff \gcd(d,t) = 1$. So for every **proper divisor** $d$ of $k$ ($d < k$, corresponding to $g > 1$) with $\gcd(t,d) = 1$ we get $s = (k/d)\,t$.

Therefore
$$
f(k,t)=\mathbf 1[t\ge k+2,\ \gcd(t,k)=1]\cdot f(k-1,t-k)\;+\!\!\sum_{\substack{d\mid k,\ d<k\\ \gcd(d,t)=1}}\! f\bigl(k-1,\tfrac{k}{d}t\bigr),\qquad f(1,t)=1 .
$$
(This agrees with the program by Pontus von Brömssen in the OEIS entry. Recomputing $a(n)$ for $n \le 14$ from it reproduces the OEIS data exactly, and a forward brute-force count over $m \le 7!$ agrees for $n \le 7$.)

### 3.2 Lemma 2: an upper bound modulo $M$

Fix any positive integer $M$. Define $U_k : \mathbb Z/M \to \mathbb Z_{\ge 0}$ recursively by $U_1 \equiv 1$ and
$$
U_k(r)=\mathbf 1\bigl[\gcd(r,\gcd(k,M))=1\bigr]\cdot U_{k-1}(r-k)\;+\!\!\sum_{\substack{d\mid k,\ d<k\\ \gcd(r,\gcd(d,M))=1}}\! U_{k-1}\!\bigl(\tfrac{k}{d}r\bigr)\pmod M .
$$

**Lemma 2.** For all $k \ge 1$ and $t \ge 1$: $f(k,t) \le U_k(t \bmod M)$.

*Proof.* Induction on $k$. For $k = 1$ both sides equal 1. Assume the claim for $k-1$ and let $r = t \bmod M$.

- If the type-A move is valid at $(k,t)$, then $\gcd(t,k) = 1$, hence $\gcd(t,\gcd(k,M)) = 1$; since $\gcd(k,M) \mid M$ this is equivalent to $\gcd(r,\gcd(k,M)) = 1$, so the corresponding term is present in $U_k$. The child satisfies $t-k \equiv r-k \pmod M$, and by induction $f(k-1,t-k) \le U_{k-1}(r-k)$.
- If $B_d$ is valid at $(k,t)$, then $\gcd(t,d) = 1 \Rightarrow \gcd(r,\gcd(d,M)) = 1$, so the term is present; the child satisfies $(k/d)t \equiv (k/d)r \pmod M$, and by induction $f(k-1,(k/d)t) \le U_{k-1}((k/d)r)$.

The terms summed in $U_k$ form a superset of those summed in $f$ ($U$ ignores the size constraint $t \ge k+2$ and optimistically treats every prime not dividing $M$ as coprime to $t$), and each term is dominated, so $f(k,t) \le U_k(r)$. $\blacksquare$

In particular $a(n) = f(n,1) \le U_n(1)$.

### 3.3 Computational certificates

Computing $U_k$ involves only $M \times n$ exact integers and is fully reproducible (`src/verify_n120.py`, about 10 seconds).

**$M = 30030 = 2\cdot3\cdot5\cdot7\cdot11\cdot13$, $n = 120$:**
```
U_120(1) = 11394273053996274105055619148547066955369530632705381020931   ≈ 1.1394e58
P(120)   = 15529704459314036564474105721008928313618125000000000000000   ≈ 1.5530e58
U_120(1) / P(120) = 0.7337 < 1
```
Hence $a(120) \le U_{120}(1) < P(120)$. $\blacksquare$

**An independent second certificate, $M = 210$, $n = 126$:**
```
U_126(1) = 41164018406803943335991849176572040469417584131025436491351436
P(126)   = 46123222244162688596488093991396517091445831250000000000000000
ratio = 0.8925 < 1
```

**Range result** ($M = 30030$): $U_k(1) < P(k)$ for **every** $120 \le k \le 300$; the ratio decreases rapidly, with $U_{300}(1)/P(300) \approx 9.9\times10^{-8}$.

### 3.4 Checks on the computation

| Check | Result |
|---|---|
| Recompute $a(n)$, $n \le 14$, from the Lemma 1 recursion | matches OEIS data term by term |
| Forward brute force over $m \le 5040$, count $a(n)$ for $n \le 7$ | matches |
| $U_n(1) \ge a(n)$ for $n \le 25$ and $M = 30, 210, 2310, 30030$ | holds in all cases |
| Pointwise $U_k(t \bmod 30) \ge f(k,t)$ for $k \le 10$, $t \le 600$ | no violations |
| Two independent implementations (hand-rolled divisor enumeration vs. sympy) | certificates agree digit for digit |
| Monotonicity in $M$: $M = 30$ never refutes; first refuted $n$ is 126 for $M = 210$, 122 for $2310$, 120 for $30030$ | as expected: larger $M$ gives tighter bounds |

All numbers are Python arbitrary-precision integers; no floating point is involved.

## 4. Why the conjecture fails (heuristic)

For a "random" $t$, the expected number of valid moves at position $j$ is
$$\beta(j)=\sum_{d\mid j}\frac{\varphi(d)}{d}=\prod_{p^a\parallel j}\Bigl(1+a\bigl(1-\tfrac1p\bigr)\Bigr),$$
whereas the conjecture implicitly needs each position to contribute $d(j)-1 = \prod(a+1)-1$.

- At a **prime** $j = p$: $\beta \approx 2 > 1 = d(p)-1$, a gain (the A move is almost always available). This is exactly why $a(n)/P(n)$ jumps up at $n = 11, 13, 17, 19, 23$.
- At **highly composite** $j$: $\beta(60) \approx 5.8$ versus $d(60)-1 = 11$; $\beta(210) \approx 8.4$ versus $d(210)-1 = 15$. A value $t$ divisible by small primes loses several $B_d$ moves and the A move at once, and type-B moves only accumulate more prime factors in $t$.

The cumulative quantity $\sum_{j \le n}\log\frac{\beta(j)}{d(j)-1}$ peaks at $\approx 4$ (ratio $\approx e^4$) around $n \approx 50$–$100$, then is dominated by negative contributions: about $-9$ at $n = 500$, $-37$ at $n = 1000$, diverging roughly like $-0.1\,n$. In other words, the "growth trend" of the first 25 terms is a transient phenomenon while small primes are still sparse; in the long run $P(n)$ (normal order $\log 2 \cdot \log\log n$ per term) grows faster than $a(n)$. The rigorous mod-$M$ bound turns this heuristic into a proof because the losses come mainly from the few small primes dividing $M$.

## 5. Locating the smallest counterexample

### 5.1 Result

| Range | Statement | Status |
|---|---|---|
| $n \le 44$ | $a(n) \ge P(n)$ | **proved** (exact values for $n \le 40$; integer lower-bound certificates for $41 \le n \le 44$) |
| $45 \le n \le 89$ | $a(n) \ge P(n)$, minimum ratio $a(84)/P(84) \approx 1.191$ | unbiased importance-sampled estimates, relative s.e. $\le 0.2\%$, every point $> 100\sigma$ from the threshold |
| $n = 90$ | $a(90) < P(90)$, ratio $\le 0.866$ (estimate $\approx 0.638$) | **proved** (integer upper-bound certificate) |

Hence the **smallest counterexample is $n^* = 90$**. Rigorously established: $45 \le n^* \le 90$; the identification $n^* = 90$ rests on the numerical evidence for $45$–$89$ (see 5.4 for why a rigorous lower bound there is beyond current computing resources).

### 5.2 Exact values $a(26)$–$a(40)$ (new terms)

Memoised on $t \bmod \operatorname{prim}(k)$ (for $t \ge k^2+k$, $f(k,t)$ depends only on that residue), implemented in C++; $n \le 25$ agrees with the OEIS term by term:

```
n   a(n)                 a(n)/P(n)        n   a(n)                 a(n)/P(n)
26  1900430851           10.34            34  8872132697704        10.22
27  5901944270           10.71            35  34766652126827       13.35
28  28028557112          10.17            36  105962930618570       5.09
29  45427675195          16.49            37  233048825328494      11.19
30  91313705322           4.73            38  427922092082052       6.85
31  288850466259         14.97            39  1490289805794031      7.95
32  850322925064          8.82            40  9273038181155701      7.07
33  4012051390889        13.87
```

### 5.3 Rigorous certificates (exact integer arithmetic, `src/cert.cpp`)

**Upper bound (refuting $n = 90$).** Enumerate exactly from $(90,1)$ down 15 levels to level 76 ($1.44\times10^7$ states, $t$ stored exactly as a 128-bit integer), then cap every state with the optimistic table $U_{75}(t \bmod M)$ for $M = 19\# = 9699690$ (Lemma 2, exact u128), accumulated in 256 bits:
```
a(90) <=  99534908722218985997013424029924657357232
P(90)  = 114953593183938717266275367535000000000000      ratio 0.8659
```

**Lower bound (confirming $41 \le n \le 44$).** Enumerate exactly down to level 29, then use a **conservative** table $L_{28}(t \bmod 19\#)$ at level 28. It starts from the exact table $F_{22}$ for $k \le 22$ (defined on $\mathbb Z/19\#$; for large $t$ it is the exact value of $f(22,t)$), drops at level 23 the A move that cannot be decided from $t \bmod 19\#$ (the prime 23 does not divide $M$), and is exact level by level otherwise; leaves with small $t$ are recursed exactly. By the structure of Lemma 1, dropping moves can only decrease the count, so this is a rigorous lower bound.
```
n   lower bound             P(n)                   ratio
41  4650051213471975        1312446693600000       3.54
42  14548162052243164       9187126855200000       1.58
43  29710976121092859       9187126855200000       3.23
44  117826368297341117      45935634276000000      2.57
```
Validation: with the cut at $k_0 = 22$ (nothing dropped) the lower bound for $n = 36$ equals $105962930618570$, exactly the true value; for $n = 40$ the lower bound $2.75\times10^{15} \le a(40) = 9.27\times10^{15}$.

### 5.4 $45 \le n \le 89$: why only numerical evidence

A rigorous lower bound must "see" the residue of $t$ modulo every prime $p \le k_0$ (otherwise each unseen prime $p$ loses the A move at level $p$, costing a factor of about 2), which forces the table modulus to be $\operatorname{prim}(k_0)$: for $k_0 \le 28$ this is $23\# \approx 2.2\times10^8$, still feasible; for $k_0 \ge 29$ it is $\ge 6.5\times10^9$, infeasible. With the exact enumeration cut at level 29, the number of states grows by a factor of about $2.5$ per unit of $n$ ($n = 44$: $10^7$ states; $n = 45$ already exceeds 4 GB). Squeezed from both sides, the limit of rigorous lower bounds is $n \approx 44$–$46$. Exact computation of $a(n)$ is blocked in the same way by state saturation in the middle levels ($k \approx 29$–$37$, where $\operatorname{prim}(k)$ is $10^{10}$–$10^{13}$).

For this range we therefore use a **Knuth-type unbiased estimator with importance sampling**: descend randomly from $(n,1)$ to level 22, at each step choosing a child with probability proportional to the proxy weight $U^{(30030)}_{k-1}(\text{child} \bmod 30030)$; the path weight is $w = \prod(\text{weight sum}/\text{chosen weight})$, and the leaf is multiplied by the exact table $F_{22}$. Then $\mathbb E[w\,F_{22}] = a(n)$ exactly (Horvitz–Thompson). Calibration: for the 11 exact values at $n = 30$–$40$ the estimates deviate by $< 0.3\%$, within the reported standard errors.

Estimated $a(n)/P(n)$ ($3\times10^5$ samples per point, relative s.e. 0.05%–0.26%):
```
n   41    42    43    44    45    46    47    48    49    50    51    52    53    54    55    56
   11.95  5.34 10.91  8.64 12.29 10.25 12.81  4.48 13.46  9.00  9.75  7.27 10.74  4.41  8.74  6.76
n   57    58    59    60    61    62    63    64    65    66    67    68    69    70    71    72
    5.84  3.98  6.41  2.33  7.18  4.53  5.47  5.03  6.81  2.56  4.25  2.38  3.46  2.75  4.29  1.47
n   73    74    75    76    77    78    79    80    81    82    83    84    85    86    87    88
    4.36  2.75  3.44  3.36  4.47  2.35  3.40  1.53  2.89  1.98  2.38  1.19  2.38  1.87  1.71  1.33
n   89    90    91    92    93    94    95    96    97    98    99   100
    2.10  0.64  1.53  1.33  1.08  0.84  1.08  0.40  0.84  0.41  0.49  0.53
```
The critical points were re-run with two independent random seeds and $2$–$3\times10^6$ samples: $a(84)/P(84) = 1.1937 \pm 0.08\%$ and $1.1909 \pm 0.06\%$; $a(88)/P(88) = 1.327$ and $1.327$; $a(90)/P(90) = 0.6385$ and $0.6371$ (consistent with the rigorous upper bound $0.866$).

All the dips occur at highly composite numbers ($48, 54, 60, 66, 72, 80, 84, 90, 96$), in line with the heuristic of Section 4: $90 = 2\cdot3^2\cdot5$ is the first place where the accumulated losses overcome the accumulated gains. For $n > 90$ the conjecture is **not** false everywhere — the ratios at $91, 92, 93, 95$ are still $> 1$ — but from $n = 96$ on the estimates stay below 1, and Section 3 proves $a(n) < P(n)$ for all $120 \le n \le 300$.

### 5.5 Open points

- A rigorous lower bound for $45 \le n \le 89$ (which would make $n^* = 90$ fully rigorous): needs two or more orders of magnitude more memory, or a new idea.
- Whether $a(n) < P(n)$ for all $n \ge 96$: strongly supported heuristically, verified computationally up to 300.

**A side remark for anyone wishing to repair the conjecture.** The generalised inequality $f(k,t) \ge \prod_{2 \le j \le k,\ \gcd(j,t)=1}(d(j)-1)$ holds for all $k \le 12$, $t < 3000$, and admits a rigorous induction in the case $\gcd(t,k) > 1$; but at $t = 1$ it is the original conjecture, so it must fail for large $k$. Hence no product-type lower bound that depends only on the set of prime factors of $t$ can hold for all $n$. A more promising replacement for $P(n)$ is a quantity of the form $\prod_{k \le n}\beta(k)$ with $\beta(k) = \sum_{d \mid k}\varphi(d)/d$.

## Appendix: sources and reproduction

- OEIS A398786: <https://oeis.org/A398786> (Sam Chapman, 2026-09-06; a(24)–a(25) added by Pontus von Brömssen, 2026-10-02)
- OEIS A091508: <https://oeis.org/A091508> (Benoit Cloitre, 2004)
- Reproduction of the $n = 120$ certificate: `python3 src/verify_n120.py` (standard library only, about 10 seconds)
- Reproduction of the $n = 90$ certificate, the lower bounds for $n \le 44$, the exact values and the Monte-Carlo estimates: `src/` and `scripts/`, see the README at the repository root
