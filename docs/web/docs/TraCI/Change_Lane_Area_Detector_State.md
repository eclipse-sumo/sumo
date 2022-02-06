---
title: Change Lane Area Detector State
---

# Command 0xc0: Change Lane Area Detector State

|  ubyte   |    string     |       ubyte       | <value_type\> |
| :------: | :-----------: | :---------------: | :-----------: |
| Variable |  Detector ID  | Type of the value |   New Value   |

Changes the state of a lane area detector. Persistently overrides the number of vehicles on the detector.
Setting a negative value resets the override. The following values can be
changed and the parameter which must be given are also shown in the table.

<center>
**Overview Changeable Detector Variables**
</center>

| Variable | ValueType | Description | [Python Method](../TraCI/Interfacing_TraCI_from_Python.md) |
|---------------------------|----------------------------------------------------------------|--------------------------------------------------------------------------------------------------------------------------------------------------------------------|---------------------|
| vehicle number (0x10) | integer | Persistently overrides the number of vehicles on the detector. For details on lanearea detectors, see [Lanearea Detectors](../Simulation/Output/Lanearea_Detectors_(E2).md). | [overrideVehicleNumber](https://sumo.dlr.de/daily/pydoc/traci._lanearea.html#LaneAreaDomain-overrideVehicleNumber) |
