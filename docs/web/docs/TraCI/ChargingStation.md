---
title: ChargingStation
---

Asks for the value of a certain variable of the named ChargingStation.
The value returned is the state of the asked variable/value within the
last simulation step. Please note that for asking values from your
ChargingStation within an {{AdditionalFile}} and load them at the start of the simulation. The `period` and `file`
attributes do not affect TraCI.

The following variable values can be retrieved, the type of the return
value is also shown in the table.

<center>
**Overview of readable ChargingStation Variables**
</center>

| Variable                                     | ValueType           | Description       |  [Python Method](../TraCI/Interfacing_TraCI_from_Python.md)    |
| -------------------------------------------- | ------------------- | ----------------- | -------------------------------------------------------------- |
| end pos                                  | double          | The end position of the stop along the lane measured in m.  | [getEndPos](https://sumo.dlr.de/pydoc/traci._chargingstation.html#ChargingStationDomain-getEndPos) |
| lane ID                                  | string          | Returns the lane of this calibrator (if it applies to a single lane)   | [getLaneID](https://sumo.dlr.de/pydoc/traci._chargingstation.html#ChargingStationDomain-getLaneID) |
| name                                 | string          | Returns the name of this stop   | [getName](https://sumo.dlr.de/pydoc/traci._chargingstation.html#ChargingStationDomain-getName) |
| start pos                                  | double          | The starting position of the stop along the lane measured in m.   | [getStartPos](https://sumo.dlr.de/pydoc/traci._chargingstation.html#ChargingStationDomain-getStartPos) |
| vehicle count                     | integer   | Get the total number of vehicles stopped at the named charging station.  | [getVehicleCount](https://sumo.dlr.de/pydoc/traci._chargingstation.html#ChargingStationDomain-getVehicleCount) |
| vehicle ID                         | stringList          | Get the IDs of vehicles stopped at the named charging station.   | [getVehicleIDs](https://sumo.dlr.de/pydoc/traci._chargingstation.html#ChargingStationDomain-getVehicleIDs) |
| power                                    | double          | The charging power in W |  [getChargingPower](https://sumo.dlr.de/pydoc/traci._chargingstation.html#ChargingStationDomain-getChargingPower) |
| efficiency                               | double          | The charging efficiency | [getEfficiency](https://sumo.dlr.de/pydoc/traci._chargingstation.html#ChargingStationDomain-getEfficiency) |
| charge in transit                        | integer         | Whether the vehicle is forced/not forced to stop for charging | [getChargeInTransit](https://sumo.dlr.de/pydoc/traci._chargingstation.html#ChargingStationDomain-getChargeInTransit) |
| charge delay                             | double          | The time delay before starting charging in s | [getChargeDelay](https://sumo.dlr.de/pydoc/traci._chargingstation.html#ChargingStationDomain-getChargeDelay) |


