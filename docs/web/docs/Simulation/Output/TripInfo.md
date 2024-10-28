---
title: TripInfo
---

## Instantiating within the Simulation

The simulation is forced to generate this output using the option **--tripinfo-output** {{DT_FILE}} on
the command line/within a configuration. This output contains the
information about each vehicle's departure time, the time the vehicle
wanted to start at (which may be lower than the real departure time) and
the time the vehicle has arrived. The information is generated for each
vehicle as soon as the vehicle arrived at its destination and is removed
from the network.

## Generated Output

The format is as following:

```xml
<tripinfos>
    <tripinfo id="<VEHICLE_ID>" \
            depart="<DEPARTURE_TIME>" departLane="<DEPARTURE_LANE_ID>" \
            departPos="<DEPARTURE_POSITION>" departSpeed="<DEPARTURE_SPEED>" \
            departDelay="<DEPARTURE_DELAY>" \
            arrival="<ARRIVAL_TIME>" arrivalLane="<DEPARTURE_LANE_ID>" \
            arrivalPos="<ARRIVAL_POSITION>" arrivalSpeed="<ARRIVAL_SPEED>" \
            duration="<TRAVEL_TIME>" routeLength="<ROUTE_LENGTH>" \
            waitingTime="<SECONDS_WAITING_FOR_TRAFFIC>"
            waitingCount="<NUMBER_OF_WAITING_EPISODES>" \
            rerouteNo="<REROUTE_NUMBER>" \
            devices="<DEVICE_LIST>" vtype="<VEHICLE_TYPE_ID>"/>

    ... information about further vehicles ...

</tripinfos>
```

Please note that, in contrary to the example above, for each time step,
all those values are reported in one line. An entry is written each time
a vehicle has arrived at his destination. In prior to this, the written
values would not be known.

| Name         | Type                 | Description                                                                                                        |
| ------------ | -------------------- | ------------------------------------------------------------------------------------------------------------------------ |
| `id`           | (vehicle) id         | The name of the vehicle that is described by this entry                                                                  |
| `depart`       | (simulation) seconds | The real departure time (the time the vehicle was inserted into the network)      |
| `departLane`   | (lane) id            | The id of the lane the vehicle started its journey       |
| `departPos`    | m                    | The position on the lane the vehicle started its journey             |
| `departSpeed`  | m/s                  | The speed with which the vehicle started its journey            |
| `departDelay`  | (simulation) seconds | The time the vehicle had to wait before it could start his journey        |
| `arrival`      | (simulation) seconds | The time the vehicle reached his destination at          |
| `arrivalLane`  | (lane) id            | The id of the lane the vehicle was on when reaching his destination         |
| `arrivalPos`   | m                    | The position on the lane the vehicle was when reaching the destination      |
| `arrivalSpeed` | m/s                  | The speed the vehicle had when reaching the destination               |
| `duration`     | (simulation) seconds | The time the vehicle needed to accomplish the route             |
| `routeLength`  | m                    | The length of the vehicle's route                        |
| `waitingTime`  | s                    | The time in which the vehicle speed was below or equal 0.1 m/s (scheduled [stops](../../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#stops_and_waypoints) do not count)          |
| `waitingCount` | \#                   | The number of times the vehicle speed went below or equal 0.1 m/s |
| `stopTime`     | s                    | The time in which the vehicle was taking a planned [stop](../../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#stops_and_waypoints)      |
| `timeLoss`     | seconds              | The time lost due to driving below the ideal speed. (ideal speed includes [the individual speedFactor](../../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#speed_distributions); slowdowns due to intersections etc. will incur timeLoss, scheduled [stops](../../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#stops_and_waypoints) do not count) |
| `rerouteNo`    | \#                   | The number the vehicle has been rerouted              |
| `devices`      | \[ID\]\*             | List of devices the vehicle had. Each device is separated from the others by a ';'.      |
| `vtype`        | ID                   | The type of the vehicle          |
| `speedFactor`  | float                | The individual speed factor of the vehicle (possibly drawn from a speed distribution at the start of the simulation)        |
| `vaporized`    | bool                 | Whether the vehicle was removed from the simulation before reaching its destination        |

### Devices

Single devices may choose to add further output to the tripinfo file.

#### Emissions

The "emissions" device instantiated using one of the options **--device.emissions.probability** {{DT_FLOAT}} or **--device.emissions.explicit** [***<ID\>\[,<ID\>\]\****](../../Basics/Notation.md#referenced_data_types). The written emissions depend
on the chosen emission class of the vehicle (see [Definition of Vehicles,
Vehicle Types, and
Routes](../../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md)
and [Models/Emissions](../../Models/Emissions.md)) contributes to the tripinfo output. It provides the sum of all emissions/consumption generated/consumed by the
vehicle during its journey.

This adds the following line:

```xml
<tripinfos>
    <tripinfo id="<VEHICLE_ID>" ... vtype="<VEHICLE_TYPE_ID>">
        <emissions CO_abs="..." CO2_abs="..." HC_abs="..." PMx_abs="..." NOx_abs="..." fuel_abs="..."/>
    </tripinfo>
    ... information about further vehicles ...

</tripinfos>
```

with units as following

| Name      | Type | Description                                                                  |
| --------- | ---- | ---------------------------------------------------------------------------- |
| `CO_abs`   | mg   | The complete amount of CO emitted by the vehicle during the trip             |
| `CO2_abs`  | mg   | The complete amount of CO<sub>2</sub> emitted by the vehicle during the trip |
| `HC_abs`   | mg   | The complete amount of HC emitted by the vehicle during the trip             |
| `PMx_abs`  | mg   | The complete amount of PM<sub>x</sub> emitted by the vehicle during the trip |
| `NOx_abs`  | mg   | The complete amount of NO<sub>x</sub> emitted by the vehicle during the trip |
| `fuel_abs` | mg   | The complete amount of fuel the vehicle used during the trip                 |
| `electricity_abs` | Wh   | The complete amount of electricity the vehicle used during the trip   |

#### Battery

The ["battery" device](../../Models/Electric.md) contributes to the tripinfo output how many times the vehicle encountered its battery depleted during its journey.
Vehicles can be forced to stop due to lack of energy using the ["stationfinder" device](../Stationfinder.md) or may recuperate energy while braking and then deplete the battery again.

This adds the following line:

```xml
<tripinfos>
    <tripinfo id="<VEHICLE_ID>" ... vtype="<VEHICLE_TYPE_ID>">
        <battery depleted="..."/>
    </tripinfo>
    ... information about further vehicles ...

</tripinfos>
```

with the attributes as following

| Name         | Type | Description                                                                        |
| ------------ | ---- | ---------------------------------------------------------------------------------- |
| `depleted`   | -   | The times the vehicle wanted to consume more energy than the battery could provide |


## Output for vehicles that have not arrived at simulation end
By default, tripinfo-output is only written on vehicle arrival. This means vehicles that have not arrived at simulation end (i.e. due to option **--end**) generate no output.
To change this, the option **--tripinfo-output.write-unfinished** can be used.

## Output only for selected vehicles or vehicle types
By default all vehicles will generated tripinfo-output. By [assigning a tripinfo device](../../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#devices) to selected vehicles or types, this can be changed.

Example:
Setting SUMO-option **--device.tripinfo.probability 0** will disable the device for all vehicles by default.
The following definition overrides this default end enables the device for busses:

```xml
<vType id="bus">
  <param key="has.tripinfo.device" value="true"/>
</vType>
```

## Person and Container Output

If the simulation contains persons or containers, the following elements
will be added to the output:

```xml
<personinfo id="person0" depart="0.00">
 <walk depart="0.00" arrival="47.00" arrivalPos="55.00"/>
 <ride waitingTime="74.00" vehicle="train0" depart="121.00" arrival="140.00" arrivalPos="92.00"/>
 <stop duration="20.00" arrival="160.00" arrivalPos="45.00" actType="singing"/>
</personinfo>
```

```xml
<containerinfo id="container0" depart="0.00">
  <tranship depart="0.00" arrival="54.00" arrivalPos="55.00"/>
  <transport waitingTime="103.00" vehicle="train0" depart="157.00" arrival="176.00" arrivalPos="92.00"/>
  <stop duration="20.00" arrival="196.00" arrivalPos="40.00" actType="waiting"/>
</containerinfo>
```

When setting option **--personinfo-output FILE**, the above elements will be written to the given filename instead of written together with the vehicle `<tripinfo>` elements.

The `<personinfo>` / `<containerinfo>` attributes have the following meaning:

| Name   | Type                 | Description                                                |
| ------ | -------------------- | ---------------------------------------------------------- |
| `id`     | (vehicle) id         | The name of the person / container described by this entry |
| `depart` | (simulation) seconds | The departure time                                         |

The attributes within the stages have the following meaning:

| Name        | Type                 | Description                                                                                                                      |
| ----------- | -------------------- | -------------------------------------------------------------------------------------------------------------------------------- |
| `depart`      | (simulation) seconds | The departure time for this stage. For `<ride>,<transport>`, this is the time where the vehicle is entered.                    (-1 if the vehicle wasn't entered) |
| `arrival`     | (simulation) seconds | The arrival time for this stage N.B. In stop stages this is the time at which the stage ends i.e. after the duration time period (-1 if the stage did not start) |
| `arrivalPos`  | m                    | The arrival position on the destination edge for this stage                                                                      |
| `duration`    | (simulation) seconds | For walking and stopping, this is time spent in that stage. For a riding stage, this **only** is the time spent inside the vehicle.  (-1 if the stage did not start or the person did not enter the vehicle)    |
| `actType`     | string               | The activity description of a stop                                                                                               |
| `waitingTime` | (simulation) seconds | The time spent waiting for a vehicle                                                                                             |
| `routeLength` | m | the distance traveled in that stage|
| `timeLoss` | s | the time lost due to traveling at speed below the maximum speed in that stage. For a `<ride>` this is the timeLoss of the vehicle during the ride|
| `maxSpeed` | s | the maximum speed permitted in that stage|

## Aggregated Output

Aggregated output for key tripinfo-output attributes can be obtained by setting
the option [**--statistic-output**](StatisticOutput.md).

For further aggregation see [attributeStats.py](../../Tools/Output.md#attributestatspy) and [Tools/Xml\#xml2csv.py](../../Tools/Xml.md#xml2csvpy).

## Visualization Examples

The TripInfo output can be used as input for several visualization tools shipped with SUMO. Some example plots are shown below.

### Depart delays over time

<img src="../../images/departDelayScatter.png" title="departDelayScatter.png" width="500"/>

Generated with [plotXMLAttributes.py](../../Tools/Visualization.md#depart_delay_over_time_from_tripinfo_data).

### Histogram of the number of stops

<img src="../../images/stopCountDist.png" title="stopCountDist.png" width="500"/>

Generated with [plot_tripinfo_distributions.py](../../Tools/Visualization.md#plot_tripinfo_distributionspy).
