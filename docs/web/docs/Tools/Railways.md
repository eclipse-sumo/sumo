---
title: Railways
---

# generateRailSignalConstraints.py

Constraints can be generated using the tool `generateRailSignalConstraints.py`. Example:

```
<SUMO_HOME>/tools/generateRailSignalConstraints.py -r <input-route-file> -n <input-net-file> -a <input-stop-file> -o <output-file>
```
The tool will analyze the order of arrival at stations (stops). 

- If vehicles have successive stops at the same station but reach this station via different tracks, constraints will be generated for the signals ahead of the merging switch. The vehicle that comes later has to wait for the vehicle that comes earlier ([predecessor constraint](../Simulation/Railways.md#predecessor_constraint)).
- If vehicles are inserted on the edge of their first stop, a [insertionPredecessor constraint](../Simulation/Railways.md#insertionpredecessor_constraint) is generated so that insertion is delayed until the train that stops earlier has passed the signal subsequent to the station.
