---
title: Statistic Output
---

# Instantiating within the Simulation

Statistic output is activated by setting the simulation option **--statistic-output** {{DT_FILE}} on the command line or in a *.sumocfg* file.

!!! note
    The elements `vehicleTripStatistics`, `pedestrianStatistics`, `rideStatistics` and `transportStatistics` are only generated when either of the options **--duration-log.statistics** or **--tripinfo-output** are set.

# Generated Output

The generated XML file looks like this:

```xml
<statistics>
    <vehicles ''ATTRIBUTES''.../>
    <teleports ''ATTRIBUTES''.../>
    <safety ''ATTRIBUTES''.../>
    <persons ''ATTRIBUTES''.../>
    <vehicleTripStatistics ''ATTRIBUTES''.../>
    <pedestrianStatistics ''ATTRIBUTES''.../>
    <rideStatistics ''ATTRIBUTES''.../>
    <transportStatistics ''ATTRIBUTES''.../>
</statistics>
```

The following output attributes are generated:

## vehicles

| Attribute Name | Value Type | Description                                                                  |
| -------------- | ---------- | ---------------------------------------------------------------------------- |
| **loaded**     | #          | Number of vehicles that were loaded into the simulation                      |
| **inserted**   | #          | Number of vehicles inserted                                                 |
| **running**    | #          | Number of vehicles that were running at simulation end                                        |
| **waiting**    | #          | Number of vehicles with delayed insertion that were still waiting for insertion at simulation end |


## teleports

| Attribute Name  | Value Type | Description                                                                                                                        |
| --------------- | ---------- | ---------------------------------------------------------------------------------------------------------------------------------- |
| **total**       | #          | The total number of teleportations that occured                                                                                    |
| **jam**         | #          | Number of teleportations due to traffic jam                                                                                        |
| **yield**       | #          | Number of teleportations due to yield (vehicle is stuck on a low-priority road and did not find a gap in the prioritized traffic)  |
| **wrongLane**   | #          | Number of teleportations due to the vehicle being stuck on a lane which has no connection to the next edge on its route            |


## safety

| Attribute Name      | Value Type | Description                            |
| ------------------- | ---------- | -------------------------------------- |
| **collisions**      | #          | Number of collisions that occurred     |
| **emergencyStops**  | #          | Number of emergency stops performed    |


## persons

| Attribute Name | Value Type | Description                                               |
| -------------- | ---------- | --------------------------------------------------------- |
| **loaded**     | #          | Number of persons that were loaded into the simulation    |
| **running**    | #          | Number of persons that were running                       |
| **jammed**     | #          | Number of persons that were jammed during the simulation  |


## vehicleTripStatistics

| Attribute Name         | Value Type | Description                                                                                 |
| ---------------------- | ---------- | ------------------------------------------------------------------------------------------- |
| **routeLength**        | m          | The average route length                                                                    |
| **speed**              | m/s        | The average trip speed                                                                      |
| **duration**           | s          | The average trip duration                                                                   |
| **waitingTime**        | s          | The average time spent standing (involuntarily)                                             |
| **timeLoss**           | s          | The average time lost due to driving slower than desired (includes waitingTime)             |
| **departDelay**        | s          | The average time vehicles had to wait before starting their journeys                        |
| **departDelayWaiting** | s          | The average waiting time of vehicles which could not be inserted due to lack of road space  |
| **totalTravelTime**    | s          | The total travel time of all vehicles |
| **totalDepartDelay**   | s          | The total depart delay of all vehicles  |


## pedestrianStatistics

| Attribute Name  | Value Type | Description                                                           |
| --------------- | ---------- | --------------------------------------------------------------------- |
| **number**      | #          | Total number of pedestrian walks                                      |
| **routeLength** | m          | The average walk length                                               |
| **duration**    | s          | The average walk duration                                             |
| **timeLoss**    | s          | The average time lost due to walking below maximum speed or stopping  |


## rideStatistics

| Attribute Name   | Value Type | Description                                                                 |
| ---------------- | ---------- | --------------------------------------------------------------------------- |
| **number**       | #          | Total number of rides (Rides are movement using a single mode of transport) |
| **routeLength**  | m          | The average ride route length                                               |
| **duration**     | s          | The average total ride duration                                             |
| **bus**          | #          | Number of rides with a public transport vehicle driving on roads            |
| **train**        | #          | Number of rides with a public transport vehicle driving on rails            |
| **taxi**         | #          | Number of taxi rides                                                        |
| **bike**         | #          | Number of rides with vehicle class *bicycle*                                |
| **aborted**      | #          | Number of rides that could not be completed                                 |


## transportStatistics

| Attribute Name   | Value Type | Description                                                               |
| ---------------- | ---------- | --------------------------------------------------------------------------|
| **number**       | #          | Total number of transports (Transports involve movement of containers)    |
| **routeLength**  | m          | The average transport route length                                        |
| **duration**     | s          | The average total transport duration                                      |
| **bus**          | #          | Number of transports with a public transport vehicle driving on roads     |
| **train**        | #          | Number of transports with a public transport vehicle driving on rails     |
| **taxi**         | #          | Number of taxi transports                                                 |
| **bike**         | #          | Number of transports with vehicle class *bicycle*                         |
| **aborted**      | #          | Number of transports that could not be completed                          |

# Usage examples

## Fair Traveltime comparison between simulations
When comparing simulations with a fixed end-time, those simulations may differ in the number of departed and arrived vehicles. In this case, care must be taken to include the "missing" vehicles in a comparison, since many outputs only include arrived vehicles by default.
To include statistics for these vehicles, the options **--tripinfo-output.write-unfinished --duration-log.statistics** must be set.
The general idea is to add up the travel time (duration) and the time that was spent waiting for departure (departDelay) for all vehicles that were defined in the input.

To simplify comparison between simulations that have the same number of vehiclles, the attributes 'totalTravelTime' and 'totalDepartDelay' are provided.

An alternative way to compute the sum of travel time and delays is to multiply the averages:

```
totalTravelTimeAndDelay = 
     vehicles.inserted * (vehicleTripStatistics.duration + vehicleTripStatistics.departDelay)
   + vehicles.waiting * vehicleTripStatistics.departDelayWaiting
```
