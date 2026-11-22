---
title: Object Variable Subscription
---

# Introduction

Variable subscriptions allow you to ask once for a set of a structure's
variables and retrieve them periodically. In accordance to plain value
retrieval commands (0xa0-0xae) the subscription commands are split by
the "domain" - the type of the structure:

- command 0xd0: retrieve information from an induction loop (for a
  list of retrievable variables, see [Induction Loop Value
  Retrieval](../TraCI/Induction_Loop_Value_Retrieval.md))
- command 0xd1: retrieve information from a multi-entry/multi-exit
  detector (see [Multi-Entry-Exit Detectors Value
  Retrieval](../TraCI/Multi-Entry-Exit_Detectors_Value_Retrieval.md))
- command 0xd2: retrieve information from a traffic light (see
  [Traffic Lights Value
  Retrieval](../TraCI/Traffic_Lights_Value_Retrieval.md))
- command 0xd3: retrieve information from a lane (see [Lane Value
  Retrieval](../TraCI/Lane_Value_Retrieval.md))
- command 0xd4: retrieve information from a vehicle (see [Vehicle
  Value Retrieval](../TraCI/Vehicle_Value_Retrieval.md))
- command 0xd5: retrieve information from a vehicle type (see [Vehicle
  Type Value Retrieval](../TraCI/VehicleType_Value_Retrieval.md))
- command 0xd6: retrieve information from a route (see [Route Value
  Retrieval](../TraCI/Route_Value_Retrieval.md))
- command 0xd7: retrieve information from a point-of-interest (see
  [PoI Value Retrieval](../TraCI/POI_Value_Retrieval.md))
- command 0xd8: retrieve information from a polygon (see [Polygon
  Value Retrieval](../TraCI/Polygon_Value_Retrieval.md))
- command 0xd9: retrieve information from a junction (see [Junction
  Value Retrieval](../TraCI/Junction_Value_Retrieval.md))
- command 0xda: retrieve information from an edge (see [Edge Value
  Retrieval](../TraCI/Edge_Value_Retrieval.md))
- command 0xdb: retrieve information from the simulation (see
  [Simulation Value
  Retrieval](../TraCI/Simulation_Value_Retrieval.md))

The subscription for a structure's variables is initiated using a
"Subscribe ... Variable" command (0xd0-0xde). The command is evaluated
immediately on request, verifying it this way. It returns a "Subscribe
... Variable" response (0xe0-0xee). In the case of vehicles, the
subscription is descheduled as soon as the vehicle leaves the
simulation.

As soon as the subscription was accepted, it is executed after each call
of [Simulation
Step(2)](../TraCI/Control-related_commands.md#command_0x02_simulation_step)
command.

# Command 0xd*X*: Subscribe ... Variable

|    time    |   time   |  string   |      ubyte      |           ubyte\[n\]            |
| :--------: | :------: | :-------: | :-------------: | :-----------------------------: |
| begin Time | end Time | Object ID | Variable Number | The list of variables to return |

Some notes:

- begin Time: the subscription is executed only in time steps \>= this
  value; in ms
- end Time: the subscription is executed in time steps <= this value;
  the subscription is removed if the simulation has reached a higher
  time step; in ms
- The size of the variables list must be equal to the field "Variable
  Number".

# Response 0xe*X*: ... Subscription Response

|  string   |     ubyte      |      ubyte      |        ubyte        |              ubyte              |     <return_type\>      | ... |      ubyte      |        ubyte        |              ubyte              |     <return_type\>      |
| :-------: | :------------: | :-------------: | :-----------------: | :-----------------------------: | :--------------------: | :-: | :-------------: | :-----------------: | :-----------------------------: | :--------------------: |
| Object ID | Variable Count | Variable \#1 Id | Variable \#1 status | Return type of the variable \#1 | <VARIABLE_VALUE\#1\> | ... | Variable \#n Id | Variable \#n status | Return type of the variable \#n | <VARIABLE_VALUE\#n\> |

The respond to a **"Subscribe ... Variable"**.

The status is 0x00 (RTYPE_OK) if the variable could be retrieved
successfully. If not, the status is 0xff (RTYPE_ERR). In the second
case, the variable type is set to string and the variable value contains
the error message.

***Variable Count*** is the number of subscriptions which are returned.

# Client library methods

- In the [python
  library](../TraCI/Interfacing_TraCI_from_Python.md#subscriptions),
  all 24 TraCI domains (`inductionloop`, `lanearea`, `multientryexit`, `trafficlight`, `lane`, `vehicle`, `vehicletype`, `route`, `poi`, `polygon`, `junction`, `edge`, `simulation`, `gui`, `person`, `busstop`, `parkingarea`, `chargingstation`, `calibrator`, `rerouter`, `routeprobe`, `overheadwire`, `variablespeedsign`, and `meandata`) support [*subscribe*, *getSubscriptionResults*, *getAllSubscriptionResults*, and *unsubscribe*](https://sumo.dlr.de/pydoc/traci/domain.html#Domain).
- In [Libsumo](../Libsumo.md) and [Libtraci](../Libtraci.md), each domain class (`Vehicle`, `Simulation`, `Edge`, and the remaining domains) provides static `subscribe`, `getSubscriptionResults`, `getAllSubscriptionResults`, and `unsubscribe` methods.
- In the legacy [C++ socket library](../TraCI/C++TraCIAPI.md), *simulation.subscribe* takes an additional command-ID argument that encodes the domain.

# Symbolic Constants and Subscribable Variables

Pass symbolic constants from `traci.constants` in Python (`import traci.constants as tc`) or `<libsumo/TraCIConstants.h>` in C++ and Java (`org.eclipse.sumo.libtraci.Constants`) when subscribing:

```python
import traci
import traci.constants as tc

traci.vehicle.subscribe(vehID, [tc.VAR_SPEED, tc.VAR_ROAD_ID, tc.VAR_LANEPOSITION, tc.VAR_WAITING_TIME])
```

Each domain's value-retrieval page lists the hexadecimal variable IDs alongside their getters, and the matching `VAR_*` constant names are defined in [`tools/traci/constants.py`](https://sumo.dlr.de/pydoc/traci/constants.html) (plus `tc.TRACI_ID_LIST` for `getIDList()` and `tc.ID_COUNT` for `getIDCount()`).

## Variables Excluded from Subscriptions

Almost all read-only state variables across the value-retrieval pages are subscribable. The following commands and variables cannot be subscribed:

- **Stateless coordinate, distance, and routing queries**: `POSITION_CONVERSION` (`0x82`), `DISTANCE_REQUEST` (`0x83`), `FIND_ROUTE` (`0x86`), and `FIND_INTERMODAL_ROUTE` (`0x87`) in `simulation`.
- **State-modifying queries**: `TL_CONSTRAINT_SWAP` (`0x32`) in `trafficlight` and `SPLIT_TAXI_RESERVATIONS` (`0x01` flag) in `person`.
- **Domain ID list on `simulation`**: `TRACI_ID_LIST` (`0x00`) and `ID_COUNT` (`0x01`) are unsupported on `traci.simulation.subscribe` (use `VAR_LOADED_VEHICLES_IDS`, `VAR_DEPARTED_VEHICLES_IDS`, or `VAR_MIN_EXPECTED_NUMBER` on `simulation`, or `TRACI_ID_LIST` on an entity domain with an arbitrary object ID).
- **Libsumo and Libtraci specifics**: [Libsumo](../Libsumo.md) does not support subscribing to `simulation` `VAR_COLLISIONS` (`0x23`), `simulation` `VAR_NET_BOUNDING_BOX` (`0x7c`), or `meandata` `VAR_MEANDATA_LANE` (`0x16`); call their direct getters (`Simulation::getCollisions()`, `Simulation::getNetBoundary()`) instead. [Libtraci](../Libtraci.md) supports parameterized subscriptions via `Vehicle::subscribeLeader` (`VAR_LEADER`) and `subscribeParameterWithKey` (`VAR_PARAMETER_WITH_KEY`), whereas multi-parameter subscriptions require the pure Python `traci` client.

## Variables Requiring Subscription Parameters

Any variable whose getter accepts arguments beyond `objectID` requires passing `parameters={varID: argument}` to `subscribe(...)` (or calling a dedicated helper where provided):

| Domain | Symbolic Constant | Extra Parameter Type |
| --- | --- | --- |
| Any domain | `tc.VAR_PARAMETER` (`0x7e`), `tc.VAR_PARAMETER_WITH_KEY` (`0x3e`) | `string` parameter key (or `subscribeParameterWithKey`) |
| `vehicle` | `tc.VAR_LEADER` (`0x68`) | `double` lookahead distance in m (or `traci.vehicle.subscribeLeader`) |
| `vehicle` | `tc.VAR_NEIGHBORS` (`0xbf`) | `ubyte` bitset mode |
| `vehicle` | `tc.VAR_NEXT_STOPS` (`0x73`) | `int` selection mode |
| `vehicle` | `tc.VAR_FOES` (`0x37`) | `double` distance |
| `vehicle`, `edge` | `tc.VAR_EDGE_TRAVELTIME` (`0x58`), `tc.VAR_EDGE_EFFORT` (`0x59`) | `double` time (on `edge`) or compound `(time, edgeID)` (on `vehicle`) |
| `vehicle` | `tc.VAR_FOLLOW_SPEED` (`0x1c`), `tc.VAR_SECURE_GAP` (`0x1d`), `tc.VAR_STOP_SPEED` (`0x1e`) | Compound tuple of speed, gap, and leader parameters |

# Special Cases

- The VAR_LEADER variable is special because the *getLeader (0x68)*
  command requires an additional distance argument. To subscribe to
  VAR_LEADER, the function *subscribeLeader* is provided by the
  [python client](../TraCI/Interfacing_TraCI_from_Python.md).
- The VAR_PARAMETER_WITH_KEY generic variable is special because the *getParameterWithKey (0x3e)*
  command requires an additional key argument. To subscribe to
  VAR_PARAMETER_WITH_KEY, the function *subscribeParameterWithKey* is provided by the
  [python client](../TraCI/Interfacing_TraCI_from_Python.md).

## Subscriptions with Parameters

In addition to the two special cases above there is a general mechanism
to subscribe to values which need extra parameters. In general you need to provide
an additional map as parameter to the function where the key is the variable
you want to subscribe to and the value is the value of the parameter (wrapped
in the corresponding TraCIResult object in the C++ / Java case).

If you need to subscribe to a value with multiple parameters this is currently only possible in python.
You need to give a tuple of values where the first item describes the types of the parameters (always starting with a "t" for tuple and then "d", "i", "s" for floats, integers and strings respectively). The second item is the number of parameters and then the actual parameter values.
One example would be: `param = {traci.constants.VAR_STOP_SPEED: ("tdd", 2, 0., 0.)}`.
