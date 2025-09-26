---
title: Logistics
---

The concept of logistics is mostly realized by
[containers](Containers.md) and [container
stops](#container_stops).

# Container stops

Container stops can be used to simulate transhipment stations, harbours
and other places for transhipping and storing containers/goods.
Similarly to the concept of [bus
stops](../Simulation/Public_Transport.md), container stops are
[stops](index.md#stops) at which containers can be
loaded onto or unloaded from a vehicle. Vehicles use the same advanced
approaching behavior at container stops as at bus stops. Definitions of
container stop locations in SUMO have the following format:

```xml
<additional>
    <containerStop id="<CONTAINER_STOP_ID>" lane="<LANE_ID>" startPos="<STARTING_POSITION>" endPos="<ENDING_POSITION>" [line="<LINE_ID>[ <LINE_ID>]*"]/>
    ...
</additional>
```
That means that a container stop is an area
on a lane. The parameters have the following meanings:

| Attribute Name | Value Type  | Value Range                                         | Default     | Description     |
| -------------- | ----------- | --------------------------------------------------- | ----------- | --------------- |
| **id**         | string      | id                                                  |             | The name of the container stop; must be unique    |
| **lane**       | string      | valid lane id                                       |             | The name of the lane the container stop shall be located at     |
| **startPos**   | float       | \-lane.length < x < lane.length (negative values count backwards from the end of the lane) | 0           | The begin position on the lane (the lower position on the lane) in meters                                                   |
| endPos         | float       | \-lane.length < x < lane.length (negative values count backwards from the end of the lane) | lane.length | The end position on the lane (the higher position on the lane) in meters, must be larger than *startPos* by more than 0.1m  |
| friendlyPos    | bool        | *true,false*                                                                                 | *false*     | whether invalid stop positions should be corrected automatically (default *false*)                                          |
| name           | string      | simple String                                                                                |             | Container stop name. This is only used for visualization purposes.                                                          |
| color           | color      | see [color definition](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#colors)                                                                                | "83,89,172"            | Container stop color. This is only used for visualization purposes.                                                               |
| lines          | string list | A list of names separated by spaces (' ')                                                    |             | meant to be the names of the logistic lines that stop at this container stop. This is only used for visualization purposes. |
| angle          | float (degrees) | The angle at which containers are drawn. This is only used for visualization purposes. |


!!! caution
    Please note that container stops must be added to a config via the *--additional-files* parameter

Vehicles must be informed that they must stop at a container stop. The
following example shows how this should be done:

```xml
<vtype id="truck" accel="2.6" decel="4.5" sigma="0.5" length="15" maxspeed="70" color="1,1,0"/>
<vehicle id="0" type="truck" depart="0" color="1,1,0">
    <route edges="2/0to2/1 2/1to1/1 1/1to1/2 1/2to0/2 0/2to0/1 0/1to0/0 0/0to1/0 1/0to2/0 2/0to2/1"/>
    <stop containerStop="containerStop1" duration="20"/>
    <stop containerStop="containerStop2" duration="20"/>
    <stop containerStop="containerStop3" duration="20"/>
    <stop containerStop="containerStop4" duration="20"/>
</vehicle>
```

What is defined here is a vehicle named "0" being a "truck". "truck" is
a referenced type declared earlier. The vehicle has an embedded route
(written by hand in this case) and a list of stop places. Each stop
place is described by two attributes, "containerStop" and "duration"
where "containerStop" is the name of the container stop the vehicle
shall halt at and "duration" is the time the vehicle shall wait at the
container stop in seconds. Please note that the order of container stops
the vehicle shall halt at must be correct.

You may also let a vehicle stop at another position than a container
stop. The short definition of a vehicle's stop is:

```xml
<stop [containerStop="<CONTAINER_STOP_ID>" | ( lane="<LANE_ID>" endPos="<POSITION_AT_LANE>" )] duration="<HALTING_DURATION>"/>
```
This means you can either use a
container stop or a lane position to define where a vehicle has to stop.
For a complete list of attributes for the "stop"-element of a vehicle
see [Specification\#Stops](index.md#stops).

## Access Lanes

Each containerStop may have additional child elements to model access from
other parts of the network (e.g. road access to a stop on the rail
network). This takes the following form:

```xml
<containerStop id="myStop" lane="A_0" startPos="230" endPos="250">
   <access lane="B_0" pos="150"/>
</containerStop>
```

## Spacing of waiting containers

The spacing between waiting containers defaults to 2.5. This is appropriate when standard shipping containers of 2.44m width are stacked next to each other in paralle..
When containers are stacked at a different angle, the distance along the stop must be increased. This can be done with params:

```xml
<containerStop id="ct_5" angle="0" lane="324040749#0_0" startPos="0.00" endPos="630.00">
        <param key="waitingWidth" value="13"/>
        <param key="waitingDepth" value="2.44"/>
 </containerStop>
```

# Trailers and Rail Cars

Containers can be used to model trailers and marshalling/switching by changing the length of the vehicle dynamically upon being loaded/unloaded.
This is done by setting [Generic Parameter](../Simulation/GenericParameters.md) `<param key="device.container.loadedType" value="TYPE_ID"/>`.

The `<vType>` with `id="TYPE_ID"` must be defined and the following of its values will be used by the simulation:

- containerCapacity
- length
- mass
- vClass
- guiShape
- param with key 'locomotiveLength'
- param with key 'carriageLength'
- param with key 'carriageGap'

Whenever the number of containers changes due to loading/unloading, the number of attached railcars/trailers is computed as `ceil(containerNumber / containerCapacity)`.
Then **mass** and **length** will multiplied by the number of attached units and added to the length and mass of the original type of the vehicle.
The **vClass**, **guiShape** and visualization params will also be applied to the vehicle until the containerNumber drops to zero again.

If the vClass changes due to loading/unloading, the vehicle will re-compute its route according to the current travel times in the network. This may be used to model changes in required turning radius by disallowing some connections for the **vClass** of the loadedType `TYPE_ID` but permitting them for the original (empty) type of the vehicle.
