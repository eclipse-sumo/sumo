---
title: Change chargingStation State
---

# Command 0x45: Change chargingStation State

|  ubyte   |      string       |       ubyte       | <value_type\> |
| :------: | :---------------: | :---------------: | :----------: |
| Variable | chargingStation ID | Type of the value |  New Value   |

Changes the state of a chargingStation. Because it is possible to change
different values of a chargingStation, the given value may have different
types. The following values can be changed, the type of the value to
given is also shown in the table.

<center>**Overview Changeable chargingStation Variables**</center>

| Variable                 | ValueType | Description       |  [Python Method](../TraCI/Interfacing_TraCI_from_Python.md)    |
| ------------------------ | ----------| ----------------- | -------------------------------------------------------------- |
| power (0x97)             | double    | The charging power in W |  [setChargingPower](https://sumo.dlr.de/pydoc/traci._chargingstation.html#ChargingStationDomain-setChargingPower) |
| efficiency (0x98)        | double    | The charging efficiency | [setEfficiency](https://sumo.dlr.de/pydoc/traci._chargingstation.html#ChargingStationDomain-setEfficiency) |
| charge in transit (0x99) | integer   | Whether the vehicle is forced/not forced to stop for charging | [setChargeInTransit](https://sumo.dlr.de/pydoc/traci._chargingstation.html#ChargingStationDomain-setChargeInTransit) |
| charge delay (0x9a)      | double    | The time delay before starting charging in s | [setChargeDelay](https://sumo.dlr.de/pydoc/traci._chargingstation.html#ChargingStationDomain-setChargeDelay) |

