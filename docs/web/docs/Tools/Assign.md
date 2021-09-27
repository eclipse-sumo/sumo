---
title: Assign
---

# Assignment Tools

## duaIterate.py

"duaIterate.py" helps you to perform the computation of a [dynamic user
assignment (DUA)](../Demand/Dynamic_User_Assignment.md). It works by alternatingly running the simulation to
discover travel times and then assigning alternative routes to some of
the vehicles according to these travel times. This is repeated for a
defined number of iteration steps. At least two files have to be given
as input the script: a SUMO-network and a set of trip definitions. A
stochastic user-equilibrium (UE)traffic state is not guaranteed after
the assignment. Increasing the number of iteration steps increases the
likelyhood of convergence to equilibrium.

Within each iteration step, the script generates a configuration file
for the [duarouter](../duarouter.md) application and starts it with
this configuration file. Then, a configuration file for
[sumo](../sumo.md) is built and [sumo](../sumo.md) is started.
Both configuration files are completely defined within the script itself.
As default, for each time step, SUMO will generate three dump files with
edge-dumps aggregated over 150, 300, and 900s, a summary information as
well as a trip information output. The names of these outputs are
numbered according to the iteration step. If you want to change the
outputs, you also have to take a look into the script, but you should
not disable the edge-based dump for aggregation over 900s, because this
is read by the [duarouter](../duarouter.md) in the next iteration
steps in order to compute the DUA.

For further options to the script look either at the source code or
start it with the "--help" option.

```
python tools/assign/duaIterate.py -n <PATH_TO_SUMO_NET> -t <PATH_TO_TRIPS>
```

### Loading vehicle types from an additional file

If the file `vtypes.add.xml` defines vehicle types that are needed by the traffic demand input for duaIterate.py, the following options must be set

**--additional vtypes.add.xml duarouter--vtype-output dummy.xml duarouter--additional-files vtypes.add.xml**

The first option passes it's arguments only to the [sumo](../sumo.md) process. The other options are passed tu [duarouter](../duarouter.md) and ensure that the types are read but are not written to the route output file (since they would than be loaded twice by sumo resulting in an error).

!!! caution
    Options, prefixed with **duarouter--** must be the last in the list of all options

## duaIterate_analysis.py

To check the evolution of different simulation parameters during the dua
estimation an additional script is provided which analyses teleport counts
and how they evolve over the iterations.

```
python tools/assign/duaIterate_analysis.py dualog.txt
```

## cadytsIterate.py

This script is a variant of duaIterate.py which uses the
Cadyts calibration tool, developed by Gunnar Flötteröd at EPFL,
Switzerland. With this script, route choices will be adjusted according
to given link counts. The validation work of the calibration between
SUMO and Cadyts is work in progress. You will need to download
[Cadyts](https://github.com/gunnarfloetteroed/java) separately and add a 
reference to the jar file to the call.

```
python tools/assign/cadytsIterate.py -n input_net.net.xml -r routes.rou.xml -d input_measurements.xml --classpath cadyts.jar
```

## one-shot.py

The one-shot.py provides a variant of the dynamic user assignment. The
assignment method is the same one which SUMO and the dua-iterate.py use.
Given trips will be assigned to respective fastest routes according to
their departure times and a given travel-time updating interval.
Different travel-time updating intervals can be defined by users, such
as 900, i.e. link travel times will be updated every 900 sec. If the
travel-time updating interval is set to -1, link travel times will not
updated and link trave times at free-flow speeds are used for all trips.

A stochastic user-equilibrium traffic state will not achieved with the
use of this script.

An exemplary execution command is shown below.

```
 one-shot.py -f <travel-time updating interval> -n <network file> -t <trip file>

 where -f travel-time updating interval (sec); -1 means no travel-time updating (default: -1,1800,300,15)
       -n network file name and the respective path
       -t trip file name and the respective path
```

Additional setting, such as outputs of summary and trip-information as
well as begin time, could be made by adding respective options which can
be found in the script.

## matrixDailyToHourly.py

This script is to generate hourly matrices from a VISUM daily matrix.
The traffic demand of the traffic zones, which have the same connection
links, will be integrated. The exemplary command is indicated below.

```
matrixDailyToHourly.py -m <matrix file> -z <district file> -t <flow time-series file> -o 

<output directory>

where -m: matrix file name
      -z: district file name
      -t: name of the file containing traffic flow time series (optional); If not specified, the defined 
          daily matrix will be regularly divided into 24 hours.
      -o: output directoy name and the respective path
```

## costFunctionChecker.py

Run duarouter repeatedly and simulate weight changes via a cost
function. (to be continued)

Still under construction\!
