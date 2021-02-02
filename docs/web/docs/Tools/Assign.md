---
title: Tools/Assign
permalink: /Tools/Assign/
---

# Assignment Tools

## duaIterate.py

"duaIterate.py" helps you to perform the computation of a dynamic user
assignment (DUA). It works by alternatingly running the simulation to
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
duaIterate.py -n <PATH_TO_SUMO_NET> -t <PATH_TO_TRIPS>
```

Furthermore a calibrator option is available in the script to call the
Cadyts calibration tool, developed by Gunnar Flötteröd at EPFL,
Switzerland. With this option, route choices will be adjusted according
to given link counts. The validation work of the calibration between
SUMO and Cadyts is work in progress.

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

## networkStatistics.py

This script is to calculate the global performance indices according to
**SUMO-based** simulation results. The calculation functions are
directly defined in this script. Basic statistics are delivered, such
as:

- average travel time (s)
- average travel length (m)
- average travel speed (m/s)
- average departure delay (s)
- average waiting time (s)

Besides, this script is also to execute a significance test for
evaluating the results from different assignment methods. The t test and
the Kruskal-Wallis test are available in this script. If not specified,
the Kruskal-Wallis test will be applied with the assumption that data
are not normally distributed.

In order to execute this script, the other two scripts, i.e.
statisticsElements.py and tables.py, are required. They all should be in
the same directory.

In the statisticsElements.py, classes regarding vehicles, their
performance measures, t values, H values as well as functions for
outputs are defined. The chi-square table and the t table are defined in
the tables.py.

An exemplary command is shown below.

```
networkStatistics.py -t <tripinfo files> -o <output file> -e

where -t: name of output files containing vehicle information, generated by SUMO
      -o: define the output file name
      -e: set true for applying the t test (default: false)
      -k: set true for applying the Kruskal-Wallis test (default: false)
```

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
