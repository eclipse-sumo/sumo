---
title: Change Inductionloop State
---
For details on induction loop detectors, see [Induction Loop](../Simulation/Output/Induction_Loops_Detectors_%28E1%29.md).

# Command 0xc0: Change Inductionloop State

|  ubyte   |    string     |       ubyte       | <value_type\> |
| :------: | :-----------: | :---------------: | :-----------: |
| Variable |    Loop ID    | Type of the value |   New Value   |

Changes the state of a induction loop. The values that can be changed and the parameter which must be given are shown in the following table.

<center>
**Overview Changeable Loop Variables**
</center>

| Variable | ValueType | Description | [Python Method](../TraCI/Interfacing_TraCI_from_Python.md) |
|---------------------------|----------------------------------------------------------------|--------------------------------------------------------------------------------------------------------------------------------------------------------------------|---------------------|
| time (0x22) | double | Persistently overrides the measured time since detection with the given value. Setting a negative value resets the override.  | [overrideTimeSinceDetection](https://sumo.dlr.de/daily/pydoc/traci._inductionloop.html#InductionLoopDomain-overrideTimeSinceDetection) |

# compound message details

The message contents are as following:

###

|         byte          |        integer         |        byte         | string  |        byte         |       double        |
| :-------------------: | :--------------------: | :-----------------: | :-----: | :-----------------: | :-----------------: |
| value type *compound* |  |  |  | |  |

