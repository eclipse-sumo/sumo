---
title: IntermodalRouting
---

# Basic Concepts

Every person may have multiple modes of transport to choose from.
Currently those are walking (which is assumed to be always possible),
riding by public transport and going by car. Intermodal routing uses
[person trips](Specification/Persons.md#persontrips) to define a
trip of a person including mode changes.

# Defining intermodal demand

To define intermodal demand use the
[`<personTrip>`-element](Specification/Persons.md#persontrips):

```xml
<routes>
  <person id="p0" depart="0">
    <personTrip from="beg" to="end"/>
  </person>
</routes>
```

The tool [randomTrips.py](Tools/Trip.md#randomtripspy) may be
used with the option **--persontrips** to generate random intermodal demand.

# Defining available cars

To allow usage of a car, either the attribute `modes="car"` or the vType of the
available car must be specified for the personTrip:

```xml
<routes>
  <vType id="typ0" vClass="passenger"/>
  <person id="p0" depart="0">
      <personTrip from="beg" to="end" vTypes="typ0"/>
  </person>
</routes>
```

# Defining public transport

In order to be included by in the public transport routing, flows (also vehicles or trips) must be defined
with a [public transport schedule](Simulation/Public_Transport.md#public_transport_schedules).

An example for a bus line and a person that may uses this line are given below:

```xml
<routes xmlns:xsi="https://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="https://sumo.dlr.de/xsd/routes_file.xsd">
    <flow id="bus" from = "beg" to ="end" line="bus" begin="0" end="1000" period="300">
                <stop busStop="busStopA" until="10"/>
                <stop busStop="busStopB" until="110"/>
                <stop busStop="busStopC" until="210"/>
    </flow>
    <person id="p0" depart="0">
        <personTrip from="beg" to="end" modes="public"/>
    </person>
</routes>
```
!!! note
    A vehicle/flow/trip must define the `line` attribute to be considered as public transport. Every vehicle with a line attribute is considered public transport and may be used for routing regardless of its capacity.

# Switching between modes

Not all places in a traffic network are suitable for transferring between walking and riding modes (i.e. parking facilities may be needed to switch from a private car to walking or public transport). The following options are provided for this purpose:

- **--persontrip.transfer.car-walk**: set a list of any of the following values: parkingAreas, ptStops, allJunctions (default *parkingAreas*)
- **--persontrip.transfer.taxi-walk**: set a list of any of the following values: ptStops, allJunctions (default *allJunctions*)
- **--persontrip.transfer.walk-taxi**: set a list of any of the following values: ptStops, allJunctions (default *allJunctions*)

The meaning of the argument values is as follows:

- **parkingAreas**: transfer only at edges with a `<parkingArea>` element
- **ptStops**: transfer only at public transport stops (i.e. `<busStop>`)
- **allJunctions**: transfer on any edge that allows the vehicle as well as pedestrians, also transfer at any junction if it connects an edge that allows the vehicle with another edge that allows pedestrians.


# Intermodal Cost Function

Generally, intermodal routing may consider multiple criteria such as
travel time, costs, reliability and convenience. Currently,
[duarouter](duarouter.md) only takes the following travel time
into account:

- Individual vehicle transport uses the standard vehicle routing
  costs. This can be influenced by loading custom weights via option **--weight-files**
- Walking uses the maximum walking speed of the person and multiplies
  this with a factor, configurable via option **--persontrip.walkfactor** {{DT_FLOAT}} (default *0.75*). The
  factor is used to account for delays due to intersections and
  pedestrian interactions
- Walking against the direction of traffic may be discouraged on edges that are shared with vehicles. This can be configured with option **--persontrip.walk-opposite-factor**  (default 1.0).
- Public transport uses the time to the next train departure (waiting time) and the difference of the *until*-times between successive stops
- [Accessing a stop from another part of the network](Simulation/Public_Transport.md#access_lanes) takes
  time in direct proportion to the straight-line distance with a
  configurable factor ([netconvert](netconvert.md) option **--railway.access-factor** {{DT_FLOAT}}). The distance can be customized in the access definition.
- The time (expected) time penalty when calling a taxi is set by option **--persontrip.taxi.waiting-time**
