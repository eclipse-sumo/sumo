# SUMO with a RoutingKit CCH routing backend

This is a fork of [Eclipse SUMO](https://github.com/eclipse-sumo/sumo) that adds a
**Customizable Contraction Hierarchies (CCH)** routing backend, selectable with
`--routing-algorithm CCH`. It answers vehicle routing queries via a
[RoutingKit](https://github.com/RoutingKit/RoutingKit) CCH over the SUMO edge
graph instead of the built-in A\*/CH routers.

The point: SUMO's stock A\*/CH re-explores (or fully re-contracts) the network on
every weight change. CCH splits routing into a **one-time, weight-independent
topology** and a **cheap per-interval re-customization** from live edge speeds, so
travel-time-dependent rerouting stays fast at city scale. On the Geneva network a
point-to-point query drops from ~0.6 ms (A\*) to ~5 µs, and results are exact
against Dijkstra.

The backend is fully **opt-in and guarded** behind the `SUMO_WITH_CCH` build
option (`HAVE_ROUTINGKIT`). If you build without it, this fork compiles and behaves
exactly like upstream SUMO — RoutingKit is not required for a normal build.

> **This CCH work also has a superset branch.** `feature/parallel-fcd`
> contains everything in this document *plus* parallelized FCD output
> (`--fcd-output.threads`) built on top of it. See
> [`WRITEUP.md`](https://github.com/stprnvsh/sumo-fastrouting/blob/feature/parallel-fcd/WRITEUP.md) for that combined writeup, measured results for
> both features together, and the full branch history
> (`feature/cch-routing-backend` → `feature/parquet-async-writer` →
> `feature/parallel-fcd`). This document covers the CCH backend on its own —
> use it as-is if you only want the routing change.

---

## What you need

| Dependency | Why | Required? |
|---|---|---|
| A C++17 compiler + CMake ≥ 3.x + Ninja or Make | builds SUMO | yes |
| SUMO's normal build libraries (Xerces-C, PROJ, FOX, GDAL, …) | builds SUMO | yes |
| [RoutingKit](https://github.com/RoutingKit/RoutingKit) (built from source) | the CCH engine | yes, for CCH |
| OpenMP / libomp | parallel CCH customization | **optional** (falls back to a ~2.7 ms serial customize) |

For SUMO's own prerequisites, follow the upstream platform guides — they are
included in this repo under `docs/web/docs/Installing/`:
- macOS: [`MacOS_Build.md`](docs/web/docs/Installing/MacOS_Build.md)
- Linux: [`Linux_Build.md`](docs/web/docs/Installing/Linux_Build.md)
- Windows: [`Windows_Build.md`](docs/web/docs/Installing/Windows_Build.md)

Quick install of SUMO's core libraries:

```bash
# macOS (Homebrew)
brew install cmake ninja xerces-c fox proj gdal gl2ps
brew install libomp        # optional — enables parallel CCH customization

# Debian / Ubuntu
sudo apt-get install cmake ninja-build g++ \
     libxerces-c-dev libfox-1.6-dev libproj-dev libgdal-dev libgl2ps-dev
sudo apt-get install libomp-dev   # optional
```

---

## Build — step by step (clone → build → test)

### 1. Clone this fork

```bash
git clone https://github.com/stprnvsh/sumo-fastrouting.git sumo-cch
cd sumo-cch
git checkout feature/cch-routing-backend
```

> Want parallel FCD output too? Check out `feature/parallel-fcd` instead —
> it contains everything in `feature/cch-routing-backend` plus that
> additional feature (see the note at the top of this document). Everything
> from step 2 onward is identical either way.

### 2. Build RoutingKit (produces the static library the backend links against)

```bash
git clone https://github.com/RoutingKit/RoutingKit.git
cd RoutingKit
./generate_make_file      # generates the Makefile (Python script)
make                      # produces lib/libroutingkit.a  (and lib/libroutingkit.so)
cd ..
```

This yields `RoutingKit/include/` and `RoutingKit/lib/libroutingkit.a`. Note the
absolute path to the RoutingKit directory — you pass it to CMake below as
`ROUTINGKIT_ROOT`.

> macOS note: `generate_make_file` defaults to `g++`. AppleClang works too; if the
> default compiler is not found, edit `compiler_name` at the top of
> `generate_make_file` to `clang++` before running `make`.

### 3. Configure and build SUMO with the CCH backend

This mirrors the exact configuration this fork was built and validated with
(Ninja, `RelWithDebInfo`):

```bash
# from the sumo-cch/ root
cmake -B build_cch -G Ninja \
      -DCMAKE_BUILD_TYPE=RelWithDebInfo \
      -DSUMO_WITH_CCH=ON \
      -DROUTINGKIT_ROOT=/absolute/path/to/RoutingKit

cmake --build build_cch --target sumo -j
```

`-DROUTINGKIT_ROOT=…` is how CMake finds `libroutingkit.a` and the RoutingKit
headers; it sets `HAVE_ROUTINGKIT` and links the library into `microsim_devices`.
(If you prefer Make over Ninja, drop `-G Ninja`.)

The built binary is `build_cch/bin/sumo`. Build the `sumoD`/`libsumo` targets the
same way if you need them.

### 4. Verify the backend is compiled in

```bash
build_cch/bin/sumo --version
```

The **Build features** line should list `CCH`. If it does not, RoutingKit was not
found at configure time — re-run step 3 with the correct `ROUTINGKIT_ROOT`.

---

## Test / run

Point any scenario at the CCH router:

```bash
build_cch/bin/sumo -c yourscenario.sumocfg --routing-algorithm CCH \
    --device.rerouting.probability 1
```

The `--routing-algorithm` values are `dijkstra` (default), `astar`, `CH`,
`CHWrapper`, and — in this fork — **`CCH`**.

### Validate against the built-in routers

CCH is designed to return the *same* routes as Dijkstra/A\* (its arc weights match
`SUMOAbstractRouter::recomputeCosts` exactly). Run the same scenario twice and
compare the outputs:

```bash
build_cch/bin/sumo -c yourscenario.sumocfg --routing-algorithm dijkstra \
    --vehroute-output routes_dijkstra.xml
build_cch/bin/sumo -c yourscenario.sumocfg --routing-algorithm CCH \
    --vehroute-output routes_cch.xml
# route edge sequences / costs should match (CCH validated 300/300 exact vs Dijkstra)
```

---

## Notes and current limitations

- **Opt-in only.** Everything CCH is guarded by `#ifdef HAVE_ROUTINGKIT`. A build
  without `-DSUMO_WITH_CCH=ON` is byte-for-byte upstream SUMO and needs no RoutingKit.
- **`weights.priority-factor` is supported.** The priority penalty is a static,
  per-edge, vehicle-independent multiplier, so it is baked directly into the
  CCH metric at customization time (the weight fill goes through the same
  effort function A\* uses). Routes match A\* with the same factor.
- **A\* fallback.** CCH automatically falls back to the embedded A\* router for
  queries a shared per-class metric cannot express (arbitrary per-query
  prohibitions, or a vehicle class with no metric yet). The per-vehicle routing
  "extras" — `weights.random-factor != 1`, bike speeds, and routing
  preferences — cannot be represented by a shared metric and are rejected up
  front when `--routing-algorithm CCH` is selected; disable them to use CCH.
- **Per-class metrics, one shared topology.** All vehicle classes share a single
  weight-independent CCH; they differ only by which arcs are masked to infinite
  weight (permissions + live closures), so road closures need no special handling.
- **OpenMP is optional.** Without libomp the per-interval customization runs
  serially (~2.7 ms/class on Geneva) — correct, just not parallelized.
- **TAZ routing** is handled by seeding the connector's member edges as
  multi-source/multi-target query endpoints (TAZ connectors are deliberately kept
  out of the hierarchy to avoid a treewidth blow-up).
- This is a **feature branch**, not (yet) an upstream pull request: it does not ship
  the RoutingKit dependency vendored in-tree, the CCH-specific regression tests, or
  the upstream docs/ChangeLog entries an eclipse-sumo PR would require.

---

See [`WRITEUP.md`](https://github.com/stprnvsh/sumo-fastrouting/blob/feature/parallel-fcd/WRITEUP.md) for measured performance numbers (routing CPU,
exactness vs. A\*, closure correctness, and a full 24-hour vanilla-vs-fork
comparison), and for the parallel FCD output feature built on top of this
backend on `feature/parallel-fcd`.

---

*CCH backend authored by Pranav Sateesh. Built on Eclipse SUMO (EPL-2.0 / GPL-2.0-or-later); RoutingKit is BSD-2-Clause.*
