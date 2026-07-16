# The RoutingKit CCH routing backend

This adds a **Customizable Contraction Hierarchies (CCH)** routing backend to
SUMO, selectable with `--routing-algorithm CCH`. It answers vehicle routing
queries via a [RoutingKit](https://github.com/RoutingKit/RoutingKit) CCH over
the SUMO edge graph instead of the built-in A\*/CH routers.

The point: SUMO's stock A\*/CH re-explores (or fully re-contracts) the network on
every weight change. CCH splits routing into a **one-time, weight-independent
topology** and a **cheap per-interval re-customization** from live edge speeds, so
travel-time-dependent rerouting stays fast at city scale. On the Geneva network a
point-to-point query drops from ~0.6 ms (A\*) to ~5 µs, and results are exact
against Dijkstra.

RoutingKit is vendored in-tree under `src/foreign/RoutingKit` (BSD-2-Clause) and
built as part of the normal SUMO build — there is no extra dependency to install
and no build option to set. The library is compiled without OpenMP; the
per-interval metric customization runs single-threaded.

---

## Build and verify

Build SUMO as usual for your platform (see `docs/web/docs/Installing/`), then:

```bash
bin/sumo --version
```

The **Build features** line lists `CCH` when the backend is compiled in
(`HAVE_ROUTINGKIT`).

---

## Test / run

Point any scenario at the CCH router:

```bash
bin/sumo -c yourscenario.sumocfg --routing-algorithm CCH \
    --device.rerouting.probability 1
```

The `--routing-algorithm` values are `dijkstra` (default), `astar`, `CH`,
`CHWrapper`, and **`CCH`**.

### Validate against the built-in routers

CCH is designed to return the *same* routes as Dijkstra/A\* (its arc weights match
`SUMOAbstractRouter::recomputeCosts` exactly). Run the same scenario twice and
compare the outputs:

```bash
bin/sumo -c yourscenario.sumocfg --routing-algorithm dijkstra \
    --vehroute-output routes_dijkstra.xml
bin/sumo -c yourscenario.sumocfg --routing-algorithm CCH \
    --vehroute-output routes_cch.xml
# route edge sequences / costs should match (CCH validated 300/300 exact vs Dijkstra)
```

---

## Notes and current limitations

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
- **Single-threaded customization.** The vendored RoutingKit is built without
  OpenMP; the per-interval customization runs serially (~2.7 ms/class on Geneva).
- **TAZ routing** is handled by seeding the connector's member edges as
  multi-source/multi-target query endpoints (TAZ connectors are deliberately kept
  out of the hierarchy to avoid a treewidth blow-up).

---

*RoutingKit is BSD-2-Clause (see `src/foreign/RoutingKit/LICENSE`).*
