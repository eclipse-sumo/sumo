---
title: Taxi
---

# Introduction
Since version 1.5.0 SUMO supports simulation of demand responsive transport (DRT)
via the taxi device. This allows a fleet of taxis to service customer requests
based on a configurable dispatch algorithm.

!!! note
    While the taxi capabilities are under development, their status can be checked via Issue #6418.

# Equipping vehicles
A vehicle can be equipped with an Taxi device to make it part of the taxi fleet.
To attach a Taxi device to a vehicle, the [standard device-equipment
procedures](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#devices) can be applied using `<device name>=taxi`.

For instance, a single vehicle can configured as taxi as in the following minimal example

```
    <vehicle id="v0" route="route0" depart="0" line="taxi">
        <param key="has.taxi.device" value="true"/>  
    </vehicle>
```

# Taxi requests

## Direct ride hailing
A person can be defined as taxi customer with the following definition:

```
    <person id="p0" depart="0.00">
        <ride from="B2C2" to="A0B0" lines="taxi"/>
    </person>
```

## Intermodal Routing
A person can also use a taxi by including it as a [personTrip](../Specification/Persons.md#persontrips) mode:

```
    <person id="p0" depart="0.00">
        <personTrip from="B2C2" to="A0B0" modes="taxi"/>
    </person>
```

Whenever a person enters a taxi during the intermodal route search, a time penalty is applied to account for the expected time loss from waiting for the taxi and embarking. The default value is set 300s and can be configure with option **--persontrip.taxi.waiting-time**. This prevents rapid switching between travel modes.

## Groups of Persons
Multiple persons can travel together as a group using attribute `group` (if the taxi has sufficient capacity):

    <person id="p0" depart="0.00">
        <ride from="B2C2" to="A0B0" lines="taxi" group="g0"/>
    </person>
    <person id="p1" depart="0.00">
        <ride from="B2C2" to="A0B0" lines="taxi" group="g0"/>
    </person>

# Multiple Taxi Fleets

By default, there is only a single taxi fleet using line attribute 'taxi' and taxi customers use attribute `lines="taxi"` for their rides.
It is permitted to define the line attribute for taxi with the prefix 'taxi:' and an arbitrary suffix (i.e. "taxi:fleetA").
Likewise, tt is permitted to define the lines attribute for rides with the prefix 'taxi:' and a suffix.
When this is done, the following rules are applied when assigning taxis to customers:

- a taxi with line 'taxi:X' may only pick up customers with matching ride attribute lines="taxi:X" (for any value of X)
- a customer with lines="taxi" may use any taxi regardless of taxi fleet suffix
- a taxi with line 'taxi' may be pick up any customer regardless of the ride fleet suffix

# Dispatch Algorithms
The dispatch algorithm assigns taxis to waiting customers. The algorithm is
selected using option **--device.taxi.dispatch-algorithm ALGONAME**. The following
algorithms are available

- greedy: Assigns taxis to customers in the order of their reservations. The
  closest taxi (in terms of travel time) is assigned. If the reservation date is
  too far in the future, the customer is postponed.

- greedyClosest: For each available taxi, the closest customer (in terms of
  travel time) is assigned. If the reservation date is too far in the future,
  the customer is postponed.

- greedyShared: like 'greedy' but tries to pick up another passenger while delivering the first passenger to it's destination. Parameters **absLossThreshold** and **relLossThreshold** to configure acceptable detours can be supplied using **--device.taxi.dispatch-algorithm.params KEY1:VALUE1[,KEY2:VALUE]**.

- routeExtension: like greedy but can pick up any passenger along the route and also extend the original route (within personCapacity limit).

- traci: Dispatch is defered to [traci control](#traci). The algorithm only keeps track of pending reservations

!!! note
    User-contributed dispatch algorithms are welcome.

# Taxi Behavior

By default, taxis will remain in the simulation until all persons have left. To make them leave the simulation at an earlier time, the end time can be defined using a generic parameter in their ```vType``` or ```vehicle```-definition:

```
```
    <vType id="taxi" vClass="taxi">
        <param key="has.taxi.device" value="true"/>
        <param key="device.taxi.end" value="3600"/>
    </vType>
```
```

## Idle Behavior
By default, vehicles will leave the simulation after reaching the end of their final edge. To avoid this, taxis have an idling behavior configurable with option **--device.taxi.idle-algorithm**:

- "stop" (default): stop at the current location (off-road) after delivering the last customer of the current service request.
- "randomCircling": continue driving to random edges until the next request is received. (caution: taxi might get stuck in a cul-de-sac if the network has those)

## Customer Stops

Taxis will stop to pick-up and drop-off customers. The 'actType' attribute of a stop indicates the purpose ('pickup' / 'dropOff') as well as the ids of the customers and their reservation id. Stop attributes can be configured using [generic parameters]() `<vType>` or `<vehicle>` definition of the taxi:

```
    <vType id="taxi" vClass="taxi">
        <param key="has.taxi.device" value="true"/>
        <param key="device.taxi.pickUpDuration" value="0"/>
        <param key="device.taxi.dropOffDuration" value="60"/>
        <param key="device.taxi.parking" value="false"/>
    </vType>
```

- duration for pick-up stop can be configurd with vType/vehicle param "device.taxi.pickupDuration" (default "0")
- duration for drop-off stop can be configurd with vType/vehicle param "device.taxi.dropOffDuration" (default "60")

By default, vehicle stops will have attribute `parking="true"` which means that the taxi will not block a driving lane. This can be changed by setting
param "device.taxi.parking" to "false".

# TraCI
To couple an external dispatch algorithm to SUMO, the following [TraCI](../TraCI.md) functions are provided:

!!! note
    To make use of these functions, the option **--device.taxi.dispatch-algorithm traci** must be set

- traci.person.getTaxiReservations(reservationState)
- traci.vehicle.getTaxiFleet(taxiState)
- traci.vehicle.dispatchTaxi(vehID, reservations)

This set of API calls can be used to simplify writing custom dispatch algorithms by letting sumo:

- manage existing reservations
- manage the taxi fleet
- dispatch a taxi to service one or more reservations by giving a list of reservation ids (vehicle routing and stopping is then automatic).

## getTaxiReservations

Returns a list of of Reservation objects that have the following attributes

- id
- persons
- group
- state
- fromEdge
- toEdge
- arrivalPos
- departPos
- depart
- reservationTime
- state (positive value, see below)

When calling `traci.person.getTaxiReservations(reservationState)` the following arguments for reservationState are supported:

- 0: return all reservations regardless of state
- 1: return only new reservations
- 2: return reservations already retrieved
- 4: return reservations that have been assigned to a taxi
- 8: return reservations that have been picked up

## getTaxiFleet

A taxi can be in any of the following states:

- 0 (emtpy) : taxi is idle
- 1 (pickup):  taxi is en-route to pick up a customer
- 2 (occupied): taxi has customer on board and is driving to drop-off
- 3 (pickup + occupied): taxi has customer on board but will pick up more customers

when calling `traci.vehicle.getTaxiFleet(taxiState)` the following arguments for taxiState are supported:

- -1: (return all taxis regardless of state)
- 0: return only empty taxis
- 1: return taxis in state 1 and 3
- 2: return taxis in state 2 and 3
- 3: return taxis in state 3

## dispatchTaxi

If a taxi is empty, the following dispatch calls are supported

- dispatchTaxi(vehID, [reservationID]): pickup and drop-off persons belonging to the given reservation ID
- If more than one reservation ID is given, each individual reservation ID must occur exactly twice in the list for complete pickup and drop-off. The first occurence of an ID denotes pick-up and the second occurence denotes drop-off.

Example 1:  dispatchTaxi(vehID, [a]) means: pick up and drop off a.

Example 2:  dispatchTaxi(vehID, [a, a, b, c, b, c]) means: pick up and drop off a, then pick up b and c and then drop off b and c.

If a taxi is not in state empty the following re-dispatch calls are supported

- new reservations have no overlap with previous reservation: append new reservations to the previous reservations
- new reservations include all previous unique reservation ids exactly twice: reset current route and stops and treat as complete new dispatch. If one of the persons of the earlier reservation is already picked up, ignore the first occurrence of the reservation in the reservation list
- new reservations mentions include all previous unique reservation ids once or twice, all customers that are mentioned once are already picked up: reset current route and stops, use the single-occurence ids as as drop-of

# Outputs

The Taxi device generates output within a tripinfo-output file in the following
form:

```
    <tripinfo id="trip_0" ... >
        <taxi customers="5" occupiedDistance="6748.77" occupiedTime="595.00"/>
    </tripinfo>
```

## Parameter Retrieval
The following parameters can be retrieved via `traci.vehicle.getParameter` and written via **--fcd-output.params**.
It is also possible to color vehicles in [SUMO-GUI 'by param (numerical)'](../sumo-gui.md#vehicle_visualisation_settings) by setting these keys.

- device.taxi.state: returns integer value (see #gettaxifleet)  
- device.taxi.customers: total number of customers served
- device.taxi.occupiedDistance: total distance driven in m with customer on board
- device.taxi.occupiedTime: total time driven in s with customers on board
- device.taxi.currentCustomers: space-separated list of persons that are to be picked up or already on board
