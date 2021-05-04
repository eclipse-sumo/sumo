---
title: Railways
---

# generateRailSignalConstraints.py

Generate constraints that enforce a given railway schedule. Example:

```
<SUMO_HOME>/tools/generateRailSignalConstraints.py -r <input-route-file> -n <input-net-file> -a <input-stop-file> -o <output-file>
```

The tool will analyze the order of arrival at stations (stops). 

- If vehicles have successive stops at the same station but reach this station via different tracks, constraints will be generated for the signals ahead of the merging switch. The vehicle that comes later has to wait for the vehicle that comes earlier ([predecessor constraint](../Simulation/Railways.md#predecessor_constraint)).
- If vehicles are inserted on the edge of their first stop, a [insertionPredecessor constraint](../Simulation/Railways.md#insertionpredecessor_constraint) is generated so that insertion is delayed until the train that stops earlier has passed the signal subsequent to the station.
- If a vehicle is inserted on the edge of their first stop, a [insertionPredecessor constraint](../Simulation/Railways.md#insertionpredecessor_constraint) is generated so that vehicles that pass this stop later will not enter that section until the vehicle to be inserted has left the section (next rail signal after the stop)


# scheduleStats.py

Compare arrival and departure at stops between an input schedule (route-file) and
simulation output (stop-output). The results will be printed on the command line.

Example:
```
<SUMO_HOME>/tools/output/scheduleStats.py -r <input-route-file> -s <stop-file>
```

Options:

- **--xml-output FILE** (**-o**): write statistics to FILE in xml format
- **--statistic-type** (**-t**): select among statistics to be computed
  - 'd' : depart delay
  - 'a' : arrival delay
  - 's' : stop duration difference
  
- **--group-by** (**-g**): group results by one or more attributes (vehID,tripId,stopID,priorStop)
- **--group-statistic-type** (**-T**): select among statistics over the group values (i.e. to compore the groups by their mean depart delay). permitted values are mean, median, min, max
- **--histogram FLOAT** (**-i**): add a histogram with the given bin width
- **--group-histogram FLOAT** (**-I**): add a histogram over the group values (when using **-g**) with the given bin width

Examples:
```
<SUMO_HOME>/tools/output/scheduleStats.py -r <input-route-file> -s <stop-file> -t d -i 50
<SUMO_HOME>/tools/output/scheduleStats.py -r <input-route-file> -s <stop-file> -t a -g stopID -T median -I 10
```
