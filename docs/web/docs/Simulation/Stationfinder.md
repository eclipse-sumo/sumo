---
title: Stationfinder device
---

## Overview

Although SUMO offers output regarding emissions and energy/fuel usage, by default SUMO vehicles would travel as long as wanted without need to charge.
Charging stations for battery electric vehicles have been introduced with the [battery device](../Models/Electric.md#defining_electric_vehicles).

The stationfinder device lets equipped vehicles monitor their energy buffer (currently battery only) when traveling and reroute to nearby
charging stations in case they won't make it to their destinations otherwise. Optionally, vehicles can break down due to low battery charge.

!!! note
    As of SUMO 1.21 the device implementation works only for vehicles with a battery device.

## Decision logic for charging

This paragraph shall convey the major conditions used in the decision logic of the device. The device has to decide whether and where to reroute the
vehicle to charge. The user can configure multiple thresholds to change how different vehicles behave.

- As long as the battery state of charge (SoC) is above `needToChargeLevel`, no action is taken. This condition is rechecked each time the SoC has lowered by 10%.
- When the SoC reaches `needToChargeLevel`, possible charging stations are searched and evaluated using the [target function](#charging_station_target_function). If a valid charging station is found within `radius` travel time, the vehicle is immediately rerouted to go there. Other programmed stops will be served after charging.
- If no charging station is available, the vehicle continues its original route. It will search again after `repeat` seconds.
- If the charging station is occupied on arrival, the vehicle will wait `waitForCharge` seconds before looking for an alternative site.
- The vehicle will charge enough to complete the planned route multiplied by `reserveFactor` and try to keep the SoC above `emptyThreshold`.
- Equipped vehicles monitor every second (not necessarily every time step) whether the battery state of charge (SoC) goes beyond `emptyThreshold`. If it does, the configured `rescueAction` is executed.

## Charging station target function

When there are more sites to choose from, the device sorts the charging stations according to a target function and chooses the one with the lowest score. The target function works as follows:
```
score = travel_time + charging_time + waiting_penalty/charging_points
```
The score consists of the travel time to get to the charging station and back to the original route, the charging time considering the charging speed and a penalty time if the charging station is already occupied.

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

## Configuration

The following table gives the full list of possible parameters for the stationfinder device. Each of these parameters must be specified as a child
element of the form `<param key=device.stationfinder.<PARAMETER NAME> value=<PARAMETER VALUE>` of the appropriate demand definition element (e.g. `<vehicle ... />`, `<vType ... />`, or `<flow ... />`).

| Parameter             | Type             | Range               | Default          | Description                                                                         |
| --------------------- | ---------------- | ------------------- | ---------------- | ----------------------------------------------------------------------------------- |
| rescueTime            | float (s)        | ≥0                  | 1800             | Time to wait for a rescue vehicle on the road side when the battery is empty        |
| rescueAction          | enum             | {remove;tow;none}   | remove     | What to do with vehicles in rescue mode: `remove` remove immediately from the simulation, `tow` teleport to a charging station after waiting or do nothing using `none` |
| reserveFactor         | float            | ≥1                  | 1.1              | Scale battery need with this factor to account for unexpected traffic situations |
| emptyThreshold        | float            | [0;1]               | 0.05             | Battery percentage to go into rescue mode |
| radius                | float (s)        | ≥0                  | 180              | Search radius in travel time seconds |
| repeat                | float (s)        | ≥0                  | 60               | When to trigger a new search if no station has been found |
| maxChargePower        | float (W)        | ≥0                  | 100000           | The maximum charging speed of the vehicle battery |
| chargeType            | enum             | {charging}          | charging         | Type of energy transfer (not used at the moment) |
| waitForCharge         | float (s)        | ≥0                  | 600              | After this waiting time vehicle searches for a new station when the initial one is blocked |
| saturatedChargeLevel  | float            | [0;1]               | 0.8              | Target state of charge after which the vehicle stops charging |
| needToChargeLevel     | float            | [0;1]               | 0.4              | State of charge the vehicle begins searching for charging stations |
