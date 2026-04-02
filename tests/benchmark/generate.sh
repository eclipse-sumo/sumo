#!/bin/bash
set -euo pipefail
#
# Generates the benchmark scenario: 15x15 grid, ~27K vehicles, 4000 sim seconds.
# Requires: SUMO binaries built (bin/netgenerate, bin/duarouter)
#
# Usage: cd <sumo-root> && bash tests/benchmark/generate.sh
#

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"
OUT_DIR="$SCRIPT_DIR"

NETGENERATE="$REPO_ROOT/bin/netgenerate"
DUAROUTER="$REPO_ROOT/bin/duarouter"
RANDOMTRIPS="$REPO_ROOT/tools/randomTrips.py"

echo "=== Generating benchmark scenario ==="

# 1. Grid network: 15x15, 300m edges, 2 lanes
echo "Step 1: Generating grid network..."
"$NETGENERATE" \
    --grid \
    --grid.x-number 15 --grid.y-number 15 \
    --grid.x-length 300 --grid.y-length 300 \
    --default.lanenumber 2 \
    --output-file "$OUT_DIR/grid.net.xml" \
    2>&1 | tail -1

# 2. Random trips: ~27K vehicles over 3600s (period 0.13)
echo "Step 2: Generating random trips..."
python3 "$RANDOMTRIPS" \
    -n "$OUT_DIR/grid.net.xml" \
    -o "$OUT_DIR/trips.xml" \
    -b 0 -e 3600 -p 0.13 --seed 42 \
    2>&1 | tail -1

# 3. Route with duarouter
echo "Step 3: Routing trips..."
"$DUAROUTER" \
    -n "$OUT_DIR/grid.net.xml" \
    -t "$OUT_DIR/trips.xml" \
    -o "$OUT_DIR/routes.xml" \
    --ignore-errors \
    2>&1 | tail -1

# 4. Clean up intermediate files
rm -f "$OUT_DIR/trips.xml" "$OUT_DIR/routes.rou.alt.xml"

echo "=== Done ==="
echo "  Network: $OUT_DIR/grid.net.xml"
echo "  Routes:  $OUT_DIR/routes.xml"
echo "  Config:  $OUT_DIR/benchmark.sumocfg"
echo ""
echo "Run benchmark:"
echo "  taskset -c 0 bin/sumo -c tests/benchmark/benchmark.sumocfg"
