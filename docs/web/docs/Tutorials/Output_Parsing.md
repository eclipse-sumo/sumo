---
title: Output Parsing
---

In this tutorial you will learn how to

- set up an abstract network using edge types,
- create repeatedly identical vehicles using flows,
- reroute vehicles dynamically such that they drive endlessly,
- analyze output files using the [sumolib python
  library](../Tools/Sumolib.md),

and as a bonus how to

- use socket output for online evaluation and saving disk space.

Despite the keywords online, socket, and python API, this tutorial does
*not* cover any TraCI related content.

## Network setup

The goal is to build a simple network where the vehicles drive in
circles so we set up four nodes at the corners as follows
(`circular.nod.xml`):

```xml
<?xml version="1.0" encoding="UTF-8"?>

<nodes version="0.13" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="http://sumo.sf.net/xsd/nodes_file.xsd">
    <node id="bottom-left" x="0" y="0"/>
    <node id="bottom-right" x="1250" y="0"/>
    <node id="top-right" x="1250" y="1250"/>
    <node id="top-left" x="0" y="1250"/>
</nodes>
```

All edges connecting the nodes should have the same number of lanes and
the same maximum speed. In order to save on typing we define the edge
type in a separate file (`circular.typ.xml`):

```xml
<?xml version="1.0" encoding="UTF-8"?>

<types xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="http://sumo.sf.net/xsd/types_file.xsd">
    <type id="edgeType" numLanes="2" speed="36.1"/>
</types>
```

Finally we define the edges connecting the nodes (`circular.edg.xml`):

```xml
<?xml version="1.0" encoding="UTF-8"?>

<edges version="0.13" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="http://sumo.sf.net/xsd/edges_file.xsd">
    <edge from="bottom-left" id="bottom" to="bottom-right" type="edgeType"/>
    <edge from="bottom-right" id="right" to="top-right" type="edgeType"/>
    <edge from="top-right" id="top" to="top-left" type="edgeType"/>
    <edge from="top-left" id="left" to="bottom-left" type="edgeType"/>
</edges>
```

The netconvert call is very straightforward

```
netconvert -n circular.nod.xml -t circular.typ.xml -e circular.edg.xml -o circular.net.xml
```

In order to simplify the resulting net (and get the highest speed out of
the simulation), we omit turnarounds and simplify the movement over
junctions by removing the junction internal lanes. The complete
netconvert configuration file is as follows (`circular.netccfg`):

```xml
<?xml version="1.0" encoding="UTF-8"?>

<configuration xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" 
    xsi:noNamespaceSchemaLocation="http://sumo.sf.net/xsd/netconvertConfiguration.xsd">

   <input>
       <node-files value="circular.nod.xml"/>
       <edge-files value="circular.edg.xml"/>
       <type-files value="circular.typ.xml"/>
   </input>

<output>
       <output-file value="circular.net.xml"/>
</output>

   <processing>
       <no-internal-links value="true"/>
       <no-turnarounds value="true"/>
   </processing>

</configuration>
```

Call netconvert again using this configuration file:

```
netconvert -c circular.netcfg
```

Try

```
sumo-gui -n circular.net.xml
```

for a look at the final network.

## Route and flow setup

Next, we define routes and traffic flows. Open a new file called `circular.rou.xml` and insert the following vehicle type definitions for cars and trucks:

```xml
<?xml version="1.0" encoding="UTF-8"?>

<routes xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" 
  xsi:noNamespaceSchemaLocation="http://sumo.sf.net/xsd/routes_file.xsd">
    <vType accel="1.5" decel="4.5" id="car" length="5" maxSpeed="36.1"/>
    <vType accel="0.4" decel="4.5" id="truck" length="12" maxSpeed="22.2"/>
</routes>
```

For further details, see the [vehicle type attributes description](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#vehicle_types).
Since our network's edges are uni-directional, we want to define corresponding circular routes, i.e., counter-clockwise, as follows:

```xml
    <route id="routeRight" edges="bottom right top left"/>
    <route id="routeLeft" edges="top left bottom right"/>
    <route id="routeTop" edges="right top left bottom"/>
    <route id="routeBottom" edges="left bottom right top"/>
```

Add these lines just before the closing `</routes>` element. More information on defining your own routes can be found [here](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#routes).
Finally, we define traffic flows on these routes. For each route, we want a flow for cars as well as for trucks:

```xml
    <flow begin="0" departPos="free" id="carRight" period="1" number="70" route="routeRight" type="car"/>
    <flow begin="0" departPos="free" id="carTop" period="1" number="70" route="routeTop" type="car"/>
    <flow begin="0" departPos="free" id="carLeft" period="1" number="70" route="routeLeft" type="car"/>
    <flow begin="0" departPos="free" id="carBottom" period="1" number="70" route="routeBottom" type="car"/>
    <flow begin="0" departPos="free" id="truckRight" period="1" number="30" route="routeRight" type="truck"/>
    <flow begin="0" departPos="free" id="truckTop" period="1" number="30" route="routeTop" type="truck"/>
    <flow begin="0" departPos="free" id="truckLeft" period="1" number="30" route="routeLeft" type="truck"/>
    <flow begin="0" departPos="free" id="truckBottom" period="1" number="30" route="routeBottom" type="truck"/>
```

These lines can be inserted just after the route definitions. More on the attributes of the `flow` element can be found [here](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#repeated_vehicles_flows).

## Rerouters

Currently, all flows taking one of the defined routes leave the simulation as soon as they reach their route's destination.
In order to obtain endlessly driving vehicles, we need to define rerouters.
Thus, we create a file called `circular.add.xml` which defines a [rerouter](../Simulation/Rerouter.md#assigning_a_new_route) on the edges `bottom` and `top` each, such that vehicles continue on routes `routeRight` and `routeLeft`, respectively.

```xml
<?xml version="1.0" encoding="UTF-8"?>

<additional xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="http://sumo.sf.net/xsd/additional_file.xsd">
    <rerouter id="rerouterBottom" edges="bottom">
        <interval begin="0" end="100000">
            <routeProbReroute id="routeRight" />
        </interval>
    </rerouter>
    <rerouter id="rerouterTop" edges="top">
        <interval begin="0" end="100000">
            <routeProbReroute id="routeLeft" />
        </interval>
    </rerouter>
</additional>
```

## Putting it all together

With all input files (network, routes, rerouters) completed, we can create a SUMO configuration file `circular.sumocfg`:

```xml
<?xml version="1.0" encoding="UTF-8"?>

<configuration xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="http://sumo.sf.net/xsd/sumoConfiguration.xsd">
    <input>
        <net-file value="circular.net.xml"/>
        <additional-files value="circular.rou.xml,circular.add.xml"/>
    </input>

    <output>
        <netstate-dump value="dump.xml"/>
    </output>

    <time>
        <begin value="0"/>
        <end value="1000"/>
    </time>
</configuration>
```

Note that the routes file `circular.rou.xml` is declared as an additionals file here to prevent a referencing error due to the [loading order of input files](../sumo.md#loading_order_of_input_files).
Since we want to analyze the output from the simulation later on, we also define the output file `dump.xml` storing the complete network state (cf. [Output Options](../sumo.md#output) and [RawDump](../Simulation/Output/RawDump.md)).

## Analyzing the output

## Bonus: Socket communication
