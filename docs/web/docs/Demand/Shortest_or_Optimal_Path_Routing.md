---
title: Demand/Shortest or Optimal Path Routing
permalink: /Demand/Shortest_or_Optimal_Path_Routing/
---

# Introduction

[DUAROUTER](../DUAROUTER.md) can be used to import demand data
given by source and destination edges (so called *trips* and ''flows
''). Furthermore, it can be used to repair an existing *route*-file i.e.
if a given route has an unconnected edge list. To achieve dynamic user
assignment, [DUAROUTER](../DUAROUTER.md) and
[SUMO](../SUMO.md) must be called iteratively. This is described in
[Demand/Dynamic User
Assignment](../Demand/Dynamic_User_Assignment.md). Beginning with
version 0.29.0, [DUAROUTER](../DUAROUTER.md) also supports
[IntermodalRouting](../IntermodalRouting.md).

# Trip Definitions

Trip definitions that can be laid into the network may be supplied to
the router using an XML-file. The syntax of a single trip definition is:
<span class="inlxml">\<trip id="<ID>" depart="<TIME>"
from="<ORIGIN_EDGE_ID>" to="<DESTINATION_EDGE_ID>"
\[type="<VEHICLE_TYPE>"\] \[color="<COLOR>"\]/\>.

| Attribute Name | Value Type                                              | Description                                                                                                                                                                                                                                                                                                                                                                                                                                                 |
| -------------- | ------------------------------------------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| id             | id (string)                                             | The name of vehicles that will be generated using this trip definition (a running number if not given)                                                                                                                                                                                                                                                                                                                                                      |
| **depart**     | int                                                     | The departure time of the (first) vehicle which is generated using this trip definition                                                                                                                                                                                                                                                                                                                                                                     |
| from           | edge id                                                 | The name of the edge the route starts at; the edge must be a part of the used network                                                                                                                                                                                                                                                                                                                                                                       |
| to             | edge id                                                 | The name of the edge the route ends at; the edge must be a part of the used network                                                                                                                                                                                                                                                                                                                                                                         |
| via            | edge ids                                                | List of intermediate edge ids which shall be part of the route; the edges must be a part of the used network                                                                                                                                                                                                                                                                                                                                                |
| fromTaz        | district id                                             | The name of the [district](../Demand/Importing_O/D_Matrices.md#describing_the_taz) the route starts at. [TAZ edges are selected so that travel time is minimized.](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#traffic_assignement_zones_taz)                                                                                                                                                                                   |
| toTaz          | district id                                             | The name of the [district](../Demand/Importing_O/D_Matrices.md#describing_the_taz) the route ends at. [TAZ edges are selected so that travel time is minimized.](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#traffic_assignement_zones_taz)                                                                                                                                                                                     |
| color          | color                                                   | This generated vehicle's color                                                                                                                                                                                                                                                                                                                                                                                                                              |
| departLane     | int/string (≥0,"random","free","departlane")            | The lane on which the vehicle shall be inserted                                                                                                                                                                                                                                                                                                                                                                                                             |
| departPos      | float(m)/string ("random","free","random_free","base") | The position at which the vehicle shall enter the net; "free" means the point closest to the start of the departlane where it is possible to insert the vehicle. "random_free" tries forcefully to find a free random position and if that fails, places the vehicle at the next "free" position. "base" sets the vehicle's depart position to the vehicle's length + eps (eps=.1m), this means the vehicle is completely at the begin of the depart lane. |
| departSpeed    | float(m/s)/string (≥0,"random","max")                   | The speed with which the vehicle shall enter the network.                                                                                                                                                                                                                                                                                                                                                                                                   |
| arrivalLane    | int/string (≥0,"current")                               | The lane at which the vehicle shall leave the network                                                                                                                                                                                                                                                                                                                                                                                                       |
| arrivalPos     | float(m)/string (≥0<sup>(1)</sup>,"random","max")       | The position at which the vehicle shall leave the network                                                                                                                                                                                                                                                                                                                                                                                                   |
| arrivalSpeed   | float(m/s)/string (≥0,"current")                        | The speed with which the vehicle shall leave the network                                                                                                                                                                                                                                                                                                                                                                                                    |

If any trips use the <span class="inlxml">type</span> attribute, the
referenced <span class="inlxml">vType</span> element must be put into
the *trip.xml* file or into an additionally loaded *rou.xml*-file or
into an .

# Flow Definitions

Flow amounts share most of the parameter with trip definitions. The
syntax is: <span class="inlxml">\<flow id="<ID>" from="<ORIGIN_EDGE_ID>"
to="<DESTINATION_EDGE_ID>" begin="<INTERVAL_BEGIN>" end="<INTERVAL_END>"
number="<VEHICLES_TO_INSERT>" \[type="<VEHICLE_TYPE>"\]
\[color="<COLOR>"\]/\></span>. Notice the following differences: the
vehicle does not take a certain departure time as not only one vehicle
is described by this parameter, but a set of, given within the attribute
"number". The departure times are spread uniformly within the time
interval described by <INTERVAL_BEGIN> and <INTERVAL_END>. All these
three attributes must be integer values. Flow definitions can also be
embedded into an interval tag. In this case one can (but does not have
to) leave the tags <span class="inlxml">begin</span> and
<span class="inlxml">end</span> out. So the following two snipples mean
the same:

    <flows>
        <flow id="0" from="edge0" to="edge1" begin="0" end="3600" number="100"/>
    </flows>

and

    <flows>
        <interval begin="0" end="3600">
            <flow id="0" from="edge0" to="edge1" number="100"/>
        </interval>
    </flows>

Let's review flow parameter:

| Attribute Name   | Value Type                                              | Description                                                                                                                                                                                                                                                                                                                                                                                                                                                 |
| ---------------- | ------------------------------------------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **id**           | id (string)                                             | The name of vehicles that will be generated using this trip definition; vehicles and routes will be named "<id>_<RUNNING>" where <RUNNING> is a number starting at 0 and increased for each vehicle.                                                                                                                                                                                                                                                       |
| from             | edge id                                                 | The name of the edge the routes start at; the edge must be a part of the used network                                                                                                                                                                                                                                                                                                                                                                       |
| to<sup>(1)</sup> | edge id                                                 | The name of an the edge the routes end at; the edge must be a part of the used network                                                                                                                                                                                                                                                                                                                                                                      |
| via              | edge ids                                                | List of intermediate edge ids which shall be part of the route; the edges must be a part of the used network                                                                                                                                                                                                                                                                                                                                                |
| type             | type id                                                 | The type id of the vehicles to generate                                                                                                                                                                                                                                                                                                                                                                                                                     |
| **begin**        | int                                                     | The begin time for the described interval                                                                                                                                                                                                                                                                                                                                                                                                                   |
| end              | int                                                     | The end time for the interval; must be greater than <begin>; vehicles will be inserted between <begin> and <end>-1                                                                                                                                                                                                                                                                                                                                          |
| number           | int                                                     | The number of vehicles that shall be inserted during this interval                                                                                                                                                                                                                                                                                                                                                                                          |
| color            | color                                                   | Defines the color of the vehicles and their routes                                                                                                                                                                                                                                                                                                                                                                                                          |
| departLane       | int/string (≥0,"random","free","departlane")            | The lane on which the vehicle shall be inserted                                                                                                                                                                                                                                                                                                                                                                                                             |
| departPos        | float(m)/string ("random","free","random_free","base") | The position at which the vehicle shall enter the net; "free" means the point closest to the start of the departlane where it is possible to insert the vehicle. "random_free" tries forcefully to find a free random position and if that fails, places the vehicle at the next "free" position. "base" sets the vehicle's depart position to the vehicle's length + eps (eps=.1m), this means the vehicle is completely at the begin of the depart lane. |
| departSpeed      | float(m/s)/string (≥0,"random","max")                   | The speed with which the vehicle shall enter the network.                                                                                                                                                                                                                                                                                                                                                                                                   |
| arrivalLane      | int/string (≥0,"current")                               | The lane at which the vehicle shall leave the network                                                                                                                                                                                                                                                                                                                                                                                                       |
| arrivalPos       | float(m)/string (≥0<sup>(1)</sup>,"random","max")       | The position at which the vehicle shall leave the network                                                                                                                                                                                                                                                                                                                                                                                                   |
| arrivalSpeed     | float(m/s)/string (≥0,"current")                        | The speed with which the vehicle shall leave the network                                                                                                                                                                                                                                                                                                                                                                                                    |

<sup>(1)</sup> [JTRROUTER](../JTRROUTER.md) does not need this
parameter, [DUAROUTER](../DUAROUTER.md) requires it

# Custom edge weights

The option  can be used to affect the weights used during routing. The
input format conforms to the output format of the [edge based traffic
measures](../Simulation/Output/Lane-_or_Edge-based_Traffic_Measures.md).
By default this uses the **traveltime** attribute of edges. The option
can be used to use other attributes for routing. Weight files that only
contain some of the network edges are permitted. If undefined, the
traveltime of an edge defaults to the free-flow traveltime. Also, only
the *weight-attribute* must be defined:

    <meandata>
       <interval begin="0" end="3600" id="whatever">
          <edge id="edgeID1" traveltime="23"/>
          <edge id="edgeID2" traveltime="1000"/>
       </interval>
    </meandata>

# Usage Examples

`duarouter --trip-files trips.xml --net-file road.net.xml --output-file
result.rou.xml`

the file ''trips.xml '' given below must contain edges contained in the
network file ''road.net.xml ''.

```
    <trips>
        <trip id="1625993_25" depart="25" from="-15229224#3" to="4474184"/>
        <trip id="1542480_35" depart="35" from="-46771582"   to="-24038909#1"/>
        <trip id="1544282_35" depart="35" from="20554351#2"  to="-4876083#5"/>
        <trip id="1565407_35" depart="35" from="-25771472#1" to="-25160470#0"/>
        <trip id="1620115_45" depart="45" from="11279351#3"  to="5198584#1"/>
        <trip id="1647560_45" depart="45" from="54048879#0"  to="-52105434#0"/>
        <trip id="3761248_45" depart="45" from="-31928311"   to="23792451#7"/>
    </trips>
```

Trips may contain [source and destination
districts](../Demand/Importing_O/D_Matrices.md#describing_the_taz)
as well as edges. If the districts shall be used for routing this has to
be specified using the option "--with-taz". Furthermore (if the net does
not contain the districts) a districts file needs to be provided:

`duarouter --trip-files=`<TRIP_DEFS>` --net-file=`<SUMO_NET>` \`
`  --output-file=MySUMORoutes.rou.xml --taz-files=`<DISTRICTS>` --with-taz`

As we have to read in the flow definitions completely into the memory -
something we do not have to do necessarily with trips, an extra
parameter (-f or --flows) is used to make them known by the router:

`duarouter --flows=`<FLOW_DEFS>` --net=`<SUMO_NET>` \`
`  --output-file=MySUMORoutes.rou.xml -b `<UINT>` -e `<UINT>

Remind that one can not insert flow descriptions into a trip definitions
file. The opposite (some trip definitions within a flow descriptions
file) is possible. You also can give both files at the input file, for
example:

`duarouter --flows=`<FLOW_DEFS>` --trip-files=`<TRIP_DEFS>` --net=`<SUMO_NET>` \`
`  --output-file=MySUMORoutes.rou.xml -b `<UINT>` -e `<UINT>

# Dealing with Errors During Routing

When running [DUAROUTER](../DUAROUTER.md) you may encounter errors
of the type

`Error: No connection between 'edge1' and 'edge2' found`

This is caused be an unconnected network. If your network has vehicle
class restrictions it may be that the connectivity is only lacking for
particular vehicle classes which is less obvious from the GUI. You can
ignore these routes using the option . However, if a large proportion of
your routes cause this error you should definitely investigate your
network file for problems. The tool
[Tools/Net\#netcheck.py](../Tools/Net.md#netcheckpy) can be used
to pin down the connectivity gaps in your network.