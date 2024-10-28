---
title: Shortest or Optimal Path Routing
---

# Introduction

[duarouter](../duarouter.md) can be used to import demand data
given by source and destination edges (so called *trips* and ''flows
''). Furthermore, it can be used to repair an existing *route*-file i.e.
if a given route has an unconnected edge list. To achieve dynamic user
assignment, [duarouter](../duarouter.md) and
[sumo](../sumo.md) must be called iteratively. This is described in
[Demand/Dynamic User
Assignment](../Demand/Dynamic_User_Assignment.md). Beginning with
version 0.29.0, [duarouter](../duarouter.md) also supports
[IntermodalRouting](../IntermodalRouting.md).

!!! note
    The *trip* and *flow* elements described below may also be [loaded directly](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#incomplete_routes_trips_and_flows) into [sumo](../sumo.md). This takes up-to date traffic states within the network into account when computing routes but slows down the simulation.

# Trip Definitions

Trip definitions that can be laid into the network may be supplied to
the router using an XML-file. The syntax of a single trip definition is:
`<trip id="<ID>" depart="" from="<ORIGIN_EDGE_ID>" to="<DESTINATION_EDGE_ID>" [type="<VEHICLE_TYPE>"] [color="<COLOR>"]/>`.

| Attribute Name | Value Type                                              | Description                                         |
| -------------- | ------------------------------------------------------- | ------------------------------------------------------------------------------ |
| id             | id (string)                                             | The name of vehicles that will be generated using this trip definition (a running number if not given)        |
| **depart**     | int                                                     | The departure time of the (first) vehicle which is generated using this trip definition               |
| from           | edge id                                                 | The name of the edge the route starts at; the edge must be a part of the used network. Optional, if one of the via-attributes is used or if the trip includes stops.  |
| to             | edge id                                                 | The name of the edge the route ends at; the edge must be a part of the used network. Optional, if one of the via-attributes is used or if the trip includes stops.                   |
| via            | edge ids                                                | List of intermediate edge ids which shall be part of the route; the edges must be a part of the used network       |
| fromTaz        | district id                                             | The name of the [district](../Demand/Importing_O/D_Matrices.md#describing_the_taz) the route starts at. [TAZ edges are selected so that travel time is minimized.](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#traffic_assignment_zones_taz)    |
| toTaz          | district id                                             | The name of the [district](../Demand/Importing_O/D_Matrices.md#describing_the_taz) the route ends at. [TAZ edges are selected so that travel time is minimized.](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#traffic_assignment_zones_taz)       |
| type           | type id                                                 | The type id of the vehicle to generate            |
| color          | color                                                   | This generated vehicle's color              |
| departLane     | int/string (≥0,"random","free","departlane")            | The lane on which the vehicle shall be inserted         |
| departPos      | float(m)/string ("random","free","random_free","base") | The position at which the vehicle shall enter the net; "free" means the point closest to the start of the departlane where it is possible to insert the vehicle. "random_free" tries forcefully to find a free random position and if that fails, places the vehicle at the next "free" position. "base" sets the vehicle's depart position to the vehicle's length + eps (eps=.1m), this means the vehicle is completely at the begin of the depart lane. |
| departSpeed    | float(m/s)/string (≥0,"random","max")                   | The speed with which the vehicle shall enter the network.        |
| arrivalLane    | int/string (≥0,"current")                               | The lane at which the vehicle shall leave the network<br><br>**Note:** see [Definition of Vehicles, Vehicle Types, and Routes#Vehicles and Routes](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#vehicles_and_routes)          |
| arrivalPos     | float(m)/string (≥0<sup>(1)</sup>,"random","max")       | The position at which the vehicle shall leave the network<br><br>**Note:** see [Definition of Vehicles, Vehicle Types, and Routes#Vehicles and Routes](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#vehicles_and_routes)       |
| arrivalSpeed   | float(m/s)/string (≥0,"current")                        | The speed with which the vehicle shall leave the network<br><br>**Note:** see [Definition of Vehicles, Vehicle Types, and Routes#Vehicles and Routes](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#vehicles_and_routes)         |
| fromJunction | junction id  | The junction from which to depart [note](#routing_between_junctions)    |
| toJunction   | junction id  | The junction at which to arrive [note](#routing_between_junctions)    |
| viaJunctions | junction ids | The junctions to pass along the way [note](#routing_between_junctions)   |
| fromXY   | float, float    | The network position from which to depart [note](#mapmatching)    |
| toXY   | float, float    | The network position from which to depart [note](#mapmatching)    |
| viaXY   | float, float [float,float]    | The network positions to pass along the way [note](#mapmatching)   |
| fromLonLat   | float, float    | The network position from which to depart in geo-coordinates [note](#mapmatching)    |
| toLonLat   | float, float    | The network position from which to depart in geo-coordinates [note](#mapmatching)    |
| viaLonLat   | float, float [float,float]    | The network positions to pass along the way in geo-coordinates  [note](#mapmatching)   |

## Routing between Junctions
Trips and flows may use the attributes `fromJunction`, `toJunction`, and `viaJunctions` to describe origin, destination and intermediate locations. This is a special form of TAZ-routing and it must be enabled by either setting the duarouter option **--junction-taz** or by loading TAZ-definitions that use the respective junction IDs. When using option **--junction-taz**, all edges outgoing from a junction may be used at the origin and all edges incoming to a junction may be used to reach the intermediate and final junctions.

## Routing between Stops

- When defining a `<trip>` with [stop](../Definition_of_Vehicles%2C_Vehicle_Types%2C_and_Routes.md#stops_and_waypoints)-elements, routing will performed for each stop (starting a the trip origin or the prior stop) and ending at the destination.
- If at least one stop is provided, either one of the `from` or `to` attributes (or `fromJunction`, `fromTaz`, ...) may be omitted.
- If at least two stops are provided both of the `from` and `to` attributes may be omitted (the first stop serves as the origin while the last stop serves as the destination)
- If a a stop with a [`jump`](../Definition_of_Vehicles%2C_Vehicle_Types%2C_and_Routes.md#jumps)-attribute is given, the subsequent part of the route (to the next stop or the destination) will be disconnected

## Mapmatching
Since version 1.2 duarouter supports mapping positions to roads using attributes that end with 'XY' or 'LonLat'. The latter only works in networks that are geo-referenced. The maximum distance for map-matching can be configured using option **--mapmatch.distance** (since version 1.5)

By setting the option **--mapmatch.junctions**, positions are mapped to junctions instead of edges. The routes are then [computed between junctions](#routing_between_junctions).

## Vehicle Types

If any trips use the `type` attribute, the
referenced `vType` element must be put into
the input file containing the trips / flow  or into an additionally loaded *rou.xml*-file or
into an {{AdditionalFile}}.

!!! note
    By default, [duarouter](../duarouter.md) will write `vType` definitions into the output route file ahead of the first vehicle using that type. By using the option **--vtype-output** these definitions can be put into another file.

# Flow Definitions

Flow amounts share most of the parameter with trip definitions. The
syntax is: `<flow id="<ID>" from="<ORIGIN_EDGE_ID>" to="<DESTINATION_EDGE_ID>" begin="<INTERVAL_BEGIN>" end="<INTERVAL_END>" number="<VEHICLES_TO_INSERT>" [type="<VEHICLE_TYPE>"] [color="<COLOR>"]/>`. Notice the following differences: the
vehicle does not take a certain departure time as not only one vehicle
is described by this parameter, but a set of, given within the attribute
"number". The departure times are spread uniformly within the time
interval described by <INTERVAL_BEGIN\> and <INTERVAL_END\>. All these
three attributes must be integer values. Flow definitions can also be
embedded into an interval tag. In this case one can (but does not have
to) leave the tags `begin` and
`end` out. So the following two snippets mean
the same:


```xml
<routes>
    <flow id="0" from="edge0" to="edge1" begin="0" end="3600" number="100"/>
</routes>
```

and

```xml
<routes>
    <interval begin="0" end="3600">
        <flow id="0" from="edge0" to="edge1" number="100"/>
    </interval>
</routes>
```

!!! note
    The input file always needs a root level element to enclose the trip/flow elements and this should be named `<routes>`.


Let's review flow parameter:

| Attribute Name   | Value Type                                              | Description              |
| ---------------- | ------------------------------------------------------- | ------------------------ |
| **id**           | id (string)                                             | The name of vehicles that will be generated using this trip definition; vehicles and routes will be named "<id\>_<RUNNING\>" where <RUNNING\> is a number starting at 0 and increased for each vehicle.    |
| from             | edge id                                                 | The name of the edge the routes start at; the edge must be a part of the used network    |
| to<sup>(1)</sup> | edge id                                                 | The name of an the edge the routes end at; the edge must be a part of the used network   |
| via              | edge ids                                                | List of intermediate edge ids which shall be part of the route; the edges must be a part of the used network     |
| type             | type id                                                 | The type id of the vehicles to generate            |
| **begin**        | int                                                     | The begin time for the described interval         |
| end              | int                                                     | The end time for the interval; must be greater than <begin\>; vehicles will be inserted between <begin\> and <end\>-1      |
| vehsPerHour    | float(\#/h)    | number of vehicles per hour, equally spaced (not together with period or probability)        |
| period         | float(s)       | insert equally spaced vehicles at that period (not together with vehsPerHour or probability) |
| probability    | float(\[0,1\]) | probability for emitting a vehicle each second (not together with vehsPerHour or period), see also [Simulation/Randomness](../Simulation/Randomness.md#flows_with_a_random_number_of_vehicles) |
| number           | int                                                     | The number of vehicles that shall be inserted during this interval         |
| color            | color                                                   | Defines the color of the vehicles and their routes                       |
| departLane       | int/string (≥0,"random","free","departlane")            | The lane on which the vehicle shall be inserted                      |
| departPos        | float(m)/string ("random","free","random_free","base") | The position at which the vehicle shall enter the net; "free" means the point closest to the start of the departlane where it is possible to insert the vehicle. "random_free" tries forcefully to find a free random position and if that fails, places the vehicle at the next "free" position. "base" sets the vehicle's depart position to the vehicle's length + eps (eps=.1m), this means the vehicle is completely at the begin of the depart lane. |
| departSpeed      | float(m/s)/string (≥0,"random","max")                   | The speed with which the vehicle shall enter the network.         |
| arrivalLane      | int/string (≥0,"current")                               | The lane at which the vehicle shall leave the network<br><br>**Note:** see [Definition of Vehicles, Vehicle Types, and Routes#Vehicles and Routes](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#vehicles_and_routes)       |
| arrivalPos       | float(m)/string (≥0<sup>(1)</sup>,"random","max")       | The position at which the vehicle shall leave the network<br><br>**Note:** see [Definition of Vehicles, Vehicle Types, and Routes#Vehicles and Routes](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#vehicles_and_routes)          |
| arrivalSpeed     | float(m/s)/string (≥0,"current")                        | The speed with which the vehicle shall leave the network<br><br>**Note:** see [Definition of Vehicles, Vehicle Types, and Routes#Vehicles and Routes](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#vehicles_and_routes)           |

<sup>(1)</sup> [jtrrouter](../jtrrouter.md) does not need this
parameter, [duarouter](../duarouter.md) requires it

# Custom edge weights

The option **--weight-files** {{DT_FILE}} can be used to affect the weights used during routing. The
input format conforms to the output format of the [edge based traffic
measures](../Simulation/Output/Lane-_or_Edge-based_Traffic_Measures.md).
By default this uses the **traveltime** attribute of edges. The option **--weight-attribute** {{DT_STR}}
can be used to use other attributes for routing. Weight files that only
contain some of the network edges are permitted. If undefined, the
traveltime of an edge defaults to the free-flow traveltime. Also, only
the *weight-attribute* must be defined:

```xml
<meandata>
    <interval begin="0" end="3600" id="whatever">
        <edge id="edgeID1" traveltime="23"/>
        <edge id="edgeID2" traveltime="1000"/>
    </interval>
</meandata>
```

# Access restrictions

Access to a network edge is typiclly restricte by the [vehicle class](../Simulation/VehiclePermissions.md) defined in a [vehicle type](../Definition_of_Vehicles%2C_Vehicle_Types%2C_and_Routes.md#vehicle_types) but can also be customized with [numerical restrictions](../Simulation/VehiclePermissions.md#custom_access_restrictions).

# Repair routes

The **--repair** option intends to repair routes with connectivity problems.
Depending on how the route is define, duarouter will try to fix it differently:

- If the route is defined with the "edges" attribute, duarouter will first try
to fix the route by repairing the connectivity between the edges. If this is not
possible, then duarouter will search for another valid route between the edges
with connectivity problems. This may result in some edges from the original
route no longer being on the new route. In other words, edges defined with the
"edges" attribute are not taken as mandatory during route repair.
- If the route is defined with the "edges" attribute but also has "stops",
duarouter will treat the edges of the stops as mandatory. First it will try to
repair the route as in the previous example, but if duarouter cannot find a valid
route that goes through the given stop edges, then the route is not possible to fix.
- If the route is defined with the attribute "via" edges, all edges will be
treated as mandatory. If duarouter cannot fix the connectivity problems between
the specified edges, a warning appears and the route is not repairable.

Duarouter also has the **--repair.from** and **--repair.to** options, which allow
to fix an invalid starting or ending edge using the first or last usable edge
of the route.

# Converting Input Styles
SUMO supports various styles of traffic demand definitions (vehicles, trips, flows) and [duarouter](../duarouter.md) can be used to convert between them.

- By default, duarouter will convert all inputs to vehicles with embedded routes (route as child element of the vehicle).
- With option **--write-trips** all input will be converted to trips
  - combined with option **--write-trips.geo** trips will be written with geo-coordinates (fromLonLat, toLonLat) instead of edges (from, to)
  - combined with option **--write-trips.junctions** trips will be written between junctions (fromJunction, toJunction)

# Usage Examples

```
duarouter --trip-files trips.xml --net-file road.net.xml --output-file result.rou.xml
```

the file ''trips.xml '' given below must contain edges contained in the
network file ''road.net.xml ''.

```xml
<routes>
    <trip id="1625993_25" depart="25" from="-15229224#3" to="4474184"/>
    <trip id="1542480_35" depart="35" from="-46771582"   to="-24038909#1"/>
    <trip id="1544282_35" depart="35" from="20554351#2"  to="-4876083#5"/>
    <trip id="1565407_35" depart="35" from="-25771472#1" to="-25160470#0"/>
    <trip id="1620115_45" depart="45" from="11279351#3"  to="5198584#1"/>
    <trip id="1647560_45" depart="45" from="54048879#0"  to="-52105434#0"/>
    <trip id="3761248_45" depart="45" from="-31928311"   to="23792451#7"/>
</routes>
```

Trips may contain [source and destination
districts](../Demand/Importing_O/D_Matrices.md#describing_the_taz)
as well as edges. If the districts shall be used for routing this has to
be specified using the option "--with-taz". Furthermore (if the net does
not contain the districts) a districts file needs to be provided:

```
duarouter --trip-files=<TRIP_DEFS> --net-file=<SUMO_NET> \
  --output-file=MySUMORoutes.rou.xml --taz-files=<DISTRICTS> --with-taz
```

!!! caution
    The edge weights are ignored when using TAZ this way. Instead TAZ edges are selected so that the travel time is minimized.

As we have to read in the flow definitions completely into the memory -
something we do not have to do necessarily with trips, an extra
parameter (-f or --flows) is used to make them known by the router:

```
duarouter --flows=<FLOW_DEFS> --net=<SUMO_NET> \
  --output-file=MySUMORoutes.rou.xml -b <UINT> -e <UINT>
```

Remind that one can not insert flow descriptions into a trip definitions
file. The opposite (some trip definitions within a flow descriptions
file) is possible. You also can give both files at the input file, for
example:

```
duarouter --flows=<FLOW_DEFS> --trip-files=<TRIP_DEFS> --net=<SUMO_NET> \
  --output-file=MySUMORoutes.rou.xml -b <UINT> -e <UINT>
```

# Dealing with Errors During Routing

When running [duarouter](../duarouter.md) you may encounter errors
of the type

```
Error: No connection between 'edge1' and 'edge2' found
```

This is caused by an unconnected network. If your network has vehicle
class restrictions it may be that the connectivity is only lacking for
particular vehicle classes which is less obvious from the GUI. You can
ignore these routes using the option **--ignore-errors**. However, if a large proportion of
your routes cause this error you should definitely investigate your
network file for problems. The tool
[Tools/Net#netcheckpy](../Tools/Net.md#netcheckpy) can be used
to pin down the connectivity gaps in your network.
