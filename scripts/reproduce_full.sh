#!/usr/bin/env bash
# Full reproduction. Peak RAM ~3.5 GB (n=44 lower bound, n=90 upper bound). ~30 min total on one core.
set -euo pipefail
cd "$(dirname "$0")/.."
bash scripts/reproduce_quick.sh
echo
echo "== Lower-bound certificate n=44 (10M states, ~3 GB) =="
bin/cert lower 44 16 | python3 scripts/compare_cert.py
echo
echo "== Upper-bound certificate n=90 (14M states, ~2 GB, ~75 s) =="
bin/cert upper 90 15 | python3 scripts/compare_cert.py
echo
echo "== Exact a(n), n=26..40 (n=40 takes ~90 s and ~2.5 GB) =="
bin/exact 26 40 20000000
echo
echo "== Importance-sampled estimates of a(n)/P(n), n=41..100 (3e5 samples each, ~3 min) =="
bin/mc 41 100 300000 2024 | python3 scripts/compare_mc.py
