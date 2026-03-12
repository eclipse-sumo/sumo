---
title: Vehicle Value Retrieval
---

## Command 0xa4: Get Vehicle Variable

|  ubyte   |   string   |
| :------: | :--------: |
| Variable | Vehicle ID |

Asks for the value of a certain variable of the named vehicle. The value
returned is the state of the asked variable/value within the last
simulation step. In the case the vehicle is loaded, but outside the
network - due not being yet inserted into the network or being
teleported within the current time step - a default "error" value is
returned.

The following variable values can be retrieved and subscribed to.
The type of the return
value is also shown in the table. It is not possible to subscribe to
next stops (0x73).

### Overview Retrievable Vehicle Variables

| Variable | ValueType | Description | [Python Method](Interfacing_TraCI_from_Python.md) |
| -------- | --------- | ----------- | ------------------------------------------------------------ |
| id list (0x00) | stringList | Returns a list of ids of all vehicles currently running within the scenario (the given vehicle ID is ignored) | [getIDList](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getIDList) |
| count (0x01) | int | Returns the number of vehicles currently running within the scenario (the given vehicle ID is ignored) | [getIDCount](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getIDCount) |
| speed (0x40) | double | Returns the speed of the named vehicle within the last step [m/s]; error value: -2^30 | [getSpeed](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getSpeed) |
| lateral speed (0x32) | double | Returns the lateral speed of the named vehicle within the last step [m/s]; error value: -2^30 | [getLateralSpeed](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getLateralSpeed) |
| acceleration (0x72) | double | Returns the acceleration in the previous time step [m/s^2] | [getAcceleration](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getAcceleration) |
| position (0x42) | position | Returns the position(two doubles) of the named vehicle (center of the front bumper) within the last step [m,m]; error value: [-2^30, -2^30]. | [getPosition](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getPosition) |
| position 3D (0x39) | position | Returns the 3D-position(three doubles) of the named vehicle (center of the front bumper) within the last step [m,m,m]; error value: [-2^30, -2^30, -2^30]. | [getPosition3D](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getPosition3D) |
| angle (0x43) | double | Returns the angle of the named vehicle within the last step [°]; error value: -2^30 | [getAngle](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getAngle) |
| road id (0x50) | string | Returns the id of the edge the named vehicle was at within the last step; error value: "" | [getRoadID](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getRoadID) |
| lane id (0x51) | string | Returns the id of the lane the named vehicle was at within the last step; error value: "" | [getLaneID](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getLaneID) |
| lane index (0x52) | int | Returns the index of the lane the named vehicle was at within the last step; error value: -2^30 | [getLaneIndex](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getLaneIndex) |
| type id (0x4f) | string | Returns the id of the type of the named vehicle | [getTypeID](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getTypeID) |
| route id (0x53) | string | Returns the id of the route of the named vehicle | [getRouteID](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getRouteID) |
| route index (0x69) | int | Returns the index of the current edge within the vehicles route or -1 if the vehicle has not yet departed | [getRouteIndex](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getRouteIndex) |
| edges (0x54) | stringList | Returns the ids of the edges the vehicle's route is made of | [getRoute](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getRoute) |
| color (0x45) | ubyte,ubyte,ubyte,ubyte | Returns the vehicle's color (RGBA). This does not return the currently visible color in the GUI but the color value set in the XML file or via TraCI. | [getColor](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getColor) |
| lane position (0x56) | double | The position of the vehicle along the lane (the distance from the front bumper to the start of the lane in [m]); error value: -2^30 | [getLanePosition](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getLanePosition) |
| distance (0x84) | double | The distance, the vehicle has already driven ([m]); error value: -2^30 | [getDistance](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getDistance) |
| signal states (0x5b) | int | An integer encoding the state of a vehicle's signals, see [TraCI/Vehicle Signalling](Vehicle_Signalling.md) for more information. | [getSignals](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getSignals) |
| routing mode (0x89) | int | An integer encoding the [current routing mode](../Simulation/Routing.md#travel-time_values_for_routing) (0: default, 1: aggregated) | [getRoutingMode](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getRoutingMode) |
| TaxiFleet (0x20) | stringList | Return the list of all taxis with the given mode:(-1: all, 0 : empty, 1 : pickup,2 : occupied, 3: pickup+occupied). Note: vehicles that are in state pickup+occupied (due to ride sharing) will also be returned when requesting mode 1 or 2 | [getTaxiFleet](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getTaxiFleet) |
| CO2 emissions<br />(id 0x60) | double | Vehicle's CO2 emissions in mg/s during this time step, to get the value for one step multiply with the step length; error value: -2^30 | [getCO2Emission](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getCO2Emission) |
| CO emissions<br />(id 0x61) | double | Vehicle's CO emissions in mg/s during this time step, to get the value for one step multiply with the step length; error value: -2^30 | [getCOEmission](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getCOEmission) |
| HC emissions<br />(id 0x62) | double | Vehicle's HC emissions in mg/s during this time step, to get the value for one step multiply with the step length; error value: -2^30 | [getHCEmission](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getHCEmission) |
| PMx emissions<br />(id 0x63) | double | Vehicle's PMx emissions in mg/s during this time step, to get the value for one step multiply with the step length; error value: -2^30 | [getPMxEmission](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getPMxEmission) |
| NOx emissions<br />(id 0x64) | double | Vehicle's NOx emissions in mg/s during this time step, to get the value for one step multiply with the step length; error value: -2^30 | [getNOxEmission](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getNOxEmission) |
| fuel consumption<br />(id 0x65) | double | Vehicle's fuel consumption in mg/s during this time step, to get the value for one step multiply with the step length; error value: -2^30 | [getFuelConsumption](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getFuelConsumption) |
| noise emission<br />(id 0x66) | double | Noise generated by the vehicle in dBA; error value: -2^30 | [getNoiseEmission](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getNoiseEmission) |
| electricity consumption (id 0x71) | double | Vehicle's electricity consumption in Wh/s during this time step, to get the value for one step multiply with the step length; error value: -2^30 | [getElectricityConsumption](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getElectricityConsumption) |
| best lanes (id 0xb2) | complex | For each lane on the current edge, the sequences of lanes that would be followed from that lane without lane-change as well as information regarding lane-change desirability are returned (see below). | [getBestLanes](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getBestLanes) |
| stop state (id 0xb5) | int | value = 1 * stopped + 2 * parking + 4 * triggered + 8 * containerTriggered + 16 * atBusStop + 32 * atContainerStop + 64 * atChargingStation + 128 * atParkingArea | [getStopState](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getStopState)<br />[isAtBusStop](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-isAtBusStop)<br />[isAtContainerStop](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-isAtContainerStop)<br />[isStopped](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-isStopped)<br />[isStoppedParking](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-isStoppedParking)<br />[isStoppedTriggered](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-isStoppedTriggered) |
| length (0x44) | double | Returns the length of the vehicles [m] | [getLength](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getLength) |
| vmax (0x41) | double | Returns the maximum speed of the vehicle [m/s] | [getMaxSpeed](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getMaxSpeed) |
| accel (0x46) | double | Returns the maximum acceleration possibility of this vehicle [m/s^2] | [getAccel](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getAccel) |
| decel (0x47) | double | Returns the maximum deceleration possibility of this vehicle [m/s^2] | [getDecel](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getDecel) |
| tau (0x48) | double | Returns the driver's desired time headway for this vehicle [s] | [getTau](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getTau) |
| sigma (0x5d) | double | Returns the driver's imperfection (dawdling) [0,1] | [getImperfection](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getImperfection) |
| speedFactor (0x5e) | double | Returns the road speed multiplier for this vehicle [double] | [getSpeedFactor](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getSpeedFactor) |
| speedDev (0x5f) | double | Returns the deviation of speedFactor for this vehicle [double] | [getSpeedDeviation](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getSpeedDeviation) |
| vClass (0x49) | string | Returns the permission class of this vehicle | [getVehicleClass](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getVehicleClass) |
| emission_class (0x4a) | string | Returns the emission class of this vehicle | [getEmissionClass](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getEmissionClass) |
| shape (0x4b) | string | Returns the shape class of this vehicle | [getShapeClass](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getShapeClass) |
| minGap (0x4c) | double | Returns the offset (gap to front vehicle if halting) of this vehicle [m] | [getMinGap](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getMinGap) |
| width (0x4d) | double | Returns the width of this vehicle [m] | [getWidth](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getWidth) |
| height (0xbc) | double | Returns the height of this vehicle [m] | [getHeight](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getHeight) |
| person capacity (0x38) | int | Returns the total number of persons that can ride in this vehicle | [getPersonCapacity](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getPersonCapacity) |
| waiting time (0x7a) | double | Returns the consecutive time in where this vehicle was standing [s] (voluntary stopping is excluded) | [getWaitingTime](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getWaitingTime) |
| accumulated waiting time (0x87) | double | Returns the accumulated waiting time [s] within the previous time interval of default length 100 s. (length is configurable per option <b>--waiting-time-memory</b> given to the main application) | [getAccumulatedWaitingTime](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getAccumulatedWaitingTime) |
| next TLS (0x70) | complex | Returns upcoming traffic lights, along with distance and state | [getNextTLS](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getNextTLS) |
| next stops (0x73) | complex | Returns the list of upcoming stops, each as compound (laneID, endPos, ID, flags, duration, until). If flag 1 is set (stop reached), duration encodes the remaining duration. Negative values indicate being blocked from re-entering traffic after a parking stop. This function is deprecated, use stops (0x74) instead. | [getNextStops](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getNextStops) |
| person id list (0x1a) | stringList | Returns the list of persons which includes those defined using attribute 'personNumber' as well as <person>-objects which are riding in this vehicle. | [getPersonIDList](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getPersonIDList) |
| speed mode (0xb3) | int bitset (see below) | Retrieves how the values set by speed (0x40) and slowdown (0x14) shall be treated. See the [set speedmode command](../TraCI/Change_Vehicle_State.md#speed_mode_0xb3) for details. | [getSpeedMode](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getSpeedMode) |
| lane change mode (0xb6) | int bitset (see [below](../TraCI/Change_Vehicle_State.md#lane_change_mode_0xb6)) | Get information on how lane changing in general and lane changing requests by TraCI are performed. | [getLaneChangeMode](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getLaneChangeMode) |
| slope (0x36) | double | Retrieves the slope at the current vehicle position in degrees | [getSlope](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getSlope) |
| allowed speed (0xb7) | double | Returns the maximum allowed speed on the current lane regarding speed factor in m/s for this vehicle. | [getAllowedSpeed](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getAllowedSpeed) |
| line (0xbd) | string | Returns the line information of this vehicle. | [getLine](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getLine) |
| Person Number(0x67) | int | Returns the total number of persons which includes those defined using attribute 'personNumber' as well as &lt;person&gt;-objects which are riding in this vehicle. | [getPersonNumber](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getPersonNumber) |
| via edges(0xbe) | stringList | Returns the ids of via edges for this vehicle. | [getVia](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getVia) |
| speed without TraCI (0xb1) | double | Returns the speed that the vehicle would drive if not speed-influencing command such as setSpeed or slowDown was given. | [getSpeedWithoutTraCI](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getSpeedWithoutTraCI) |
| valid route (0x92) | bool | Returns whether the current vehicle route is connected for the vehicle class of the given vehicle. | [isRouteValid](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-isRouteValid) |
| lateral lane position (0xb8) | double | Returns the lateral position of the vehicle on its current lane measured in m. | [getLateralLanePosition](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getLateralLanePosition) |
| max lateral speed (0xba) | double | Returns the maximum lateral speed in m/s of this vehicle. | [getMaxSpeedLat](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getMaxSpeedLat) |
| boarding duration (0x2f) | double | Returns the boarding duration of the vehicle in s | [getBoarding](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getBoardingDuration) |
| current impatience (0x26) | double | Returns the current [dynamic impatience](../Definition_of_Vehicles%2C_Vehicle_Types%2C_and_Routes.md#impatience) of this vehicle. | [getImpatience](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getImpatience) |
| lateral gap (0xbb) | double | Returns the desired lateral gap of this vehicle at 50km/h in m. | [getMinGapLat](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getMinGapLat) |
| lateral alignment (0xb9) | string | Returns the preferred lateral alignment of the vehicle. | [getLateralAlignment](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getLateralAlignment) |
| parameter (0x7e) | string | [Returns the value for the given string parameter](#device_and_lanechangemodel_parameter_retrieval_0x7e) | [getParameter](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getParameter) |
| action step length (0x7d) | double | Returns the current action step length for the vehicle in s. | [getActionStepLength](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getActionStepLength) |
| last action time (0x7f) | double | Returns the time of the last action step in s. | [getLastActionTime](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getLastActionTime) |
| stops (0x74) | complex | Returns the list of the next or last n stops as StopData objects. | [getStops](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getStops) |
| timeLoss (0x8c) | double | Returns the accumulated timeLoss of the vehicle in s | [getTimeLoss](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getTimeLoss) |
| loaded list (0x24) | stringList | Returns a list of all loaded vehicle ids that have not yet arrived. This includes vehicles that are meant to depart in the future. | [getLoadedIDList](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getLoadedIDList) |
| teleporting list (0x25) | stringList | Returns a list of all vehicles that are currently [teleporting/jumping](../Simulation/Why_Vehicles_are_teleporting.md) | [getTeleportingIDList](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getTeleportingIDList) |
| next links (0x33) | complex | Returns the list of upcoming links with each compound containing info about (lane, via, priority, opened, foe, state, direction, length) | [getNextLinks](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getNextLinks) |
| actual departure time (0x3a) | double | Returns the actual departure time (after possibly queueing for insertion) | [getDeparture](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getDeparture) |
| departure delay (0x3b) | double | Returns the time difference between the planned and the actual departure | [getDepartDelay](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getDepartDelay) |
| segment id (0xa1) | string | Returns the id of the segment on which the vehicle is driving (mesosim) | [getSegmentID](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getSegmentID) |
| segment index (0xa2) | int | Returns the index of the segment on which the vehicle is driving (mesosim) | [getDepartSegmentIndex](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getSegmentIndex) |
| mass (0xc8) | double | Returns the mass of the vehicle | [getMass](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getMass) |
| reference distance (0x95) | double | Returns the position of the vehicle within the [linear reference system of the edge](../Simulation/Distances.md#defining_and_using_linear_coordinates) | [getDepartSegmentIndex](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getReferenceDistance) |

## Response 0xb4: Vehicle Variable

|  ubyte   |   string   |            ubyte            |  <return_type\>   |
| :------: | :--------: | :-------------------------: | :--------------: |
| Variable | Vehicle ID | Return type of the variable | <VARIABLE_VALUE\> |

The respond to a **"Command Get Vehicle Variable"**.

**next stops (0x73)**

|         byte          |                 int                  |       ...        |
| :-------------------: | :----------------------------------: | :--------------: |
| value type *compound* | number of following stop information | stop information |

Each stop information is:

|        byte         | string  |        byte         | double |        byte         |     string      |         byte         |                                           integer                                           |        byte         |       double        |        byte         |      double      |
| :-----------------: | :-----: | :-----------------: | :----: | :-----------------: | :-------------: | :------------------: | :-----------------------------------------------------------------------------------------: | :-----------------: | :-----------------: | :-----------------: | :--------------: |
| value type *string* | lane-id | value type *double* | endPos | value type *string* | stoppingPlaceID | value type *integer* | stopFlags (bit set, see [setStop()](../TraCI/Change_Vehicle_State.md#stop_0x12)) | value type *double* | duration in seconds | value type *double* | until in seconds |

**best lanes (0xb2)**

|         byte          |                 int                  |       ...        |
| :-------------------: | :----------------------------------: | :--------------: |
| value type *compound* | number of following edge information | edge information |

Each edge information is:

|        byte         | string  |        byte         | double |        byte         |   double   |       byte        |    byte (signed)    |        byte        |                         byte (unsigned)                         |          byte           |          stringList           |
| :-----------------: | :-----: | :-----------------: | :----: | :-----------------: | :--------: | :---------------: | :-----------------: | :----------------: | :-------------------------------------------------------------: | :---------------------: | :---------------------------: |
| value type *string* | lane-id | value type *double* | length | value type *double* | occupation | value type *byte* | offset to best lane | value type *ubyte* | 0: lane may not be used for continuing drive, 1: it may be used | value type *stringlist* | list of best subsequent lanes |

## Extended retrieval messages

Some further messages require additional parameters.

<center>
**Overview Extended Variables Retrieval**
</center>

| Variable | Request ValueType | Response ValueType | Description | [Python Method](Interfacing_TraCI_from_Python.md) |
| -------- | ----------------- | ------------------ | ----------- | ------------------------------------------------------------ |
| edge travel time information (0x58) | compound (time, edgeID), see below | double | Returns the edge travel time for the given time as stored in the vehicle's internal container. If such a value does not exist, -1 is returned. | [getAdaptedTraveltime](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getAdaptedTraveltime) |
| edge effort information (0x59) | compound (time, edgeID), see below | double | Returns the edge effort for the given time as stored in the vehicle's internal container. If such a value does not exist, -1 is returned. | [getEffort](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getEffort) |
| leader (0x68) | double, see below | compound (string, double) | Returns the id of the leading vehicle and its distance, if the string is empty, no leader was found within the given range. Only vehicles ahead on the currently list of *best lanes* are considered (see above). This means, the leader is only valid until the next lane-change maneuver. The returned distance is measured from the ego vehicle front bumper + minGap to the back bumper of the leader vehicle. | [getLeader](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getLeader) |
| distance request (0x83) | compound (see [Command 0x83: Distance Request](https://sumo.dlr.de/docs/TraCI/Simulation_Value_Retrieval.html#command_0x83_distance_request)) | double, see below | Returns the distance between the current vehicle position and the specified position (for the given distance type) | [getDrivingDistance](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getDrivingDistance) [getDrivingDistance2D](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getDrivingDistance2D) |
| change lane information (0x13) | compound , [see below](#change_lane_information_0x13) | int, int [see below](#change_lane_information_0x13) | Return whether the vehicle could change lanes in the specified direction in the previous step (right: -1, left: 1. sublane-change within current lane: 0).<br />Return the lane change state for the vehicle. | [getLaneChangeState](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getLaneChangeState) [couldChangeLane](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-couldChangeLane) [wantsAndCouldChangeLane](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-wantsAndCouldChangeLane) |
| neighboring vehicles (0xbf) | byte , [see below](#neighboring_vehicles_0xbf) | stringList [see below](#neighboring_vehicles_0xbf) | Returns a list of IDs for neighboring vehicle relevant to lane changing (>1 elements only possible for [sublane model](../Simulation/SublaneModel.md)) | [getNeighbors](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getNeighbors), [getLeftFollowers](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getLeftFollowers), [getLeftLeaders](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getLeftLeaders), [getRightFollowers](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getRightFollowers), [getRightLeaders](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getRightLeaders) |
| followSpeed (0x1c) | compound, [see below](#followspeed_0x1c) | double | Return the follow speed computed by the carFollowModel of vehicle | [getFollowSpeed](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getFollowSpeed) |
| secureGap (0x1e) | compound, [see below](#securegap_0x1e) | double | Return the secure gap computed by the carFollowModel of vehicle | [getSecureGap](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getSecureGap) |
| stopSpeed (0x1d) | compound, [see below](#stopspeed_0x1d) | double | Return the safe speed for stopping at gap computed by the carFollowModel of vehicle | [getStopSpeed](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getStopSpeed) |
| junction foes (0x37) | complex | Returns the list of foes within a certain distance of the ego vehicle. | [getJunctionFoes](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getJunctionFoes) |
| stop parameters (0x55) | string | Returns the attribute by the given name for the stop of the given index (0 is the next stop, -1 is the previous stop etc) for the specified vehicle. If customParam is set to True (1), the user defined custom parameter will returned instead. | [getStopParameter](https://sumo.dlr.de/pydoc/traci._vehicle.html#VehicleDomain-getStopParameter) |

The request message contents are as following:

### edge travel time information (0x58)

|         byte          |              int              |        byte         |        double         |        byte         | string  |
| :-------------------: | :---------------------------: | :-----------------: | :-------------------: | :-----------------: | :-----: |
| value type *compound* | number of elements (always=2) | value type *double* | requested time (in s) | value type *string* | edge id |

### edge effort information (0x59)

|         byte          |              int              |        byte         |        double         |        byte         | string  |
| :-------------------: | :---------------------------: | :-----------------: | :-------------------: | :-----------------: | :-----: |
| value type *compound* | number of elements (always=2) | value type *double* | requested time (in s) | value type *string* | edge id |

### leader (0x68)

|        byte         |               double               |
| :-----------------: | :--------------------------------: |
| value type *double* | minimum look ahead distance (in m) |

### next TLS (0x70)

|         byte          |        int         |         byte         |                 int                 |        byte         | string |       byte       |      int       |        byte         |     double      |       byte        |                                    byte                                     |
| :-------------------: | :----------------: | :------------------: | :---------------------------------: | :-----------------: | :----: | :--------------: | :------------: | :-----------------: | :-------------: | :---------------: | :-------------------------------------------------------------------------: |
| value type *compound* | number of elements | value type *integer* | number of traffic light links ahead | value type *string* | TLS id | value type *int* | TLS link index | value type *double* | distance to TLS | value type *byte* | [link state](../Simulation/Traffic_Lights.md#signal_state_definitions) |

The last 4 items occur multiple times according to the number of traffic
lights ahead (2nd return value)

### change lane information (0x13)

|         byte          |                                     int                                     |
| :-------------------: | :-------------------------------------------------------------------------: |
| value type *compound* | change direction (-1:right, 0:sublane-change within current lane or 1:left) |

The result consists of two integer values. The first value returns the
state as computed by the lane change model and the second value returns
the state after requests where incorporated (see
[changeLane](../TraCI/Change_Vehicle_State.md#change_lane_0x13),
[setLaneChangeMode](../TraCI/Change_Vehicle_State.md#lane_change_mode_0xb6)).
Each value is a bitset with the following meaning:

- 2^0: stay
- 2^1: left
- 2^2: right
- 2^3: strategic
- 2^4: cooperative
- 2^5: speedGain
- 2^6: keepRight
- 2^7: TraCI
- 2^8: urgent
- 2^9: blocked by left leader
- 2^10: blocked by left follower
- 2^11: blocked by right leader
- 2^12: blocked by right follower
- 2^13: overlapping
- 2^14: insufficient space
- 2^15: sublane
- 2^28: insufficient speed
- 2^30: undetermined

### neighboring vehicles (0xbf)

|        byte        |     ubyte     |
| :----------------: | :-----------: |
| value type *ubyte* | mode (bitset) |

The mode's bits specify which neighboring vehicles should be returned:

- 2^0: right neighbors (else: left)
- 2^1: neighbors ahead (else: behind)
- 2^2: only neighbors blocking a potential lane change (else: all)

The returned string list contains the IDs of all corresponding
neighboring vehicles.

### followSpeed (0x1c)

|         byte          | int   | double | double | double      | double         | string   |
| :-------------------: | :---: | :----: | :----: | :---------: | :------------: | :------: |
| value type *compound* | 5     | speed  | gap    | leaderSpeed | leaderMaxDecel | leaderID |

The leaderID is optional. Only a few car follow models such as CACC may require the identity of the leader to return the correct value
The returned value is the safe follow speed in m/s

### secureGap (0x1e)

|         byte          | int   | double | double      | double         | string   |
| :-------------------: | :---: | :----: | :---------: | :------------: | :------: |
| value type *compound* | 4     | speed  | leaderSpeed | leaderMaxDecel | leaderID |

The leaderID is optional. Only a few car follow models such as CACC may require the identity of the leader to return the correct value
The returned value is the safe gap for following the given leader in m.

### stopSpeed (0x1d)

|         byte          | int   | double | double |
| :-------------------: | :---: | :----: | :----: |
| value type *compound* | 2     | speed  | gap    |

The returned value is the safe speed in m/s for stopping after gap when braking
from the given speed.

### junction foes (0x37)

|         byte          |  double |
| :-------------------: |  :----: |
| value type *double*   |  lookahead distance in m   |

The returned value is the safe speed in m/s for stopping after gap when braking
from the given speed.

### stopParameter (0x55)

|         byte          | int   | int | string | byte
| :-------------------: | :---: | :----: | :----: |:----: |
| value type *compound* | 3     | stopIndex  | param    | customParam

The stopIndex must be in range [-numberOfPassedStops, numberORemaingStops - 1].
The customParam is interpreted as a boolean and determines whether an attribute values or a user defined parameter is returned.


## Device and LaneChangeModel Parameter Retrieval 0x7e

Vehicles supports retrieval of device parameters using the [generic
parameter retrieval
call](../TraCI/GenericParameters.md#get_parameter).

### Supported Device Parameters

- device.battery.energyConsumed
- device.battery.energyCharged
- device.battery.chargeLevel
- device.battery.capacity
- device.battery.chargingStationId
- device.battery.totalEnergyConsumed
- device.battery.totalEnergyRegenerated
- device.battery.maximumChargeRate
- device.person.IDList
- device.container.IDList
- device.rerouting.period (returns individual rerouting period in
  seconds)
- device.rerouting.edge:EDGE_ID (returns assumed travel time for
  rerouting where EDGE_ID is the id of a network edge)
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
- device.driverstate.actionStepLength
- device.stationfinder.chargingStation
- device.stationfinder.batteryNeed
- device.stationfinder.needToChargeLevel
- device.stationfinder.saturatedChargeLevel
- device.stationfinder.waitForCharge
- device.stationfinder.repeat
- device.stationfinder.radius
- device.stationfinder.reserveFactor
- device.taxi.state
- device.taxi.customers
- device.taxi.occupiedTime
- device.taxi.occupiedDistance
- device.taxi.currentCustomers
- device.tripinfo.waitingTime (total waiting time)
- device.tripinfo.waitingCount
- device.tripinfo.stopTime (total stopping time)
- device.tripinfo.arrivalTime (only retrievable after arrival with option **--keep-after-arrival TIME**)
- device.tripinfo.arrivalLane (only retrievable after arrival with option **--keep-after-arrival TIME**)
- device.tripinfo.arrivalPos (only retrievable after arrival with option **--keep-after-arrival TIME**)
- device.tripinfo.arrivalPosLat (only retrievable after arrival with option **--keep-after-arrival TIME**)
- device.tripinfo.arrivalSpeed (only retrievable after arrival with option **--keep-after-arrival TIME**)
- device.toc.state
- device.toc.holder
- device.toc.hasDynamicToC
- device.toc.manualType
- device.toc.automatedType
- device.toc.initialAwareness
- device.toc.lcAbstinence
- device.toc.currentAwareness
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
- device.example.customValue1 (return the value of option **--device.example.parameter**)
- device.example.customValue2 (return the value of vehicle parameter
  *example*)
- device.example.meaningOfLife (return 42)
- has.DEVICENAME.device (returns "true" or "false" depending on
  whether a device with DEVICENAME is equipped)

!!! caution
    If the vehicles does not carry the respective device an error is returned when retrieving parameters with the **device**. prefix. This happens when requesting *device.person.IDList* for a vehicle that has not carried any persons up to that point. Either check for existence or handle the error (i.e. by catching TraCIException in the python client).

### Supported LaneChangeModel Parameters

- laneChangeModel.<ATTRNAME\> (where <ATTRNAME\> is one of the
  [parameters supported by the
  laneChangeModel](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#lane-changing_models)
  of the vehicle. i.e. *lcStrategic*)

### Supported carFollowModel Parameters

Some carFollowModels permit access to further vehicle-specific parameters.

- [carFollowModel.ignoreIDs](../Definition_of_Vehicles%2C_Vehicle_Types%2C_and_Routes.md#transient_carfollowmodel_parameters)
- [carFollowModel.ignoreTypes](../Definition_of_Vehicles%2C_Vehicle_Types%2C_and_Routes.md#transient_carfollowmodel_parameters)
- carFollowModel.caccCommunicationsOverride (only CACC)
- carFollowModel.caccVehicleMode (only CACC)

### Supported junctionModel Parameters

- [junctionModel.ignoreIDs](../Definition_of_Vehicles%2C_Vehicle_Types%2C_and_Routes.md#transient_parameters)
- [junctionModel.ignoreTypes](../Definition_of_Vehicles%2C_Vehicle_Types%2C_and_Routes.md#transient_parameters)

### Supported further Parameters

- parking.rerouteCount: number of times vehicle rerouted to another parkingArea during the current search
- parking.memory.IDList:  all parkingAreas considered so far
- parking.memory.score:  latest scores for the list of considered parkingAreas (smaller is better)
- parking.memory.blockedAtTime: times for the list of considered parkingAreas indicating the last time the area was found to be blocked (possibly via remote information)
- parking.memory.blockedAtTimeLocal: times for the list of considered parkingAreas indicating the last time the area was found to be blocked (upon physically visiting that area)
