---
title: Protocol
---

This page details the [TraCI](../TraCI/index.md) protocol. Please see
there for a general introduction.

## Basic Flow

After starting [sumo](../sumo.md), clients connect to
[sumo](../sumo.md) by setting up a TCP connection to the appointed
[sumo](../sumo.md) port. TraCI supports multiple clients and
executes all commands of a client in a sequence until it issues the
[TraCI/Control-related commands\#Command 0x02: Simulation
Step](../TraCI/Control-related_commands.md#command_0x02_simulation_step)
command. In order to have a predefined execution order every client
should issue a [TraCI/Control-related commands\#Command 0x03:
SetOrder](../TraCI/Control-related_commands.md#command_0x03_setorder)
command before the first simulation step. It assigns a number to the
client and commands from different clients during the same simulation
step will be executed in the order of that numbering (which does not
need to be consecutive nor positive but unique). When using multi client
setups the number of clients needs to be known when starting SUMO and
all clients need to connect before the first simulation step.

!!! caution
    Up to SUMO 0.30.0 only a single client could connect to a running sumo server.

**Figure:TraCI: establishing a connection to SUMO**

![Image:traciConnect_sequence.png](../images/TraciConnect_sequence.png
"Image:traciConnect_sequence.png")

The client application sends commands to [sumo](../sumo.md) to
control the simulation run, to influence single vehicle's behavior or to
ask for environmental details. [sumo](../sumo.md) answers with a
<em>Status</em>-response to each command and additional results that
depend on the given command.

The client has to trigger each simulation step in
[sumo](../sumo.md) using the [TraCI/Control-related
commands\#Command 0x02: Simulation
Step](../TraCI/Control-related_commands.md#command_0x02_simulation_step)
command. If any
[subscriptions](../TraCI/Object_Variable_Subscription.md) have been
done, the subscribed values are returned. The simulation will advance to
the next once all clients have sent the simulation step command.
Currently all clients receive all subscription results (even if the
subscription has been issued by a different client).

The client is responsible for shutting down the connection using the
[close](../TraCI/Control-related_commands.md#command_0x7f_close)
command. See [Simulation/Basic Definition\#Defining the Time Period to
Simulate](../Simulation/Basic_Definition.md#defining_the_time_period_to_simulate)
for further information on differences between running the simulation in
TraCI-mode. When all clients issued close, the simulation will end,
freeing all resources.

**Figure:TraCI: closing a connection to SUMO**

![Image:traciColose_sequence.png](../images/TraciColose_sequence.png
"Image:traciColose_sequence.png")

## Messages

A TCP message acts as container for a list of commands or results.
Therefore, each TCP message consists of a small header that gives the
overall message size and a set of commands that are put behind it. The
length and identifier of each command is placed in front of the command.
A scheme of this container is depicted below:

```
 0                 7 8               15
+--------------------------------------+
| Message Length including this header |
+--------------------------------------+
|      (Message Length, continued)     |
+--------------------------------------+  \
|     Length        |    Identifier    |  |
+--------------------------------------+   > Command_0
|           Command_0 content          |  |
+--------------------------------------+  /
                   ...
+--------------------------------------+  \
|     Length        |    Identifier    |  |
+--------------------------------------+   > Command_n-1
|          Command_n-1 content         |  |
+--------------------------------------+  /
```

In some cases the length of a single command may not suffice, since the
maximum command length is limited to 255. In those cases an extended
length can be used by setting the original ubyte length field to zero
and adding an integer length field. This extended scheme for a command
looks as follows:

```
 0                 7 8               15
+--------------------------------------+  \
|   Length = 0      |                  |  |
+-------------------+                  |  |
|        32 bit Integer Length         |  |
|                   +------------------+   > Command
|                   |   Identifier     |  |
+--------------------------------------+  |
|            Command content           |  |
+--------------------------------------+  /
```

To simplify the usage of TraCI, we provide a class for handling the
socket connection and another for composing a message out of basic data
types. The C++ implementation of these classes is available at [{{SUMO}}/src/foreign/tcpip]({{Source}}src/foreign/tcpip), the
Python implementation is in [{{SUMO}}/tools/traci/connection.py]({{Source}}tools/traci/connection.py) and [{{SUMO}}/tools/traci/storage.py]({{Source}}tools/traci/storage.py). For a Java implementation have a look
at [{{SUMO}}/tools/contributed/traas]({{Source}}tools/contributed/traas). The class `tcpip::Socket` handles a TCP connection on server as
well as on client side. The class `tcpip::Storage` is designed to hold a
list of elementary data types. By using the method
`tcpip::Socket::sendExact(tcpip::Storage)` and
`tcpip::Socket::receiveExact(Storage)`, messages can be send and
received as Storage objects.

### Status Response

| ubyte  |   string    |
| :----: | :---------: |
| Result | Description |

Each command is acknowledged by a status response, included are a
*Result* and a *Description*. The identifier refers to the identifier of
the respective command that is acknowledged.
*Result* can have the following values:

- 0x00 in case of success
- 0xFF if the requested command failed
- 0x01 if the requested command is not implemented in the network
  simulator (in addition, a *Description* text must be added)

## Data types

### Atomar Types

The following table shows the supported elementary data types:

| Data type  | Size  | Description  | Identifier  |
|------------|-------|--------------|-------------|
| ubyte  | 8 bit  | integer numbers (0 to 255)  | 0x07  |
| byte  | 8 bit  | integer numbers (-128 to 127)  | 0x08  |
| integer  | 32 bit  | integer numbers (-2<sup>31</sup> to 2<sup>31</sup>-1)<br>includes bitsets with bit0 meaning the least significant bit  | 0x09  |
| double  | 64 bit  | IEEE754 floating point numbers  | 0x0B  |
| string  | variable  | 32 bit string length, followed by text coded as 8 bit ASCII  | 0x0C  |
| stringList  | variable  | 32 bit string count n, followed by n strings  | 0x0E  |
| compound object  | variable  | Compound object, internal structure depends on concrete object. The compound object identifier is always followed by an 32bit-int denoting the number of component types. Then the components are given in sequence.  | 0x0F  |

### Composed Types

In the following, composed data types are described.

#### Position Representations

**2DPosition (*ubyte* identifier: 0x01)**

A cartesian 2D position within the simulation network, described by two
double values (x and y coordinate).

| ubyte (identifier bits) | double | double |
|:-----------------------:|:------:|:------:|
| 0 0 0 0 0 0 0 1 | X | Y |

**3DPosition (*ubyte* identifier: 0x03)** A cartesian 3D position within
the simulation network, described by three double values (x, y and z
coordinates).

| ubyte (identifier bits) | double | double | double |
|:-----------------------:|:------:|:------:|:------:|
| 0 0 0 0 0 0 1 1 | X | Y | Z |

**Road Map Position (*ubyte* identifier: 0x04)** Alternative position
description that is also used by [sumo](../sumo.md) in most cases.
*RoadId* identifies a road segment (edge), *Pos* describes the position
of the node in longitudinal direction (ranging from 0 to the road's
length). *LaneId* identifies the driving lane on the road segment. Lanes
are numbered sequentially from right to left starting with 0.

| ubyte (identifier bits) | string | double | ubyte |
|:-----------------------:|:------:|:------:|:-----:|
| 0 0 0 0 0 1 0 0 | RoadId | Pos | LaneId |

**Lon-Lat-Position (*ubyte* identifier: 0x00)** A position within the
simulation network in geo-coordinates, described by two double values
(longitude and latitude).

| ubyte (identifier bits) | double | double |
|:-----------------------:|:------:|:------:|
| 0 0 0 0 0 0 0 0 | Longitude | Latitude |

**Lon-Lat-Alt-Position (*ubyte* identifier: 0x02)** A position within
the simulation network in geo-coordinates with altitude, described by
three double values (longitude, latitude, and altitude).

| ubyte (identifier bits) | double | double | double |
|:-----------------------:|:------:|:------:|:------:|
| 0 0 0 0 0 0 1 0 | Longitude | Latitude | Altitude |

#### Polygon (*ubyte* identifier: 0x06)

A sequence of 2D points, representing a polygon shape. *Length* is the
number of (x,y) points that make up the polygon.

| ubyte (identifier bits) | ubyte | double | double | ... |
|:-----------------------:|:-----:|:------:|:------:|:---:|
| 0 0 0 0 0 1 1 0 | Length | X | Y | ... |

#### Traffic Light Phase List (*ubyte* identifier: 0x0D)

This type is used to report the different phases of a traffic light. A
total of *Length* phases is reported together with the preceding and
succeeding roads that are affected by the respective light phase.

The following identifiers for a phase exist:

- 0x01: red
- 0x02: yellow
- 0x03: green
- 0x04: traffic light is off and blinking
- 0x05: traffic light is off, not blinking

| ubyte (identifier bits) | ubyte | string | string | ubyte | ... |
|:-----------------------:|:-----:|:------:|:------:|:-----:|:---:|
| 0 0 0 0 1 1 0 1 | Length | PrecRoad | SuccRoad | Phase | ... |

#### Color (*ubyte* identifier: 0x11)

A R,G,B,A-quadruple of unsigned byte (0-255).

| ubyte (identifier bits) | ubyte | ubyte | ubyte | ubyte |
|:-----------------------:|:-----:|:-----:|:-----:|:-----:|
| 0 0 0 1 0 0 0 1 | R | G | B | A |
