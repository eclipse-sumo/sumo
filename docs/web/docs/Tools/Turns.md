---
title: Turns
---

# routeSampler.py
The script generates traffic (routed vehicles or flows) from *any combination* of turn-count data, edge-count and even origin-destination-count data. It requires a route file as input that defines possible routes. Routes are sampled (heuristically) from the input so that
the resulting traffic fulfills the counting data.

## edge counts
The data, most frequently available takes the form of traffic counts on roads (edges). This data can be passed to routeSampler.py in an [edgeData
file](../Simulation/Output/Lane-_or_Edge-based_Traffic_Measures.md) using option **--edgedata-files**.

```
python tools/routeSampler.py -r <input-route-file> --edgedata-files <edgedata-files> -o <output-file>
```

Only the edge attribute 'id' and another attribute for the traffic count are needed:

```xml
<data>
    <interval id="arbitrary" begin="0.0" end="300">
        <edge id="-58" entered="4"/>
        <edge id="45" entered="3"/>
        <edge id="-31" entered="15"/>
        ...
     </interval>
     ...
</data>
```

The attributes for reading the counts from edge-data file can be set with option **--edgedata-attribute** (default 'entered')
You can put any number of counting attributes in the same `<edge>` element if you intend to call **routeSamper.py** [multiple times for different vehicle types](#multiple_vehicle_types).

!!! note
    The default attribute is 'entered' because this attribute corresponds best to the through-traffic count in sumo-generated edgeData files.

## Obtaining counting data files

When using routeSampler as a replacement for [dfrouter](../dfrouter.md) or [flowrouter.py](Detector.md#flowrouterpy), the flow input files can be converted to edgeData files with the tool [edgeDataFromFlow.py](Detector.md#edgedatafromflowpy)

For smaller scenarios it is often feasible to define edgeData files with [netedit edgeData mode (in the 'Data' supermode)](../Netedit/editModesData.md). Turn files can also be created with netedit using the edgeRelation mode (also part of the 'Data' supermode).

In other cases it is necessary to write custom code for converting counting data into the required format.

Further source for edgeData files are listed at the [visualizing edge related data](../sumo-gui.md#visualizing_edge-related_data) page.

## turn counts

Traffic counts related to turning traffic can be provided in the following format:

The turn-count data must be provided in the format:

```xml
<data>
  <interval id="generated" begin="0.0" end="99.0">
    <edgeRelation from="-58.121.42" to="64" count="1"/>
    <edgeRelation from="-58.121.42" to="-31" count="3"/>
    <edgeRelation from="45" to="-68" count="3"/>
    <edgeRelation from="-31.80.00" to="31" count="1"/>
    <edgeRelation from="-31.80.00" to="37" count="1"/>
    <edgeRelation from="-31.80.00" to="-23" count="13"/>
    <edgeRelation from="-92.180.00" to="-60" count="1"/>
  </interval>
</data>
```

Example call:
```
python tools/routeSampler.py -r <input-route-file> --turn-files <turn-files> -o <output-file>
```
The attributes for reading the counts from the turn-data file can be set with option **--turn-attribute** (default 'count')
You can put any number of counting attributes in the same `<edgeRelation>` element if you intend to call **routeSamper.py** [multiple times for different vehicle types](#multiple_vehicle_types).

In case an existing scenario should be modified (i.e. in combination with different kinds of counting data) it is also possible to [convert an existing file with vehicles into a turn-count file](#generateturnratiospy).

!!! note
    When working with turning counts at a roundabout, the incoming and outgoing edges are not directly connected (since vehicles must pass a number of edges within the roundabout first). In this case, the option [--turn-max-gap](#generalized_route_restrictions) must be used and should be set to the number of edges within the largest roundabout.

## turn ratios

Traffic data related to turning traffic can also be provided as ratios in the following format:
```xml
<data>
  <interval id="generated" begin="0.0" end="99.0">
    <edgeRelation from="-58.121.42" to="64" probability="0.3"/>
    <edgeRelation from="-58.121.42" to="-31" probability="0.7"/>
    <edgeRelation from="-31.80.00" to="31" probability="0.1"/>
    <edgeRelation from="-31.80.00" to="37" probability="0.1"/>
    <edgeRelation from="-31.80.00" to="-23" probability="0.8"/>
  </interval>
</data>
```

Example call:
```
python tools/routeSampler.py -r <input-route-file> --turn-ratio-files <turn-ratio-files> -o <output-file>
```

The attributes for reading the ratios from the turn-data file can be set with option **--turn-ratio-attribute** (default *probability*).
You can put any number of counting attributes in the same `<edgeRelation>` element if you intend to call **routeSamper.py** [multiple times for different vehicle types](#multiple_vehicle_types).

!!! caution
    Turning ratios must be combined with some other counting data to define the absolute level of traffic. This can be any of the other counting data formats or by setting option **--total-count**.

## Obtaining initial routes
Routes generated by randomTrips.py (**--route-output**) can be a good input source. The following randomTrips.py options may be helpful:

- **--fringe-factor**: setting a high value will generated lots of through-traffic which is plausible for small networks
- **--min-distance**: restricting short routes increases the chance that routes passing multiple counting locations are generated
- **--speed-exponent**, **--lanes**: both options can be used to increase routes starting and ending on "important" roads. This combines well with the **--fringe-factor**-option to generate many routes that enter and leave the network on major roads.
- **--random-routing-factor**: randomly disturb the route choice so that trips with the same origin and destination may use different routes. Values must be larger than 1. A value of 2 may change the apparent travel time on an edge by up to to between 100% and 200% of its empty-network-travel-time. This ensures that the resulting routes take at most twice as long as the "fastest" route. The value is randomized anew for each vehicle so there are no systematic biases.
- **--edge-type-file**: Load edge-type specific probabilities for generating origins and destinations
- **--marouter**: compute routes with macroscopic assignment. These routes consider traffic conditions and therefore give more variety computed to default duarouter outputs (fastes-routes-in-empty-network).

The most realistic routes are those that result from [dynamic user assignment](../Demand/Dynamic_User_Assignment.md). Even when starting with random traffic demand, such routes will cover a better variaty of plausible routes through the network.

Example:
```
python tools/randomTrips.py -n <input-net-file> -r sampleRoutes.rou.xml
python tools/routeSampler.py -r sampleRoutes.rou.xml --edgedata-files <edgedata-files> -o <output-file>
```

!!! note
    departure times in route files are ignored and only the `<route>`-elements are used. Route with named routes but without vehicles may also be used.

## Generalized route restrictions
By default, the input options --edgedata-files and --turn-files allow restricting counts for single edges and pairs of consecutive edges.

To define count restrictions for longer consecutive edge sequences the optional 'via' attribute can be used for `<edgeRelation>` elements:

```xml
<edgeRelation from="A" to="D" via="B C" count="42"/>
```

To define count restrictions on non-consecutive edges the option **--turn-max-gap <INT>** can be used. Example:
When setting option **--turn-max-gap 2**, the edgeRelation `<edgeRelation from="A" to="D" .../>` would apply to routes containing "A B", "A X D" or "A X Y D" but not "A X Y Z D".

## Origin-Destination restrictions

The following section describes counts that refer to the origin and destination of a route rather than to some intermediate route edges.
Using such counts with routeSampler.py permits to combine them with other forms of counts or ratios. (Unlike [od2trips](../od2trips.md) which only imports OD-counts and nothing else).

If origin destination counts are loaded, it is assumed that they give complete coverage of the scenario. This means, if no count is given for a particular Origin-Destination-pair, it is assumed that the count is 0 and no traffic is generated between the respective edges or TAZ.  To change this, the option **--extra-od** may be set. In this case, traffic between any relations can be generated as long as it does not exceed the loaded counts.

### Edge Based

When loading an edgeRelation file with the option **--od-files**, origin-destination counts will be added.
This can be used to combine (edge-based) OD-relations with other counting data.

```xml
<edgeRelation from="A" to="D" count="42"/>
```

The above counting data example will match routes which start on edge `A` and end on edge `D`.

The tool [route2OD.py](Routes.md#route2odpy) supports option **--edge-relations** to transform any kind of route file into a suitable file of edge-based origin-destination edgeRelations.

### TAZ (district) Based

When loading an [TAZ-file](../Demand/Importing_O/D_Matrices.md#describing_the_taz) with option **--taz-files**, it is possible to define counts between Traffic Assignment Zones (TAZ, also called districts).
The counting data itself must be given in the following from and loaded with option **--od-files**:

```
<data>
    <interval id="generated" begin="0.0" end="99.0">
        <tazRelation from="1_2" to="2_0" count="5"/>
    </interval>
</data>
```

The TAZ ids which are referenced by the `tazRelation` attributes `from` and `to` must be defined within the loaded **--taz-files**.
The above counting data example will match routes which start on any edge of TAZ `1_2` and end on any edge of taz `2_0`.

## Depart / Arrival restrictions

Additional attributes can be read from edgeData files to set the total number of departures or arrivals for each edge. This can be used to create traffic where there would be no traffic otherwise but also to restrict the sampled routes by action as additional constraints together with other counting data (i.e. turn-counts). The following options are available:

- **--arrival-attribute**: set the attribute to read the number of arrivals on an edge
- **--depart-attribute**: set the attribute to read the number of departures on an edge

!!! note
    When loading an edgedata-file that only contains the arrival or depart-attributes, the value of **--edgedata-attribute** may be set to 'None' to suppress the warning about missing attributes that would otherwise be issued.

## Total count restrictions

The option **--total-count INT[,INT,..]** can be used to specify the total number of generated vehicles. The following two cases are generally possible:

- total-count is **higher** than the traffic that would be generated by matching counts with all other given locations: In this case any routes that do not pass a counting location are used to generated additional traffic (thus avoiding to exceed the count at any specific location). The user must ensure that such routes are available.
- total-count is **lower** than the traffic that would be generated by matching counts with all other given locations: In this case sampling is aborted upon reaching the total count. Option [**--optimize**](#optimization) may be used to improve matching all counting locations subject to the total count constraint.

The following rules apply depending on the argument value of **--total-count**:

- if total-count is a list of integer values that matches the number of data intervals, each data interval will use the respective count value from the input list
- if total-count is a single integer value and there are multiple data intervals, the given count is split in proportion to the sum of all counting locations per interval
- if total-count is set to the special value `input` (**--total-count input**) then the number of vehicles passed in the input route files is used as total-count. If there are multiple data intervals, the input vehicles are assigned to each interval based on their departure time.  If option **--pedestrians** is set, the departures of persons will be counted instead (regardless of whether their plan includes walks or rides).

## Output Styles
By default, routeSampler will generate individual vehicles with embedded routes. This can be changed with the following options (which can also be combined):

- **--write-route-ids** (shortcut **-I**): write named routes and let vehicles reference the route via its id
- **--write-route-distribution STR** (shortcut **-u**): put all routes into a route distribution (with appropriate route probabilities) and let a all vehicles reference this distribution (Simulation counts will then vary due to sampling from the distribution)
- **--write-flows number** (shortcut **-f**): write `<flow>`-definitions instead of vehicles. The exact number of flow vehicles will be spaced evenly between the earliest and latest vehicle departures that would have been generated by default
- **--write-flows probability**: write `<flow>`-definitions instead of vehicles. Flows will be defined with attribute 'probability' so that the expected number of vehicles is equal to the number of vehicles that would have been generated by default but the specific number will vary due to sampling effects
- **--pedestrians**: write persons and personFlows with walks instead of vehicles and flows
- **--prefix**: prefix vehicle- and route-IDs with the given string. This is needed to combine output of multiple routeSampler runs (i.e. for different modes of traffic) into a single simulation. Otherwise there would be errors regarding duplicate IDs.

## Vehicle attributes
With the option **--attributes** {{DT_STR}}, additional parameters can be given to the generated
vehicles (note, usage of the quoting characters).

```
python tools/routeSampler.py -n input_net.net.xml -r candidate.rou.xml -o out.rou.xml -d data.xml
  --attributes="departLane=\"best\" departSpeed=\"max\" departPos=\"random\""
```

The above attributes would make the vehicles be distributed randomly on their
starting edges and inserted with high speed on a reasonable lane.

!!! note
    Quoting of trip attributes on Linux may also use the style **--attributes 'departLane="best" departSpeed="max" departPos="random"'**

## Multiple vehicle types

To distinguish vehicles of different types, routeSampler may be run multiple times with different attributes. Note, that it is also necessary to set the option **--prefix** to prevent duplicate ids. The example below creates traffic consisting of cars and trucks using two edgedata files with distinct count values (stored in the default attribute 'entered').

```
python tools/routeSampler.py --attributes="type=\"car\"" --edgedata-files carcounts.xml --prefix c -o cars.rou.xml -r candidate.rou.xml
python tools/routeSampler.py --attributes="type=\"heavy\"" --edgedata-files truckcounts.xml --prefix t -o trucks.rou.xml -r candidate.rou.xml
```

Alternatively, the count values might also be stored in different attributes of the same file (i.e. 'count1', 'count2'):

```
python tools/routeSampler.py --attributes="type=\"car\"" --edgedata-files counts.xml --edgedata-attribute count1 --prefix c -o cars.rou.xml -r candidate.rou.xml
python tools/routeSampler.py --attributes="type=\"heavy\"" --edgedata-files counts.xml --edgedata-attribute count2 --prefix t -o trucks.rou.xml  -r  candidate.rou.xml
```

When running the simulation, the types 'car' and 'heavy' (previously set as vehicle attributes), must be defined in an additional file which could look like the following example (types.add.xml):

```xml
<additional>
  <vType id="car"/>
  <vType id="heavy" vClass="truck"/>
</additional>
```

The simulation could then be called like this:

```
  sumo -n net.net.xml -a types.add.xml -r cars.rou.xml,trucks.rou.xml
```

## Sampling
In this stage, routes are selected randomly within the limits imposed by the input counts.
Only routes that pass a minimum number of counting locations (option **--min-count**, default *1*) are eligible for sampling.

### Default Sampling

By default, sampling will be performed iteratively by:

1. selecting a random counting location that has not yet reached its count (and which still has viable routes)
2. selecting a random route that passes this counting location

until all counting locations have reached their measured count or there are no viable routes (routes which have all their passed counting locations below the input count)

### Weighted Sampling

By setting the option **--weighted**. The sampling algorithm is changed. For each route a probability value is loaded from the input. The probability can either be specified explicitly using route attribute 'probability' or implicitly if a route with the same sequence of edges appears multiple times in the route input. Sampling will be performed iteratively by:

1. selecting a random viable route sampled by probability

until all counting locations have reached their measured count or there are no viable routes (routes which have all their passed counting locations below the input count)

## Optimization
By default, routes will be sampled from the input route set until no further routes can be added without exceeding one of the counts. This may still leave some counts below their target values.

The problem of selecting a multi-set of route so that all count values are matched can be formulated as an [Integer linear programming problem (ILP)](https://en.wikipedia.org/wiki/Integer_programming).

RouteSampler computes an approximate solution to this ILP by relaxing it to a linear programming problem (LP) and passing it to an LP-solver library (scipy).
  The resulting non-integral solution is then rounded to an integer solution.
It is often desirable to find an optimized solution that is close to the initial sampling-solution. This way, route probabilities that were part of the input can be maintained to some degree.

By setting option **--optimize `<INT>`**. The number of times that a route is used can be changed by up to **`<INT>`** times. This defines a trade-off between using routes in the same distribution as found in the input and optimizing the counts.
When setting option **--optimize full**. No constraints on the route distribution are set and any route can be used as often as needed to reach the counts.

The option **--minimize-vehicles <FLOAT>** can be used to configure a weighting term for simultaneously minimizing the total number of used routes (vehicles).
  A higher value, favours solutions with few vehicles that pass multiple counting locations rather than more vehicles that pass fewer locations. Hard constraints on the minimum number of counting locations passed by each route can also be set with option **-min-count**.

!!! note
    Optimization requires [scipy](https://www.scipy.org/).

## Further Calibration
It is possible to load the resulting output into routeSampler.py for another round of optimization. By setting the option **--optimize-input** the sampling step is skipped and the optimizer is run directly on the input route set.

By removing specific routes or adding new ones, the user can thus tailor the generating traffic in an iterative manner.

## Quality Control

There is a range of reasons that could lead to a deviation between the routeSampler results and the user expectation. This section lists common reasons and documents which outputs can be used to judge the quality of the results.

### Problem areas

- Selecting a set of routes to match a given set of counts is generally an underdetermined problem which means there is of not a unique solution. If there are insufficient constraints on the solution space (not enough counts), the chosen solution may appear implausible
- Input routes are not representative of the real routes
     - not covering the counting locations
     - containing unlikely routes
- Temporal patterns are not captured by the given interval data (i.e. by only having counts for the whole day)
- Traffic counts are not stationary within an interval (i.e. a single vehicle passing a long a list of widely spaced counting locations and each interval registers a different location). In this case the route that covers all counting locations will not be sampled. (Longer intervals must be chosen in this case).
- Inconsistency in the counting data
- Many short routes (can be prevented by setting option **--min-count**)

### Textual quality metrics

- underflow locations: Statistics on all counting locations with less traffic than defined in the input
- overflow locations: Statistics on all counting locations with more traffic than defined in the input (only happens due to rounding of fractional counts after optimization)
- [GEH](https://en.wikipedia.org/wiki/GEH_statistic) statistics: Statistics on locations that reach a defined GEH threshold (configurable with option **--geh-ok**, default *5*)
- total number and percentage of matched counts (note that this is distinct from the number of vehicles since each vehicle may be counted multiple times)

### Spatial quality data

By setting option **--mismatch-output FILE** an [edgeData] file will be created that holds the mismatch between input and output count (`deficit`) as well as the input count (`measuredCount`) for each time interval:

Example:
```
<data>
    <interval id="deficit" begin="0.0" end="99.0">
        <edgeRelation from="-58.121.42" to="64" measuredCount="1" deficit="0"/>
        <edgeRelation from="-58.121.42" to="-31" measuredCount="3" deficit="0"/>
        <edgeRelation from="45" to="-68" measuredCount="3" deficit="1"/>
        ...
    </interval>
</data>
```

Such a data file can be visualized in [sumo-gui](../sumo-gui.md#visualizing_edge-related_data) and also plotted with [plot_net_dump.py](Visualization.md#plot_net_dumppy).

# generateTurnRatios.py

This script is used to calculate the turn ratios from a
an edge to its downstream edge with a given route file. The output file
can be directly used as input with [routeSampler.py](#routesamplerpy) and [jtrrouter](../jtrrouter.md). The time interval will span the minimum and maximum departure times of the route file but it can also be configured with options **--begin**, **--end** and  **--interval**.

```
python tools/turn-defs/generateTurnRatios.py -r <route-file>
```

The standard output is the traffic volumes (which jtrrouter normalizes automatically). With the option **-p**,  turning ratios will be written as values from [0,1].


# generateTurnDefs.py

This script allows generation of the turn definitions based on the
number of lanes allowing particular turns. The basic functionality
distributes the traffic uniformly, that is:

1.  distribute the incoming traffic uniformly across the lanes forming
    the road
2.  distribute the amount of traffic assigned to each lane uniformly
    among the destinations that the lane allows turns to.
3.  sum up the values for each of the destinations that the road being
    processed allows turning to.

Example use

```
python tools/turn-defs/generateTurnDefs.py --connections-file connections.con.xml --turn-definitions-file output.turndefs.xml
```

The script allows to be extended with new traffic distribution policies
(for example, based on Gaussian distribution) easily. See the
*DestinationWeightCalculator* class for details.

The script processes the connections given in the provided *\*.con.xml*
file. For usage details, execute the *generateTurnDefs.py* script with
*--help* option.

!!! note
    You can generate a connections file with all the connections in the network using [netconvert](../netconvert.md) - see the *--plain-output-prefix* option.

# turnCount2EdgeCount.py

This script converts turn-count data into [edgeData](../Simulation/Output/Lane-_or_Edge-based_Traffic_Measures.md).

```
python tools/turn-defs/turnCount2EdgeCount.py -t <turn-file> -o <output-file>
```

# turnFile2EdgeRelations.py

This script converts the deprecated turn-file format into edgeRelation format

```
python tools/turn-defs/turnFile2EdgeRelations.py -t <turn-file> -o <output-file>
```

# jtcrouter.py
The **J**unction**T**urn**C**ountRouter generates vehicle routes from turn-count data.
It does so by converting the turn counts into into flows and turn-ratio files that are suitable as [jtrrouter](../jtrrouter.md) input.
Then it calls jtrrouter in the background.

Example call:

```
python tools/jtcrouter.py -n <net-file> -t <turn-file> -o <output-file>
```
There are three basic styles of converting turn-counts to routes:

- Flows start at all turn-count locations in the network but end when reaching the next count location
- Flows start at all turn-count locations in the network and are discounted when reaching the next count location (**--discount-sources**)
- Flows only start on the fringe of the network (**--fringe-flows**)

## Turn count data format

The turn-count data must be provided in the format which is the same as for [routeSampler](#routesamplerpy):

```xml
<data>
  <interval id="generated" begin="0.0" end="99.0">
    <edgeRelation from="-58.121.42" to="64" count="1"/>
    <edgeRelation from="-58.121.42" to="-31" count="3"/>
    <edgeRelation from="45" to="-68" count="3"/>
    <edgeRelation from="-31.80.00" to="31" count="1"/>
    <edgeRelation from="-31.80.00" to="37" count="1"/>
    <edgeRelation from="-31.80.00" to="-23" count="13"/>
    <edgeRelation from="-92.180.00" to="-60" count="1"/>
  </interval>
</data>
```
