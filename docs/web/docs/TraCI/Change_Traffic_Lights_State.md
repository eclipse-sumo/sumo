---
title: Change Traffic Lights State
---

# Command 0xc2: Change Traffic Light State

|  ubyte   |      string       |       ubyte       | <value_type\> |
| :------: | :---------------: | :---------------: | :----------: |
| Variable | Traffic Lights ID | Type of the value |  New Value   |

Changes the state of a traffic light. Because it is possible to change
different values of a traffic light, the given value may have different
types. The following values can be changed, the type of the value to
given is also shown in the table.

<center>**Overview Changeable Traffic Lights Variables**</center>

| Variable                            | ValueType            | Description   | [Python Method](../TraCI/Interfacing_TraCI_from_Python.md)     |
| ----------------------------------- | -------------------- | --------------------------------------------------------------------- | ----------------- |
| state (light/priority tuple) (0x20) | string               | Sets the phase definition to the given. Assumes the given string is a tuple of light definitions from rRgGyYoO, for red, green, yellow, off, where lower case letters mean that the stream has to decelerate. After this call the program-ID of the traffic light will be set to "online" and the state will be maintained until the next call of setRedYellowGreenState() or until setting another program with setProgram(). | [setRedYellowGreenState](https://sumo.dlr.de/pydoc/traci/_trafficlight.html#TrafficLightDomain.setRedYellowGreenState) [setLinkState](https://sumo.dlr.de/pydoc/traci/_trafficlight.html#TrafficLightDomain.setLinkState) |
| phase index (0x22)                  | integer              | Sets the phase of the traffic light to the given. The given index must be valid for the current program of the traffic light, this means it must be between 0 and the number of phases known to the current program of the tls - 1.                                                                                                                                                                                            | [setPhase](https://sumo.dlr.de/pydoc/traci/_trafficlight.html#TrafficLightDomain.setPhase)                                                                                                                                |
| program (0x23)                      | string               | Switches the traffic light to the given program. No WAUT algorithm is used, the program is directly instantiated. The index of the traffic light stays the same as before.                                                                                                                                                                                                                                                     | [setProgram](https://sumo.dlr.de/pydoc/traci/_trafficlight.html#TrafficLightDomain.setProgram)                                                                                                                            |
| phase duration (0x24)               | double               | Sets the remaining duration of the current phase in seconds.                                                                                                                                                                                                                                                                                                                                                                   | [setPhaseDuration](https://sumo.dlr.de/pydoc/traci/_trafficlight.html#TrafficLightDomain.setPhaseDuration)                                                                                                                |
| complete program definition (0x2c)  | compound (see below) | Inserts a completely new program.                                                                                                                                                                                                                                                                                                                                                                                              | [setProgramLogic](https://sumo.dlr.de/pydoc/traci/_trafficlight.html#TrafficLightDomain.setProgramLogic)    |
| add constraint (0x37)             | compound (tripId, foeSignal, foeId, type, limit)  | Add the given constraint  | [addConstraint](https://sumo.dlr.de/pydoc/traci/_trafficlight.html#TrafficLightDomain.addConstraint)      |
| remove constraint (0x35)          | compound (tripId, foeSignal, foeId)  | Remove the given constraint  | [removeConstraint](https://sumo.dlr.de/pydoc/traci/_trafficlight.html#TrafficLightDomain.removeConstraint)      |
| update constraints (0x36)         | compound (vehID, tripId)  | Removes all constraints that can no longer be met because the route of vehID does not pass the signal involved in the constraint with the given tripId. This includes constraints on tripId as well as constraints where tripId is the foeId  | [updateConstraints](https://sumo.dlr.de/pydoc/traci/_trafficlight.html#TrafficLightDomain.updateConstraints)      |

The message contents are as following:

### state (light/priority tuple) (0x20)

|                 string                  |
| :-------------------------------------: |
| Phase Definition (light/priority tuple) |

### state (phase/brake/yellow) (0x21)

|    string    |    string    |    string     |
| :----------: | :----------: | :-----------: |
| Green bitset | Brake bitset | Yellow bitset |

### phase index (0x22)

|   integer   |
| :---------: |
| Phase Index |

### program (0x23)

|   string   |
| :--------: |
| Program ID |

### phase duration (0x24)

|        integer        |
| :-------------------: |
| Phase Duration \[ms\] |

### complete program definition (0x2c)

The value is a compound with five elements. An optional sixth element may be
inserted after the current phase index and before the phases:

| type + string | type + integer | type + integer | type + double (optional) | type + compound | type + compound |
| :-----------: | :------------: | :------------: | :----------------------: | :-------------: | :-------------: |
| Program ID | Type | Current phase index | Offset (seconds) | Phases | Parameters |

Clients omit the optional offset element when the offset is zero and send it
only for non-zero offsets. The server accepts both five and six elements.

Phases and parameters use the same structure as the
[complete program retrieval](Traffic_Lights_Value_Retrieval.md#structure_of_compound_object_traffic_light_program_0x2b).
The offset defaults to zero in the clients and is used when creating a new program.
For static programs, a non-zero offset shifts the program in the same way as the `offset` attribute of a
`tlLogic`: it determines the position within the cycle and therefore the current phase
as well as the time of the next switch. For actuated, NEMA and delay-based programs the
offset is stored and passed to the controller unchanged.

!!! note
    For more information about what value should be put in each of the fields mentioned above, see [Tutorials/TraCI4Traffic_Lights#Appendix](../Tutorials/TraCI4Traffic_Lights.md#appendix). Remember that you can check which integer value is associated with particular value type [here](../TraCI/Protocol.md#data_types).

# Setting Traffic light parameters (0x7e)

Traffic lights support setting of additional parameters using the [generic
parameter setting call](../TraCI/GenericParameters.md#set_parameter).

|      key        | value type (expressed as string) | supported by tlType |  description |
| :----------------: | :------------------------------: | :-----------------: | :----------: |
| cycleTime          | double (s)  | static, actuated, delay_based | default cycle duration (has no effect for static tls) |
| offset             | double (s)  | static, actuated, delay_based | offset for cycle start relative to simulation start |
| coordinated        | bool        | static, actuated, delay_based | whether time in cycle is computed relative to simulation time |
| max-gap            | double (s)  | actuated                      | detector time-out for abandoning phase
| show-detectors     | bool        | actuated                      | show/hide detectors in view
| inactive-threshold | double (s)  | actuated                      | time-out for switching to an unserved phase when running with the default phase-skipping logic
