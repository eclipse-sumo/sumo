---
title: Change Calibrator State
---

# Command 0xc4: Change Calibrator State

|  ubyte   |     string    |       ubyte       | <value_type\> |
| :------: | :-----------: | :---------------: | :-----------: |
| Variable | Calibrator ID | Type of the value |   New Value   |

Changes the state of a calibrator. Because it is possible to change
different values of a calibrator, the number of parameter to supply and
their types differ between commands. The following values can be
changed, the parameter which must be given are also shown in the table.

<center>
**Overview Changeable Calibrator Variables**
</center>

| Variable | ValueType | Description | [Python Method](../TraCI/Interfacing_TraCI_from_Python.md) |
|---------------------------|----------------------------------------------------------------|--------------------------------------------------------------------------------------------------------------------------------------------------------------------|---------------------|
| flow (0x18) | compound (string, double, double, double, double, string, string, string, string), see below | Set the hourly flow and speed at the calibrator location in a given time range. Missing traffic will be added with the given routeID (or routeDistribution ID) and type. DepartLane and departSpeed default to "first" and "max" but can optionally be set. For details on calibrating only flow, speed or type individually, see [Calibrator](../Simulation/Calibrator.md). | [setFlow](https://sumo.dlr.de/pydoc/traci._calibrator.html#CalibratorDomain-setFlow) |

# compound message details

The message contents are as following:

### flow (0x18)

|         byte          |       integer        |        byte         |     string    |        byte         |    double    |         byte        |   double   |        byte         |    double    |         byte        |   double   |         byte        |   string   |        byte         |       string        |              byte              |         string          |              byte              |     string     |
| :-------------------: | :------------------: | :-----------------: | :-----------: | :-----------------: | :----------: | :-----------------: | :--------: | :-----------------: | :----------: | :-----------------: | :--------: | :-----------------: | :--------: | :-----------------: | :-----------------: | :----------------------------: | :---------------------: | :----------------------------: | :------------: |
| value type *compound* | item number (7 to 9) | value type *string* | calibrator ID | value type *double* |     begin    | value type *double* |     end    | value type *double* |  vehsPerHour | value type *double* |    speed   | value type *string* |  type id   | value type *string* |       route id      | value type *string* (optional) |       depart lane       | value type *string* (optional) |  depart speed  |
