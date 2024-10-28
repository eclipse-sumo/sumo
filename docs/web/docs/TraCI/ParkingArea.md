---
title: ParkingArea
---

Asks for the value of a certain variable of the named ParkingArea.
The value returned is the state of the asked variable/value within the
last simulation step. Please note that for asking values from your
ParkingArea [you have to define
them](../Simulation/ParkingArea.md)
within an {{AdditionalFile}} and load them at the start of the simulation. The `period` and `file`
attributes do not affect TraCI.

The following variable values can be retrieved, the type of the return
value is also shown in the table.

<center>
**Overview ParkingArea Variables**
</center>

| Variable                                     | ValueType           | Description       |  [Python Method](../TraCI/Interfacing_TraCI_from_Python.md)    |
| -------------------------------------------- | ------------------- | ----------------- | -------------------------------------------------------------- |
| end pos                                  | double          | The end position of the stop along the lane measured in m.  | [getEndPos](https://sumo.dlr.de/pydoc/traci._parkingarea.html#ParkingAreaDomain-getEndPos) |
| lane ID                                  | string          | Returns the lane of this calibrator (if it applies to a single lane)  | [getLaneID](https://sumo.dlr.de/pydoc/traci._parkingarea.html#ParkingAreaDomain-getLaneID) |
| name                                | string          | Returns the name of this stop  | [getName](https://sumo.dlr.de/pydoc/traci._parkingarea.html#ParkingAreaDomain-getName) |
| start pos                                  | double          | The starting position of the stop along the lane measured in m.  | [getStartPos](https://sumo.dlr.de/pydoc/traci._parkingarea.html#ParkingAreaDomain-getStartPos) |
| vehicle count                                 | integer          | Get the total number of vehicles stopped at the named parking area.  | [getVehicleCount](https://sumo.dlr.de/pydoc/traci._parkingarea.html#ParkingAreaDomain-getVehicleCount) |
| vehicle ID                                 | stringList          |  Get the IDs of vehicles stopped at the named parking area.  | [getVehicleIDs](https://sumo.dlr.de/pydoc/traci._parkingarea.html#ParkingAreaDomain-getVehicleIDs) |
