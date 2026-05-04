---
title: Simulation Value Retrieval
---

## Command 0xab: Get Simulation Variable

|  ubyte   |            string             |
| :------: | :---------------------------: |
| Variable | Simulation ID (always unused) |

Asks for the value of a certain simulation variable

The following variable values can be retrieved and subscribed to.
The type of the return
value is also shown in the table. It is not possible to subscribe to
position conversion (0x82), distance request (0x83), find route (0x86),
find intermodal route (0x87) and all bus stop functions.

### Overview Retrievable Simulation Variables

| Variable                                                        | ValueType  | Description                                                                                                                                                                                                  | [Python Method](../TraCI/Interfacing_TraCI_from_Python.md)                                                                                          |
| --------------------------------------------------------------- | ---------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ | --------------------------------------------------------------------------------------------------------------------------------------------------- |
| current simulation time (0x66)                                  | double     | Returns the current simulation time (in s)                                                                                                                                                                   | [getTime](https://sumo.dlr.de/pydoc/traci/_simulation.html#SimulationDomain.getTime)                                                               |
| current simulation time (0x70) (deprecated)                     | int        | Returns the current simulation time (in ms)                                                                                                                                                                  | [getCurrentTime](https://sumo.dlr.de/pydoc/traci/_simulation.html#SimulationDomain.getCurrentTime)                                                 |
| number of loaded vehicles (id 0x71)                             | int        | The number of vehicles which were loaded in this time step.                                                                                                                                                  | [getLoadedNumber](https://sumo.dlr.de/pydoc/traci/_simulation.html#SimulationDomain.getLoadedNumber)                                               |
| ids of loaded vehicles (id 0x72)                                | stringList | A list of ids of vehicles which were loaded in this time step.                                                                                                                                               | [getLoadedIDList](https://sumo.dlr.de/pydoc/traci/_simulation.html#SimulationDomain.getLoadedIDList)                                               |
| number of departed vehicles (id 0x73)                           | int        | The number of vehicles which departed (were inserted into the road network) in this time step.                                                                                                               | [getDepartedNumber](https://sumo.dlr.de/pydoc/traci/_simulation.html#SimulationDomain.getDepartedNumber)                                           |
| ids of departed vehicles (id 0x74)                              | stringList | A list of ids of vehicles which departed (were inserted into the road network) in this time step.                                                                                                            | [getDepartedIDList](https://sumo.dlr.de/pydoc/traci/_simulation.html#SimulationDomain.getDepartedIDList)                                           |
| number of vehicles <br/> that start to teleport (id 0x75)             | int        | The number of vehicles which started to teleport in this time step.                                                                                                                                          | [getStartingTeleportNumber](https://sumo.dlr.de/pydoc/traci/_simulation.html#SimulationDomain.getStartingTeleportNumber)                           |
| ids of vehicles <br/> that start to teleport (id 0x76)                | stringList | A list of ids of vehicles which started to teleport in this time step.                                                                                                                                       | [getStartingTeleportIDList](https://sumo.dlr.de/pydoc/traci/_simulation.html#SimulationDomain.getStartingTeleportIDList)                           |
| number of vehicles <br/> that end being teleported (id 0x77)          | int        | The number of vehicles which ended to be teleported in this time step.                                                                                                                                       | [getEndingTeleportNumber](https://sumo.dlr.de/pydoc/traci/_simulation.html#SimulationDomain.getEndingTeleportNumber)                               |
| ids of vehicles <br/> that end being teleported (id 0x78)             | stringList | A list of ids of vehicles which ended to be teleported in this time step.                                                                                                                                    | [getEndingTeleportIDList](https://sumo.dlr.de/pydoc/traci/_simulation.html#SimulationDomain.getEndingTeleportIDList)                               |
| number of arrived vehicles (id 0x79)                            | int        | The number of vehicles which arrived (have reached their destination and are removed from the road network) in this time step.                                                                               | [getArrivedNumber](https://sumo.dlr.de/pydoc/traci/_simulation.html#SimulationDomain.getArrivedNumber)                                             |
| ids of arrived vehicles (id 0x7a)                               | stringList | A list of ids of vehicles which arrived (have reached their destination and are removed from the road network) in this time step.                                                                            | [getArrivedIDList](https://sumo.dlr.de/pydoc/traci/_simulation.html#SimulationDomain.getArrivedIDList)                                             |
| network bounding box (id 0x7c)                                  | 2D polygon | The lower left and the upper right corner of the bounding box of the simulation network.                                                                                                                     | [getNetBoundary](https://sumo.dlr.de/pydoc/traci/_simulation.html#SimulationDomain.getNetBoundary)                                                 |
| minimum number of vehicles <br/> that are still expected to leave the net (id 0x7d) | int | The number of vehicles which are in the net plus the ones still waiting to start. This number may be smaller than the actual number of vehicles still to come because of delayed route file parsing. | [getMinExpectedNumber](https://sumo.dlr.de/pydoc/traci/_simulation.html#SimulationDomain.getMinExpectedNumber)                                     |
| number of vehicles <br/> that begin a stop (id 0x68)                  | int        | The number of vehicles that halted on a scheduled stop in this time step.                                                                                                                                    | [getStopStartingVehiclesNumber](https://sumo.dlr.de/pydoc/traci/_simulation.html#SimulationDomain.getStopStartingVehiclesNumber)                   |
| ids of vehicles <br/> that begin a stop (id 0x69)                     | stringList | A list of ids of vehicles that halted on a scheduled stop in this time step.                                                                                                                                 | [getStopStartingVehiclesIDList](https://sumo.dlr.de/pydoc/traci/_simulation.html#SimulationDomain.getStopStartingVehiclesIDList)                   |
| number of vehicles <br/> that end a stop (id 0x6a)                    | int        | The number of vehicles that begin to continue their journey, leaving a scheduled stop in this time step.                                                                                                     | [getStopEndingVehiclesNumber](https://sumo.dlr.de/pydoc/traci/_simulation.html#SimulationDomain.getStopEndingVehiclesNumber)                       |
| ids of vehicles <br/> that end a stop (id 0x6b)                       | stringList | A list of ids of vehicles that begin to continue their journey, leaving a scheduled stop in this time step.                                                                                                  | [getStopEndingVehiclesIDList](https://sumo.dlr.de/pydoc/traci/_simulation.html#SimulationDomain.getStopEndingVehiclesIDList)                       |
| number of vehicles <br/> involved in a collision (id 0x80)            | int        | The number of vehicles that were involved in a collision in this time step.                                                                                                                                  | [getCollidingVehiclesNumber](https://sumo.dlr.de/pydoc/traci/_simulation.html#SimulationDomain.getCollidingVehiclesNumber)                         |
| ids of vehicles <br/> involved in a collision (id 0x81)               | stringList | A list of ids of vehicles that were involved in a collision in this time step.                                                                                                                               | [getCollidingVehiclesIDList](https://sumo.dlr.de/pydoc/traci/_simulation.html#SimulationDomain.getCollidingVehiclesIDList)                         |
| number of vehicles <br/> that begin to be parked (id 0x6c)            | int        | The number of vehicles that enter a parking position in this time step.                                                                                                                                      | [getParkingStartingVehiclesNumber](https://sumo.dlr.de/pydoc/traci/_simulation.html#SimulationDomain.getParkingStartingVehiclesNumber)             |
| ids of vehicles <br/> that begin to be parked (id 0x6d)               | stringList | A list of ids of vehicles that enter a parking position in this time step.                                                                                                                                   | [getParkingStartingVehiclesIDList](https://sumo.dlr.de/pydoc/traci/_simulation.html#SimulationDomain.getParkingStartingVehiclesIDList)             |
| number of vehicles <br/> that end to be parked (id 0x6e)              | int        | The number of vehicles that begin to continue their journey, leaving a scheduled parking in this time step.                                                                                                  | [getParkingEndingVehiclesNumber](https://sumo.dlr.de/pydoc/traci/_simulation.html#SimulationDomain.getParkingEndingVehiclesNumber)                 |
| ids of vehicles <br/> that end being parked (id 0x6f)                 | stringList | A list of ids of vehicles that begin to continue their journey, leaving a scheduled parking in this time step.                                                                                               | [getParkingEndingVehiclesIDList](https://sumo.dlr.de/pydoc/traci/_simulation.html#SimulationDomain.getParkingEndingVehiclesIDList)                 |
| bus stop waiting (id 0x67)                                      | int        | Get the total number of waiting persons at the named bus stop (deprecated, use busstop.getPersonCount instead).                                                                                              | [getBusStopWaiting](https://sumo.dlr.de/pydoc/traci/_simulation.html#SimulationDomain.getBusStopWaiting)                                           |
| bus stop waiting ids (id 0xef)                                  | stringList | Get the ids of waiting persons at the named bus stop (deprecated, use busstop.getPersonIDs instead).                                                                                                        | [getBusStopWaitingIDList](https://sumo.dlr.de/pydoc/traci/_simulation.html#SimulationDomain.getBusStopWaitingIDList)                               |
| delta T (id 0x7b)                                               | double     | Returns the length of one simulation step in seconds.                                                                                                                                                        | [getDeltaT](https://sumo.dlr.de/pydoc/traci/_simulation.html#SimulationDomain.getDeltaT)                                                           |
| parameter (0x7e)                                                | string     | Returns the value for the given string parameter.                                                                                                                                                            | [getParameter](https://sumo.dlr.de/pydoc/traci/_simulation.html#SimulationDomain.getParameter)                                                     |
| scale (id 0x8e)                                                 | double     | Returns the traffic scaling factor.                                                                                                                                                                          | [getScale](https://sumo.dlr.de/pydoc/traci/_simulation.html#SimulationDomain.getScale)                                                             |
| option (0x3f)                                                   | string     | Returns the value of one of the global SUMO options.                                                                                                                                                         | [getOption](https://sumo.dlr.de/pydoc/traci/_simulation.html#SimulationDomain.getOption)                                                           |

## Response 0xbb: Simulation Variable

|  ubyte   |  string   |            ubyte            |  <return_type\>   |
| :------: | :-------: | :-------------------------: | :--------------: |
| Variable | <invalid\> | Return type of the variable | <VARIABLE_VALUE\> |

The respond to a **"Command Get Simulation Variable"**.

## Extended retrieval messages

Some further messages require additional parameters.

<center>
**Overview Extended Variables Retrieval**
</center>

| Variable                      | Request ValueType      | Response ValueType   | Description                                                                                                                                                          | [Python Method](../TraCI/Interfacing_TraCI_from_Python.md)                                                                                                                                                                                                   |
| ----------------------------- | ---------------------- | -------------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| position conversion (0x82)    | compound, see below    | Position, as wished  | Reads a position information and returns it converted into the given representation.                                                                                 | [convert2D](https://sumo.dlr.de/pydoc/traci/_simulation.html#SimulationDomain.convert2D) [convert3D](https://sumo.dlr.de/pydoc/traci/_simulation.html#SimulationDomain.convert3D) [convertGeo](https://sumo.dlr.de/pydoc/traci/_simulation.html#SimulationDomain.convertGeo) [convertRoad](https://sumo.dlr.de/pydoc/traci/_simulation.html#SimulationDomain.convertRoad) |
| distance request (0x83)       | compound, see below    | double               | Reads two positions and an indicator whether the air or the driving distance shall be computed. Returns the according distance.                                       | [getDistanceRoad](https://sumo.dlr.de/pydoc/traci/_simulation.html#SimulationDomain.getDistanceRoad) [getDistance2D](https://sumo.dlr.de/pydoc/traci/_simulation.html#SimulationDomain.getDistance2D)                                                       |
| find route (0x86)             | compound, see below    | compound, see below  | Reads origin and destination edge together with some vehicle parameters and computes the currently fastest driving route for the vehicle (for pedestrians / passengers use find intermodal route). | [findRoute](https://sumo.dlr.de/pydoc/traci/_simulation.html#SimulationDomain.findRoute)                                                                                                                                                    |
| find intermodal route (0x87)  | compound, see below    | compound, see below  | Reads origin and destination position together with usable modes and other person parameters and computes the currently fastest route for the person using the available modes. | [findIntermodalRoute](https://sumo.dlr.de/pydoc/traci/_simulation.html#SimulationDomain.findIntermodalRoute)                                                                                                                                  |
| get collisions (0x23)         | -                      | compound, see below  | Retrieves details on all collision events in the last time step. Collision detection is disabled by default (**--collision.action none**).                            | [getCollisions](https://sumo.dlr.de/pydoc/traci/_simulation.html#SimulationDomain.getCollisions)                                                                                                                                                            |

### Command 0x82: Position Conversion

The request needs the following parameter:

| byte                | integer                | *Position*            | byte             | ubyte                          |
|---------------------|------------------------|---------------------|------------------|--------------------------------|
| value type compound | item number (always 2) | position to convert | value type *ubyte* | Type of the Position to return |

Use this command to convert between the different position types used by
TraCI. The specified *Position* is converted into the requested format
given through *PositionId*. When converting from coordinates to road
positions, the result is the closest position that was found to match
the given one. In all other cases, the conversion will give an exact
match. The return values consists of the converted value only.

To simplify position conversions, a number of convenience functions are provided by [many traci client libraries](../TraCI/index.md#interfaces_by_programming_language),and [libsumo](../Libsumo.md):

- [convert2D](https://sumo.dlr.de/pydoc/traci/_simulation.html#SimulationDomain.convert2D): convert road,laneIndex,offset to x,y or lon,lat
- [convert3D](https://sumo.dlr.de/pydoc/traci/_simulation.html#SimulationDomain.convert3D): convert road,laneIndex,offset to x,y,z or lon,lat
- [convertGeo](https://sumo.dlr.de/pydoc/traci/_simulation.html#SimulationDomain.convertGeo): convert x,y to lon,lat or vice versa
- [convertRoad](https://sumo.dlr.de/pydoc/traci/_simulation.html#SimulationDomain.convert3D): convert x,y or lon,lat to road,laneInex,offset

See [TraCI/Protocol\#Position
Representations](../TraCI/Protocol.md#position_representations) for
available position formats.

### Command 0x83: Distance Request

The request needs the following parameter:

|         byte          |        integer         |   *Position*   |  *Position*  |     ubyte     |
| :-------------------: | :--------------------: | :------------: | :----------: | :-----------: |
| value type *compound* | item number (always 3) | start position | end position | Distance Type |

where

- Distance Type==0: air distance
- Distance Type==1: driving distance

The resulting distance is returned.

### Command 0x86: Find Route

|         byte          |        integer         |        byte         |   string   |        byte         |      string      |        byte         |    string    |        byte         |       double       |         byte         |         integer          |
| :-------------------: | :--------------------: | :-----------------: | :--------: | :-----------------: | :--------------: | :-----------------: | :----------: | :-----------------: | :----------------: | :------------------: | :----------------------: |
| value type *compound* | item number (always 5) | value type *string* | start edge | value type *string* | destination edge | value type *string* | vehicle type | value type *double* | departure time (s) | value type *integer* | routing mode (see below) |

!!! caution
    Routing mode shall be used to specify which data source to use for estimation of edge travel times. This is not yet implemented and the same data sources as for command [vehicle.rerouteTraveltime is used](../Simulation/Routing.md#travel-time_values_for_routing).

The following response is returned:

|      type + string      |  type + string   | type + stringList |  type + double  |  type + double  |
| :---------------------: | :--------------: | :---------------: | :-------------: | :-------------: |
| type (always "driving") | line (always "") |       edges       | travel time (s) | travel time (s) |

!!! note
    The python and C++ client return a namedtuple / struct with field names *stageType, line, destStop, edges, travelTime, cost*.

!!! note
    The seemingly unused/redundant fields are reserved for route-by-cost and intermodal routing (see below).

### Command 0x87: Find Intermodal Route

|         byte          |         integer         |        byte         |   string   |        byte         |      string      |        byte         |     string      |        byte         |       double       |         byte         |         integer          |        byte         |       double        |        byte         |   double    |        byte         |       double        |        byte         |        double        |        byte         |      double      |        byte         |        string        |        byte         |         string          |
| :-------------------: | :---------------------: | :-----------------: | :--------: | :-----------------: | :--------------: | :-----------------: | :-------------: | :-----------------: | :----------------: | :------------------: | :----------------------: | :-----------------: | :-----------------: | :-----------------: | :---------: | :-----------------: | :-----------------: | :-----------------: | :------------------: | :-----------------: | :--------------: | :-----------------: | :------------------: | :-----------------: | :---------------------: |
| value type *compound* | item number (always 12) | value type *string* | start edge | value type *string* | destination edge | value type *string* | available modes | value type *double* | departure time (s) | value type *integer* | routing mode (see above) | value type *double* | walking speed (m/s) | value type *double* | walk factor | value type *double* | depart position (m) | value type *double* | arrival position (m) | value type *double* | departPosLat (m) | value type *string* | vtype of the vehicle | value type *string* | vtype of the pedestrian |

The available modes are "car", "public", "bicycle" or space separated
combination. Walking is always considered possible and is the default
(or used when giving an empty string). When "car" is an available mode
the vehicle type can be given using the vtype parameter. If this is
empty, the default vehicle type is used.

The response is a list of stages similar to the one returned by
findRoute.

### Command 0x23: Get Collisions

The response is a list of Collision objects:

| type + string | type + string | type + string | type + string |  type + double | type + double | type + string | type + string |type + double |
| :-----------: | :-----------: | :-----------: | :-----------: | :------------: | :-----------: |:------------: |:------------: |:-----------: |
|  collider id  | victim id     | collider type | victim type   | collider speed | victim speed  | collision type| lane id       | pos on lane  |

## Generic Parameter Retrieval 0x7e

The simulation supports retrieval of additional object parameters using
the [generic parameter retrieval call](../TraCI/GenericParameters.md#get_parameter).

### Stoppping Place Information

The *object id* of `getParameter` refers to the object (i.e. chargingStation) id. The following parameters
are supported:

- chargingStation.totalEnergyCharged
- chargingStation.name
- chargingStation.lane
- parkingArea.capacity
- parkingArea.occupancy
- parkingArea.name
- parkingArea.lane
- busStop.name
- busStop.lane

!!! note
    Since version 1.7.0 these and other values can be retrieved via new domains (i.e. `traci.busstop`)


### Device Parameter Retrieval

The following parameters can be retrieved. They are not for a specific device holder but are instead global measures. The *object id* must be the empty string "".

- device.tripinfo.count
- device.tripinfo.routeLength
- device.tripinfo.speed
- device.tripinfo.duration
- device.tripinfo.waitingTime
- device.tripinfo.timeLoss
- device.tripinfo.departDelay
- device.tripinfo.departDelayWaiting
- device.tripinfo.totalTravelTime
- device.tripinfo.totalDepartDelay
- device.tripinfo.vehicleTripStatistics.count
- device.tripinfo.vehicleTripStatistics.routeLength
- device.tripinfo.vehicleTripStatistics.speed
- device.tripinfo.vehicleTripStatistics.duration
- device.tripinfo.vehicleTripStatistics.waitingTime
- device.tripinfo.vehicleTripStatistics.timeLoss
- device.tripinfo.vehicleTripStatistics.departDelay
- device.tripinfo.vehicleTripStatistics.departDelayWaiting
- device.tripinfo.vehicleTripStatistics.totalTravelTime
- device.tripinfo.vehicleTripStatistics.totalDepartDelay
- device.tripinfo.bikeTripStatistics.count
- device.tripinfo.bikeTripStatistics.routeLength
- device.tripinfo.bikeTripStatistics.speed
- device.tripinfo.bikeTripStatistics.duration
- device.tripinfo.bikeTripStatistics.waitingTime
- device.tripinfo.bikeTripStatistics.timeLoss
- device.tripinfo.bikeTripStatistics.totalTravelTime
- device.tripinfo.pedestrianStatistics.count
- device.tripinfo.pedestrianStatistics.number
- device.tripinfo.pedestrianStatistics.routeLength
- device.tripinfo.pedestrianStatistics.duration
- device.tripinfo.pedestrianStatistics.timeLoss
- device.tripinfo.rideStatistics.count
- device.tripinfo.rideStatistics.number
- device.tripinfo.rideStatistics.waitingTime
- device.tripinfo.rideStatistics.routeLength
- device.tripinfo.rideStatistics.duration
- device.tripinfo.rideStatistics.bus
- device.tripinfo.rideStatistics.train
- device.tripinfo.rideStatistics.taxi
- device.tripinfo.rideStatistics.bike
- device.tripinfo.rideStatistics.aborted
- device.tripinfo.transportStatistics.count
- device.tripinfo.transportStatistics.number
- device.tripinfo.transportStatistics.waitingTime
- device.tripinfo.transportStatistics.routeLength
- device.tripinfo.transportStatistics.duration
- device.tripinfo.transportStatistics.bus
- device.tripinfo.transportStatistics.train
- device.tripinfo.transportStatistics.taxi
- device.tripinfo.transportStatistics.bike
- device.tripinfo.transportStatistics.aborted

### Statistics Parameter Retrieval

The retrievable values are those described at [statistic-output](../Simulation/Output/StatisticOutput.md) as applicable to the current simulation time.

- stats.vehicles.loaded
- stats.vehicles.inserted
- stats.vehicles.running
- stats.vehicles.waiting
- stats.teleports.total
- stats.teleports.jam
- stats.teleports.yield
- stats.teleports.wrongLane
- stats.safety.collisions
- stats.safety.emergencyStops
- stats.safety.emergencyBraking
- stats.persons.loaded
- stats.persons.running
- stats.persons.jammed
- stats.personTeleports.total
- stats.personTeleports.abortWait
- stats.personTeleports.wrongDest
