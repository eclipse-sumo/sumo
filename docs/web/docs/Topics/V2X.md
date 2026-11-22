---
title: V2X
---

# V2X simulations in SUMO

SUMO is usually coupled to a communication network simulator using a
middleware.

## Coupling to network simulators

<center>

**Coupling Middlewares Overview**

</center>

| Name                           | Used Network Simulator                                                                                           | Under Development |
| ------------------------------ | ---------------------------------------------------------------------------------------------------------------- | ----------------- |
| [Veins](#veins)     | Focused on OMNeT++/MiXiM with an extendable, modular framework (ports exist for INET, INETMANET, and JiST/SWANS) | yes               |
| [iCS](#ics)         | ns3                                                                                                              | yes               |
| [MOSAIC](#eclipse_mosaic) | ns3, JiST/SWANS or OMNeT++ (extendable)                                                                          | yes               |


### Veins

See [the Veins Homepage](https://veins.car2x.org/) for a short overview,
download links, beginner's tutorial, full publication list, and
documentation.

### iCS

iCS is the "iTETRIS Control System", a middleware developed in the
[iTETRIS](https://www.ict-itetris.eu/) project to join SUMO, ns-3, and
V2X application simulators. It is currently used and extended within the
[TransAID](https://www.transaid.eu/) project.

### Eclipse MOSAIC

Eclipse MOSAIC (formerly known as the V2X Simulation Runtime
Infrastructure, VSimRTI) is a framework which
enables the preparation and execution of V2X simulations. The easy
integration and exchange of simulators enables the substitution of the
most relevant simulators for a realistic presentation of vehicle
traffic, emissions, wireless communication, and the execution of V2X
applications. For immediate use, a set of simulators is already coupled
with MOSAIC. For example, the traffic simulators VISSIM and SUMO, the
communication simulators ns3, JiST/SWANS and OMNeT++, the application
simulator VSimRTI_App, and several visualization and analysis tools are
prepared for MOSAIC. For more information see [MOSAIC
Homepage](https://www.dcaiti.tu-berlin.de/research/simulation/).

## Coupling External Ego Vehicles and OpenSCENARIO Test Cases

SUMO does not parse ASAM OpenSCENARIO story files natively. External scenario runners (such as OpenSCENARIO engines or driving simulators) must insert the ego vehicle into the SUMO simulation so surrounding background traffic reacts to it and obeys right-of-way rules:

- **Predefined ego trajectory**: Export the ego trajectory as an [FCD file](../Simulation/Output/FCDOutput.md) and load it alongside background demand using [FCDReplay](../Simulation/FCDReplay.md) (**--device.fcd-replay.files**).
- **Closed-loop ego synchronization**: Add an ego vehicle in SUMO and update its pose on every simulation step via [TraCI `moveToXY`](../TraCI/Change_Vehicle_State.md#move_to_xy_0xb4) (`traci.vehicle.moveToXY` or `libsumo.vehicle.moveToXY`), while translating catalog infrastructure and story actions into additional files and TraCI commands.
