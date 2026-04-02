#!/bin/bash
set -euo pipefail
#
# Benchmark SUMO on a 15x15 grid scenario (~5K peak vehicles).
# Runs N iterations with CPU pinning and reports median wall-clock time.
#
# Usage:
#   # First time: generate scenario + build
#   bash tests/benchmark/generate.sh
#   mkdir -p build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release && make -j$(nproc) sumo && cd ..
#
#   # Run benchmark
#   bash tests/benchmark/bench.sh
#

REPO_ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
SUMO="$REPO_ROOT/bin/sumo"
SCENARIO="$REPO_ROOT/tests/benchmark/benchmark.sumocfg"
N=${1:-5}

if [ ! -f "$SUMO" ]; then echo "Build sumo first"; exit 1; fi
if [ ! -f "$REPO_ROOT/tests/benchmark/routes.xml" ]; then
    echo "Run tests/benchmark/generate.sh first"; exit 1
fi

echo "Running $N iterations (CPU-pinned to core 0)..."
TIMES=()
for i in $(seq 1 "$N"); do
    START=$(date +%s%N)
    taskset -c 0 "$SUMO" -c "$SCENARIO" 2>/dev/null
    END=$(date +%s%N)
    T=$(echo "scale=3; ($END - $START) / 1000000000" | bc)
    TIMES+=("$T")
    echo "  Run $i: ${T}s"
done

IFS=$'\n' SORTED=($(sort -n <<<"${TIMES[*]}")); unset IFS
MEDIAN=${SORTED[$(( (N-1) / 2 ))]}
echo "Median: ${MEDIAN}s  (all: ${TIMES[*]})"
