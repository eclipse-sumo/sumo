---
title: Misc
---

# createVehTypeDistribution.py

Creates a vehicle type distribution by sampling from configurable value
distributions for the [desired `vType`-parameters](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#vehicle_types). Example:

```
python tools/createVehTypeDistribution.py config.txt
```

The only required parameter is the configuration file in the format
shown below (*example config.txt*):

```
tau; normal(0.8,0.1)
sigma; normal(0.5,0.2)
length; normal(4.9,0.2); [3.5,5.5]
param; myCustomParameter; normal(5, 2); [0, 12]
vClass; passenger
carFollowModel; Krauss
```

In the config file, one line is used per vehicle type attribute. The
syntax is: \[param; \] <AtrributeOrParameterName\>; <ValueOrDistribution\>
\[; <limits\>\]

If the prefix *param* is given at the beginning of a line, it is assumed
that the values of a vehicle *parameter* (given as a `param` child
element of the `vehicle` element) are to be sampled. Otherwise, values
of an *attribute* of the `vehicle` element are sampled.
ValueOrDistribution can be a string, a scalar value or a distribution
definition. Available distributions and its syntax are:

- "normal(mu,sd)" with mu and sd being floating numbers: Normal distribution with mean mu
and standard deviation sd.
- "normalCapped(mu, sd, min, max)" By default, no negative values are accepted but may be
enabled by setting a negative lower limit.
- "lognormal(mu,sd)" with mu and sd being floating numbers: Normal distribution with mean mu
and standard deviation sd.
- "uniform(a,b)" with limits a and b being floating numbers: Uniform distribution between a and b.
- "gamma(alpha,beta)" with parameters alpha and beta: Gamma distribution.

Additional options:

- **--output-file** configures the name of the output file to be written
- **--name** Name of the created distribution
- **--size** Number of s to be sampled for filling the distribution
- **--seed** Set the seed for the random number generator

## Retrieving parameters from measurements of individual vehicles

To obtain mean and deviation a number of values must be obtained from
the data set. The following is recommenced:

- **accel**: the maximum (or high percentile) acceleration for each
  vehicle
- **deccel**: the maximum (or high percentile) deceleration for each
  vehicle
- **speedFactor**: the maximum (or high percentile) quotient of
  speed/speedLimit for each vehicle


# extractTest.py

This scripts extracts test scenarios if you like to run a simulation scenario which is included in the test folder <SUMO_HOME>/tests. In order to do so, you can either:
- [download the complete sumo package](../Downloads.md#all-inclusive-tarball) and call:
```
python tools/extractTest.py <path to test directory>
```
- or use the [online test extraction](https://sumo.dlr.de/extractTest.php). In the online tool you enter the path to the test you like (e.g. [{{SUMO}}/tests/sumo/extended/rerouter/use_routing_device](https://github.com/eclipse/sumo/blob/master/tests/sumo/extended/rerouter/use_routing_device) into the form and get a zip containing all the files.

# generateParkingAreas.py

This tool generates parking areas for a network, and saves them in an additional file. If the output filename is not defined (using -o or --output-file), it will use as default *parkingareas.add.xml*.

Most basic call:
```
python tools/generateParkingAreas.py -n <my network>
```
Other Example:
```
python tools/generateParkingAreas.py -n <my network> -o <name of output file> --space-length <visual length per parking space> --min <minimum capacity> --max <maximum capacity>
```

The required parameter is the network (-n or --net-file). More options can be obtained by calling `python tools/generateParkingAreas.py --help`.

Additional options:

- **--output-file** define the output filename
- **--probability** probability for an edge to receive a parkingArea
- **--length** length required per parking space
- **--space-length** visual length of each parking space
- **--width** visual width of each parking space
- **--random-capacity** randomize roadsideCapacity
- **--min** minimum capacity for parkingAreas
- **--max** maximum capacity for parkingAreas
- **--angle** parking area angle
- **--prefix** prefix for the parkingArea ids
- **--seed** random seed
- **--random** use a random seed to initialize the random number generator
- **--vclass** only use edges which permit the given vehicle class
- **--verbose** tell me what you are doing

# generateParkingLots.py

This script generates parking lots. Example:

```
python tools/generateParkingLots.py -b <xmin, ymin, xmax, ymax> -c <connecting edge>
 [-i <parking-id> -n <number of parking spaces> -l <space-length> -a <space-angle> ...]
```

or

```
python tools/generateParkingLots.py -x <x-pos> -y <y-pos> -c <connecting edge>
 [-i <parking-id> -n <number of parking spaces> -l <space-length> -a <space-angle> ...]
```

The required parameter are the shape (--bounding-box) or the position
(--x-axis and --y-axis) of the parking lot and the connecting edge
(--connecting-edge). More options can be obtained by calling
python tools/generateParkingLots.py --help.

Additional options:

- **--parking-id** defines the name/id of the parking lot
- **--parking-spaces** defines the number of the parking spaces
- **--start-position** defines the begin position of the entrance/exit of the parking lot
- **--end-position** defines the end position of the entrance/exit of the parking lot
- **--space-length** defines the length of each parking space
- **--space-angle** defines the angle of the parking spaces
- **--x-space-distance** defines the lateral distance (x-direction) between the locations of
  two parking spaces
- **--y-space-distance** defines the longitudinal distance (y-direction) between the
  locations of two parking spaces
- **--rotation-degree** defines the rotation degree of the parking lot
- **--adjustrate-x** defines the modification rate of x-axis if the rotation exists
- **--adjustrate-y** defines the modification rate of y-axis if the rotation exists
- **--output-suffix** output suffix
- **--fullname** full name of parking area
- **--verbose** tell me what you are doing

# generateStationEdges.py

This script generates a pedestrian edge for each public transport stop
(in the form of *.nod.xml* and *.edg.xml* files. The output is suitable
for extending rail-only networks with the bare minimum of pedestrian
infrastructure for departing, changing trains and arriving. Example:

```
python tools/generateStationEdges.py rail.net.xml stops.xml
 netconvert -s rail.net.xml -e stops.access.edg.xml -n stops.access.nod.xml --ptstop-files stops.xml -o railForPersons.net.xml --ptstop-output stopsWithAccess.xml
```

# generateContinuousRerouters.py

This script generates rerouter definitions for a continuously running simulation. Rerouters are placed ahead of each intersection with routes leading up to the next intersection and configurable turning ratios. Vehicles that enter the simulation will circulate continuously (unless hitting a dead-end). Example:

```
python tools/generateContinuousRerouters.py -n <net-file> -o <output-file> 
```

# generateParkingAreaRerouters.py

This script generates parking area rerouters from a parking area definition. Example:

```
python tools/generateParkingAreaRerouters.py -n <net-file> -a <parkingArea-file> -o <output-file> 
```

# averageTripStatistics.py

This script runs a given sumo configuration multiple times with
different random seeds and averages the trip statistics output (see [trip statistics](../Simulation/Output/index.md#aggregated_traffic_measures)).

Example:

```
python tools/averageTripStatistics.py <sumocfg-file>
```

As default, the simulation will be run 10 times with an initial seed for 
random seed generation of 42. These values can be changed with the options
**-n** and **-s** respectively.

# ptlines2flows.py

This script determines feasible stop-to-stop travel times and creates a public
transport schedule (regular interval timetable) for all lines. The stop-to-stop 
travel times are determined by running a background simulation on an empty network using either a given route or shortest paths between stops. Example:

```
python tools/ptlines2flows.py -n <net-file> -s <ptstops-file> -l <ptlines-file> -o <output-file>
```

As output, the public transport lines are written as [flows](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md).
By default a period of 600 seconds is adopted as regular interval, which can be 
changed with the **-p** option.

With the option **--use-osm-routes**, public transport routes from the given osm
ptlines-file will be used, rather than creating new shortest path routes between stops.

A *ptlines-file* is typically created by [netconvert](../netconvert.md) option **--ptlines-output** when importing OSM data.
However it can also be customized or created from scratch for a non OSM network.

A minimal description for a bus line looks like this:

```
<additional>
    <ptLine id="0" line="123" type="bus">
        <busStop id="stopA"/>
        <busStop id="stopB"/>
        <busStop id="stopC"/>
    </ptLine>
</additional>
```

The used busStops must be defined in an additional file and passed with option **-s** when running the tool.
The resulting bus definition may look like this:

```
<routes xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="http://sumo.dlr.de/xsd/routes_file.xsd">
    <vType id="bus" vClass="bus"/>
    <route id="bus_123:0"" edges="110450334#1 110450334#2 338412122 391493949 391493947 391493950#0 391493950#1 391493952#0 391493952#1 391493952#2 391493954#0 391493954#1 391493954#2 391493954#3" >
        <stop busStop="stopA" duration="20" until="35.0"/> 
        <stop busStop="stopB" duration="20" until="101.0"/> 
        <stop busStop="stopC" duration="20" until="221.0"/>
    </route>
    <flow id="bus_123:0" type="bus" route="bus_123:0" begin="0.0" end="3600.0" period="600" line="123:0" /> 
</routes>
```


# tileGet.py

This script retrieves background images from ESRI ArcGIS tile servers and other imaging APIs
such as Google Maps and MapQuest. The simplest usage is to call it with a SUMO
network file only. It will generate a visualization settings file containing the coordinates which
can be loaded with sumo-gui or netedit. The most useful options are -t for the
(maximum) number of tiles to retrieve and -u to give the URL of the tile server. Examples:

- Retrieving data from the public ArcGIS online instance:
```
python tools/tileGet.py -n test.net.xml -t 10
```

- Retrieving satellite data from Google or MapQuest (Requires obtaining an API-key first):
```
python tools/tileGet.py -n test.net.xml -t 10 --url maps.googleapis.com/maps/api/staticmap --key YOURKEY
python tools/tileGet.py -n test.net.xml -t 10 --url open.mapquestapi.com/staticmap/v4/getmap --key YOURKEY
```

The generated setting file can be loaded in sumo-gui with:
```
sumo-gui -n test.net.xml -g settings.xml
```

# stateReplay.py

Synchronizes saved state files from a (remote) simulation and replays them in a
local sumo-gui instance to observe the remote simulation (requires rsync).

To observer every step in a simulation with step length 1s, the remote simulation must be started with option **--save-state.period 1**.
In order to conserve disk space, the option **--save-state.period.keep 3** is recommended. (i.e. to retain only the last 3 simulation state files at any time).

To replay the state files the following call can be used:
```
python tools/stateReplay --sumo-config replay.sumocfg --src REMOTE_FOLDER --dst LOCAL_FOLDER
```

The given .sumocfg file only needs to include the network and any additional infrastructure referenced by the remote simulation. The value of REMOTE_FOLDER can be any folder as understood by rsync (i.e. remotehost:~/myfolder)

!!! caution
    Option **--save-state.period 1** can slow down a simulation significantly.

