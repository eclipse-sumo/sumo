#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2010-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    header_weight.py
# @author  Michael Behrisch
# @date    2026-06-15

"""
Rank C/C++ headers by build-time impact: reach (number of translation units
that include the header) times cost (preprocessed line count of the header
when included by a real TU's compile command).

Typical workflow for spotting include-time hotspots:

    cmake -B build .
    cmake --build build -j$(nproc)
    tools/build_config/header_weight.py --build build --top 30

The build directory must contain ``compile_commands.json`` (CMake's
CMAKE_EXPORT_COMPILE_COMMANDS=ON, on by default for Makefile/Ninja generators)
and the compiler-generated dependency files (``*.o.d``).  Both are produced by
a standard build.

Output columns:
  reach   number of TUs that include the header (from .o.d dep files)
  cost    preprocessed line count of `#include "<header>"` with one consumer
          TU's real compile flags (locale-free wc -l)
  total   reach * cost (rough proxy for total preprocessor lines per build)

Platform support
----------------
This script targets GCC- or Clang-compatible toolchains driven by CMake's
Makefile or Ninja generator (Linux, macOS, BSD, and Windows under MSYS2 /
MinGW / Cygwin / clang+Ninja in gcc-compatibility mode).

It is **not** compatible with the Visual Studio generator (no
``compile_commands.json``) or with MSVC's ``cl.exe`` (no ``*.o.d`` dep files,
different flag syntax ``/I`` ``/D``).
"""

import argparse
import csv
import json
import os
import re
import shlex
import subprocess
import sys
import tempfile
from collections import Counter
from pathlib import Path


_DEP_TOKEN_RE = re.compile(r"[^\s\\:]+")


def parse_dep_file(path: Path) -> list[str]:
    """Yield header paths from one Makefile-format dep file.

    GCC/Clang emit ``target: a.h b.h \\\n  c.h`` with backslash-newline
    continuations.  We normalise newlines, drop the target, then split on
    whitespace ignoring backslashes and colons.
    """
    try:
        text = path.read_text(errors="replace")
    except OSError:
        return []
    # Strip line continuations
    text = text.replace("\\\n", " ")
    # Drop everything before the first colon (the target)
    colon = text.find(":")
    if colon < 0:
        return []
    body = text[colon + 1:]
    return [t for t in _DEP_TOKEN_RE.findall(body) if t.endswith((".h", ".hpp", ".hxx", ".hh", ".inc"))]


def build_reach(build_dir: Path, source_root: Path, filter_prefix: str | None) -> tuple[Counter, dict[str, str]]:
    """Scan all dep files; return (header -> reach, header -> one .o.d file)."""
    reach: Counter = Counter()
    sample_dep: dict[str, str] = {}
    for dep in build_dir.rglob("*.o.d"):
        headers = set(parse_dep_file(dep))
        for h in headers:
            abs_h = os.path.abspath(h)
            try:
                rel = os.path.relpath(abs_h, source_root)
            except ValueError:
                rel = abs_h
            if filter_prefix and not rel.startswith(filter_prefix):
                continue
            reach[rel] += 1
            sample_dep.setdefault(rel, str(dep))
    return reach, sample_dep


def load_compile_db(build_dir: Path) -> dict[str, dict]:
    """Map absolute source file path -> compile-commands entry."""
    cdb_path = build_dir / "compile_commands.json"
    if not cdb_path.is_file():
        sys.exit(f"error: {cdb_path} not found (configure CMake with -DCMAKE_EXPORT_COMPILE_COMMANDS=ON)")
    out: dict[str, dict] = {}
    for entry in json.loads(cdb_path.read_text()):
        out[os.path.abspath(entry["file"])] = entry
    return out


def dep_to_source(dep_path: str) -> str | None:
    """Best-effort inverse of CMake's ``foo.cpp.o.d`` -> ``foo.cpp``.

    CMake/GCC writes ``target.o: \\\n  source.cpp \\\n  header.h \\\n ...``,
    so we have to honour the backslash-newline continuations to reach the
    first dependency (the source file).
    """
    try:
        text = Path(dep_path).read_text(errors="replace")
    except OSError:
        return None
    text = text.replace("\\\n", " ")
    colon = text.find(":")
    if colon < 0:
        return None
    rest = text[colon + 1:].strip()
    if not rest:
        return None
    return rest.split()[0]


def extract_preproc_flags(command: str) -> list[str]:
    """Pull -I/-D/-U/-isystem and standard-version flags out of a command line."""
    tokens = shlex.split(command)
    out: list[str] = []
    i = 0
    while i < len(tokens):
        t = tokens[i]
        if t.startswith(("-I", "-D", "-U")) and len(t) > 2:
            out.append(t)
        elif t in ("-I", "-D", "-U", "-isystem", "-iquote", "-include"):
            out.append(t)
            if i + 1 < len(tokens):
                out.append(tokens[i + 1])
                i += 1
        elif t.startswith("-isystem"):
            out.append(t)
        elif t.startswith("-std=") or t.startswith("-stdlib="):
            out.append(t)
        i += 1
    return out


def find_consumer_entry(header_rel: str, sample_dep: dict[str, str],
                        compile_db: dict[str, dict], source_root: Path) -> dict | None:
    """Find a compile-commands entry for some TU that includes header_rel."""
    dep = sample_dep.get(header_rel)
    if dep is None:
        return None
    src = dep_to_source(dep)
    if src is None:
        return None
    src_abs = os.path.abspath(os.path.join(source_root, src))
    return compile_db.get(src_abs)


def measure_cost(header_rel: str, compiler: str, flags: list[str],
                 cwd: str, source_root: Path) -> int | None:
    """Run the preprocessor on a synthetic TU including only header_rel.

    Returns the number of non-empty lines emitted (with -P, locale-free).
    """
    header_abs = os.path.abspath(os.path.join(source_root, header_rel))
    with tempfile.NamedTemporaryFile("w", suffix=".cpp", delete=False) as tmp:
        tmp.write(f'#include "{header_abs}"\n')
        tmp_path = tmp.name
    try:
        cmd = [compiler, "-E", "-P", *flags, "-x", "c++", tmp_path]
        try:
            proc = subprocess.run(cmd, cwd=cwd, capture_output=True, check=False)
        except FileNotFoundError:
            return None
        if proc.returncode != 0:
            return None
        # Count non-empty lines (closer to "real" cost than wc -l)
        return sum(1 for line in proc.stdout.splitlines() if line.strip())
    finally:
        try:
            os.unlink(tmp_path)
        except OSError:
            pass


def collect_dir_edges(build_dir: Path, source_root: Path,
                      filter_prefix: str | None, depth: int) -> tuple[Counter, Counter]:
    """Aggregate (consumer_dir -> header_dir) edges from dep files.

    Returns (edges, dir_totals) where:
      edges[(from_dir, to_dir)] = number of TUs in from_dir that include at
                                  least one header located in to_dir
      dir_totals[dir]           = number of TUs whose source lives in dir
                                  (used to size nodes in the DOT output)
    """
    edges: Counter = Counter()
    dir_totals: Counter = Counter()

    def to_relative(abs_path: str) -> str:
        try:
            return os.path.relpath(abs_path, source_root)
        except ValueError:
            return abs_path

    def to_dir(rel_path: str) -> str | None:
        if filter_prefix and not rel_path.startswith(filter_prefix):
            return None
        parts = rel_path.split(os.sep)
        if len(parts) <= depth:
            return os.sep.join(parts[:-1]) or rel_path
        return os.sep.join(parts[:depth])

    for dep in build_dir.rglob("*.o.d"):
        src = dep_to_source(str(dep))
        if not src:
            continue
        src_rel = to_relative(os.path.abspath(os.path.join(source_root, src)))
        src_dir = to_dir(src_rel)
        if src_dir is None:
            continue
        dir_totals[src_dir] += 1
        seen_targets: set[str] = set()
        for h in parse_dep_file(dep):
            h_rel = to_relative(os.path.abspath(h))
            h_dir = to_dir(h_rel)
            if h_dir is None or h_dir == src_dir:
                continue
            if h_dir in seen_targets:
                continue
            seen_targets.add(h_dir)
            edges[(src_dir, h_dir)] += 1
    return edges, dir_totals


def write_dot(path: Path, edges: Counter, dir_totals: Counter, min_edge: int) -> None:
    nodes: set[str] = set()
    for (a, b), w in edges.items():
        if w >= min_edge:
            nodes.add(a)
            nodes.add(b)
    with path.open("w") as fh:
        fh.write("digraph deps {\n")
        fh.write('  rankdir=LR;\n')
        fh.write('  node [shape=box, fontname="Helvetica", fontsize=10];\n')
        fh.write('  edge [fontname="Helvetica", fontsize=8];\n')
        for n in sorted(nodes):
            label = n
            if n in dir_totals:
                label = f"{n}\\n({dir_totals[n]} TUs)"
            fh.write(f'  "{n}" [label="{label}"];\n')
        for (a, b), w in sorted(edges.items(), key=lambda kv: -kv[1]):
            if w < min_edge:
                continue
            # Edge thickness scales gently with weight (log).
            import math
            penwidth = 1 + math.log10(max(w, 1))
            fh.write(f'  "{a}" -> "{b}" [label="{w}", penwidth={penwidth:.2f}];\n')
        fh.write("}\n")


def run_dir_graph(build_dir: Path, source_root: Path, args) -> int:
    print(f"# scanning dep files under {build_dir}", file=sys.stderr)
    edges, dir_totals = collect_dir_edges(build_dir, source_root, args.filter, args.dir_depth)
    if not edges:
        sys.exit("error: no directory edges found "
                 "(check --filter, --dir-depth and that the build emitted *.o.d files)")
    print(f"# {len(dir_totals)} directories, {len(edges)} directed edges "
          f"({sum(edges.values())} TU/dir reaches)", file=sys.stderr)

    ranked = sorted(((w, a, b) for (a, b), w in edges.items()
                     if w >= args.min_edge), reverse=True)
    print(f"{'weight':>6}  from -> to")
    print(f"{'------':>6}  ----------")
    for w, a, b in ranked[:args.top]:
        print(f"{w:>6}  {a} -> {b}")

    if args.csv:
        with args.csv.open("w", newline="") as fh:
            wcsv = csv.writer(fh)
            wcsv.writerow(["from_dir", "to_dir", "tu_reaches"])
            for w, a, b in ranked:
                wcsv.writerow([a, b, w])
        print(f"# wrote {args.csv}", file=sys.stderr)
    if args.dot:
        write_dot(args.dot, edges, dir_totals, args.min_edge)
        print(f"# wrote {args.dot} (visualise: dot -Tsvg {args.dot} -o {args.dot.with_suffix('.svg')})",
              file=sys.stderr)
    return 0


def main() -> int:
    p = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    p.add_argument("--build", "-b", default="build", type=Path,
                   help="CMake build directory containing compile_commands.json and *.o.d files (default: build)")
    p.add_argument("--source-root", "-s", default=None, type=Path,
                   help="Repository root")
    p.add_argument("--filter", "-f", default="src",
                   help="Only consider headers whose path (relative to source root) starts with this prefix, e.g. 'src/netedit/'")  # noqa
    p.add_argument("--top", "-n", type=int, default=30, help="Show this many headers/edges (default: 30)")
    p.add_argument("--min-reach", type=int, default=2,
                   help="Skip headers included by fewer TUs than this (default: 2)")
    p.add_argument("--sort", choices=("total", "reach", "cost"), default="total",
                   help="Sort key (default: total = reach * cost)")
    p.add_argument("--csv", type=Path, default=None, help="Also write full ranking to this CSV file")
    p.add_argument("--dir-graph", action="store_true",
                   help="Instead of ranking headers, report a directory-to-directory include graph "
                        "(edge weight = number of TUs in dir A that include at least one header "
                        "from dir B). Uses dep files only; no preprocessor invocations.")
    p.add_argument("--dir-depth", type=int, default=2,
                   help="Path-component depth used to group files into directories for --dir-graph "
                        "(default: 2, e.g. 'src/netedit'; use 3 for 'src/netedit/elements')")
    p.add_argument("--dot", type=Path, default=None,
                   help="With --dir-graph, also write a Graphviz DOT file "
                        "(visualise with: dot -Tsvg deps.dot -o deps.svg)")
    p.add_argument("--min-edge", type=int, default=1,
                   help="With --dir-graph, hide edges with weight below this threshold (default: 1)")
    args = p.parse_args()

    build_dir = args.build.resolve()
    if not build_dir.is_dir():
        sys.exit(f"error: build dir {build_dir} not found")
    source_root = (args.source_root or Path(__file__).parent.parent.parent).resolve()

    if args.dir_graph:
        return run_dir_graph(build_dir, source_root, args)

    print(f"# scanning dep files under {build_dir}", file=sys.stderr)
    reach, sample_dep = build_reach(build_dir, source_root, args.filter)
    if not reach:
        sys.exit("error: no headers found in dep files")
    print(f"# {len(reach)} distinct headers, {sum(reach.values())} include edges", file=sys.stderr)

    rows: list[tuple[str, int, int | None, int | None]] = []
    compile_db = load_compile_db(build_dir)
    candidates = [(h, r) for h, r in reach.items() if r >= args.min_reach]
    candidates.sort(key=lambda hr: hr[1], reverse=True)
    # To bound runtime, only measure cost for headers we might actually display.
    # We measure a generous superset (top reach * 5) and rerank by `--sort`.
    measure_budget = max(args.top * 5, 100)
    to_measure = candidates[:measure_budget]
    print(f"# measuring cost for {len(to_measure)} headers", file=sys.stderr)
    for h, r in to_measure:
        entry = find_consumer_entry(h, sample_dep, compile_db, source_root)
        cost: int | None = None
        if entry is not None:
            tokens = shlex.split(entry["command"])
            compiler = tokens[0] if tokens else "c++"
            flags = extract_preproc_flags(entry["command"])
            cost = measure_cost(h, compiler, flags, entry["directory"], source_root)
        total = (cost * r) if cost is not None else None
        rows.append((h, r, cost, total))
    # Remaining (unmeasured) headers still appear in CSV with cost=None
    for h, r in candidates[measure_budget:]:
        rows.append((h, r, None, None))

    key_map = {
        "reach": lambda row: row[1] or 0,
        "cost": lambda row: row[2] or 0,
        "total": lambda row: row[3] or 0,
    }
    rows.sort(key=key_map[args.sort], reverse=True)

    # Pretty-print the top N
    print(f"{'reach':>6}  {'cost':>8}  {'total':>11}  header")
    print(f"{'-----':>6}  {'----':>8}  {'-----':>11}  ------")
    for h, r, c, t in rows[:args.top]:
        c_s = f"{c:>8}" if c is not None else f"{'?':>8}"
        t_s = f"{t:>11}" if t is not None else f"{'?':>11}"
        print(f"{r:>6}  {c_s}  {t_s}  {h}")

    if args.csv:
        with args.csv.open("w", newline="") as fh:
            w = csv.writer(fh)
            w.writerow(["header", "reach", "cost_lines", "total_lines"])
            for h, r, c, t in rows:
                w.writerow([h, r, c if c is not None else "", t if t is not None else ""])
        print(f"# wrote {args.csv}", file=sys.stderr)
    return 0


if __name__ == "__main__":
    sys.exit(main())
