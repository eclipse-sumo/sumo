---
title: Object Context Subscription
---

# Introduction

Context subscriptions are allowing the obtaining of specific values from
surrounding objects of a certain so called "EGO" object. With these
data one can determine the traffic status around that EGO object. Such
an EGO Object can be any possible Vehicle, inductive loop,
points-of-interest, and such like. A vehicle driving through a city, for
example, is surrounded by a lot of different and changing vehicles,
lanes, junctions, or points-of-interest along his ride. Context
subscriptions can provide selected variables of those objects that
surround the EGO object within a certain range.

!!! note
    The EGO object itself is included in the context subscription result when its domain is queried (e.g. a vehicle is always in the set of all vehicles in range of itself).

The subscription for a structure's context is initiated using a
"Subscribe ... Context" command (0x80-0x8e). The command is evaluated
immediately on request, verifying it this way. It returns a "Subscribe
... Context" response (0x90-0x9e). If EGO is a vehicle, the subscription
is descheduled as soon as the vehicle leaves the simulation.

As soon as the subscription was accepted, it is executed after each call
of [Simulation
Step(2)](../TraCI/Control-related_commands.md#command_0x02_simulation_step)
command.

The command ID describes the type of the EGO-object which is the center
of the context; currently, the following EGO-objects are supported:

| Type               | Command ID | Notes |
| ------------------ | ---------- | ----- |
| inductive loops    | 0x80       |       |
| lanes              | 0x83       |       |
| vehicles           | 0x84       |       |
| points-of-interest | 0x87       |       |
| polygons           | 0x88       |       |
| junctions          | 0x89       |       |
| edges              | 0x8a       |       |

For a given EGO-structure, each subscription allows to address multiple
variables from structures of a certain kind. This means, if you have an
EGO-vehicle, you may ask for speeds and positions of vehicles in range
of EGO. If you also need information about surrounding
points-of-interest, you have to add a further context subscription for
PoIs. Vehicles, and PoIs belong to different "context domains". The
following domains are available:

| Type               | Context Domain ID | Notes                                                            |
| ------------------ | ----------------- | ---------------------------------------------------------------- |
| inductive loops    | 0xa0              | [retrievable variables](../TraCI/Induction_Loop_Value_Retrieval.md) |
| lanes              | 0xa3              | [retrievable variables](../TraCI/Lane_Value_Retrieval.md)           |
| vehicles           | 0xa4              | [retrievable variables](../TraCI/Vehicle_Value_Retrieval.md)        |
| points-of-interest | 0xa7              | [retrievable variables](../TraCI/POI_Value_Retrieval.md)            |
| polygons           | 0xa8              | [retrievable variables](../TraCI/Polygon_Value_Retrieval.md)        |
| junctions          | 0xa9              | [retrievable variables](../TraCI/Junction_Value_Retrieval.md)       |
| edges              | 0xaa              | [retrievable variables](../TraCI/Edge_Value_Retrieval.md)           |


For each domain, the same variables can be retrieved as using the Get
... Variable commands (see above).

# Command 0x8*X*: Subscribe ... Context

|    time    |   time   |  string   |     ubyte      |    double     |      ubyte      |           ubyte\[n\]            |
| :--------: | :------: | :-------: | :------------: | :-----------: | :-------------: | :-----------------------------: |
| begin Time | end Time | Object ID | Context Domain | Context Range | Variable Number | The list of variables to return |

Some notes:

- begin Time: the subscription is executed only in time steps \>= this
  value; in ms
- end Time: the subscription is executed in time steps <= this value;
  the subscription is removed if the simulation has reached a higher
  time step; in ms
- Context Domain: the type of objects in the addressed object's
  surroundings to ask values from
- Context Range: the radius of the surroundings
- The size of the variables list must be equal to the field "Variable
  Number".

# Response 0x9*X*: ... Subscription Response

|  string   |     ubyte      |     ubyte      |      int      |    string     |            ubyte             |              ubyte               |                    ubyte                     |       <return_type\>       | ... |            ubyte             |              ubyte               |                    ubyte                     |       <return_type\>       | ... |    string     |            ubyte             |              ubyte               |                    ubyte                     |       <return_type\>       | ... |            ubyte             |              ubyte               |                    ubyte                     |       <return_type\>       |
| :-------: | :------------: | :------------: | :-----------: | :-----------: | :--------------------------: | :------------------------------: | :------------------------------------------: | :-----------------------: | :-: | :--------------------------: | :------------------------------: | :------------------------------------------: | :-----------------------: | :-: | :-----------: | :--------------------------: | :------------------------------: | :------------------------------------------: | :-----------------------: | :-: | :--------------------------: | :------------------------------: | :------------------------------------------: | :-----------------------: |
| Object ID | Context Domain | Variable Count | Objects Count | Object \#1 ID | Object \#1 / Variable \#1 Id | Object \#1 / Variable \#1 status | Object \#1 / Return type of the variable \#1 | Object \#1 / <VALUE\#1\> | ... | Object \#1 / Variable \#n Id | Object \#1 / Variable \#n status | Object \#1 / Return type of the variable \#n | Object \#1 / <VALUE\#n\> | ... | Object \#m ID | Object \#m / Variable \#1 Id | Object \#m / Variable \#1 status | Object \#m / Return type of the variable \#1 | Object \#m / <VALUE\#1\> | ... | Object \#m / Variable \#n Id | Object \#m / Variable \#n status | Object \#m / Return type of the variable \#n | Object \#m / <VALUE\#n\> |

The response to a **"Subscribe ... Variable"**.

The status is 0x00 (RTYPE_OK) if the variable could be retrieved
successfully. If not, the status is 0xff (RTYPE_ERR). In the second
case, the variable type is set to string and the variable value contains
the error message. Please note, that the status should be ok in all
cases.

- ***Variable Count*** is the number of returned variables per object
- ***Objects Count*** is the number of objects in range

This implies that n\*m values are returned (Variable Count \* Objects
Count).

# Command 0x7E: Add Context Subscription Filter

|    ubyte    |   \[ubyte\]    | \[double/list(byte)/list(string)\] |
| :---------: | :------------: | :--------------------------------: |
| filter type | parameter type |          parameter value           |

When adding a context subscription filter, it is applied to the *vehicle* context subscription (command ID 0x84) that was created most recently.

!!! note
    Context subscription filters are only supported for ego objects of type 'vehicle'.

Currently, the following context subscription filter types are implemented:

| filter name | filter type |   parameter value   |     description     |       uses context range       |
|  ---------  | :---------: | :-----------------: |  -----------------  | :----------------------------: |
| [lanes](https://sumo.dlr.de/daily/pydoc/traci._vehicle.html#VehicleDomain-addSubscriptionFilterLanes)       | 0x01        |    list(byte)       | only return vehicles on list of lanes relative to ego vehicle | no |
| [no-opposite](https://sumo.dlr.de/daily/pydoc/traci._vehicle.html#VehicleDomain-addSubscriptionFilterNoOpposite) | 0x02        |    -                | exclude vehicles on opposite (and other) lanes | yes |
| [downstream distance](https://sumo.dlr.de/daily/pydoc/traci._vehicle.html#VehicleDomain-addSubscriptionFilterDownstreamDistance) | 0x03|    double           | only return vehicles within the given downstream distance | no |
| [upstream distance](https://sumo.dlr.de/daily/pydoc/traci._vehicle.html#VehicleDomain-addSubscriptionFilterUpstreamDistance) | 0x04  |    double           | only return vehicles within the given maximal upstream distance | no |
| [leader/follower](https://sumo.dlr.de/daily/pydoc/traci._vehicle.html#VehicleDomain-addSubscriptionFilterLeadFollow)   | 0x05  |    -                | only return leader and follower on the specified lanes (requires 'lanes' filter) | no |
| [turn](https://sumo.dlr.de/daily/pydoc/traci._vehicle.html#VehicleDomain-addSubscriptionFilterTurn)        | 0x07        |    -                | only return foes on upcoming junctions | no |
| [vClass](https://sumo.dlr.de/daily/pydoc/traci._vehicle.html#VehicleDomain-addSubscriptionFilterVClass)     | 0x08        |    list(string)      | only return vehicles of the given vClass(es) | yes |
| [vType](https://sumo.dlr.de/daily/pydoc/traci._vehicle.html#VehicleDomain-addSubscriptionFilterVType)       | 0x09        |    list(string)     | only return vehicles of the given vType(s) | yes |
| [field of vision](https://sumo.dlr.de/daily/pydoc/traci._vehicle.html#VehicleDomain-addSubscriptionFilterFieldOfVision) | 0x0A    |    double           | only return vehicles within field of vision (angle in degrees) | yes |
| [lateral distance](https://sumo.dlr.de/daily/pydoc/traci._vehicle.html#VehicleDomain-addSubscriptionFilterLateralDistance) | 0x0B    |    double          | only return vehicles within the given lateral distance | no |

### Intercompatibility

While the context subscription filters can - in principal - be combined, be aware that not all combinations will work (or at least not as expected).
This is due to the fact that a sub-class of the filters (see last column 'uses context range' above) does not operate on the set of objects collected within the context subscription's range but operates on the set of objects defined by their respective filter parameters instead.
Consequently, a combination of, e.g., the field-of-vision and lateral distance filter is infeasible.

# Client library methods

- In the [python
  library](../TraCI/Interfacing_TraCI_from_Python.md#context_subscriptions),
  all domains support the methods [*subscribeContext* and
  *unsubscribeContext*](http://sumo.dlr.de/daily/pydoc/traci.domain.html#Domain)
- For the python client several [methods for adding context filters
  for vehicle-to-vehicle context
  subscriptions](http://sumo.dlr.de/daily/pydoc/traci._vehicle.html#VehicleDomain-addSubscriptionFilterCFManeuver)
  are included.
- In the [C++ library](../TraCI/C++TraCIAPI.md), the method
  *simulation.subscribeContext* takes an additional argument that
  encodes the source domain

# Notes

- When asking for domains "point-of-interest" and "polygon", the
  current version does not support asking for these structures when
  their position was changed using TraCI
