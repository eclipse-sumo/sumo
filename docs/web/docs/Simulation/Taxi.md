---
title: Simulation/Taxi
permalink: /Simulation/Taxi
---
# Introduction
Since version 1.5 SUMO supports simulation of demand responsive transport (DRT)
via the taxi device. This allows a fleet of taxis to service customer requests
based on a configurable dispatch algorithm

# Equipping vehicles
A vehicle can be equipped with an Taxi device to make it part of the taxi fleet.
To attach a Taxi device to a vehicle, the [standard device-equipment
procedures](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#devices) can be applied using `<device name>=taxi`.

For instance, a single vehicle can configured as taxi as in the following minimal example

```
    <vehicle id="v0" route="route0" depart="0" line="taxi">
        <param key="has.taxi.device" value="true"/>
        <stop lane="C0D0_1" triggered="person"/>
    </vehicle>
```

Note, that the stop definition is used to keep the taxi waiting in the
simulation until the first dispatch is received.

# Taxi requests

## Direct ride hailing
A person can be defined as taxi customr with the following definition

```
    <person id="p0" depart="0.00">
        <ride from="B2C2" to="A0B0" lines="taxi"/>
    </person>
```

# Dispatch
The dispatch algorithm assigns taxis to waiting customers. The algorithm is
selected using option **--device.taxi.dispatch-algorithm ALGONAME**. The following
algorithms are available
- greedy: Assigns taxis to customers in the order of their reservations. The
  closest taxi (in terms of travel time) is assigned. If the reservation data is
  too far in the future, the customr is postponed

- greedyClosest: For each available taxi, the closest customer (in terms of
  travel time) is assigned. If the reservation data is too far in the future,
  the customr is postponed.

!!! Note:
    User-contributed dispatch algorithms are welcome.

# Outputs

The Taxi device generates output within a tripinfo-output file in the following
form:

```
    <tripinfo id="trip_0" ... >
        <taxi customers="5" occupiedDistance="6748.77" occupiedTime="595.00"/>
    </tripinfo>
```

