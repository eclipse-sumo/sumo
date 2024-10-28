---
title: Change Vehicle State
---

# Command 0xc4: Change Vehicle State

|  ubyte   |   string   |       ubyte       | <value_type\> |
| :------: | :--------: | :---------------: | :----------: |
| Variable | Vehicle ID | Type of the value |  New Value   |

Changes the state of a vehicle. Because it is possible to change
different values of a vehicle, the number of parameter to supply and
their types differ between commands. The following values can be
changed, the parameter which must be given are also shown in the table.
Furthermore it is possible to change all values mentioned in
[TraCI/Change_VehicleType_State](../TraCI/Change_VehicleType_State.md).
If you do so, the vehicle gets a new type (named "typeid\@vehid") and
won't be affected by further changes to the original type.

<center>
**Overview Changeable Vehicle Variables**
</center>

| Variable | ValueType | Description | [Python Method](../TraCI/Interfacing_TraCI_from_Python.md) |
|---------------------------|----------------------------------------------------------------|--------------------------------------------------------------------------------------------------------------------------------------------------------------------|---------------------|
| stop (0x12) | compound (string, double, byte, double), see below | Lets the vehicle stop at the given edge, at the given position and lane. The vehicle will stop for the given duration. Re-issuing a stop command with the same lane and position allows changing the duration. Setting the duration to 0 cancels an existing stop.  | [setStop](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-setStop)<br>[setBusStop](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-setBusStop)<br>[setContainerStop](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-setContainerStop)<br>[setChargingStationStop](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-setChargingStationStop)<br>[setParkingAreaStop](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-setParkingAreaStop) |
| change lane (0x13) | compound (byte, double), see below | Forces a lane change to the lane with the given index; if successful, the lane will be chosen for the given amount of time (in seconds). | [changeLane](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-changeLane) |
| change sublane (0x15) | double (lateral distance) | Forces a lateral change by the given amount (negative values indicate changing to the right, positive to the left). This will override any other lane change motivations but conform to safety-constraints as configured by laneChangeMode.  | [changeSublane](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-changeSublane) |
| slow down (0x14) | compound (double, double), see below | Changes the speed smoothly to the given value over the given amount of time in seconds (can also be used to increase speed). | [slowDown](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-slowDown) |
| resume (0x19) | compound (), see below | Resumes from a stop  | [resume](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-resume) |
| change target (0x31) | string (destination edge id)  | The vehicle's destination edge is set to the given. The route is rebuilt. | [changeTarget](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-changeTarget) |
| speed (0x40) | double (new speed) | Sets the vehicle speed to the given value. The speed will be followed according to the current [speed mode](#speed_mode_0xb3). By default the vehicle may drive slower than the set speed according to the safety rules of the car-follow model. When sending a value of -1 the vehicle will revert to its original behavior (using the *maxSpeed* of its vehicle type and following all safety rules).  | [setSpeed](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-setSpeed) |
| acceleration (0x72) | compound (double, double), see below | Changes the acceleration to the given value for the given amount of time in seconds. | [setAcceleration](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-setAcceleration) |
| previous speed (0x3c) | double (previous speed) | Retroactively sets the vehicle speed for the previous simulation step to the given value and also updates the previous acceleration. This speed value will be used when computing [ballistic position updates](../Simulation/Basic_Definition.md#defining_the_integration_method). Also, other vehicles will react to ego in the current step as if it had driven with the given speed in the previous step. | [setPreviousSpeed](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-setPreviousSpeed) |
| color (0x45)  | ubyte,ubyte,ubyte,ubyte (RGBA) |  Sets the vehicle's color.  | [setColor](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-setColor) |
| change route by id (0x53) | string (route id)  | Assigns the named route to the vehicle, assuming a) the named route exists, and b) it starts on the edge the vehicle is currently at<sup>(1)(2)</sup>.  | [setRouteID](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-setRouteID) |
| change route (0x57)  | stringList (ids of edges to pass) | Assigns the list of edges as the vehicle's new route assuming the first edge given is the one the vehicle is currently at<sup>(1)(2)</sup>.  | [setRoute](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-setRoute) |
| reroute parking area (0xc2) | string (parking area id) | Changes the next parking area in parkingAreaID, updates the vehicle route, and preserve consistency in case of passengers/containers on board. | [rerouteParkingArea](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-rerouteParkingArea) |
| dispatchTaxi | stringList (reservation ids)  | dispatches the taxi with the given id to service the given reservations. If only a single reservation is given, this implies pickup and drop-off If multiple reservations are given, each reservation id must occur twice (once for pickup and once for drop-off) and the list encodes ride sharing of passengers (in pickup and drop-off order)  | [dispatchTaxi](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-dispatchTaxi) |
| change edge travel time information (0x58)  | compound (begin time, end time, edgeID, value), see below  | Inserts the information about the travel time (in seconds) of edge "edgeID" valid from begin time to end time (in seconds) into the vehicle's internal edge weights container.  | [setAdaptedTraveltime](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-setAdaptedTraveltime) |
| change edge effort information (0x59)  | compound (begin time, end time, edgeID, value), see below  | Inserts the information about the effort of edge "edgeID" valid from begin time to end time (in seconds) into the vehicle's internal edge weights container.  | [setEffort](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-setEffort) |
| signal states (0x5b)  |  	int  |  	Sets a new state of signal. See [TraCI/Vehicle Signalling](../TraCI/Vehicle_Signalling.md) for more information.  |  	[setSignals](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-setSignals) |
| routing mode (0x89)  |  	int  |  	Sets the [routing mode](../Simulation/Routing.md#travel-time_values_for_routing) (0: default, 1: aggregated, 8: ignore rerouter changes, 9: aggregated and ignoring rerouter changes)  |  [setRoutingMode](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-setRoutingMode) |
| move to (0x5c)  | compound (lane ID, position along lane)  | Moves the vehicle to a new position along the current route <sup>(3)</sup>.  | [moveTo](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-moveTo) |
| move to XY (0xb4) | compound (edgeID, laneIndex, x, y, angle, keepRoute) (see below)  | Moves the vehicle to a new position after normal vehicle movements have taken place. Also forces the angle of the vehicle to the given value (navigational angle in degree). [See below for additional details](#move_to_xy_0xb4) | [moveToXY](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-moveToXY) |
| replaceStop (0x17) | compound (edgeID, vehID, nextStopIndex, edgeID, pos, laneIndex, duration, flags, startPos, until, teleport) (see below)  | Replaces stop at the given index with a new stop. Automatically modifies the route if the replacement stop is at another location. [See below for additional details](#replacestop_0x17) | [replaceStop](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-replaceStop) |
| insertStop (0x18) | compound (edgeID, vehID, nextStopIndex, edgeID, pos, laneIndex, duration, flags, startPos, until, teleport) (see below)  | inserts stop at the given index. Automatically modifies the route to accommodate the new stop. [See below for additional details](#insertstop_0x18) | [insertStop](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-insertStop) |
| setStopParameter (0x55) | compound (nextStopIndex, param, value, customParam) (see below)  | modifies attribute of stop at the given index. Changing location ('edge', 'busStop', etc.) behave like replaceRoute. If customParam is set to 1, modifies a user defined param instead.[See below for additional details](#setstopparameter_0x55) | [setStopParameter](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-setStopParameter) |
| reroute (compute new route) by travel time (0x90)  | compound (<empty\>), see below  | Computes a new route to the current destination that minimizes travel time. The assumed values for each edge in the network can be customized in various ways. See [Simulation/Routing#Travel-time_values_for_routing](../Simulation/Routing.md#travel-time_values_for_routing). Replaces the current route by the found<sup>(2)</sup>.  | [rerouteTraveltime](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-rerouteTraveltime) |
| reroute (compute new route) by effort (0x91) | compound (<empty\>), see below  | Computes a new route using the vehicle's internal and the global edge effort information. Replaces the current route by the found<sup(2)</sup>. | [rerouteEffort](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-rerouteEffort) |
| speed mode (0xb3) | int bitset (see below)  | Sets how the values set by speed (0x40) and slowdown (0x14) shall be treated. Also allows to configure the behavior at junctions. [See below](#speed_mode_0xb3).  | [setSpeedMode](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-setSpeedMode) |
| speed factor (0x5e)  |  double  | Sets the vehicle's speed factor to the given value | [setSpeedFactor](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-setSpeedFactor) |
| max speed (0x41)  | double  | Sets the vehicle's maximum speed to the given value  | [setMaxSpeed](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-setMaxSpeed) |
| lane change mode (0xb6)  | int bitset (see [below](#lane_change_mode_0xb6))  | Sets how lane changing in general and lane changing requests by TraCI are performed. See below.  | [setLaneChangeMode](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-setLaneChangeMode) |
| update bestLanes (0x6a)  | | updates internal data structures for strategic lane choice. (e.g. after modifying access permissions).<br><br>**Note:** This happens automatically when changing the route or moving to a new edge | [updateBestLanes](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-updateBestLanes) |
| add (0x85)                | complex (see below)                                            | Adds the defined vehicle. See below.                                                                                                                               | [add](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-add) (alias addFull) |
| add_legacy (0x80)         | complex (see below)                                            | Adds the defined vehicle (fewer parameters, obsolete). See below.                                                                                                  | [addLegacy](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-addFull)           |
| remove (0x81)             | complex (see below)                                            | Removes the defined vehicle. See below.                                                                                                                            | [remove](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-remove)              |
| length (0x44)             | double                                                         | Sets the vehicle's length to the given value                                                                                                                       | [setLength](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-setLength)           |
| vehicle class (0x49)      | string                                                         | Sets the vehicle's vehicle class to the given value                                                                                                                | [setVehicleClass](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-setVehicleClass)     |
| emission class (0x4a)     | string                                                         | Sets the vehicle's emission class to the given value                                                                                                               | [setEmissionClass](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-setEmissionClass)    |
| width (0x4d)              | double                                                         | Sets the vehicle's width to the given value                                                                                                                        | [setWidth](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-setWidth)            |
| height (0xbc)             | double                                                         | Sets the vehicle's height to the given value                                                                                                                       | [setHeight](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-setHeight)           |
| min gap (0x4c)            | double                                                         | Sets the vehicle's minimum headway gap to the given value                                                                                                          | [setMinGap](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-setMinGap)           |
| shape class (0x4b)        | string                                                         | Sets the vehicle's shape class to the given value                                                                                                                  | [setShapeClass](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-setShapeClass)       |
| acceleration (0x46)       | double                                                         | Sets the vehicle's wished maximum acceleration to the given value                                                                                                  | [setAccel](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-setAccel)            |
| deceleration (0x47)       | double                                                         | Sets the vehicle's wished maximum deceleration to the given value                                                                                                  | [setDecel](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-setDecel)            |
| imperfection (0x5d)       | double                                                         | Sets the vehicle's driver imperfection (sigma) to the given value                                                                                                  | [setImperfection](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-setImperfection)     |
| tau (0x48)                | double                                                         | Sets the vehicle's wished headway time to the given value.                                                                                                         | [setTau](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-setTau)              |
| type (0x4f)               | string                                                         | Sets the id of the type for the named vehicle.                                                                                                                     | [setType](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-setType)             |
| via (0xbe)                | stringList                                                     | Changes the via edges to the given edges list (to be used during subsequent rerouting calls).                                                                      | [setVia](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-setVia)              |
| max lateral speed (0xba)  | double                                                         | Sets the maximum lateral speed in m/s for this vehicle.                                                                                                            | [setMaxSpeedLat](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-setMaxSpeedLat)      |
| lateral gap (0xbb)        | double                                                         | Sets the minimum lateral gap of the vehicle at 50km/h in m.                                                                                                        | [setMinGapLat](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-setMinGapLat)        |
| lateral alignment (0xb9)  | string                                                         | Sets the preferred lateral alignment for this vehicle.                                                                                                             | [setLateralAlignment](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-setLateralAlignment) |
| boarding duration (0x2f)        | double   | Sets the boarding duration for passengers entering/leaving this vehicle.                                                                                                   | [setBoardingDuration](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-setBoardingDuration)        |
| impatience (0x26)  | string      | sets the current [dynamic impatience](../Definition_of_Vehicles%2C_Vehicle_Types%2C_and_Routes.md#impatience) of this vehicle. This value gets reset when the vehicle starts driving.                                                                                                             | [setImpatience](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-setImpatience) |
| parameter (0x7e)          | string, string                                                 | [Sets the string value for the given string parameter](#setting_device_and_lanechangemodel_parameters_0x7e)                                                                                                               | [setParameter](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-setParameter)        |
| action step length (0x7d) | double (new action step length), boolean (reset action offset) | Sets the current action step length for the vehicle in s. If the boolean value resetActionOffset is true, an action step is scheduled immediately for the vehicle. | [setActionStepLength](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-setActionStepLength) |
| highlight (0x6c)          | highlight specification, see below                             | Adds a highlight to the vehicle                                                                                                                                    | [highlight](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-highlight)           |
| mass (0xc8)          | double                             | Sets the mass of the vehicle to the given value                                                                                                                           | [setMass](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-setMass)           |

Please note:

1.  The first occurrence of the edge is currently at is searched within
    the new route; the vehicle continues the route from this point in
    the route from. If the edge the vehicle is currently does not exist
    within the new route, an error is generated.
2.  changing the route works only if the vehicle is not within an
    intersection.
3.  The specified lane must lie ahead of the vehicle on its current
    route. The next occurrence is used. Intermediate edges will count the
    vehicle when entering and exiting.

# compound message details

The message contents are as following:

### stop (0x12)

|         byte          |       integer        |        byte         | string  |        byte         |    double    |       byte        |    byte    |        byte         |       double        |             byte             |           int           |              byte              |     double     |              byte              |      double      |
| :-------------------: | :------------------: | :-----------------: | :-----: | :-----------------: | :----------: | :---------------: | :--------: | :-----------------: | :-----------------: | :--------------------------: | :---------------------: | :----------------------------: | :------------: | :----------------------------: | :--------------: |
| value type *compound* | item number (4 to 7) | value type *string* | Edge ID | value type *double* | end position | value type *byte* | Lane Index | value type *double* | Duration in seconds | value type *int* (optional) | stop flags: (see below) | value type *double* (optional) | start position | value type *double* (optional) | Until in seconds |

The stop flags are a bitset with the following additive components

- 1 : parking
- 2 : triggered
- 4 : containerTriggered
- 8 : busStop (Edge ID is re-purposed as busStop ID)
- 16 : containerStop (Edge ID is re-purposed as containerStop ID)
- 32 : chargingStation (Edge ID is re-purposed as chargingStation ID)
- 64 : parkingArea (Edge ID is re-purposed as parkingArea ID)

### change lane (0x13)

|         byte          |       integer        |       byte        |    byte    |        byte         |       double        |             byte             |             byte              |
| :-------------------: | :------------------: | :---------------: | :--------: | :-----------------: | :-----------------: | :--------------------------: | :---------------------------: |
| value type *compound* | item number (2 or 3) | value type *byte* | Lane Index | value type *double* | Duration in seconds | value type *byte* (optional) | bool for relative lane change |

Please note:
The duration for the lane change is the time the vehicle tries to perform the lane change(s). If the duration is too small to perform all lane changes then the vehicle stops changing lanes after the duration.
After the vehicle has successfully performed the lane change(s) it will remain on that lane for the remainder of the duration.

### slow down (0x14)

|         byte          |        integer         |        byte         | double |        byte         |       double        |
| :-------------------: | :--------------------: | :-----------------: | :----: | :-----------------: | :-----------------: |
| value type *compound* | item number (always 2) | value type *double* | Speed  | value type *double* | Duration in seconds |

### set acceleration (0x72)

|         byte          |        integer         |        byte         |    double    |        byte         |       double        |
| :-------------------: | :--------------------: | :-----------------: | :----------: | :-----------------: | :-----------------: |
| value type *compound* | item number (always 2) | value type *double* | Acceleration | value type *double* | Duration in seconds |

### open gap (0x16)

|         byte          |       integer        |        byte         |             double              |        byte         |              double              |        byte         |       double        |        byte         |   double   |        byte         |  double  |              byte              |        string        |
| :-------------------: | :------------------: | :-----------------: | :-----------------------------: | :-----------------: | :------------------------------: | :-----------------: | :-----------------: | :-----------------: | :--------: | :-----------------: | :------: | :----------------------------: | :------------------: |
| value type *compound* | item number (4 or 5) | value type *double* | Adapted time headway in seconds | value type *double* | Adapted space headway in seconds | value type *double* | Duration in seconds | value type *double* | changeRate | value type *double* | maxDecel | value type *string* (optional) | reference vehicle ID |

This command induces a temporary increase of the vehicles desired time
headway (car-following parameter tau) and specifies a minimal space
headway to keep, as well. The execution is separated into an adaptation
phase, where the headways are gradually altered at the specified rate.
As soon as the desired headways are established they are kept for the
specified duration. Afterwards they are reset to the original value.
Deactivation of the control can be achieved by sending
newHeadway=duration=maxDecel=-1. The desired time headway of the vehicle
is left unchanged if a value of -1 is send at the corresponding
position. If -1 is sent for duration, the largest possible time is set.
If -1 is sent for the maxDecel, no limitation of the induced braking
rate is applied. If the optional reference vehicle ID is specified, the
method does not use the current leader as a reference for the gap
creation but the specified vehicle.

!!! note
    When using the Euler integration method, the time headway control does not work properly for low speeds.

### move to (0x5c)

|         byte          |        integer         |        byte         | string  |        byte         |       double        |
| :-------------------: | :--------------------: | :-----------------: | :-----: | :-----------------: | :-----------------: |
| value type *compound* | item number (always 2) | value type *string* | Lane ID | value type *double* | Position along lane |

The vehicle will be removed from its lane and moved to the given position on the given lane. No collision checks are done, this means that moving the vehicle may cause a collisions or a situations leading to collision. The vehicle keeps its speed - in the next time step it is at given position + speed. Note that the lane must be a part of the route, this means it must be either a part of any of the edges within the vehicles route or an internal lane that connects route edges. To overcome this limitation, the route can be modified prior to calling moveTo.

!!! note
    This can also be used to force a vehicle into the network that [has been loaded](../Simulation/VehicleInsertion.md#loading) but could not depart due to having its departure lane blocked.

### move to XY (0xb4)

The vehicle (the center of its front bumper) is moved to the network
position that best matches the given x,y network coordinates.

The arguments edgeID and lane are optional and can be set to "" and -1 respectively if not known.
Their use is to resolve ambiguities when there are multiple roads on top of each other (i.e. at bridges) or to provide additional guidance on intersections (where internal edges overlap).
If the edgeID and lane are given, they are compared against the 'origID'-attribute of the road lanes (which may be set to provide a mapping to some other network such as OpenDRIVE) and if the attribute isn't set against the actual lane id.

The optional keepRoute flag is a bitset that influences
mapping as follows:

- **bit0** (keepRoute = 1 when only this bit is set)
  - **1**: The vehicle is mapped to the closest edge within its existing route.
           If no suitable position is found within 100m   mapping fails with an error.
  - **0**: The vehicle is mapped to the closest edge within the network.
           If that edge does not belong to the original route, the current route is replaced by a new
           route which consists of that edge only.
           If no suitable position is found within 100m mapping fails with an error.
           When using the [sublane model](../Simulation/SublaneModel.md) the best lateral position
           that is fully within the lane will be used. Otherwise, the vehicle  will drive in the center of the closest lane.
- **bit1** (keepRoute = 2 when only this bit is set)
  - **1**: The vehicle is mapped to the exact position in
  the network (including the exact lateral position). If that position
  lies outside the road network, the vehicle stops moving on its own
  accord until it is placed back into the network with another TraCI
  command. (if keeproute = 3, the position must still be within 100m of the vehicle route)
  - **0**: The vehicle is always on a road
- **bit2** (keepRoute = 4 when only this bit is set)
  - **1**: lane permissions are ignored when mapping
  - **0**: The vehicle is mapped only to lanes that allow its vehicle class

The angle value is assumed to be in navigational degrees (between 0 and
360 with 0 at the top, going clockwise). The angle is used when scoring
candidate mapping lanes to resolve ambiguities. Upon mapping the vehicle
is angle set to the given value for purposes of drawing and all outputs.
If the angle is set to the special TraCI constant
INVALID_DOUBLE_VALUE, the angle will not be used for scoring and the
vehicle will assume the angle of the best found lane. When mapping the
vehicle outside the road network, the angle will be computed from the
previous and the new position instead.

!!! note
    This function can also be used to force a vehicle into the network that [has been loaded](../Simulation/VehicleInsertion.md#loading) but could not depart due to having its departure lane blocked.

!!! caution
    When mapping a vehicle to an edge that is not currently on its route, the new route will consist of that edge only. Once the vehicle reaches the end of that edge it disappears unless receiving another moveToXY command in that simulation step. This means, vehicles may disappear when calling *traci.simulationStep* with arguments that cause SUMO to perform multiple steps.

|         byte          |       integer        |        byte         |                       string                       |       byte       |                        double                         | | | | | | | | |
| :-------------------: | :------------------: | :-----------------: | :------------------------------------------------: | :--------------: | :---------------------------------------------------: | :-: |:-: |:-: |:-: |:-: |:-: |:-: |:-: |
| value type *compound* | item number (5 or 6) | value type *string* | edge ID (to resolve ambiguities, may be arbitrary) | value type *int* | lane index (to resolve ambiguities, may be arbitrary) | value type double | x Position (network coordinates) | value type double | y Position (network coordinates) | value type double | angle | value type byte | keepRoute (0 - 7) |

### replaceStop (0x17)

|         byte          |       integer        |        byte         | string  |        byte         |    double    |       byte        |    byte    |        byte         |       double        |             byte             |           int           |              byte              |     double     |              byte              |      double      | byte | int | byte | byte |
| :-------------------: | :------------------: | :-----------------: | :-----: | :-----------------: | :----------: | :---------------: | :--------: | :-----------------: | :-----------------: | :--------------------------: | :---------------------: | :----------------------------: | :------------: | :----------------------------: | :--------------: | :---: | :---: | :---: | :---: |
| value type *compound* | item number (8 or 9) | value type *string* | Edge ID | value type *double* | end position | value type *byte* | Lane Index | value type *double* | Duration in seconds | value type *int* | stop flags (see [stop](#stop_0x12)) | value type *double* | start position | value type *double* | Until in seconds | value type *int* | nextStopIndex | value type *byte* (optional) | teleport |

Replaces stop at the given index with a new stop. Automatically modifies the route if the replacement stop is at another location

- For edgeID a stopping place id may be given if the flag marks this stop as stopping on busStop, parkingArea, containerStop etc.
- If edgeID is "", the stop at the given index will be removed without replacement and the route will not be modified.
   - if teleport is set to 2, the vehicle will rerouting in the section of the removed stop (i.e. from the previous to the successive stop).
- If teleport is set to 1, the route to the replacement stop will be disconnected (forcing a teleport).
  - If stopIndex is 0 the gap will be between the current edge and the new stop.
  - Otherwise the gap will be between the stop edge for nextStopIndex - 1 and the new stop.
  - It is recommended to also set sumo option **--time-to-teleport.disconnected** when using this

### insertStop (0x18)

|         byte          |       integer        |        byte         | string  |        byte         |    double    |       byte        |    byte    |        byte         |       double        |             byte             |           int           |              byte              |     double     |              byte              |      double      | byte | int | byte | byte |
| :-------------------: | :------------------: | :-----------------: | :-----: | :-----------------: | :----------: | :---------------: | :--------: | :-----------------: | :-----------------: | :--------------------------: | :---------------------: | :----------------------------: | :------------: | :----------------------------: | :--------------: | :---: | :---: | :---: | :---: |
| value type *compound* | item number (8 or 9) | value type *string* | Edge ID | value type *double* | end position | value type *byte* | Lane Index | value type *double* | Duration in seconds | value type *int* | stop flags (see [stop](#stop_0x12)) | value type *double* | start position | value type *double* | Until in seconds | value type *int* | nextStopIndex | value type *byte* (optional) | teleport |

Inserts stop at the given index. Automatically modifies the route to accommodate the new stop

- For edgeID a stopping place id may be given if the flag marks this stop as stopping on busStop, parkingArea, containerStop etc.
- if nextStopIndex is equal to the number of upcoming stops, the new stop will be added after all other stops
- If teleport is set to 1, the route to the new stop will be disconnected (forcing a teleport).
  - If stopIndex is 0 the gap will be between the current edge and the new stop.
  - Otherwise the gap will be between the stop edge for nextStopIndex - 1 and the new stop.
  - It is recommended to also set sumo option **--time-to-teleport.disconnected** when using this

### setStopParameter (0x55)

| byte | integer         |int           | string | string  | byte  |
| :----: | :--------------------: |:-----------: | :----: | :-----: |:-----: |
| value type *compound*  item number (always 4) | nextStopIndex | param  | value   | customParam

Updates stop parameter at the given index.

- nextStopIndex must be a non-negative number smaller than the number of remaining stops
- param may be [any value permitted as xml-attribute for stops](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#stops_and_waypoints) (except `index`)
- if param changes the location ('edge', 'busStop', ...), the function behaves like [replaceStop](#replacestop_0x17)
- if customParam is set to 1, param may be an arbitrary string and the function will set a custom parameter instead of an attribute

### resume (0x19)

|         byte          |        integer         |
| :-------------------: | :--------------------: |
| value type *compound* | item number (always 0) |

### change target (0x31)

|        byte         | string  |
| :-----------------: | :-----: |
| value type *string* | Edge ID |

### change route by id (0x54)

|        byte         |  string  |
| :-----------------: | :------: |
| value type *string* | Route ID |

### change route (0x57)

|           byte           |        int         | string  |
| :----------------------: | :----------------: | :-----: |
| value type *string list* | number of elements | Edge ID |

### change edge travel time information (0x58)

This command can be sent using 4, 2, or 1 parameters. In the first form,
it adds a new override of edge information to the list of existing
overrides for the given vehicle, valid from begin time to end time. In
the second form, it adds a new override that is valid throughout the
simulation, replacing an earlier one for the same edge. In the third
form, it removes as earlier override for the given edge, valid
throughout the simulation.

|         byte          |              int              |        byte         |         double          |        byte         |        double         |        byte         | string  |        byte         |          double          |
| :-------------------: | :---------------------------: | :-----------------: | :---------------------: | :-----------------: | :-------------------: | :-----------------: | :-----: | :-----------------: | :----------------------: |
| value type *compound* | number of elements (always=4) | value type *double* | begin time (in seconds) | value type *double* | end time (in seconds) | value type *string* | edge id | value type *double* | travel time value (in s) |

|         byte          |              int              |        byte         | string  |        byte         |          double          |
| :-------------------: | :---------------------------: | :-----------------: | :-----: | :-----------------: | :----------------------: |
| value type *compound* | number of elements (always=2) | value type *string* | edge id | value type *double* | travel time value (in s) |

|         byte          |              int              |        byte         | string  |
| :-------------------: | :---------------------------: | :-----------------: | :-----: |
| value type *compound* | number of elements (always=1) | value type *string* | edge id |

### change edge effort information (0x59)

This command can be sent using 4, 2, or 1 parameters. In the first form,
it adds a new override of edge information to the list of existing
overrides for the given vehicle, valid from begin time to end time. In
the second form, it adds a new override that is valid throughout the
simulation, replacing an earlier one for the same edge. In the third
form, it removes as earlier override for the given edge, valid
throughout the simulation.

|         byte          |              int              |        byte         |         double          |        byte         |        double         |        byte         | string  |        byte         |         double          |
| :-------------------: | :---------------------------: | :-----------------: | :---------------------: | :-----------------: | :-------------------: | :-----------------: | :-----: | :-----------------: | :---------------------: |
| value type *compound* | number of elements (always=4) | value type *double* | begin time (in seconds) | value type *double* | end time (in seconds) | value type *string* | edge id | value type *double* | effort value (abstract) |

|         byte          |              int              |        byte         | string  |        byte         |         double          |
| :-------------------: | :---------------------------: | :-----------------: | :-----: | :-----------------: | :---------------------: |
| value type *compound* | number of elements (always=2) | value type *string* | edge id | value type *double* | effort value (abstract) |

|         byte          |              int              |        byte         | string  |
| :-------------------: | :---------------------------: | :-----------------: | :-----: |
| value type *compound* | number of elements (always=1) | value type *string* | edge id |

### reroute (compute new route) by travel time (0x90)

|         byte          |              int              |
| :-------------------: | :---------------------------: |
| value type *compound* | number of elements (always=0) |

### reroute (compute new route) by effort (0x91)

|         byte          |              int              |
| :-------------------: | :---------------------------: |
| value type *compound* | number of elements (always=0) |

### speed mode (0xb3)

This command controls how speeds set with the command *setSpeed (0x40)*
and *slowDown (0x14*) are used. Per default, the vehicle may only drive
slower than the speed that is deemed safe by the car following model and
it may not exceed the bounds on acceleration and deceleration.
Furthermore, vehicles follow the right-of-way rules when approaching an
intersection and if necessary they brake hard to avoid driving across a
red light. One can control this behavior using the speed mode (0xb3)
command. The given integer is a bitset (bit0 is the least significant
bit) with the following fields:

- bit0: Regard safe speed
- bit1: Regard maximum acceleration
- bit2: Regard maximum deceleration
- bit3: Regard right of way at intersections (only applies to approaching foe vehicles outside the intersection)
- bit4: Brake hard to avoid passing a red light
- bit5: **Disregard** right of way within intersections (only applies to foe vehicles that have entered the intersection).

Setting the bit enables the check (the according value is regarded),
keeping the bit==zero disables the check.

Examples:

- default (all checks on) -\> \[0 1 1 1 1 1\] -\> Speed Mode = 31
- most checks off (legacy) -\> \[0 0 0 0 0 0\] -\> Speed Mode = 0
- all checks off  -\> \[1 0 0 0 0 0\] -\> Speed Mode = 32
- disable right of way check -\> \[1 1 0 1 1 1\] -\> Speed Mode = 55
- run a red light \[0 0 0 1 1 1\] = 7 (also requires setSpeed or slowDown)
- run a red light even if the intersection is occupied \[1 0 0 1 1 1\] = 39 (also requires setSpeed or slowDown)

!!! caution
    bit5 has inverted semantics and must be set to '1' in order to disable the safety function. This achieves backward compatibility with earlier versions of SUMO where this bit was not defined and right of way within intersection could not be ignored explicitly.

### lane change mode (0xb6)

The
[laneChangeModel](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#vehicle_types)
discriminates four reasons to change lanes:

- strategic (change lanes to continue the route)
- cooperative (change in order to allow others to change)
- speed gain (the other lane allows for faster driving)
- obligation to drive on the right

During each simulation step the laneChangeModel computes an *internal*
request to change the lane or to stay on the current lane.

If an *external* change lane command (0x13) command is in conflict with
the *internal* request this is resolved by the current value of the
vehicles *lane change mode*. The given integer is interpreted as a
bitset (bit0 is the least significant bit) with the following fields:

- bit1, bit0: 00 = do no strategic changes; 01 = do strategic changes
if not in conflict with a TraCI request; 10 = do strategic change
even if overriding TraCI request
- bit3, bit2: 00 = do no cooperative changes; 01 = do cooperative
changes if not in conflict with a TraCI request; 10 = do cooperative
change even if overriding TraCI request
- bit5, bit4: 00 = do no speed gain changes; 01 = do speed gain changes if not in
conflict with a TraCI request; 10 = do speed gain change even if
overriding TraCI request
- bit7, bit6: 00 = do no right drive changes; 01 = do right drive
changes if not in conflict with a TraCI request; 10 = do right drive
change even if overriding TraCI request


- bit9, bit8:
  - 00 = do not respect other drivers when following TraCI requests,
    adapt speed to fulfill request
  - 01 = avoid immediate collisions when following a TraCI request,
    adapt speed to fulfill request
  - 10 = respect the speed / brake gaps of others when changing
    lanes, adapt speed to fulfill request
  - 11 = respect the speed / brake gaps of others when changing
    lanes, no speed adaption


- bit11, bit10: 00 = do no sublane changes; 01 = do sublane changes if
not in conflict with a TraCI request; 10 = do sublane change even if
overriding TraCI request

The default lane change mode is 0b011001010101 = **1621** which means
that the laneChangeModel may execute all changes unless in conflict with
TraCI. Requests from TraCI are handled urgently (with cooperative speed
adaptations by the ego vehicle and surrounding traffic) but with full
consideration for safety constraints.

To disable all autonomous changing but still handle safety checks in the
simulation, either one of the modes **256** (collision avoidance) or
**512** (collision avoidance and safety-gap enforcement) may be used.

### addLegacy (deprecated) (0x80)

|         byte          |              int              |        byte         |            string            |        byte         |        string         |         byte         |       int        |        byte         |     double      |        byte         |    double    |       byte        |    byte     |
| :-------------------: | :---------------------------: | :-----------------: | :--------------------------: | :-----------------: | :-------------------: | :------------------: | :--------------: | :-----------------: | :-------------: | :-----------------: | :----------: | :---------------: | :---------: |
| value type *compound* | number of elements (always=6) | value type *string* | vehicle type ID (must exist) | value type *string* | route ID (must exist) | value type *integer* | depart time (ms) | value type *double* | depart position | value type *double* | depart speed | value type *byte* | depart lane |

!!! note
    Please note that the values are not checked in a very elaborated way. Make sure they are correct before sending.

If a negative departure time is set, one of the following fixed time
settings will be used:

- \-1: "triggered"
- \-2: "containerTriggered"

If a negative departure speed is set, one of the following fixed speed
settings will be used:

- \-2: "random"
- \-3: "max"

If a negative departure position is set, one of the following position
settings will be used:

- \-2: "random"
- \-3: "free"
- \-4: "base"
- \-5: "last"
- \-6: "random_free"

Please note that giving 0 as depart position will result in the vehicle
starting with its front at the begin of lane (unlike the simulation
default which is the vehicle placed completely on the lane "base")

If a negative departure lane is set, one of the following lane settings
will be used:

- \-2: "random"
- \-3: "free"
- \-4: "allowed"
- \-5: "best"
- \-6: "first"

If an empty routeID is given, the vehicle will be placed on an route
that consists of a single arbitrary edge (with suitable vClass
permissions). This can be used to simply the initialization of remote
controlled vehicle (moveToXY).

### add (alias addFull) (0x85)

|         byte          |              int               |        byte         |        string         |        byte         |            string            |        byte         |   string    |        byte         |   string    |        byte         |     string      |        byte         |    string    |        byte         |    string    |        byte         |      string      |        byte         |    string     |        byte         |           string           |        byte         |            string             |        byte         |           string            |         byte         |       int       |         byte         |      int      |
| :-------------------: | :----------------------------: | :-----------------: | :-------------------: | :-----------------: | :--------------------------: | :-----------------: | :---------: | :-----------------: | :---------: | :-----------------: | :-------------: | :-----------------: | :----------: | :-----------------: | :----------: | :-----------------: | :--------------: | :-----------------: | :-----------: | :-----------------: | :------------------------: | :-----------------: | :---------------------------: | :-----------------: | :-------------------------: | :------------------: | :-------------: | :------------------: | :-----------: |
| value type *compound* | number of elements (always=14) | value type *string* | route ID (must exist) | value type *string* | vehicle type ID (must exist) | value type *string* | depart time | value type *string* | depart lane | value type *string* | depart position | value type *string* | depart speed | value type *string* | arrival lane | value type *string* | arrival position | value type *string* | arrival speed | value type *string* | from taz (origin district) | value type *string* | to taz (destination district) | value type *string* | line (for public transport) | value type *integer* | person capacity | value type *integer* | person number |

If an empty routeID is given, the vehicle will be placed on an route
that consists of a single arbitrary edge (with suitable vClass
permissions). This can be used to simply the initialization of remote
controlled vehicle (moveToXY).

#### special cases

- if routeID is "", the vehicle will be inserted on a random network edge. This may be useful when intending the vehicle with moveToXY (and now route information is available)
- if the route consists of 2 disconnected edges, the vehicle will be treated like a `<trip>` and use the fastest route between the two edges

!!! note
    Please note that the values are not checked in a very elaborated way. Make sure they are correct before sending.

### remove (0x81)

|       byte        |  byte  |
| :---------------: | :----: |
| value type *byte* | reason |

The following reasons may be given:

- 0: NOTIFICATION_TELEPORT
- 1: NOTIFICATION_PARKING
- 2: NOTIFICATION_ARRIVED
- 3: NOTIFICATION_VAPORIZED
- 4: NOTIFICATION_TELEPORT_ARRIVED

### highlight (0x6c)

|         ubyte         |               int                |       ubyte        | color |        ubyte        |       double        |       ubyte        |        ubyte        |        ubyte        |       double        |       ubyte        |                      ubyte                      |
| :-------------------: | :------------------------------: | :----------------: | :---: | :-----------------: | :-----------------: | :----------------: | :-----------------: | :-----------------: | :-----------------: | :----------------: | :---------------------------------------------: |
| value type *compound* | number of items in {0,1,2,3,4,5} | value type *color* | color | value type *double* | size (radius in m.) | value type *ubyte* | maximal alpha value | value type *double* | duration (in secs.) | value type *ubyte* | type ID (allows different highlight categories) |

The highlight method adds a circle of the specified size and color
centered at the vehicle. The circles alpha value is animated over the
specified duration (fade in and out) if a positive value is given,
maxAlpha <= 0 implies persistent highlighting without animation.

# Setting Device and LaneChangeModel Parameters (0x7e)

Vehicles supports setting of device parameters using the [generic
parameter setting
call](../TraCI/GenericParameters.md#set_parameter).

## Supported Device Parameters

- device.battery.actualBatteryCapacity (double literal)
- device.battery.maximumBatteryCapacity (double literal)
- device.battery.vehicleMass (double literal)
- device.rerouting.period (double literal, set rerouting period in
  seconds)
- device.rerouting.edge:EDGE_ID (double literal, set assumed travel
  time for rerouting for **all** vehicles (where EDGE_ID is the id if
  a network edge). This value is overwritten at the next update
  interval (**--device.rerouting.adaptation-interval**).
- device.driverstate.awareness
- device.driverstate.errorState
- device.driverstate.errorTimeScale
- device.driverstate.errorNoiseIntensity
- device.driverstate.minAwareness
- device.driverstate.initialAwareness
- device.driverstate.errorTimeScaleCoefficient
- device.driverstate.errorNoiseIntensityCoefficient
- device.driverstate.speedDifferenceErrorCoefficient
- device.driverstate.headwayErrorCoefficient
- device.driverstate.speedDifferenceChangePerceptionThreshold
- device.driverstate.headwayChangePerceptionThreshold
- device.driverstate.maximalReactionTime
- device.driverstate.originalReactionTime
- device.toc.manualType
- device.toc.automatedType
- device.toc.responseTime
- device.toc.recoveryRate
- device.toc.initialAwareness
- device.toc.lcAbstinence
- device.toc.currentAwareness
- device.toc.mrmDecel
- device.toc.requestToC
- device.toc.requestMRM
- device.toc.awareness
- device.toc.dynamicToCThreshold
- device.toc.dynamicMRMProbability
- device.toc.mrmKeepRight
- device.toc.mrmSafeSpot
- device.toc.mrmSafeSpotDuration
- device.toc.maxPreparationAccel
- device.toc.ogNewTimeHeadway
- device.toc.ogNewSpaceHeadway
- device.toc.ogChangeRate
- device.toc.ogMaxDecel
- device.example.customValue1 (double literal)
- has.rerouting.device ("true"): can be used to dynamically enable
  [automatic rerouting](../Demand/Automatic_Routing.md)

!!! caution
    If the vehicles does not carry the respective device an error is returned.

## Supported LaneChangeModel Parameters

- laneChangeModel.<ATTRNAME\> , (double literal): <ATTRNAME\> must be
  one of the [parameters supported by the
  laneChangeModel](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#lane-changing_models)
  of the vehicle. i.e. *lcStrategic*)

# Relationship between lanechange model attributes and vTypes

All [lanechange model attributes](../Definition_of_Vehicles%2C_Vehicle_Types%2C_and_Routes.md#lane-changing_models) are initialized from the vehicles vType and then stored in the individual lane change model instance of each vehicle. This has important consequences

- setting a new vType for a vehicle doesn't affect lane change model attributes (the vehicle keeps using its individual values)
- changing lane change model attributes on the vType of a vehicle does not affect the vehicle (the vehicle keeps using its individual values)
- changing lane change model attributes for a vehicle does not affect its vType (and instead changes the individual values of the vehicle)

!!! caution
    Attribute 'minGapLat' also counts as a lanechange model attribute since version 1.12.0

# Application order of traci commands and simulation step

Step **n**:

1.  *traci.vehicle.getPosition* and *traci.vehicle.getSpeed* show the
    values from step **n-1**
2.  when calling *traci.vehicle.moveTo* the vehicle is instantly placed
    (and still moves in this step)
3.  *vNext* (the speed for this step) is computed based on the values of
    the previous step (*traci.vehicle.setSpeed* overrides this)
4.  vehicle position is updated based on *vNext* (either by adding
    *vNext* or by adding *(vPrev + vNext) / 2* depending on the value of
    option **--step-method.ballistic**)
5.  the value given to *traci.vehicle.moveToXY* overrides the previously
    computed position
6.  GUI is updated to show final state of step **n**, outputs like
    *fcd-output* also show this state for step **n**

Step **n+1**

This means, that the position given to *moveToXY* in step **n** will be
visible in the outputs for step **n** whereas
*traci.vehicle.getPosition* will always show the value from the previous
step.

## Vehicle insertion

When a vehicle is added using method *add* it is not
immediately inserted into the network. Only after the next call to
*simulationStep* does the simulation try to insert it (and this may fail
when in conflict with other traffic). The result of *getIDList* only
contains vehicles that have been inserted into the network which means
the vehicle will not be listed immediately. You can force a vehicle to
be inserted instantly by calling the function *vehicle.moveTo* or
*vehicle.moveToXY* after adding it to the simulation.
