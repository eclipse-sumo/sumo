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

Different types of constraints are generated in different cases:

### 1. predecessor
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

A more complicated case arises when the next stop after the merging switch differs for
both trains even though their routes are the same. In this case the ordering of
the trains must be deduced from their next common stop and the algorithm adds
a "virtual" intermediateStop directly after the switch to "normalize" the input.

### 2. insertionPredecessor
Whenever a vehicle B departs at a stop (assumed to coincide with the "until"
attribute of it's first stop), the prior train A that leaves this stop is
identified (also based on "until"). Then a constraint is created that prevents
insertion of B until train A has passed the next signal that lies beyond the
stop.

These constraints are also needed in the context of parking-stops because these
have the potential to alter train ordering:

If vehicle A has a parking stop with 'ended' time and vehicle B has a
parking stop at the same location without 'ended' (only an 'until' time),
an insertionPredecessor constraint is created for B to ensure that A leaves
first. This is because availability of an 'ended' value implies that the even is
in the past whereas the lack of the value indicates that the stop is still in
the future.

In the case where an intermediateStop (see above) is also a parking stop, an
insertionPredecessor constraint is added if the parking vehicle is scheduled to
go second.

### 3. foeInsertion
Whenever a vehicle A departs at a stop (assumed to coincide with the "until"
attribute of it's first stop), the latter train B that enters this stop is
identified (also based on "until"). Then a constraint is created that prevents
B from entering the section with the stop until A has passed the next signal that lies beyond the
stop.

### 4. insertionOrder
Whenever two vehicles depart at the same stop and their until/ended times at that stop
are in a different order from their departure times, an insertionOrder constraint
is added the delays insertion to achieve the desired order.
This may happen if departure times reflect the schedule
but until/ended times reflect post-facto timing (see below)

### 5. bidiPredecessor
Whenever two trains approach the same track section from different directions, a bidiPredecessor
constraint may optionally be generated to enforce the order of entering that section based on the
stop arrival times that follow the section on the respective side.
(stop arrival times must be corrected for the estimated travel time between the end of the conflict section
and the stop)

## Inconsistencies

Inconsistent constraints may arise from inconsistent input and cause simulation
deadlock. To avoid this, the option --abort-unordered can be used to avoid
generating constraints that are likely to be inconsistent.
When the option is set the ordering of vehicles is cross-checked with regard to
arrival and until times:

Given two vehicles A and B which stop at the same location, if A arrives at the
stop later than B, but A also leaves earlier than B, then B is "overtaken" by A.
All subsquent stops of B are marked as invalid and will not
participate in constraint generation. If the stop where overtaking took place
doesn't have a 'started' value (which implies that the original schedule is
inconsistent), then this stop is also mared as invalid.

If two vehicles have a 'parking'-stop with the same 'until' time at the same
location, their stops will also be marked as invalid since the simulation cannot
enforce an order in this case (and local desired order is ambiguous).

Another kind of inconsistency is indicated by 'ended' times that lie ahead of
the 'until' time of the respective stop by a significant margin (--.
This situation may corrrespond to the actions of
a real-life dispatcher. In such a case, the must not be constraint any further
since it is no longer running according to the schedule.

## Post-Facto Stop Timings

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

## Further Options 
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
