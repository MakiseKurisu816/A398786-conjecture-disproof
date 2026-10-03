#!/usr/bin/env bash
# Quick reproduction (~1-2 min on one core, < 1 GB RAM).
set -euo pipefail
cd "$(dirname "$0")/.."
echo "== [1/3] n=120 and n=126 upper-bound certificates (pure Python, exact integers) =="
python3 src/verify_n120.py
echo
echo "== [2/3] Lower-bound certificates a(n) >= P(n), n=36,40 (checked against exact), 41..44 =="
for args in "36 14" "40 14" "41 13" "42 14" "43 15"; do bin/cert lower $args; done | python3 scripts/compare_cert.py
echo
echo "== [3/3] Monte-Carlo estimator calibration vs exact values n=36..40, then n=84,88,90 =="
bin/mc 36 40 100000 5 | python3 scripts/compare_mc.py
bin/mc 84 84 300000 5 | python3 scripts/compare_mc.py
bin/mc 88 88 300000 5 | python3 scripts/compare_mc.py
bin/mc 90 90 300000 5 | python3 scripts/compare_mc.py
