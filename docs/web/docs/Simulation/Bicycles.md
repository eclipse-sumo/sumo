---
title: Bicycles
---

# Bicycle Simulation

This page describes simulations of bicycles in SUMO. To build an
intermodal simulation scenario with bicycles, additional steps have to
be take in comparison to a plain vehicular simulation.

!!! caution
    The simulation of bicycles is a developing subject and still carries some difficulties. These are discussed below.

# Approaches to bicycle modelling

Currently, no exclusive movement model for bicycles is implemented.
Existing models need to be re-purposed

## Bicycles as slow vehicles

In this case, vehicles are specified as vehicles with the appropriate
type:

```xml
<vType id="bike" vClass="bicycle"/>
<vehicle type="bike" .../>
```

Note, that that the `guiShape="bicycle"` along with [sensible default
parameters](../Vehicle_Type_Parameter_Defaults.md) are
automatically used when specifying `vClass="bicycle"`. By adapting [vType-parameters for
acceleration,deceleration,maximumSpeed,etc..](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#vehicle_types)
different cyclist types can be modelled.

## Useful Model Parameters
Once the vClass "bicycle" is chosen, the following parameters, which can still be customized, are set for bicycle:

 - minGap = 0.5 m
 - max. acceleration = 1.2 m/s^2
 - max. deceleration = 3 m/s^2
 - emergency deceleration = 7 m/s^2
 - Length = 1.6 m
 - max speed = 20 kmh where you can modify it by defining vClass specific speed limit (see the point in the Problems and worksarounds below)

The vaules of some other parameters for bicycles are different from those for vehicles apparently. If no real data for the respective calibrations is available, some intuitive suggestions are listed below for reference.

- latAlignment = "right" (cyclists ride on the right side of their lane)
- carFollowModel = IDM (if smoothed acceleration is desired)
- lcStrategic = 0.5 (in comparison to the default value (1) this setting makes bicycles to perform strategic lane changing later)
- lcCooperativeRoundabout = 0. (cyclists keep on the right lane in a multi-lane roundabout)
- lcTurnAlignmentDistance = 20. (distance to a location where bicycles start to keeping right/left of their lane for preparing to make turns)
- jmCrossingGap = 4 (Minimum distance to pedestrians that are walking towards the conflict point with a bike - lower than the default of 10 for cars)
- jmSigmaMinor = 0. (no imperfection while passing a minor link)
- jmStoplineGap = 0.5 (Stopping distance in front of prioritary / TL-controlled stop line)

### Problems and workarounds

- No bi-directional movements on bicycle lanes
- No shared space for bicycles and pedestrians by default. This can be fixed by using the [Sublane Model](../Simulation/SublaneModel.md) and defining lanes that allow bicycles and pedestrians.
- No overtaking by vehicles on a single-lane road. This can be fixed
  by using the [Sublane Model](../Simulation/SublaneModel.md).
- The intersection model has no special adaptations for bicycles. This
  results in unrealistic (large) safety gaps when bicycles are
  approaching a large priority intersection from a prioritized road
- The road speed limit is not meaningful for bicycles. This is a problem because the [default way of modelling speed distributions is by setting a random multiplier for the speed limit](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#speed_distributions). There are several possibilities remedies:
  - define multiple vehicle types with different 'maxSpeed' for the bicycle fleet. This can be done efficiently with [Tool createVehTypeDistribution](../Tools/Misc.md#createvehtypedistributionpy)
  speed distribution for bicycles there are several options
  - define a meaningful speed limit for bicycle lanes (only useful if bikes always mostly use dedicated lanes)
  - define [vClass-specific speed limits for bicycles](../Networks/PlainXML.md#vehicle-class_specific_speed_limits) on all edges where bicycles are used 

One way for overcoming most of these problems is to control bicycle
movements at intersections with an [external control
script](../TraCI.md). This approach is described in [Integration of
an external bicycle model in SUMO, Heather
Twaddle 2016](https://www.researchgate.net/publication/302909195_Integration_of_an_External_Bicycle_Model_in_SUMO).

## Bicycles as fast pedestrians

In this case, persons walking at high speed are used.

### Problems and workarounds

  - No support for proper visualization
  - Movement model is not validated

# Building a network for bicycle simulation

## Automatic import

The import of bicycle lanes from OpenStreetMap is supported since
version 0.24.0. To use this, [an appropriate
typemap](../Networks/Import/OpenStreetMap.md#recommended_typemaps)
must be loaded.

## Generating a network with bike lanes

A bike lane is a lane which only permits the vClass *bicycle*. There are various different options for generating a network with bike lanes which are explained below. All of these options recognize the presence of an existing bike lane and will not add another lane in that case. 

### Explicit specification of additional lanes

Bike lanes may be defined explicitly in plain XML input when describing edges [edges
(plain.edg.xml)](../Networks/PlainXML.md#lane-specific_definitions). This is done by defining an additional lane which only permits the vClass “bicycle” and setting the appropriate width. In this case it may be useful to disallow bicycles on other lanes. Also, any pre-exisiting connection definitions must be modified to account for the new bike lane. 

### Explicit specification of bike lanes

Alternatively to the above method, the `<edge>`-attribute [`bikeLanWidth` may be used](../Networks/PlainXML.md#edge_descriptions). It will cause a bike lane of the specified width to be added to that edge, connections to be remapped and bicycle permissions to be removed from all other lanes.

!!! note
    The heuristic methods described below, also perform automatic connection shifting and removal of bicycle permissions from non-bike lanes

### Type-base generation

When importing edges with defined types, it is also possible to declare that certain types should receive a sidewalk. This can be used to automatically generate bike lanes for residential streets while omitting them for motorways when importing OSM data. 

```xml
<types>
   <type id="highway.motorway" numLanes="3" speed="44.44" priority="13" oneway="true" disallow="pedestrian bicycle"/>
   <type id="highway.unclassified"   numLanes="1" speed="13.89" priority="5" bikeLaneWidth="1" disallow="bicycle"/>
   <type id="highway.residential"    numLanes="1" speed="13.89" priority="4" bikeLaneWidth="1" disallow="bicycle"/>
   <type id="highway.living_street"  numLanes="1" speed="2.78"  priority="3"/>
   ...
</types>
```

A special type file that imports bike lanes based on additional OSM attributes can be found in [{{SUMO}}/data/typemap/osmNetconvertBicycle.typ.xml]({{Source}}data/typemap/osmNetconvertBicycle.typ.xml). This is to be preferred for importing bike lanes from OSM as it uses more accurate data. 


### Heuristic generation

A third option which can be used if no edge types are available is a heuristic based on edge speed. It adds a bike lane for all edges within a given speed range. This is controlled by using the following options:

| Option                                 | Description                                                                        |
|----------------------------------------|------------------------------------------------------------------------------------|
| **--bikelanes.guess** {{DT_BOOL}}              | Guess bike lanes based on edge speed                                               |
| **--bikelanes.guess.max-speed** {{DT_FLOAT}}    | Add bike lanes for edges with a speed equal or below the given limit *default:13.89* |
| **--bikelanes.guess.min-speed** {{DT_FLOAT}}    | Add bike lanes for edges with a speed above the given limit *default:5.8*            |
| **--bikelanes.guess.exclude** {{DT_ID}}[,<ID>]* | Specify a list of edges that shall not receive a bike lane                         |

### Permission-based generation

Option **--bikelanes.guess.from-permissons** {{DT_BOOL}} is suitable for networks which specify their edge permissions (such as [DlrNavteq](../Networks/Import/DlrNavteq.md)). It adds a bike lane for all edges which allow bicycles on any of their lanes. The option **--bikelanes.guess.exclude** {{DT_IDList}}[,{{DT_IDList}}\]* applies here as well. 

### Adding bike lanes with [netedit](../Netedit/index.md)

To add bike lanes to a set of edges in [netedit](../Netedit/index.md) select these and right click on them. From the context-menu select *lane operations->add restricted lane->Bikelane*. 

## Notes on Right-of-Way rules

When using bicycle lanes in a network, right-turning vehicles must yield
for straight-going bicycles. The intersection model supports these
right-of-way rules and builds internal junctions where appropriate.

Likewise, left-turning bicycles one a bicycle lane (on the right side of
the road) must yield to straight-going vehicles.
    
## Indirect left turn
In reality, left-turning bicycles may move in two stages:
1. move straight across, and then 
2. turn 90° left and then move straight across

By default, [netconvert](../netconvert.md) generates a wide curve rather than going straight twice as above. Currently, this can only be remedied by setting [custom shapes for these internal lanes](../Netedit/index.md#connection). To adjust the waiting position of the bicycle (the point where the first stage ends), [connection attribute 'contPos' must be set](../Netedit/index.md#setting_connection_attributes).

To define a controlled indirect turn where both stages respect the traffic light corresponding to the current movement direction another custom setting is needed. The first part of the left-turn connection will be controlled automatically by the traffic ligh according to the 'linkIndex' attribute of the connection.
The second part can be controlled by [setting the optional attribute 'linkIndex2'](../Netedit/index.md#setting_connection_attributes). The easiest setup is to copy the linkIndex that controls the movement of vehicles (or pedestrians) going straight from right to left. In addition, parallel waiting of bicycles/motorpads in front of an intersection can be achived by activating the [Sublane-Model](../Simulation/SublaneModel.md). 
 
Sometimes, bicycles/motorpads do not wait side by side properly. The following parameters can help to solve/improve this issue.
  - latAlignment="nice"
  - minGapLat="0.1"
    
# Bicycle routing

When [routing bicycles in the simulation](../Demand/Automatic_Routing.md) the option **--device.rerouting.bike-speeds** can be used to enable separate tracking of bicycle speeds. This ensure that routing for bicycles which can use a dedicated bicycle lane is not affected by jammed cars.
