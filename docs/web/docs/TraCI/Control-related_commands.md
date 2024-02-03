---
title: Control-related commands
---

## Command 0x00: Get Version

| Parameter     | [Python Method](../TraCI/Interfacing_TraCI_from_Python.md)|
| :-----------: | :-----------: |
|      \-       | [getVersion](https://sumo.dlr.de/pydoc/traci.main.html#-getVersion) |

### Response 0x00: Version

| ValueType   | Description| [Python Method](../TraCI/Interfacing_TraCI_from_Python.md)|
| --------- | -------- | ----------- |
|   tuple     |   Returns a tuple containing the TraCI API version number (integer) and a string identifying the SUMO version running on the TraCI server in human-readable form.   | [getVersion](https://sumo.dlr.de/pydoc/traci.main.html#-getVersion) |

The server responds to a **Get Version** command by sending two items:

- an integer API version number identifying the current state of the TraCI API
  - It is guaranteed to increase as soon as
    - the TraCI message format changes
    - the TraCI command format changes
    - any TraCI command has a different meaning (e.g., "0xff:
      stop" changes from "stop immediately" to "slow down, then
      stop if possible")
    - any TraCI parameter or return value has a different meaning (e.g., a "time"
      parameter changes from seconds to milliseconds)
    - any TraCI parameter or return value has a different type (e.g., a "time"
      parameter changes from int to double)
    - removal of a command
  - It will not increase in the following events
    - addition of a new command
    - addition of optional parameters
    - addition of parameter types
    - addition of result types
    - changes in the python (or any other) client implementation which do not affect the byte sequence sent
  - The API version should be incremented as soon as the change is introduced
    but at least at the next release
- an identifier string identifies the software version running on the
TraCI server in human-readable form
  - no guarantee is made regarding the content of this string
  - for SUMO, this string has the format "SUMO " and then the version, e.g. "SUMO v1_6_0" or "SUMO v1_6_0+1493-02f4414fa4"
  - for further information concerning the SUMO version have a look at [Versioning](../Versioning.md).

!!! Note
    The client can find out if a particular command is supported by the
    current TraCI server (i.e., SUMO) by looking at the [status response](../TraCI/Protocol.md#status_response) of the command. It
    may report "not implemented".

## Command 0x02: Simulation Step

| Variable       | ValueType      | Description    | [Python Method](../TraCI/Interfacing_TraCI_from_Python.md)|
| -------------- | -------------- | -------------- | -------------- |
| simulationStep (0x02) |  double     | Make a simulation step. | [simulationStep](https://sumo.dlr.de/pydoc/traci.main.html#-simulationStep) |

Forces SUMO to perform simulation. If *TargetTime* is 0 (zero), SUMO
performs exactly one time step. Otherwise SUMO performs the simulation
until the given time step is reached. If the given time step is smaller
than or equal to the current simulation step then SUMO does nothing. For
multi client scenarios currently only TargetTime 0 is supported.

### Response 0x02: Simulation Step

The response of this command is a list of subscription responses to
[TraCI/Object Variable Subscriptions](../TraCI/Object_Variable_Subscription.md) and/or
[TraCI/Object Context Subscriptions](../TraCI/Object_Context_Subscription.md) referring
to the last step executed:

|                                                |                          |     |                          |
| :--------------------------------------------: | :----------------------: | :-: | :----------------------: |
|                    integer                     | <SUBSCRIPTION RESPONSE\> | ... | <SUBSCRIPTION RESPONSE\> |
| **Number of following subscription responses** |    **<RESPONSE\#1\>**    | ... |    **<RESPONSE\#n\>**    |

Please note, that both subscription types differ in their return values,
so the parser should regard the id of the returned command.

## Command 0x7D: Execute Move

| Parameter     | [Python Method](../TraCI/Interfacing_TraCI_from_Python.md)|
| :-----------: | :-----------: |
|      \-       | [executeMove](https://sumo.dlr.de/pydoc/traci.main.html#-executeMove) |

Performs only the first part of a simulation step until the vehicles have moved but before the outputs are generated.
A subsequent call to simulation step will then create the output.

## Command 0x7F: Close

| Parameter     | [Python Method](../TraCI/Interfacing_TraCI_from_Python.md)|
| :-----------: | :-----------: |
|      \-       | [close](https://sumo.dlr.de/pydoc/traci.main.html#-close) |

Tells TraCI to close the connection to any client, stop simulation and
shut down sumo.

## Command 0x01: Load

| Options     |  Description | [Python Method](../TraCI/Interfacing_TraCI_from_Python.md)|
| :-----------: |:-----------: | :-----------: |
| stringList     | Let sumo load a simulation using the given command line like options | [load](https://sumo.dlr.de/pydoc/traci.main.html#-load) |

Tells TraCI to reload the simulation with the given options.

!!! caution
    Loading does not work when using multiple clients, currently.

## Command 0x03: SetOrder

| Parameter     | ValueType | Description | [Python Method](../TraCI/Interfacing_TraCI_from_Python.md)|
| ----------- | ----------- | ----------- | ----------- |
| number of the client    | int  | Specify the execution order (when using multiple clients) | [setOrder](https://sumo.dlr.de/pydoc/traci.main.html#-setOrder) |

Tells TraCI to give the current client the given position in the
execution order. It is mandatory to send this as the first command after
connecting to the TraCI server when using multiple clients. Each client
must be assigned a unique integer but there are not further restrictions
on numbering.
