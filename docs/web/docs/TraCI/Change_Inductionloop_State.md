---
title: Change Inductionloop State
---

# Command 0xc0: Change Inductionloop State

|  ubyte   |    string     |       ubyte       | <value_type\> |
| :------: | :-----------: | :---------------: | :-----------: |
| Variable |    Loop ID    | Type of the value |   New Value   |

Changes the state of a induction loop. Persistently overrides the measured time since detection with the given value. The following values can be
changed and the parameter which must be given are also shown in the table. Setting a negative value resets the override.

<center>
**Overview Changeable Loop Variables**
</center>

| Variable | ValueType | Description | [Python Method](../TraCI/Interfacing_TraCI_from_Python.md) |
|---------------------------|----------------------------------------------------------------|--------------------------------------------------------------------------------------------------------------------------------------------------------------------|---------------------|
| time (0x18) | compound (string, double), see below | Persistently overrides the measured time since detection with the given value. For details on induction loop detectors, see [Induction Loop](../Simulation/Output/Induction_Loops_Detectors_%28E1%29.md). | [overrideTimeSinceDetection](https://sumo.dlr.de/daily/pydoc/traci._inductionloop.html#InductionLoopDomain-overrideTimeSinceDetection) |

# compound message details

The message contents are as following:

### time (0x18)

|         byte          |       integer        |        byte         |     string    |        byte         |    double    |         byte        |   double   |        byte         |    double    |         byte        |   double   |         byte        |   string   |        byte         |       string        |              byte              |         string          |              byte              |     string     |
| :-------------------: | :------------------: | :-----------------: | :-----------: | :-----------------: | :----------: | :-----------------: | :--------: | :-----------------: | :----------: | :-----------------: | :--------: | :-----------------: | :--------: | :-----------------: | :-----------------: | :----------------------------: | :---------------------: | :----------------------------: | :------------: |
| value type *compound* | item number (7 to 9) | value type *string* | calibrator ID | value type *double* |     begin    | value type *double* |     end    | value type *double* |  vehsPerHour | value type *double* |    speed   | value type *string* |  type id   | value type *string* |       route id      | value type *string* (optional) |       depart lane       | value type *string* (optional) |  depart speed  |

