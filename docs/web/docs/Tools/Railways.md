---
title: Railways
---

# generateRailSignalConstraints.py

Generate [constraints that enforce a given railway schedule](../Simulation/Railways.md#schedule_constraints). 
Example:

```
<SUMO_HOME>/tools/generateRailSignalConstraints.py -r <input-route-file> -n <input-net-file> -a <input-stop-file> -o <output-file>
```

## Generated Constraints

the following constraints are generated in different cases:

### 1. `<predecessor>`

When two vehicles stop subsequently at the same busStop (trainStop) and they reach that stop
via different routes, the switch where both routes merge is identified and a
constraint is created for the rail signals that guard this merging switch:
    The vehicle B that arrives at the stop later, must wait (at its signal Y)
    for the vehicle A that arrives first (to pass it's respective signal X)
    This uses the 'arrival' attribute of the vehicle stops

A complication arises if the signal of the first vehicle is passed by other
trains which are en route to another stop. This makes it necessary to record a
larger number of passing vehicles within the simulation (controlled by the
limit attribute). The script attempts to determine the necessary limit value by
identifying all vehicles that pass the signal X en route to other stops between
the time A and B reach their respective signals (counting backwards from the
next stop based on "arrival". To account for delays the
options --delay and --limit can be used to override the limit values

### 2. `<insertionPredecessor>` ###

Whenever a vehicle B departs at a stop (assumed to coincide with the "until"
attribute of it's first stop), the prior train A that leaves this stop is
identified (also based on "until"). Then a constraint is created that prevents
insertion of B until train A has passed the next signal that lies beyond the
stop.

### 3. `<predecessor>` ###
Whenever a vehicle A departs at a stop (assumed to coincide with the "arrival"
attribute of it's first stop), the latter train B that enters this stop is
identified (also based on "arrival"). Then a constraint is created that prevents
B from entering the section with the stop until A has passed the next signal that lies beyond the
stop.

## Inconsistencies ##

Inconsistent constraints may arise from inconsistent input and cause simulation
deadlock. To avoid this, the option --abort-unordered can be used to avoid
generating constraints that are likely to be inconsistent.
When the option is set the ordering of vehicles is cross-checked with regard to
arrival and until times:

Given two vehicles A and B which stop at the same location, if A arrives at the
stop later than B, but A also leaves earlier than B, then B is "overtaken" by A.
The stop of A and all subsequent stops of A are marked as invalid and will not
participate in constraint generation.

If two vehicles have a 'parking'-stop with the same 'until' time at the same
location, their stops will also be marked as invalid since the simulation cannot
enforce an order in this case (and local desired order is ambiguous).

## Post-Facto Stop Timings ##

When simulating the past (i.e. to predict the future), additional timing data
besides the scheduled arrival and until times may be available and included in
the 'started' and 'ended' attributes for each stop.
They can be used to detect changes in train order that occurred during the actual
train operation and which must be taken into account during constraint
generation to avoid deadlock.
If train A has 'started' information for a
stop while train B has not, this implies that A has reached the stop ahead of B.
Likewise, both trains may have 'started' information but in the reverse order
compared to the schedule.
For all stops with complete started,ended information, those times can be used
as an updated schedule (replacing arrival and until). However, if an order
reversal was detected for a train, no constraints based on the old schedule
should be generated anymore (stops are ignored after started,ended information ends)


## Further Options ##

If constraints shall be modified during the simulation (traci.trafficlight.swapConstraints)
it may be useful to add additional constraints which would otherwise be
redundant. This can be accomplished by setting option --redundant with a time
range. When set, trains that follow a constrained train within the given time
range (and which would normally be constrained implicitly by their leading
train) will also receive a constraint. In this case option --limit must be used
to ensure that all constraint foe vehicles are recorded during the simulation.

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
- **--group-statistic-type** (**-T**): select among statistics over the group values (i.e. to compare the groups by their mean depart delay). permitted values are mean, median, min, max
- **--histogram FLOAT** (**-i**): add a histogram with the given bin width
- **--group-histogram FLOAT** (**-I**): add a histogram over the group values (when using **-g**) with the given bin width

Examples:
```
<SUMO_HOME>/tools/output/scheduleStats.py -r <input-route-file> -s <stop-file> -t d -i 50
<SUMO_HOME>/tools/output/scheduleStats.py -r <input-route-file> -s <stop-file> -t a -g stopID -T median -I 10
```
