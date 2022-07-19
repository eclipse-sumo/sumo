---
title: Change Simulation State
---

# Command 0xcc: Change Simulation State

|  ubyte   |          string *(arbitrary)*          |       ubyte       | <value_type\> |
| :------: | :------------------------------------: | :---------------: | :----------: |
| Variable | Simulation ID *(arbitrary, see below)* | Type of the value |  New Value   |

Changes the simulation state. Because there is currently only one
simulation, the 'Simulation ID' is ignored. The following values can be
changed, the parameter(s) which must be given are also shown in the
table.

<center>**Overview Changeable GUI Variables**</center>

| Variable                      | ValueType         | Description                                                                                                                                                                                                  | [Python Method](../TraCI/Interfacing_TraCI_from_Python.md)                                |
| ----------------------------- | ----------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ | ---------------------------------------------------------------------------------------------- |
| clear pending vehicles (0x94) | string *routeID*  | Discards all loaded vehicles with a depart time below the current time step which could not be inserted yet. If the given *routeID* has non-zero length, only vehicles that have this routeID are discarded. | [clearPending](https://sumo.dlr.de/pydoc/traci._simulation.html#SimulationDomain-clearPending) |
| save state (0x95)             | string *filename* | Saves current simulation state to the given *filename*.                                                                                                                                                      | [saveState](https://sumo.dlr.de/pydoc/traci._simulation.html#SimulationDomain-saveState)       |
| load state (0x96)             | string *filename* | Quick-loads [simulation state](../Simulation/SaveAndLoad.md) from the given *filename*. This is faster than simulation.load since the network and additional objects are retained. Caution: Vehicles from an incrementally loaded route file as well as flow-vehicles may be missing when loading the state this way. If all route input is in `<vehicle>` or <`trip>` format and loaded with option **--additional-files** (non-incremental loading), traffic is guaranteeed to be complete.                                                                                                                                                      | [loadStateState](https://sumo.dlr.de/pydoc/traci._simulation.html#SimulationDomain-loadState)       |
| scale (0x8e)             | double | Set traffic scaling factor   | [setScale](https://sumo.dlr.de/pydoc/traci._simulation.html#SimulationDomain-setScale)     |
