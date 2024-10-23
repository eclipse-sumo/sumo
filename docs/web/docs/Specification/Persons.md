---
title: Persons
---

# Persons

A person moves through the net by walking or [using
vehicles](../Simulation/Public_Transport.md). The walking behavior
is customizable by selecting a [pedestrian
model](../Simulation/Pedestrians.md#pedestrian_models). A person
element has child elements defining stages of its plan. The stages are a
connected sequence of [ride](#rides),
[walk](#walks) and
[stop](#stops) elements as described
below. Each person must have at least one stage in its plan.

```xml
<person id="foo" depart="0">
    <walk edges="a b c"/>
    <ride from="c" to="d" lines="busline1"/>
    <ride .../>
    <walk .../>
    <stop .../>
</person>
```

## Available Person Attributes

| Attribute           | Type      | Range              | Default         | Remark      |
|---------------------|-----------|--------------------|-----------------|---------------------------|
| id                  | string    | valid XML ids      | -               |                          |
| depart              | float (s) or [human-readable-time](../Other/Glossary.md#t) or *triggered* | ≥0 or 'triggered'  | -               | See [below](#starting_the_simulation_in_a_vehicle) for an explanation of 'triggered'|
| departPos           | float(s)  | ≥0                 | -               | the distance along the departure edge where the person is created      |
| type                | string    | any declared vType | DEFAULT_PEDTYPE | the type should have vClass pedestrian              |
| speedFactor         | float     | > 0                | 1.0 | Sets custom speedFactor (factor on desiredMaxSpeed of vType) and overrides the speedFactor distribution of the vType |
| color               | [RGB-color](../Definition_of_Vehicles%2C_Vehicle_Types%2C_and_Routes.md#colors) | | "1,1,0" (yellow)    | This person color       |


## Available vType Attributes

| Attribute           | Type      | Range              | Default         | Remark      |
|---------------------|-----------|--------------------|-----------------|---------------------------|
| width               | float (m) | ≥0                 | 0.48            | The person's width [m]        |
| length              | float (m) | ≥0                 | 0.21            | The person's netto-length (length) [m]       |
| height              | float (m) | ≥0                 | 1.72            | The person's height [m]      |
| mingap              | float (s) | ≥0                 | 0.25            | Empty space after leader [m]                |
| maxSpeed            | float (s) | ≥0                 | 10.44           | The person's absolute maximum velocity [m/s]             |
| desiredMaxSpeed     | float (s) | ≥0                 | 1.39            | The person's desired maximum velocity [m/s]             |
| speedFactor         | float or [distribution spec](../Definition_of_Vehicles%2C_Vehicle_Types%2C_and_Routes.md#defining_a_normal_distribution_for_vehicle_speeds) | >0 | 1.0 | The persons expected multiplier for desiredMaxSpeed   |
| speedDev          | float                 | ≥0      | 0.1      | The deviation of the speedFactor distribution |
| color             | [RGB-color](../Definition_of_Vehicles%2C_Vehicle_Types%2C_and_Routes.md#colors)  |          | "1,1,0" (yellow)    | This person type's color       |
| jmDriveAfterRedTime | float (s)           | ≥0       | -1       | This value causes persons to violate a red light if the duration of the red phase is lower than the given threshold. When set to 0, persons will always walk at yellow but will try to stop at red. If this behavior causes a person to walk so fast that stopping is not possible any more it will not attempt to stop. |
| jmIgnoreFoeProb        | float | \[0,1\]             | 0          | This value causes vehicles and pedestrians to ignore foe vehicles that have right-of-way with the given probability. The check is performed anew every simulation step.|
| jmIgnoreFoeSpeed       | float (m/s) | ≥0            | 0          | This value is used in conjunction with *jmIgnoreFoeProb*. Only vehicles with a speed below or equal to the given value may be ignored. |
| impatience         | float or 'off'       | <= 1     | 0.0      | Willingness of persons to walk across the street at an unprioritized crossing when there are vehicles that would have to brake  |
| vClass            | class (enum) |        |          | "pedestrian" | Should either be "pedestrian" or "ignoring" (to allow walking anywhere) |

!!! note
    Up to version 1.14.1, speed distributions for walking persons (pedestrians) worked differently from those for vehicles. Whereas the individual speed factor of vehicles is multiplied with the road speed limit to arrive at the desired speed, the individual speed factor of persons was multiplied with the maxSpeed of their vType (since road speed limits do not apply to persons). In later versions, person use [desiredMaxSpeed and maxSpeed](../Simulation/VehicleSpeed.md#desiredmaxspeed) in the exact same manner as vehicles. For backward compatibility reasons, if `maxSpeed` is configured and `desiredMaxSpeed` is not given in the vType, the `desiredMaxSpeed` is initialized from the given `maxSpeed` value.

When specifying a `type`, the set of
attributes which are in effect during simulation depend on the selected
[pedestrian model](../Simulation/Pedestrians.md#pedestrian_models).
Attributes such as `width`, `length`, `imgFile` and `color` are always used for visualization.


# Repeated persons (personFlows)

To define multiple persons with the same plan, the element `<personFlow>` can be used.
It uses the same parameters and child elements as `<person>` except for the
departure time. The ids of the created persons are
"personFlowId.runningNumber" and they are distributed either equally or
randomly in the given interval. The following additional parameters are
known:

| Attribute Name  | Value Type     | Description                                                                                          |
| --------------- | -------------- | ---------------------------------------------------------------------------------------------------- |
| begin           | float (s) or [human-readable-time](../Other/Glossary.md#t) or *triggered*  | first person departure time. See [below](#starting_the_simulation_in_a_vehicle) for an explanation of 'triggered'   |
| end             | float(s)       | end of departure interval (if undefined, defaults to 24 hours)                                       |
| personsPerHour* _or_ perHour\* | float(\#/h) | number of persons per hour, equally spaced                                                |
| period*         | float(s)       | insert equally spaced persons at that period                                                         |
| probability*    | float(\[0,1\]) | probability for emitting a person each second, see also [Simulation/Randomness](../Simulation/Randomness.md#flows_with_a_random_number_of_vehicles) |
| number*         | int(\#)        | total number of persons, equally spaced                                                              |

\*: Only one of these attributes is allowed.

## Examples

```xml
   <personFlow id="p" begin="0" end="10" period="2">
       <walk from="beg" to="end"/>
   </personFlow>
```

```xml
   <personFlow id="person" begin="0" end="1" number="4" departPos="80">
       <walk from="2/3to1/3" to="1/3to0/3" arrivalPos="55"/>
       <ride from="1/3to0/3" to="0/4to1/4" lines="train0"/>
       <walk from="0/4to1/4" to="1/4to2/4" arrivalPos="45"/>
       <stop lane="1/4to2/4_0" duration="20" startPos="40" actType="singing"/>
       <ride from="1/4to2/4" to="3/4to4/4" lines="car0"/>
   </personFlow>
```

# Simulation input

The input for a person consists of a sequence of *stages* of the 3 types
given below.

!!! note
    Person plans can also be generated with [intermodal routing](../IntermodalRouting.md)

## Rides

Rides define the start and end point of a movement with a single mode of
transport (e.g. a car or a bus). They are child elements of plan
definitions.

| Attribute  | Type     | Range                              | Default | Remark                                            |
| ---------- | -------- | ---------------------------------- | ------- | ------------------------------------------------- |
| lines  | list     | valid line or vehicle ids or *ANY* | ANY      | list of vehicle alternatives to take for the ride    |
| from       | string   | valid edge ids                     | \-      | id of the start edge (optional, if it is a subsequent movement or [starts in a vehicle](Persons.md#starting_the_simulation_in_a_vehicle)) |
| to         | string   | valid edge ids                     | \-      | id of the destination edge (optional, if a busStop or other stopping place is given)  |
| fromPos    | float(m) |                                    | middle of edge  | depart position on the start edge          |
| arrivalPos | float(m) |                                    | end of edge  | arrival position on the destination edge      |
| busStop    | string   | valid bus stop ids                 | \-      | id of the destination stop                         |
| parkingArea| string   | valid parkingArea ids              | \-      | id of the destination stop                         |
| trainStop  | string   | valid trainStop ids                | \-      | id of the destination stop                         |
| chargingStation| string | valid chargingStation ids        | \-      | id of the destination stop                         |
| containerStop| string   | valid containerStop ids          | \-      | id of the destination stop                         |
| group      | string   |                                    | ""      | id of the travel group. Persons with the same group may share a taxi ride |

The vehicle to use has to exist already (either public transport or some
existing passenger car) and the route to take is defined by the vehicle.
The person enters the vehicle if it stops on the 'from' edge and any of
the following conditions are met

- the 'line' attribute of the vehicle or the 'id' of the vehicle is
  given in the list defined by the `lines` attribute of the ride OR
  the lines attribute contains 'ANY' and the vehicle stops at the
  destination 'busStop' of the ride (or at the destination edge if no destination busStop is defined).
- the `lines` attribute can be used to apply taxis, see [taxi](../Simulation/Taxi.md) for more information
- the vehicle has a triggered stop and the person position is within
  the range of `startpos,endPos` of the stop.
- the vehicle has a timed stop and the person is waiting within 10m of
  the vehicle position

The position of the person is either its `departPos` or the arrival position of
the preceding plan element

A given bus stop (or any other stopping place) may serve as a replacement for a destination edge and
arrival position. If an arrival position is given nevertheless it has to
be inside the range of the stop.

The positions of persons in a vehicle depend on the 'guiShape' parameter of the vehicle as well as its dimensions. The offset between the front of the vehicle and the first passenger placement can be configured by adding `<param key="frontSeatPos" value="3.14"/>` to the vType definition of the vehicle.

The number of persons sitting side-by-side depends on the vehicle width but can be overruled by setting `<param key="seatingWidth" value="1.3"/>` in the vType definition.

!!! note
    up to version 1.15.0 attribute 'lines' was mandatory.

## Walks

Walks define a [pedestrian movement](../Simulation/Pedestrians.md).
They are child elements of plan definitions.

| Attribute  | Type       | Range              | Default | Remark                                                                          |
| ---------- | ---------- | ------------------ | ------- | ------------------------------------------------------------------------------- |
| route      | string     | valid route id     | \-      | the id of the route to walk                                                     |
| edges      | list       | valid edge ids     | \-      | id of the edges to walk                                                         |
| from       | string     | valid edge ids     | \-      | id of the start edge  (optional, if it is a subsequent movement)                |
| to         | string     | valid edge ids                     | \-      | id of the destination edge (optional, if a busStop or other stopping place is given)  |
| duration   | float(s)   | \>0                | \-      | override walk duration (otherwise determined by the person type and the pedestrian dynamics)         |
| speed      | float(m/s) | \>0                | \-      | override walking speed (otherwise determined by the person type and individual speed factor)         |
| arrivalPos | float(m)   |                    | middle of edge  | arrival position on the destination edge                                        |
| departPosLat | float(m), string ("random", "left", "right", "center")   |        | right side in walking direction  | custom lateral position on lane at departure ([details](../Definition_of_Vehicles%2C_Vehicle_Types%2C_and_Routes.md#departposlat))|
| busStop    | string     | valid bus stop ids | \-      | id of the destination stop                                                      |
| parkingArea| string   | valid parkingArea ids              | \-      | id of the destination stop                        |
| trainStop  | string   | valid trainStop ids              | \-      | id of the destination stop                        |
| chargingStation| string   | valid chargingStation ids              | \-      | id of the destination stop                        |
| containerStop| string   | valid containerStop ids              | \-      | id of the destination stop                        |

You can define either a `route`-id, or a list of `edges` to travel or a `from` and a `to` edge.
In the first and second case the route edges are traveled in the listed
order. They do not need to be joined in the net. If traveling between
stops on the same edge then only include the edge once. In the latter
case a shortest path calculation is performed and it is an error if
there is no path connecting `from` and `to`.

When given as router input input using the attributes `from` and `to` will be
transformed into a walk using the attribute `edges` by routing along edges
permissible for pedestrian (i.e. sidewalks).

A given bus stop (or other stopping place) may serve as a replacement for a destination edge and
arrival position. If an arrival position is given nevertheless, it has
to be inside the range of the stop.

## Stops

Stops define a delay until the next element of a plan is started. They
can be used to model activities such as working or shopping. Stops for
persons follow the specification at
[Specification\#Stops](index.md#stops). However, only
the attributes `lane`, `duration` and `until` are evaluated. Using these attributes it is
possible to model activities with a fixed duration as well as those with
a fixed end time. If a person needs to be transferred between two
positions without delay, it is possible to use two stops in conjunction.

## Parameters

All stages can hold user defined parameters in the usual `<param key="k" value="v"/>` form.
If a stage is generated using a [personTrip](#persontrips) the stages inherited the parameters
of the trip.

# Simulation behavior

A person is starting her life at her depart time on the source (resp.
first) edge of the first walk, ride or stop. She tries to start the next
step of her plan.

## Riding

The person checks whether a vehicle with a line from the given list is
stopping at the given edge. If such a vehicle exists and the person is
positioned between the start and end position of the vehicle's stop, the
person will enter the vehicle and start its ride. If such a vehicle
exists but the person is not positioned between the start and end
position of the vehicle's stop, the person will still enter if the
vehicle is triggered by the person and the distance between person and
vehicle is at most 10 metres. It does not check whether the vehicle has
the aspired destination on the current route. The first time the vehicle
stops (on a well defined stop) at the destination edge, the ride is
finished and the person proceeds with the next step in the plan.

If option **--time-to-teleport.ride** is set, persons will be teleported to their ride destination after the specified time rather than waiting until the end of the simulation (or until all active vehicles have left the simulation).

## Walking

The walking behavior of a person depends on the selected [pedestrian
model](../Simulation/Pedestrians.md#pedestrian_models). Generally,
the person follows the given sequence of edges with a speed bounded by
the `desiredMaxSpeed` attribute of the persons type. It starts either at the position
from the previous stage of its plan or at the specified `departPos` if no previous
stage exists. The walk concludes at the specified `arrivalPos` which defaults to the
end of the final edge. Both position attributes support the special
values `max` and `random` which work as described [for
vehicles](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#a_vehicles_depart_and_arrival_parameter).


## Stopping

The person stops for the maximum of `currentTime` + `duration` and `until`.

## Access

Whenever a person starts or ends a walk at a busStop or trainStop (collectively called *stoppingPlace*), an access stage is inserted into the person plan under the following conditions:

- the walk ends on an edge that is different from the stoppingPlace edge and the stoppingPlace has an `<access>` definition that connects it with the final edge of the walk
- the walk starts on an edge that is different from the stoppingPlace edge and the stoppingPlace has an `<access>` definition that connects it with the first edge of the walk

The time spent in an access stage is equal to the "length" attribute of the access divided by the walking speed of the person. No interaction between persons on the same access element takes place.


# Starting the simulation in a Vehicle
It is possible to start the person simulation simultaneously with the start of a vehicle simulation within that vehicle. I.e. the person starts with a ride within the vehicle, when the vehicle is inserted to the simulation. This is possible for a `person` definition and a `personFlow`. In both cases the vehicle must already be loaded in the input file.

!!! note
    The starting vehicle must already be loaded in the input file

## Starting a person in a vehicle
To start the simulation of a person while riding in a vehicle, the `depart` attribute of the person must be set to `triggered`.
Additionally the first stage of the plan must be a `ride`. The `from` attribute is not necessary, since the vehicle start position is already defined and used.
The vehicle is indicated by using only the vehicle ID for the `lines` attribute of the ride. Alternatively, the lines attribute may hold the id of a flow. In this case, most recent vehicle belonging to that flow will receive the person.

## Starting multiple persons in a vehicle
To start the simulation of multiple persons with the same plan while riding in a vehicle, `personFlow` can be used. This works by setting the attribute `begin="triggered"`.
Additionally the first stage of the plan must be a `ride`. The `from` attribute is not necessary, since the vehicle start position is already defined and used.
The vehicle is indicated by using only the vehicle ID for the `lines` attribute of the ride. Alternatively, the lines attribute may hold the id of a flow. In this case, most recent vehicle belonging to that flow will receive the person.
If the personFlow is defined with attribute `number`, then all persons will be inserted into the same vehicle.
If the personFlow is defined with attribute `period`, then the persons will be created with the indicated period and be put in the vehicle at a later time (and possibly different vehicles if a flow id was used in `lines`).

## Examples
Person `p0` starts within the vehicle defined by trip `v0` at edge `gneE0`. The ride ends at edge `gneE1`.
```xml
<trip id="v0" depart="15.00" from="gneE0" to="gneE2">
    <stop lane="gneE1_1" duration="60.00"/>
</trip>
<person id="p0" depart="triggered">
    <ride to="gneE1" lines="v0"/>
    <walk to="gneE3"/>
</person>
```

Persons defined by personFlow `p0` start within the vehicle defined by trip `v0` at edge `gneE0`. The ride ends at edge `gneE1`.
```xml
<trip id="v0" depart="15.00" from="gneE0" to="gneE2">
    <stop lane="gneE1_1" duration="60.00"/>
</trip>
<personFlow id="p0" begin="triggered" number="2">
    <ride to="gneE1" lines="v0"/>
    <walk to="gneE3"/>
</person>
```


# Router input
The following definitions can be processed with [duarouter](../duarouter.md) and [sumo](../sumo.md).

## PersonTrips

A personTrip defines the start and end point of a movement with optional
changes in mode. They are child elements to the person. In order to
process a personTrip with mode *public*, the [public transport
network](../IntermodalRouting.md) has to be defined as well.
Currently bicycle and/or car can only be the first mode. It is not
possible to switch to a car or bicycle after a different mode except when adding 'taxi' to the available modes.

The rules for selecting the best itinerary through the network are described at [intermodal routing](../IntermodalRouting.md).

If the computed plan starts with a car or bicycle, a vehicle for use by the person will be automatically generated and set to depart="triggered". A car will be named 'PERSON_ID_0' and bicycle will be named 'PERSON_ID_b0' where 'PERSON_ID' is the id of the person. If a vehicle with this id already exists in the simulation, this vehicle will be used instead (and the user has to ensure that it starts at an appropriate location).

| Attribute  | Type     | Range                                         | Default | Remark                                                        |
| ---------- | -------- | --------------------------------------------- | ------- | ------------------------------------------------------------- |
| from       | string   | valid edge ids                                | \-      | id of the start edge (optional, if it is a subsequent movement)  |
| to         | string   | valid edge ids                                | \-      | id of the destination edge                                    |
| via        | string   | valid edge ids                                | \-      | ids of the intermediate edges (not implemented yet)           |
| busStop    | string   | valid bus stop ids                            | \-      | id of the destination stop                                    |
| vTypes     | list     | valid vType ids                               | \-      | list of possible vehicle types to take                        |
| modes      | list     | any combination of "public", "car", "bicycle", ["taxi"](../Simulation/Taxi.md) | \-      | list of possible traffic modes (separated by ' '). Walking is always possible regardless of this value.     |
| departPos  | float(m) |                                               | 0       | initial position on the starting edge (deprecated, determined by the departPos of the person or the arrival pos of the previous step) |
| departPosLat  | float(m) |            | 0       | initial lateral position on the starting edge when walking |
| arrivalPos | float(m) |                                               | middle of edge | arrival position on the destination edge                      |
| group| string           |               | ""      | id of the travel group. Persons with the same group may share a taxi ride     |

!!! note
    It is an error for subsequent trips to be unconnected.

!!! note
    If no itinerary for performing the trip is found and the option **--ignore-route-errors** is set, the trip will be transformed into a walk which consists of the start and arrival edge. The person will teleport to complete the walk.

!!! note
    When attribute vTypes is used, the person may start with any of the given vehicle types at the from-edge. Including 'car' in modes is equivalent to vTypes="DEFAULT_VEHTYPE". Including 'bicycle' in modes is equivalent to vTypes="DEFAULT_BIKETYPE". The vehicles will be automatically generated when used.

# Example

The following is an example for a person who walks to a train station,
rides the train, alights and walks for a bit, then stops for an activity
and finally gets into a car and drives away.

```xml
<routes>
    <person id="person0" depart="0">
        <walk from="2/3to1/3" to="1/3to0/3" departPos="80" arrivalPos="55"/>
        <ride from="1/3to0/3" to="0/4to1/4" lines="train0"/>
        <walk from="0/4to1/4" to="1/4to2/4" arrivalPos="30"/>
        <stop lane="1/4to2/4_0" duration="20" startPos="40" actType="singing"/>
        <ride from="1/4to2/4" to="3/4to4/4" lines="car0"/>
    </person>

    <vehicle id="train0" depart="50">
        <route edges="1/4to1/3 1/3to0/3 0/3to0/4 0/4to1/4 1/4to1/3"/>
        <stop busStop="busStop0" until="120" duration="10"/>
        <stop busStop="busStop1" until="180" duration="10"/>
    </vehicle>

    <vehicle id="car0" depart="triggered">
        <route edges="1/4to2/4 2/4to3/4 3/4to4/4" departPos="30"/>
        <stop lane="1/4to2/4_0" duration="20" startPos="40" endPos="60"/>
    </vehicle>

</routes>
```

You can omit the from attribute in the second and following movements
and you can use bus stops as destinations. For trains the busStops
should have access lanes. The movement of person0 in the example above
could also be written as

```xml
<person id="person0" depart="0">
    <walk from="2/3to1/3" busStop="busStop0" departPos="80" arrivalPos="55"/>
    <ride busStop="busStop1" lines="train0"/>
    <walk to="1/4to2/4" arrivalPos="30"/>
    <stop lane="1/4to2/4_0" duration="20" startPos="40" actType="singing"/>
    <ride from="1/4to2/4" to="3/4to4/4" lines="car0"/>
</person>
```

# Visualization

Persons are rendered in the GUI with the configured detail-level. When
assigning a type with attribute *imgFile*, the person may be rendered
with an image. By default, the image will not be rotated (suitable for
abstract icons). However, when setting the types *guiShape*="pedestrian"
the image will be rotated according to the persons location and stage.

# Person Output

Most of the [Simulation outputs](../Simulation/Output/index.md) are
tailored for vehicles. Only a small number of output formats support
persons:

- [tripinfo-output](../Simulation/Output/TripInfo.md)
- [vehroute-output](../Simulation/Output/VehRoutes.md)
- [fcd-output](../Simulation/Output/FCDOutput.md)
- [netstate-dump](../Simulation/Output/RawDump.md)
- [aggregated simulation statistics](../Simulation/Output/index.md#aggregated_traffic_measures)

# Devices

Person-devices are used to model and configure different aspects such
as output (person-device.fcd) or behavior (person-device.rerouting).

The following device names are supported and can be used for the
placeholder `<DEVICENAME>` below:

- [btreiver](../Simulation/Bluetooth.md)
- [btsender](../Simulation/Bluetooth.md)
- [rerouting](../Demand/Automatic_Routing.md)
- [fcd](../Simulation/Output/FCDOutput.md)

Assignment of devices via simulation options works the same [as for vehicles](../Definition_of_Vehicles%2C_Vehicle_Types%2C_and_Routes.md#assignment_by_global_options) except for the fact that all options are prefixed with
**--person-device.DEVICENAME** instead of **--device.DEVICENAME**.

Assignment of devices via `<vType>` or `<person>`-parameters works in the same way [as it does for vehicles](../Definition_of_Vehicles%2C_Vehicle_Types%2C_and_Routes.md#assignment_by_generic_parameters).


# Planned features

The following features are not yet implemented.

- [Simulation routing for
  persons](../Demand/Automatic_Routing.md)
  (person-device.rerouting)
