---
title: Change parkingArea State
---

# Command 0x44: Change parkingArea State

|  ubyte   |      string       |       ubyte       | <value_type\> |
| :------: | :---------------: | :---------------: | :----------: |
| Variable | parkingArea ID | Type of the value |  New Value   |

Changes the state of a parkingArea. Because it is possible to change
different values of a parkingArea, the given value may have different
types. The following values can be changed, the type of the value to
given is also shown in the table.

<center>**Overview Changeable parkingArea Variables**</center>

| Variable                 | ValueType   | Description       |  [Python Method](../TraCI/Interfacing_TraCI_from_Python.md)    |
| ------------------------ | ----------- | ----------------- | -------------------------------------------------------------- |
| accepted badges (0x9b)   | stringList  | Set the accepted badges for parkingArea access. |  [setAcceptedBadges](https://sumo.dlr.de/pydoc/traci._parkingarea.html#ParkingAreaDomain-setAcceptedBadges) |
