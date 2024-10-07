---
title: Stationfinder device
---

## Overview

Although SUMO offers output regarding emissions and energy/fuel usage, by default SUMO vehicles would travel as long as wanted without need to charge.
Charging stations for battery electric vehicles have been introduced with the [battery device](../Models/Electric.md#defining_electric_vehicles).

The stationfinder device lets equipped vehicles monitor their energy buffer (currently battery only) when traveling and reroute to nearby
charging stations in case they won't make it to their destinations otherwise. Vehicles can break down due to low battery charge (see `rescueAction` parameter).

!!! note
    As of SUMO 1.21 the device implementation works only for vehicles with a battery device.

## Configuration

The following table gives the full list of possible parameters for the stationfinder device. Each of these parameters must be specified as a child
element of the form `<param key=device.stationfinder.<PARAMETER NAME> value=<PARAMETER VALUE>` of the appropriate demand definition element (e.g. `<vehicle ... />`, `<vType ... />`, or `<flow ... />`).
The parameters take effect only on vehicles which have been assigned a stationfinder device by one of the [device assignment options](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#devices).

| Parameter             | Type             | Range                   | Default          | Description                                                                         |
| --------------------- | ---------------- | ----------------------- | ---------------- | ----------------------------------------------------------------------------------- |
| rescueTime            | float (s)        | ≥0                      | 1800             | Time to wait for a rescue vehicle on the road side when the battery is empty        |
| rescueAction          | enum             | {remove;tow;none}       | remove           | What to do with vehicles in rescue mode: `remove` remove immediately from the simulation, `tow` teleport to a charging station after waiting or do nothing using `none` |
| reserveFactor         | float            | ≥1                      | 1.1              | Scale battery need with this factor to account for unexpected traffic situations |
| emptyThreshold        | float            | [0;1]                   | 0.05             | Battery percentage to go into rescue mode |
| radius                | float (s)        | ≥0                      | 180              | Search radius in travel time seconds |
| maxEuclideanDistance  | float (m)        |                         | -1               | Euclidean search radius in meters (negative values disable this restriction) |
| repeat                | float (s)        | ≥0                      | 60               | When to trigger a new search if no station has been found |
| maxChargePower        | float (W)        | ≥0                      | 100000           | The maximum charging speed of the vehicle battery |
| chargeType            | enum             | {charging}              | charging         | Type of energy transfer (not used at the moment) |
| waitForCharge         | float (s)        | ≥0                      | 600              | After this waiting time vehicle searches for a new station when the initial one is blocked |
| saturatedChargeLevel  | float            | [0;1]                   | 0.8              | Target state of charge after which the vehicle stops charging |
| needToChargeLevel     | float            | [0;1]                   | 0.4              | State of charge the vehicle begins searching for charging stations |
| replacePlannedStop    | float            | [0;inf[                 | 0                | Share of the time until the departure at the next planned stop used for charging (values >1 will cause skipping the next planned stop) |
| maxDistanceToReplacedStop | float        | ≥0                      | 300              | Distance in meters along the network from charging station to the next planned stop |
| chargingStrategy      | string           | {balanced;latest;none}  | none             | Chosen charging strategy (`balanced` will spread the charging across the whole stopping duration, `latest` will shift charging towards the end of the stopping duration) |

## Decision logic for charging

This paragraph shall convey the major conditions used in the decision logic of the device. The device has to decide whether and where to reroute the
vehicle to charge. The user can configure multiple thresholds to change how different vehicles behave.

- As long as the battery state of charge (SoC) is above `needToChargeLevel`, no action is taken. This condition is rechecked each time the SoC has lowered by 10%.
- When the SoC reaches `needToChargeLevel`, possible charging stations are searched and evaluated using the [target function](#charging_station_target_function). If a valid charging station is found within `radius` travel time (and optionally within `maxEuclideanDistance` air line), the vehicle is immediately rerouted to go there. Other programmed stops will be served after charging.
- If no charging station is available, the vehicle continues its original route. It will search again after `repeat` seconds.
- If the charging station is occupied on arrival, the vehicle will wait `waitForCharge` seconds before looking for an alternative site.
- The vehicle will charge enough to complete the planned route multiplied by `reserveFactor` and try to keep the SoC above `emptyThreshold`.
- Equipped vehicles monitor every second (not necessarily every time step) whether the battery state of charge (SoC) goes beyond `emptyThreshold`. If it does, the configured `rescueAction` is executed.

## Charging station target function

When there are more sites to choose from, the device sorts the charging stations according to a target function and chooses the one with the lowest score. The target function works the same way as
[the one for parking search](Rerouter.md#determining_the_alternative_parking_area). Thus it consists of a linear combination of components and corresponding weight factors. Each of these parameters must be specified as a child
element of the form `<param key=device.stationfinder.<PARAMETER NAME> value=<PARAMETER VALUE>` of the appropriate demand definition element (e.g. `<vehicle ... />`, `<vType ... />`, or `<flow ... />`).
The available components and their weight factors are described in the table below:

| Parameter Name              | Default value | Description                                                              | Inverse (Bigger is better) |
| --------------------------- | ------------- | ------------------------------------------------------------------------ | -------------------------- |
| charging.probability.weight  | 0             | currently not applicable  | yes                        |
| charging.capacity.weight     | 0             | The total capacity of the charging station                              | yes                        |
| charging.absfreespace.weight | 0             | The absolute number of free spaces                                       | yes                        |
| charging.relfreespace.weight | 0             | The relative number of free spaces                                       | yes                        |
| charging.distanceto.weight   | 0             | The road distance to the charging station                                | no                         |
| charging.timeto.weight       | 1             | The assumed travel time to the charging station                          | no                         |
| charging.distancefrom.weight | 0             | The road distance from the charging station to the vehicle destination / next stop | no                         |
| charging.timefrom.weight     | 1             | The assumed travel time from the charging station to the vehicle destination / next stop | no                         |
| charging.chargingTime.weight | 1             | The assumed charging time at the charging station                       | no                         |
| charging.waitingTime.weight  | 1             | The assumed waiting time at the charging station for a free charging point | no                         |

Normalization is done by dividing the single value by the maximum value found among all available charging stations within the search range. Inversion means taking the remainder to 1 instead of the normalized value itself.

When `charging.probability.weight` is set to a positive value, a random number between 0 and 1 is drawn for each candidate charging station.
This value is then normalized to the range [0,1] by dividing with the maximum probability value of all charging station elements.
The inverted normalized value is then multiplied with `charging.probability.weight` to enter into the candidate score.

### Further parameters to affect charging station search behavior
Each of these parameters must be specified as a child
element of the form `<param key=device.stationfinder.<PARAMETER NAME> value=<PARAMETER VALUE>` of the appropriate demand definition element (e.g. `<vehicle ... />`, `<vType ... />`, or `<flow ... />`).

|Parameter Name         | Default value | Description                                                              |
| --------------------- | ------------- | ------------------------------------------------------------------------ |
| charging.frustration  | 100           | increases the preference for visibly free charging stations over time (after x unsuccessful trials, targets with unknown occupancy will assumed to be *almost* full)                                 |
| charging.knowledge    | 0             | Let driver "guess" the exact occupancy of invisible charging stations with probability x                   |

### Inspect the target function result
The target function value of each searched charging station of the vehicle can be inspected in [sumo-gui](../sumo-gui.md). Proceed with the following steps to see the results next to the charging stations:

- Open the vehicle visulization settings](../sumo-gui.md#vehicle_visualisation_settings) and check "Show charging info"
- Move to vehicle of interest and open its context menu by right click, then choose "Show Current Route"

Some charging stations may not show target function results. They may be excluded beforehand due to stationfinder device settings like `radius` and `maxEuclideanDistance`.

## Break down due to lack of energy

Vehicles may not always manage to reach a free charging point before they run out of energy. This can be due to a number of reasons:

- low battery capacity (actual or maximum battery capacity)
- high energy consumption
- low number of charging stations nearby
- high occupancy of charging stations

The stationfinder device can be configured using the parameters `rescueTime` and `rescueAction` to handle the lack of energy in the following ways:

- `rescueAction="none"`: let vehicles continue on their route although they don't have enough energy
- `rescueAction="remove"`: the vehicle will come to a standstill and then will be removed from the simulation
- `rescueTime="TIME" rescueAction="tow"`: the vehicle will come to a standstill and wait for `TIME` seconds (waiting time for a tow truck). Then it will be teleported to a free charging point, charge and continue its route. Thus the travel from the break down to the charging station is not included in [emissions output](../Simulation/Output/EmissionOutput.md).

## Interaction with other planned stops
By default, the stationfinder device will add stops at charging stations without changing any other planned stops. This means the vehicle may need longer to complete its route due to additional charging stops. Under real-world conditions users may prefer charging the vehicle close to their activity location and walk the remaining distance.
This can be modeled using the parameters `replacePlannedStop` and `maxDistanceToReplacedStop` and stops with defined departure times through the `until` attribute (see [stops](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#stops_and_waypoints)):

- `replacePlannedStop` > 0: Defines the share of stopping time to be transferred from the next planned stop to charging. If the value exceeds 1, the next planned stop is skipped. If the value is set to 0, no stopping time is transferred.
- `maxDistanceToReplacedStop` defines the acceptable distance in meters between the charging station and the stop modeling the activity location. If the distance is above this threshold, no stopping time will be transferred

### Charging strategies
The charging process is defined by time, location and charging power. Time and location are determined using the stationfinder logic as [explained](#decision_logic_for_charging) .
When the vehicle remains attached to the charging station longer than needed there is the opportunity to alter the charging power curve, thus shift charging in time.
Here a charging strategy is defined as the charging power curve agreed between the vehicle and the charging station. Electricity grid operators may use such strategies to lower the peak energy they have to provide.

Currently two charging strategies can be set in the stationfinder device using the `chargingStrategy` parameter:

- `chargingStrategy="balanced"`: limit the charging power such that charging to the target SoC needs the whole stopping duration ("flat" charging curve)
- `chargingStrategy="latest"`: still charge at the maximum charge rate but rather than start charging immediately after arrival, start the latest time possible to reach the target SoC
