---
title: Change Lane State
---

# Command 0xc3: Change Lanes State

|  ubyte   | string  |       ubyte       | <value_type\> |
| :------: | :-----: | :---------------: | :----------: |
| Variable | Lane ID | Type of the value |  New Value   |

Changes the state of a lane. Because it is possible to change different
values of a lane, the given value may have different types. The
following values can be changed, the type of the value to given is also
shown in the table.

<center>**Overview Changeable Lane Variables**</center>

| Variable                          | ValueType  | Description                                             | [Python Method](../TraCI/Interfacing_TraCI_from_Python.md)       |
| --------------------------------- | ---------- | ---------------------------------------------------- | ------------------------------------------- |
| allowed vehicle classes (0x34)    | stringList | Sets the given classes as classes allowed on the lane.                                | [setAllowed](https://sumo.dlr.de/pydoc/traci/_lane.html#LaneDomain.setAllowed)       |
| disallowed vehicle classes (0x35) | stringList | Sets the given classes as classes not allowed on the lane.                            | [setDisallowed](https://sumo.dlr.de/pydoc/traci/_lane.html#LaneDomain.setDisallowed) |
| lane change permissions (0x3c)    | stringList,ubyte | Sets the given classes as classes allowed to change to the left/right neighbor lane.  | [setChangePermissions](https://sumo.dlr.de/pydoc/traci/_lane.html#LaneDomain.setChangePermissions) |
| length (0x44)                     | double     | Sets the given value as the lane's new length \[m\].                                  | [setLength](https://sumo.dlr.de/pydoc/traci/_lane.html#LaneDomain.setLength)         |
| vmax (0x41)                       | double     | Sets the given value as the new maximum velocity allowed on the lane \[m/s\].         | [setMaxSpeed](https://sumo.dlr.de/pydoc/traci/_lane.html#LaneDomain.setMaxSpeed)     |

!!! note
    Closing or reopening a lane at runtime with `setAllowed` (`0x34`) or `setDisallowed` (`0x35`) updates lane permissions without recomputing cached strategic lane choices (`bestLanes`) on vehicles already in the simulation. Call `traci.vehicle.updateBestLanes(vehID)` for active vehicles on the affected edge (and upstream edges) so they immediately switch to remaining permitted lanes and connections.
