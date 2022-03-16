---
title: Change Lane Area Detector State
---
For details on lanearea detectors, see [Lanearea Detectors](../Simulation/Output/Lanearea_Detectors_(E2).md).

# Command 0xcd: Change Lane Area Detector State

|  ubyte   |    string     |       ubyte       | <value_type\> |
| :------: | :-----------: | :---------------: | :-----------: |
| Variable |  Detector ID  | Type of the value |   New Value   |

Changes the state of a lane area detector. The values that can be changed and the parameter which must be given are shown in the following table.

<center>
**Overview Changeable Detector Variables**
</center>

| Variable | ValueType | Description | [Python Method](../TraCI/Interfacing_TraCI_from_Python.md) |
|---------------------------|----------------------------------------------------------------|--------------------------------------------------------------------------------------------------------------------------------------------------------------------|---------------------|
| vehicle number (0x22) | integer | Persistently overrides the number of vehicles on the detector. Setting a negative value resets the override. | [overrideVehicleNumber](https://sumo.dlr.de/daily/pydoc/traci._lanearea.html#LaneAreaDomain-overrideVehicleNumber) |
