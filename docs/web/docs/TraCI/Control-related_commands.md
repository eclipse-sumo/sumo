---
title: TraCI/Control-related commands
permalink: /TraCI/Control-related_commands/
---

# Control-related commands

## Command 0x00: Get Version

|               |
| :-----------: |
|      \-       |
| **no parameters** |

### Response 0x00: Version

|             |            |
| :---------: | :--------: |
|   integer   |   string   |
| **API Version** | **Identifier** |

The server responds to a **Get Version** command by sending two items:

- an API Version of **10** identifies the current state of the TraCI
API (sumo 0.24.0 or later).
  - It is guaranteed to increase as soon as
    - the TraCI message format changes
    - the TraCI command format changes
    - any TraCI command has a different meaning (e.g., "0xff:
      stop" changes from "stop immediately" to "slow down, then
      stop if possible")
    - any TraCI parameter has a different meaning (e.g., a "time"
      parameter changes from seconds to milliseconds)
    - addition of a new command
    - removal of a command
    - addition of optional parameters
    - addition of optional parameter types
    - addition of optional result types
  - The API version is only incremented for releases and not
    inbetween
- an identifier string identifies the software version running on the
TraCI server in human-readable form
  - no guarantee is made regarding the content of this string
  - as an example, for SUMO 0.12.0, this string is "SUMO 0.12.0"

#### Tip

The client can find out if a particular command is supported by the
current TraCI server (i.e., SUMO) by looking at the [status response](../TraCI/Protocol.md#status_response) of the command. It
may report "not implemented."

## Command 0x02: Simulation Step

|                  |
| :--------------: |
|      double      |
| **TargetTime \[s\]** |

Forces SUMO to perform simulation. If *TargetTime* is 0 (zero), SUMO
performs exactly one time step. Otherwise SUMO performs the simulation
until the given time step is reached. If the given time step is smaller
than or equal to the current simulation step then SUMO does nothing. For
multi client scenarios currently only TargetTime 0 is supported.

The response of this command is a list of subscription responses to
[TraCI/Object Variable Subscriptions](../TraCI/Object_Variable_Subscription.md) and/or
[TraCI/Object Context Subscriptions](../TraCI/Object_Context_Subscription.md) referring
to the last step executed:

|                                            |                         |     |                         |
| :----------------------------------------: | :---------------------: | :-: | :---------------------: |
|                  integer                   | <SUBSCRIPTION RESPONSE\> | ... | <SUBSCRIPTION RESPONSE\> |
| **Number of following subscription responses** |     **<RESPONSE\#1\>**     | ... |     **<RESPONSE\#n\>**     |

Please note, that both subscription types differ in their return values,
so the parser should regard the id of the returned command.

## Command 0x7F: Close

Tells TraCI to close the connection to any client, stop simulation and
shut down sumo.

## Command 0x01: Load

|             |
| :---------: |
| stringList  |
| **option list** |

Tells TraCI to reload the simulation with the given options.

!!! caution
    Loading does not work when using multiple clients, currently.

## Command 0x03: SetOrder

|                      |
| :------------------: |
|       integer        |
| **number of the client** |

Tells TraCI to give the current client the given position in the
execution order. It is mandatory to send this as the first command after
connecting to the TraCI server when using multiple clients. Each client
must be assigned a unique integer but there are not further restrictions
on numbering.