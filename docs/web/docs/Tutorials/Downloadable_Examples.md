---
title: Downloadable examples
---

# Downloadable examples

Use these examples to try SUMO applications and tools on small, runnable scenarios. SUMO builds the examples in `docs/examples` from the [examples manifest](https://github.com/eclipse-sumo/sumo/blob/main/tests/examples.txt). Each link downloads that scenario from the [nightly test suite](https://sumo.dlr.de/extractTest.php) as a ZIP file.

Extract the ZIP and open a terminal in the extracted scenario directory. Set [`SUMO_HOME`](../Basics/Basic_Computer_Skills.md#sumo_home) for the generated launchers. On Linux or macOS, use `bash test.sh`. On Windows, use `.\test.bat`. Use the installed command path if your package keeps binaries outside `SUMO_HOME/bin`.

For example, download [the one-lane intersection](https://sumo.dlr.de/extractTest.php?path=sumo/basic/simple_nets/cross/cross1l&app=sumo). Open its network and routes in [SUMO-GUI](../sumo-gui.md) with the following command.

```bash
sumo-gui --net-file net.net.xml --route-files input_routes.rou.xml
```

## Traffic simulation

| Example download | Directory in `docs/examples` |
| --- | --- |
| [Run a simulation on the Hokkaido example network](https://sumo.dlr.de/extractTest.php?path=sumo/basic/hokkaido) | `sumo/hokkaido` |
| [Change a lane's speed limit during a run](https://sumo.dlr.de/extractTest.php?path=sumo/extended/variable_speed_signs/b0_offset_100) | `sumo/variable_speed_signs` |
| [Stop a vehicle at a specified position](https://sumo.dlr.de/extractTest.php?path=sumo/extended/stops/simple_stop) | `sumo/vehicle_stops` |
| [Simulate three buses serving stops](https://sumo.dlr.de/extractTest.php?path=sumo/extended/busses/three_busses) | `sumo/busses` |
| [Simulate angled roadside parking](https://sumo.dlr.de/extractTest.php?path=sumo/extended/rerouter/parking/angled_roadside_parking) | `sumo/angled_roadside_parking` |
| [View vehicle classes in SUMO-GUI](https://sumo.dlr.de/extractTest.php?path=sumo/gui/visualization/parade) | `sumo/visualization/parade` |
| [View person types in SUMO-GUI](https://sumo.dlr.de/extractTest.php?path=sumo/gui/visualization/paradePersons) | `sumo/visualization/paradePersons` |
| [View containers in SUMO-GUI](https://sumo.dlr.de/extractTest.php?path=sumo/gui/visualization/paradeContainers) | `sumo/visualization/paradeContainers` |
| [One-lane intersection with traffic lights](https://sumo.dlr.de/extractTest.php?path=sumo/basic/simple_nets/cross/cross1ltl) | `sumo/simple_nets/cross/cross1ltl` |
| [One-lane intersection](https://sumo.dlr.de/extractTest.php?path=sumo/basic/simple_nets/cross/cross1l&app=sumo) | `sumo/simple_nets/cross/cross1l` |
| [Three-lane intersection with traffic lights](https://sumo.dlr.de/extractTest.php?path=sumo/basic/simple_nets/cross/cross3ltl) | `sumo/simple_nets/cross/cross3ltl` |
| [Three-lane intersection](https://sumo.dlr.de/extractTest.php?path=sumo/basic/simple_nets/cross/cross3l) | `sumo/simple_nets/cross/cross3l` |
| [One-lane box network](https://sumo.dlr.de/extractTest.php?path=sumo/basic/simple_nets/box/box1l) | `sumo/simple_nets/box/box1l` |
| [Two-lane box network](https://sumo.dlr.de/extractTest.php?path=sumo/basic/simple_nets/box/box2l) | `sumo/simple_nets/box/box2l` |
| [Three-lane box network](https://sumo.dlr.de/extractTest.php?path=sumo/basic/simple_nets/box/box3l) | `sumo/simple_nets/box/box3l` |
| [Four-lane box network](https://sumo.dlr.de/extractTest.php?path=sumo/basic/simple_nets/box/box4l) | `sumo/simple_nets/box/box4l` |
| [Simulate mixed traffic with the sublane model](https://sumo.dlr.de/extractTest.php?path=sumo/sublane_model/1edge_2lanes_compact_mixed&app=sumo) | `sumo/sublane_model` |
| [Run an interactive model railway](https://sumo.dlr.de/extractTest.php?path=sumo/rail/model_railroad) | `sumo/model_railroad` |

## Simulation outputs

| Example download | Directory in `docs/examples` |
| --- | --- |
| [Export vehicle emissions](https://sumo.dlr.de/extractTest.php?path=sumo/output/examples/cross3ltl_emission) | `sumo/output/cross3ltl_emission` |
| [Export vehicle trajectories as XML](https://sumo.dlr.de/extractTest.php?path=sumo/output/examples/cross3ltl_fcd) | `sumo/output/cross3ltl_fcd` |
| [Export vehicle trajectories as CSV](https://sumo.dlr.de/extractTest.php?path=sumo/output/examples/cross3ltl_fcd_csv) | `sumo/output/cross3ltl_fcd_csv` |
| [Export vehicle trajectories as Parquet](https://sumo.dlr.de/extractTest.php?path=sumo/output/examples/cross3ltl_fcd_parquet) | `sumo/output/cross3ltl_fcd_parquet` |
| [Export full simulation output](https://sumo.dlr.de/extractTest.php?path=sumo/output/examples/cross3ltl_full) | `sumo/output/cross3ltl_full` |
| [Measure passage with induction loops](https://sumo.dlr.de/extractTest.php?path=sumo/output/examples/cross3ltl_inductloops) | `sumo/output/cross3ltl_inductloops` |
| [Export MeanData for selected vehicles](https://sumo.dlr.de/extractTest.php?path=sumo/output/examples/cross3ltl_meandata_constrained) | `sumo/output/cross3ltl_meandata_constrained` |
| [Aggregate traffic data by edge](https://sumo.dlr.de/extractTest.php?path=sumo/output/examples/cross3ltl_meandata_edges) | `sumo/output/cross3ltl_meandata_edges` |
| [Aggregate traffic data by lane](https://sumo.dlr.de/extractTest.php?path=sumo/output/examples/cross3ltl_meandata_lanes) | `sumo/output/cross3ltl_meandata_lanes` |
| [Export queue lengths](https://sumo.dlr.de/extractTest.php?path=sumo/output/examples/cross3ltl_queue) | `sumo/output/cross3ltl_queue` |
| [Export simulation-wide summary statistics](https://sumo.dlr.de/extractTest.php?path=sumo/output/examples/cross3ltl_summary) | `sumo/output/cross3ltl_summary` |
| [Export statistics for completed trips](https://sumo.dlr.de/extractTest.php?path=sumo/output/examples/cross3ltl_tripinfo) | `sumo/output/cross3ltl_tripinfo` |
| [Export driven vehicle routes](https://sumo.dlr.de/extractTest.php?path=sumo/output/examples/cross3ltl_vehroutes) | `sumo/output/cross3ltl_vehroutes` |
| [Export positions for vehicles of a given type](https://sumo.dlr.de/extractTest.php?path=sumo/output/examples/cross3ltl_vtypeprobe) | `sumo/output/cross3ltl_vtypeprobe` |

## Network import and conversion

| Example download | Directory in `docs/examples` |
| --- | --- |
| [Define connections between edges](https://sumo.dlr.de/extractTest.php?path=netconvert/import/XML/connections/cross3l_edge2edge_conns) | `netconvert/connections/cross3l_edge2edge_conns` |
| [Define connections between individual lanes](https://sumo.dlr.de/extractTest.php?path=netconvert/import/XML/connections/cross3l_lane2lane_conns) | `netconvert/connections/cross3l_lane2lane_conns` |
| [Disable U-turns at an intersection](https://sumo.dlr.de/extractTest.php?path=netconvert/import/XML/connections/cross3l_no_turnarounds) | `netconvert/connections/cross3l_no_turnarounds` |
| [Set connection prohibitions](https://sumo.dlr.de/extractTest.php?path=netconvert/import/XML/connections/cross3l_prohibitions) | `netconvert/connections/cross3l_prohibitions` |
| [Let NETCONVERT infer intersection connections](https://sumo.dlr.de/extractTest.php?path=netconvert/import/XML/connections/cross3l_unconstrained) | `netconvert/connections/cross3l_unconstrained` |
| [Import a network with shape data](https://sumo.dlr.de/extractTest.php?path=netconvert/import/XML/shapes/hokkaido) | `netconvert/hokkaido` |
| [Convert the DLR test-track network](https://sumo.dlr.de/extractTest.php?path=netconvert/examples/messstrecke) | `netconvert/dlr-testtrack` |
| [Set edge speeds in km/h](https://sumo.dlr.de/extractTest.php?path=netconvert/import/XML/speed_in_kmh/cross_notypes_kmh) | `netconvert/speed_in_kmh/cross_notypes_kmh` |
| [Set edge type speeds in km/h](https://sumo.dlr.de/extractTest.php?path=netconvert/import/XML/speed_in_kmh/cross_usingtypes_kmh) | `netconvert/speed_in_kmh/cross_usingtypes_kmh` |
| [Define edges directly](https://sumo.dlr.de/extractTest.php?path=netconvert/import/XML/types/cross_notypes) | `netconvert/types/cross_notypes` |
| [Reuse edge types](https://sumo.dlr.de/extractTest.php?path=netconvert/import/XML/types/cross_usingtypes) | `netconvert/types/cross_usingtypes` |
| [Import a Berlin-Adlershof network from OpenStreetMap](https://sumo.dlr.de/extractTest.php?path=netconvert/import/OSM/adlershof/adlershof_dlr) | `netconvert/OSM/adlershof_dlr` |

## Routing and demand

| Example download | Directory in `docs/examples` |
| --- | --- |
| [Generate minute-by-minute demand from detector counts](https://sumo.dlr.de/extractTest.php?path=dfrouter/emitters/tri_completely_defined/tri_cd_1min_more_left) | `dfrouter` |
| [Route flows with one-second input intervals](https://sumo.dlr.de/extractTest.php?path=duarouter/flows/1s_interval) | `duarouter/flows2routes` |
| [Route flows with 100-second input intervals](https://sumo.dlr.de/extractTest.php?path=duarouter/flows/100s_interval) | `duarouter/flows2routes_100s_interval` |
| [Route flows with extended 100-second input](https://sumo.dlr.de/extractTest.php?path=duarouter/flows/100s_interval_ext) | `duarouter/flows2routes_100s_interval_ext` |
| [Route flows with 200-second input intervals](https://sumo.dlr.de/extractTest.php?path=duarouter/flows/200s_interval) | `duarouter/flows2routes_200s_interval` |
| [Compute routes from trips](https://sumo.dlr.de/extractTest.php?path=duarouter/trips/one_time_notype) | `duarouter/trips2routes` |
| [Generate routes from turn ratios](https://sumo.dlr.de/extractTest.php?path=jtrrouter/function/turns/definite_xml) | `jtrrouter/turns` |
| [Route traffic straight to specified sink edges](https://sumo.dlr.de/extractTest.php?path=jtrrouter/function/plain/straight_only_sinks) | `jtrrouter/straight_only_sinks` |

## Python tools

| Example download | Directory in `docs/examples` |
| --- | --- |
| [Iterate dynamic traffic assignment](https://sumo.dlr.de/extractTest.php?path=tools/assign/dua-iterate/simple) | `tools/dua-iterate` |
| [Generate routes from measured detector flow](https://sumo.dlr.de/extractTest.php?path=tools/detector/flowrouter/bell_pfe/plain) | `tools/flowrouter` |
| [Convert vehicle trajectories to NS-2 mobility traces](https://sumo.dlr.de/extractTest.php?path=tools/output/traceExporter/ns2/mobility_only) | `tools/traceExporter` |
