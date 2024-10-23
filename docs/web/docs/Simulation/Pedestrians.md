---
title: Pedestrians
---

# Pedestrian Simulation

This page describes simulations of pedestrians in SUMO. Pedestrians are
[persons that walk](../Specification/Persons.md). To build an
intermodal simulation scenario with proper interactions between road
vehicles and pedestrians, additional steps have to be taken in comparison
to a plain vehicular simulation. When using multiple modes all edges and
lanes need to have the [correct permissions to separate the different modes of traffic](../Simulation/VehiclePermissions.md).

Edges that allow only pedestrian traffic are called **footpaths** in the
following. For edges that allow both modes, typically the rightmost lane
(the one with index 0) allows only pedestrians and the other lanes
disallow pedestrians. The rightmost lane is then called a **sidewalk**.

![pedestrian_network](../images/pedestrian_network.png "elements of a pedestrian network")

# Building a network for pedestrian simulation

When walking along an edge, pedestrians use sidewalks where available. A
sidewalk is a lane which allows only the sumo `vClass` *pedestrian*. When
crossing a road at an intersection, pedestrians use special lanes of
type *crossing*. The area that connects sidewalks with crossings is
modeled by special lanes of the type *walkingarea*. In the following, we
describe how to build a simulation network that *contains sidewalks*,
*crossings* and *walkingareas*.

When performing pedestrian routing, the router distinguishes between
networks that contain *walkingareas* and those which do not. In the
former, pedestrians may only cross a street whenever there is a
pedestrian crossing. In the latter they may "jump" between any two edges
which allow pedestrians at an intersection.

!!! note
    Almost all of the methods described below can be used for building a pedestrian network either based on an existing *.net.xml* file or while doing the initial import (i.e. from [OSM](../Networks/Import/OpenStreetMap.md)). The exception is [type based generation](#type-based_generation) which can only be done during import.

## Generating a network with sidewalks

A sidewalk is a lane which only permits the vClass *pedestrian*. There
are various different options for generating a network with sidewalks
which are explained below.

!!! caution
    The current pedestrian models assume that each simulation edge has at most one sidewalk. In order to have sidewalks at both sides of a one-way street, a simulation edge in the reverse direction (which only allows pedestrians) must be added.

!!! note
    The methods described below, perform checks to prevent generating duplicate sidewalks. If the rightmost lane is recognized as a sidewalk (it only permits vClass *pedestrian*) no additional sidewalk will be added.

### Explicit specification of additional lanes

Sidewalks may be defined explicitly in plain XML input when describing
[edges (plain.edg.xml)](../Networks/PlainXML.md#lane-specific_definitions).
This is done by defining an additional lane which only permits the
vClass “pedestrian” and setting the appropriate width. In this case it
is important to disallow pedestrians on all other lanes. Also, any
pre-existing connection definitions must be modified to account for the
new sidewalk lane.

### Explicit specification of sidewalks

Alternatively to the above method, the `<edge>`-attribute
[sidewalkWidth may be used](../Networks/PlainXML.md#edge_descriptions) when loading edges from an *.edg.xml*-file.
It will cause a sidewalk of the specified width to be added to that
edge, connections to be remapped and pedestrian permissions to be
removed from all other lanes.

!!! note
    The heuristic methods described below, also perform automatic connection shifting and removal of pedestrian permissions from non-sidewalk lanes.

### Direct Import

When importing [OSM](../Networks/Import/OpenStreetMap.md), the option **--osm.sidewalks** may be used to import sidewalks for all roads that carry this information.

### Type-based generation

When importing edges with defined types, it is also possible to declare
that certain types should receive a sidewalk. This can be used to
automatically generate sidewalks for residential streets while omitting
them for motorways when importing OSM data. An example type file can be
found in [{{SUMO}}/data/typemap/osmNetconvertPedestrians.typ.xml]({{Source}}data/typemap/osmNetconvertPedestrians.typ.xml).

```xml
<types>
  <type id="highway.motorway" numLanes="3" speed="44.44" priority="13" oneway="true" disallow="pedestrian bicycle"/>
  <type id="highway.unclassified"   numLanes="1" speed="13.89" priority="5" sidewalkWidth="2" disallow="pedestrian"/>
  <type id="highway.residential"    numLanes="1" speed="13.89" priority="4" sidewalkWidth="2" disallow="pedestrian"/>
  <type id="highway.living_street"  numLanes="1" speed="2.78"  priority="3"/>
  <type id="highway.service"        numLanes="1" speed="5.56"  priority="2" allow="delivery pedestrian"/>
  ...
</types>
```

### Heuristic generation

A third option which can be used if no edge types are available is a
heuristic based on edge speed. It adds a sidewalk for all edges within a
given speed range. This is controlled by using the following [netconvert](../netconvert.md) options:

| Option                                 | Description                                                                       |
|----------------------------------------|-----------------------------------------------------------------------------------|
| **--sidewalks.guess** {{DT_BOOL}}               | Guess pedestrian sidewalks based on edge speed                                    |
| **--sidewalks.guess.max-speed** {{DT_FLOAT}}    | Add sidewalks for edges with a speed equal or below the given limit *default:13.89* |
| **--sidewalks.guess.min-speed** {{DT_FLOAT}}    | Add sidewalks for edges with a speed above the given limit *default:5.8*            |
| **--sidewalks.guess.exclude** {{DT_IDList}}[,{{DT_IDList}}\]\* | Specify a list of edges that shall not receive a sidewalk                         |

### Permission-based generation

Option **--sidewalks.guess.from-permissions** {{DT_BOOL}} is suitable for networks which specify their edge permissions
(such as [DlrNavteq](../Networks/Import/DlrNavteq.md)). It adds a
sidewalk for all edges which allow pedestrians on any of their lanes.
The option **--sidewalks.guess.exclude** {{DT_IDList}}[,{{DT_IDList}}\]\* applies here as well.

### Adding sidewalks with [netedit](../Netedit/index.md)

To add sidewalks to a set of edges in [netedit](../Netedit/index.md)
select these and right click on them. From the context-menu select *lane operations->add restricted lane->Sidewalks*.

### Non-exclusive sidewalks

The following rules are used to determine the sidewalk lane in case of
ambiguities:

- if there are multiple lanes with permission *pedestrian* the
  rightmost one is used
- if there are lanes that allow only pedestrians and lanes that allow
  pedestrians along with other vehicle classes, the rightmost lane
  that **only** allows pedestrians is used
- if there are no lanes that allow only pedestrians but some lanes
  that allow pedestrians along with other vehicle classes the
  rightmost of those lanes is used

## Generating a network with crossings and walkingareas

In order to be able to cross a street at all, crossing places must
defined at junctions in the network. These places model the following
three situations:

- crossing the street with priority (zebra crossing)
- crossing the street without priority
- crossing the street at a traffic light

In order to be eligible for a crossing there must be sidewalks or
footpaths which are separated by one or more lanes which forbid
pedestrian traffic.

!!! note
    If crossing should be possible between two far-away junctions, additional junctions must be inserted by splitting the edge(s) between the junctions and defining a crossing at the inserted junctions.

Crossings may be defined explicitly in plain XML input when describing
[connections (plain.con.xml) using the XML element `crossings`](../Networks/PlainXML.md#pedestrian_crossings).
They can also be placed with [netedit](../Netedit/editModesNetwork.md#crossings).

The second available method for adding crossing information to a network
is with the [netconvert](../netconvert.md) option **--crossings.guess** {{DT_BOOL}}. This enables a heuristic which adds crossings
wherever sidewalks with similar angle are separated by lanes which
forbid pedestrians. If the edges to be crossed have sufficient distance
between them or vary a by a sufficient angle, two crossings with an
intermediate walking area are generated. For roads with a speed limit
beyond 50km/h, crossings are only generated at tls-controlled
intersections. This threshold can be set with the option **crossings.guess.speed-threshold**. To
successfully generate crossings with this option [sidewalks should be defined](../Simulation/Pedestrians.md#generating_a_network_with_sidewalks)
for the network.

!!! caution
    To ensure proper generation of crossings, road lanes need to prohibit pedestrians either by setting `disallow="pedestrian"` or by explicitly specifying all other allowed classes using attribute `allow` When adding sidewalks via attribute `sidewalkWidth` or any of the heuristics above, pedestrians will be forbidden automatically on the remaining lanes.

### walkingareas

At every junction corner multiple sidewalks footpaths and pedestrian
crossings may meet. The connectivity between these elements is modeled
with a *walkingarea*. These walkingareas are generated automatically as
long as the [netconvert](../netconvert.md) option **--crossings.guess** is set or there is at least one user-defined
crossing in the network. If no crossings are wanted in the network it is
also possible to enable the creation of walkingareas by setting the
option **--walkingareas**.

!!! note
    It is not necessary to define connections between sidewalks or footpaths in a *.con.xml* file. Instead the connections to and from walkingareas are generated automatically along with the walkingareas.

Example: If multiple footpaths meet at an intersection they will be
automatically connected by a single walkingarea which allows
connectivity in all directions.

!!! caution
    If pedestrians are simulated in a network without walkingareas, they will assume full-connectivity at every junction.

## Shared space

If persons are not restricted from walking on the roads (i.e. by not defining any sidewalks and keeping roads at their default permissions), the network models shared space. Each junction approached by shared space roads will be modelled as a single *walkingarea* that covers the whole junction.
Thus, shared space simulations are indicated in [sumo-gui](../sumo-gui.md) by having grey rather than black junction areas.

Cars will interact by pedestrians (by slowing down or stopping) when encountering them on a shared road lane or when passing a walkingarea that is used by pedestrians. Likewise, pedestrians will take some care to avoid walking into vehicles.

## Short overview

| Input                                           | Desired output                           | Options to use               |
|-------------------------------------------------|------------------------------------------|------------------------------|
| shapefile or other data without pedestrian info | no pedestrian infrastructure             | none                         |
|                                                 | sidewalks and crossings where applicable | **--sidewalks.guess --crossings.guess** |
| OpenStreetMap                                   | no pedestrian infrastructure             | none (use a typemap without footpaths) |
|                                                 | sidewalks and crossings as in the input  | **--osm.sidewalks --osm.crossings** |
|                                                 | guessed sidewalks and crossings (discarding input) | a typemap which gives a sidewalk width to all street types which should receive a side walk or **--sidewalks.guess --crossings.guess** |

The options above only apply to adding further lanes for existing streets. Separate foot paths are always imported
(if the typemap or other filter options do not prevent it).

# Generating pedestrian demand

## Explicit

Pedestrian demand may be specified explicitly as described at
[Specification/Persons\#Walks](../Specification/Persons.md#walks)
or it may be generated. Since version 1.2 [pedestrian flows](../Specification/Persons.md#repeated_persons_personflows)
are directly supported. In older versions, the tool
{{SUMO}}**/tools/pedestrianFlow.py** may also be used.

## Random

The tool
[Tools/Trip\#randomTrips.py](../Tools/Trip.md#randomtripspy)
supports generating random pedestrian demand using the option **--pedestrians**. The
option **--max-dist** {{DT_FLOAT}} may be used to limit the maximum air distance of pedestrian
walks.

Pedestrian demand may also arise from [intermodal routing](../IntermodalRouting.md). One way to obtain such a
scenario is with
[Tools/Trip\#randomTrips.py](../Tools/Trip.md#randomtripspy) using
the option **--persontrips**.

## OD-Based

The [od2trips](../od2trips.md) application generates pedestrian
demand when setting the option **--pedestrians**. Alternative it generates intermodal
demand by setting option **--persontrips**.

## From local counting data

The tools [routeSampler](../Tools/Turns.md#routesamplerpy) and [flowrouter](../Tools/Detector.md#flowrouterpy) both support the option **--pedestrians** to generate pedestrians instead of vehicular traffic based on counting data. See also [Choosing the right tool](../Demand/Routes_from_Observation_Points.md#choosing_the_right_tool).

# Pedestrian-related Attributes
See [Person attributes](../Specification/Persons.md#available_vtype_attributes)

# Pedestrian Models

The pedestrian model to use can be selected by using the simulation
option **--pedestrian.model** {{DT_STR}} with the available parameters being *nonInteracting* and
*striping* or *jupedsim* (default is *striping*). The interface between the pedestrian
model and the rest of the simulation was designed with the aim of having
a high degree of freedom when implementing new models.

## Model *nonInteracting*

This is a very basic walking model. Pedestrians walk bidirectionally
along normal edges and “jump” across intersections. They may either be
configured to complete a walk in a fixed amount of time or to move along
the edges with a fixed speed. No interaction between pedestrians and
vehicles or other pedestrians takes place. This model has a very high
execution speed and may be useful if the pedestrian dynamics are not
important.

## Model *striping*

This model assigns 2D-coordinates within a lane (of type sidewalk,
walkingarea or crossing) to each pedestrian. These coordinates which are
defined relative to the leftmost side of the start of the lane are
updated in every simulation step. This is in contrast to the coordinates
of vehicles, which (generally) only have 1D-coordinates within their
respective lane (except the case that the [sublane model](../Simulation/SublaneModel.md) is used). Pedestrians advance
along a lane towards the next node which may either correspond to the
natural direction of the lane (forward movement) or it may opposite to
the natural direction (backward movement). Thus, the x coordinate
monotonically increases or decreases while on a lane. Once the end of a
lane has been reached, the pedestrian is placed on the next lane (which
may either be unique or determined dynamically with a routing
algorithm).

The most important feature of pedestrian interactions is collision
avoidance. To achieve this, the “striping”-model divides the lateral
width of a lane into discrete stripes of fixed width. This width is user
configurable using the option **--pedestrian.striping.stripe-width** {{DT_FLOAT}} and defaults to 0.65 m and
doesn't have to match the lateral resolution of the sublane model. These stripes are
similar to lanes of a multi-lane road. Collision avoidance is thus
reduced to maintaining sufficient distance within the same stripe.
Whenever a pedestrian comes too close to another pedestrian within the
same stripe it moves in the y-direction (laterally) as well as in the
x-direction to change to a different stripe. The y-coordinate changes
continuously which leads to situations in which a pedestrian temporarily
occupies two stripes and thus needs to ensure sufficient distances in
both. The algorithm for selecting the preferred stripe is based on the
direction of movement (preferring evasion to the right for oncoming
pedestrians) and the expected distance the pedestrian will be able to
walk in that stripe without a collision. The model assumes that the pedestrian
can fit into a single strip when walking in its center. When **--pedestrian.striping-width** {{DT_FLOAT}}
is lower than a given path width, 100% safety is not guaranteed on shared lanes, i.e. collisions may occur.
The warning to change the stripe-width will then be shown during simulation.

During every simulation step, each pedestrian advances as fast as
possible while still avoiding collisions. The updates happen in a single
pass for each walking direction with the pedestrian in the front being
updated first and then its followers sorted by their x-coordinate. The
speed in the x-direction may be reduced by a random amount with the
maximum amount defined as a fraction of the maximum speed, using the
global option **--pedestrian.striping.dawdling** {{DT_FLOAT}} (defaulting to 0.2). As a consequence of the above
movement rules, pedestrians tend to walk side by side on sidewalks of
sufficient width. They wait in front of crossings in a wide queue and
they form a jam if the inflow into a lane is larger than its outflow.

More complicated movement rules apply when moving on a *walkingarea*.
Here, pedestrians paths cross in multiple directions. The actual path
taken across the walkingarea consists of 1-3 linear segments and is
unique for each pair of adjacent sidewalks or crossings. The pedestrians
on each path of these paths compute their movements as if they were on a
sidewalk. However, all other pedestrians are mapped into the coordinate
system of that path in order achieve collision avoidance.

### Jamming

There are several situations in which pedestrian jams are possible

- high traffic from different directions approaching the same walkingarea
- interaction with vehicles on shared space
- trying to reach a busStop when it has reached its `personCapacity` limit
- oncoming traffic on narrow sidewalks

There are several mitigations to prevent the frequency of jams and to resolve them after they have occurred

- on crossings and walkingareas, pedestrians reserve 1/3 of the road space for oncoming traffic (configurable with option **--pedestrian.striping.reserve-oncoming.junctions**)
- on normal lanes / sidewalks, such a reservation can be activated with option **--pedestrian.striping.reserve-oncoming**
- if a pedestrian was unable to move for 300s (configurable with option **--pedestrian.striping.jamtime**) he goes into a 'jammed' state accompanied by the warning "Person ... is jammed ...". In this state the person starts moving regardless of obstacles at 1/4 of its maximum speed. While jammed, no pedestrian collisions are registered. The jammed state ends as soon as there as there are no more obstacles in front of the person.
- while on a crossing the time to register as jammed is reduced to 10s (configurable with **--pedestrian.striping.jamtime.crossing**)
- while on a network elements that online permits a single pedestrian abreast the time time to jammed is set to 1s (configurable with option **pedestrian.striping.jamtime.narrow**)

## Model *jupedsim*

JuPedSim is a pedestrian simulator developed by the Jülich Research Center. It is based on advanced social force models and has been coupled to SUMO. The coupling is a work in progress and information will be regularly updated as needed. For more information on JuPedSim itself, please have a look at [this page](https://github.com/PedestrianDynamics/jupedsim).

### Installation

If you are on Windows, the latest release of JuPedSim is distributed together with the SUMO software distribution so no additional steps are required. Alternatively, if you want to build the latest JuPedSim version available on Windows, please have a look at [the build instructions](../Installing/Windows_Build.md#how_to_build_jupedsim_and_then_build_sumo_with_jupedsim). If you are on Linux or MacOS, you need to [build JuPedSim, then build SUMO](../Installing/Linux_Build.md#how_to_build_jupedsim_and_then_build_sumo_with_jupedsim).

### Use

To activate JuPedSim, either set option **--pedestrian.model jupedsim** or add the following line to your SUMO configuration file:

``` xml
<pedestrian.model value="jupedsim"/>
```

As with pedestrians in general in SUMO, you also need to add at least one pedestrian to your SUMO scenario (routes file) to activate the model. JuPedSim offers several pedestrian sub-models based on the social force model. However for the moment only one is used by SUMO and its parameters aren't fully exposed.

When the SUMO network is loaded, all pedestrian lanes, walking areas and crossings are collected and a geometric algorithm creates large polygons that correspond to the connected components of the pedestrian network. This process is called _geometry generation_. For the moment SUMO only supports one connected component; the one with the largest area will be the one retained for the simulation in case your SUMO pedestrian network had several connected components.

In addition to this geometry created by SUMO for JuPedSim, additional polygons can be used in the form of a SUMO additional file (see [here](../sumo.md#format_of_additional_files)). These polygons need to be *simple* (non self-intersecting). The polygons that are considered as walkable areas must have the type `jupedsim.walkable_area` whereas the ones considered as obstacles must have the type `jupedsim.obstacle`. To illustrate this, here is a toy example:

``` xml
<additional xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="http://sumo.dlr.de/xsd/additional_file.xsd">
    <poly id="walkable_area_22F" type="jupedsim.walkable_area" color="179,217,255" fill="True" layer="0" shape="32.00,46.00 32.00,36.00 42.00,36.00 42.00,46.00 62.00,46.00 62.00,56.00 32.00,56.00 32.00,46.00"/>
    <poly id="walkable_area_230" type="jupedsim.walkable_area" color="179,217,255" fill="True" layer="0" shape="60.00,60.00 60.00,20.00 100.00,20.00 100.00,60.00 60.00,60.00"/>
    <poly id="obstacle_233" type="jupedsim.obstacle" color="255,204,204" fill="True" layer="1" shape="70.00,30.00 90.00,30.00 90.00,50.00 70.00,50.00 70.00,30.00"/>
    <poly id="obstacle_48F" type="jupedsim.obstacle" color="255,204,204" fill="True" layer="1" shape="63.21,35.00 62.82,37.34 61.69,39.43 59.94,41.04 57.77,41.99 55.40,42.19 53.10,41.60 51.12,40.31 49.66,38.43 48.89,36.19 48.89,33.81 49.66,31.57 51.12,29.69 53.10,28.40 55.40,27.81 57.77,28.01 59.94,28.96 61.69,30.57 62.82,32.66 63.21,35.00"/>
</additional>
```

Because for the moment SUMO supports only one connected component, these polygons need to intersect the SUMO pedestrian network, otherwise if their area is less than the total area of the SUMO pedestrian network, they won't be taken into account in the simulation. Please also note that the polygons need to have coordinates in the same coordinate system used for the SUMO network. It is possible to generate such an additional file from a DXF file using the tool `dxf2jupedsim`, here is an example of calling the tool:

``` bash
python %SUMO_HOME%/tools/import/dxf/dxf2jupedsim.py polygons.dxf
```

The DXF file should be only composed of two layers, one for the walkable areas, one for the obstacles. The default names for these layers are `walkable_areas` and `obstacles`, respectively, but custom names can be provided by using the options `--walkable-layer` and `--obstacle-layer` of the script. The additional file generated by the script then needs to be added to your SUMO configuration file. For the moment, only POLYLINE and CIRCLE DXF entities can be converted.

If you want to visualize the geometry of the pedestrian network that is created for JuPedSim, please use the options `--pedestrian.jupedsim.wkt` followed by some filename, as well as the boolean `--pedestrian.jupedsim.wkt.geo` to force or not the use of geocoordinates.

In JuPedSim, pedestrians have a radius. At that time, the radius is computed as half the maximum between the length and the width of the pedestrian, both quantities can be set in a custom `vType` if desired. By changing the `desiredMaxSpeed` attribute for any `vType` in your route file, you can customize the "free flow" speed of your pedestrians.

### Jamming

Sometimes pedestrians get jammed at the front of a gate, which represents the last waypoint into their journey. In this case you can remove the pedestrians by creating a special polygon type named `jupedsim.vanishing_area`. The attribute `period` of this polygon defines the rate at which a pedestrian will be removed in this special polygon (a pedestrian will be removed every period, the period being given in seconds).

### Speed modification

In order to modify the speed of agents depending on location it is possible to define a special polygon type named `jupedsim.influencer`. The attribute `speed` of this polygon defines the new maximum speed of any agent entering the polygon. Be aware that currently the speed change is permanent (it will persist even if the agent leaves the polygon). This might change in future versions. Furthermore the functionality will probably be adapted to allow the change of other parameters as well.


# Pedestrian Routing

If the user does not specify a complete list of edges for walking,
pedestrian will be routed according to the fastest path along the road
edges. This is usually also the shortest path since edges typically have
no speed restraints that are relevant to pedestrians. This type of
routing may happen before the simulation starts (using
[duarouter](../duarouter.md)) or it may be done at insertion time
when loading only origin and destination into the simulation. Routes may
later be influenced via [TraCI](../TraCI/Change_Person_State.md).

Since persons may use each edge in both directions some extra configuration may be needed in shared space scenarios.
The option **--persontrip.walk-opposite-factor** may be use with [duarouter](../duarouter.md) and [sumo](../sumo.md) to set a factor when walking against traffic flow on shared space. A factor below 1 discourages walking against the flow.

When using the *striping*-model, pedestrians will also be routed within
intersections to selected a sequence of *walkingareas* and *crossings*
for moving onto the other side of the intersection. This type of routing
takes the shortest part but avoids red lights if possible. The routing
behavior within intersections cannot currently be influenced.

When using the *jupedsim*-model, pedestrians are routed through the generated geometry
according to the shortest path in a Delaunay triangulation of this geometry. In addition, _waypoints_ can be created anywhere in the pedestrian network, to make sure that
pedestrians follow a particular route, including inside the additional walkable areas. To create these waypoints, please use the edge mode of
NetEdit and click on "create default edge short" (at the top of the left panel). You can then use these edges as usual in the demand mode.

<img src="../images/pedestrian_network_jupedsim.png" width="500" title="walkable areas, obstacles and waypoints with JuPedSim">

# Interaction between pedestrians and other modes

A pedestrian wishing to cross the street at an uncontrolled intersection
can only do so if its expected time slot for using the intersection does
not interfere with that of an approaching vehicle. It should be noted
that the dynamics at unprioritized crossings are conservative in
estimating the time required gap. In the simulation, pedestrians will
only use such a crossing if the whole length of the crossing is free of
vehicles for the whole time needed to cross. At priority crossings,
pedestrians cross without regard for vehicles.

Vehicles are prevented from driving across a pedestrian crossing which
is occupied by pedestrians. If a pedestrian is found which is not yet
past the intersection point (between the crossing and the vehicles
trajectory) but within a threshold distance to that point (configurable
via [junction parameter jmCrossingGap](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#junction_model_parameters))
the crossing is considered to be blocked.

When vehicles and pedestrians move on the same lane, they will do their
best to avoid a collision. If a vehicle occupies the whole width of the
lane and gets too close to a pedestrian, the pedestrian may briefly move
to the side of the lane in order to let the vehicle pass.

!!! note
    Using the model 'nonInteracting', no interactions between pedestrians and other modes take place.

# Pedestrian Outputs

Pedestrians are included in the following outputs:

- [tripinfo-output](../Simulation/Output/TripInfo.md)
- [vehroute-output](../Simulation/Output/VehRoutes.md)
- [fcd-output](../Simulation/Output/FCDOutput.md)
- [netstate-dump](../Simulation/Output/RawDump.md)
- [aggregated simulation statistics](../Simulation/Output/index.md#aggregated_traffic_measures)

## Detectors for Pedestrians

The detector attribute 'detectPersons' for configuring output of pedestrians and riding persons can be used with the following detectors:

- [Inductive loop detectors (E1)](../Simulation/Output/Induction_Loops_Detectors_(E1).md)
- [Lane area detectors (E2)](../Simulation/Output/Lanearea_Detectors_(E2).md) (only single-lane detectors are supported)
- [Multi-Entry-Exit detectors (E3)](../Simulation/Output/Multi-Entry-Exit_Detectors_(E3).md)

If the attribute 'detectPersons' is used, all detector output values will pertain to persons (i.e. 'nVehContrib').
The attribute 'detectPersons' supports the following values:

- walk : detect pedestrians in any direction
- walkForward : detect pedestrians walking in edge direction
- walkBackward : detect pedestrians walking against edge direction
- bicycle : detect persons riding a bicycle
- public : detect persons riding public transport (bus or any rail vehicle)
- taxi : detect persons riding taxi
- car : detect persons riding any other vehicle


# Overview on networks and model behavior

This section gives an overview on how the different network types with respect to pedestrian infrastructure
can be generated which use cases they have and what is to be expected concerning the model behaviors.

## pedestrians forbidden

- Use case: Vehicle only network
- Generated by: Car-only network from osmWebWizard; OSM import with a typemap explicitly discarding all pedestrian paths
- Pedestrian routing: always fails since no usable edges are present
- Striping model: won't work, no usable edges
- Non-interacting model: won't work, no usable edges

## No dedicated infrastructure

- Use case: Generic networks with universal permissions
- Generated by: netgenerate; shapefile import; everything which does not set permissions / add infrastructure explicitly
- Pedestrian routing: Pedestrians use all edges in both directions, junctions are assumed to provide full connectivity (regardless of existing or non-existing connections)
- Striping model: Persons walk on the street and interact with vehicles but jump over junctions
- Non-interacting model: Persons walk beside the street and jump over junctions

## Only sidewalks but no walking areas or crossings

- Use case: Rather exceptional
- Generated by: Everything which adds sidewalks either via a typemap or by using netedit without using **--crossings.guess**
- Pedestrian routing: Pedestrians use all allowed edges (usually the ones with sidewalks) in both directions, junctions are assumed to provide full connectivity (regardless of existing or non-existing connections)
- Striping model: Persons walk on the sidewalk or shared lanes and interact with each other but jump over junctions
- Non-interacting model: Persons walk on the sidewalk and jump over junctions

You could think of other subtypes here which have only walking areas and no crossings or the other way round but this would make things too complicated.
The one property which distinguishes these networks from the next type is the presence of at least one walking area.

## Full pedestrian infrastructure

- Use case: Everything where you need to know where pedestrians walk and cross the street
- Generated by: osmWebWizard (if pedestrian demand is selected) or other processes adding sidewalks and using **--crossings.guess**
- Pedestrian routing: Pedestrians use all allowed edges (usually the ones with sidewalks) in both directions, junction connectivity is determined from junctions and walking areas
- Striping model: Persons walk on the sidewalk or shared lanes, interact with each other and use the crossings and walking areas just like the sidewalks
- Non-interacting model: Persons walk on the sidewalk, crossings and walking areas but without interaction
