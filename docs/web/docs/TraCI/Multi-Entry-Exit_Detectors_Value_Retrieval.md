---
title: Multi-Entry-Exit Detectors Value Retrieval
---

## Command 0xa1: Get MeMeDetector Variable

|  ubyte   |               string               |
| :------: | :--------------------------------: |
| Variable | Multi-Entry/Multi-Exit Detector ID |

Asks for the value of a certain variable of the named
[multi-entry/multi-exit (e3)
detector](../Simulation/Output/Multi-Entry-Exit_Detectors_(E3).md).
The value returned is the state of the asked variable/value within the
last simulation step. Please note that for asking values from your
[multi-entry/multi-exit
detectors](../Simulation/Output/Multi-Entry-Exit_Detectors_(E3).md)
with TraCI, you have to define them in an additional input file (see
[multi-entry/multi-exit (e3)
detectors](../Simulation/Output/Multi-Entry-Exit_Detectors_(E3).md))
and cannot add them via TraCI (yet). The following variable values can
be retrieved, the type of the return value is also shown in the table.

<center>
**Overview Retrievable Multi-Entry/Multi-Exit Detector Variables**
</center>

| Variable                                 | ValueType  | Description                                                                                                                                | [Python Method](../TraCI/Interfacing_TraCI_from_Python.md)                                                                |
| ---------------------------------------- | ---------- | ------------------------------------------------------------------------------------------------------------------------------------------ | ------------------------------------------------------------------------------------------------------------------------------ |
| id list (0x00)                           | stringList | Returns a list of ids of all multi-entry/multi-exit detectors within the scenario (the given DetectorID is ignored)                        | [getIDList](https://sumo.dlr.de/pydoc/traci._multientryexit.html#MultiEntryExitDomain-getIDList)                               |
| count (0x01)                             | int        | Returns the number of multi-entry/multi-exit detectors within the scenario (the given DetectorID is ignored)                               | [getIDCount](https://sumo.dlr.de/pydoc/traci._multientryexit.html#MultiEntryExitDomain-getIDCount)                             |
| entry lanes (0x30)               | stringList | Returns a list of ids of the multi-entry/multi-exit detector's entry lanes          | [getEntryLanes](https://sumo.dlr.de/pydoc/traci._multientryexit.html#MultiEntryExitDomain-getEntryLanes)                               |
| exit lanes (0x31)               | stringList | Returns a list of ids of the multi-entry/multi-exit detector's exit lanes          | [getExitLanes](https://sumo.dlr.de/pydoc/traci._multientryexit.html#MultiEntryExitDomain-getExitLanes)                               |
| entry positions (0x42)               | doubleList | Returns a list of lane positions (in meters) of the multi-entry/multi-exit detector's entry positions          | [getEntryPositions](https://sumo.dlr.de/pydoc/traci._multientryexit.html#MultiEntryExitDomain-getEntryPositions)                               |
| exit positions (0x43)               | doubleList | Returns a list of lane positions (in meters) of the multi-entry/multi-exit detector's exit positions          | [getExitPositions](https://sumo.dlr.de/pydoc/traci._multientryexit.html#MultiEntryExitDomain-getExitPositions)                               |
| last step vehicle number (0x10)          | int        | Returns the number of vehicles that have been within the named multi-entry/multi-exit detector within the last simulation step \[\#\];<br><br>**Note:** If the interval length for the detector is equal to the timestep length, this value corresponds to the *vehicleSumWithin* measure of [multi-entry/multi-exit detectors](../Simulation/Output/Multi-Entry-Exit_Detectors_(E3).md).     | [getLastStepVehicleNumber](https://sumo.dlr.de/pydoc/traci._multientryexit.html#MultiEntryExitDomain-getLastStepVehicleNumber) |
| last step mean speed (0x11)              | double     | Returns the mean speed of vehicles that have been within the named multi-entry/multi-exit detector within the last simulation step \[m/s\]<br><br>**Note:** If the interval length for the detector is equal to the timestep length, this value corresponds to the *meanSpeedWithin* measure of [multi-entry/multi-exit detectors](../Simulation/Output/Multi-Entry-Exit_Detectors_(E3).md). | [getLastStepMeanSpeed](https://sumo.dlr.de/pydoc/traci._multientryexit.html#MultiEntryExitDomain-getLastStepMeanSpeed)         |
| last step vehicle ids (0x12)             | stringList | Returns the list of ids of vehicles that have been within the named multi-entry/multi-exit detector in the last simulation step            | [getLastStepVehicleIDs](https://sumo.dlr.de/pydoc/traci._multientryexit.html#MultiEntryExitDomain-getLastStepVehicleIDs)       |
| last step halting vehicles number (0x14) | int        | Returns the number of vehicles which were halting during the last time step                                                                | [getLastStepHaltingNumber](https://sumo.dlr.de/pydoc/traci._multientryexit.html#MultiEntryExitDomain-getLastStepHaltingNumber) |
| last interval travel time (0x58) | double        | Returns the average travel time of vehicles that passed the detector in the last complete measurement interval | [getLastIntervalMeanTravelTime](https://sumo.dlr.de/pydoc/traci._multientryexit.html#MultiEntryExitDomain-getLastIntervalMeanTravelTime) |
| last interval halts per vehicle (0x20) | double        | Returns the average number of halts of vehicles that passed the detector in the last complete measurement interval | [getLastIntervalMeanHaltsPerVehicle](https://sumo.dlr.de/pydoc/traci._multientryexit.html#MultiEntryExitDomain-getLastIntervalMeanHaltsPerVehicle) |
| last interval time loss (0x8c) | double        | Returns the average time loss of vehicles that passed the detector in the last complete measurement interval  | [getLastInervalMeanTimeLoss](https://sumo.dlr.de/pydoc/traci._multientryexit.html#MultiEntryExitDomain-getLastIntervalMeanTimeLoss) |
| last interval vehicles (0x21) | int        | Returns the number of vehicles that passed the detector in the last complete measurement interval  | [getLastStepHaltingNumber](https://sumo.dlr.de/pydoc/traci._multientryexit.html#MultiEntryExitDomain-getLastStepHaltingNumber) |

## Response 0xb1: MeMeDetector Variable

|  ubyte   |               string               |            ubyte            |  <return_type\>   |
| :------: | :--------------------------------: | :-------------------------: | :--------------: |
| Variable | Multi-Entry/Multi-Exit Detector ID | Return type of the variable | <VARIABLE_VALUE\> |

The respond to a **"Command Get MeMeDetector Variable"**.

## Notes

- You can find [some further description on multi-entry/multi-exit
  detectors](../Simulation/Output/Multi-Entry-Exit_Detectors_(E3).md)
