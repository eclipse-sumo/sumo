---
title: TraCI/Protocol
permalink: /TraCI/Protocol/
---

This page details the [TraCI](../TraCI.md) protocol. Please see
there for a general introduction.

## Basic Flow

After starting [SUMO](../SUMO.md), clients connect to
[SUMO](../SUMO.md) by setting up a TCP connection to the appointed
[SUMO](../SUMO.md) port. TraCI supports multiple clients and
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

The client application sends commands to [SUMO](../SUMO.md) to
control the simulation run, to influence single vehicle's behavior or to
ask for environmental details. [SUMO](../SUMO.md) answers with a
<em>Status</em>-response to each command and additional results that
depend on the given command.

The client has to trigger each simulation step in
[SUMO](../SUMO.md) using the [TraCI/Control-related
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
 0                 7 8               15
+--------------------------------------+
| Message Length including this header |
+--------------------------------------+
|      (Message Length, continued)     |
+--------------------------------------+  \
|     Length        |    Identifier    |  |
+--------------------------------------+   > Command_0
|           Command_0 content          |  |
+--------------------------------------+  /
                   ...
+--------------------------------------+  \
|     Length        |    Identifier    |  |
+--------------------------------------+   > Command_n-1
|          Command_n-1 content         |  |
+--------------------------------------+  /
```

In some cases the length of a single command may not suffice, since the
maximum command length is limited to 255. In those cases an extended
length can be used by setting the original ubyte length field to zero
and adding an integer length field. This extended scheme for a command
looks as follows:

```
 0                 7 8               15
+--------------------------------------+  \
|   Length = 0      |                  |  |
+-------------------+                  |  |
|        32 bit Integer Length         |  |
|                   +------------------+   > Command
|                   |   Identifier     |  |
+--------------------------------------+  |
|            Command content           |  |
+--------------------------------------+  /
```

To simplify the usage of TraCI, we provide a class for handling the
socket connection and another for composing a message out of basic data
types. The C++ implementation of these classes is available at , the
Python implementation is in  and . For a Java implementation have a look
at . The class `tcpip::Socket` handles a TCP connection on server as
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

- 0x00 in case of succes
- 0xFF if the requested command failed
- 0x01 if the requested command is not implemented in the network
  simulator (in addition, a *Description* text must be added)

## Data types

### Atomar Types

The following table shows the supported elementary data types:

<table>
<thead>
<tr class="header">
<th><p>Data type</p></th>
<th><p>Size</p></th>
<th><p>Description</p></th>
<th><p>Identifier</p></th>
</tr>
</thead>
<tbody>
<tr class="odd">
<td><p>ubyte</p></td>
<td><p>8 bit</p></td>
<td><p>integer numbers (0 to 255)</p></td>
<td><p>0x07</p></td>
</tr>
<tr class="even">
<td><p>byte</p></td>
<td><p>8 bit</p></td>
<td><p>integer numbers (-128 to 127)</p></td>
<td><p>0x08</p></td>
</tr>
<tr class="odd">
<td><p>integer</p></td>
<td><p>32 bit</p></td>
<td><p>integer numbers (-2<sup>31</sup> to 2<sup>31</sup>-1)<br />
includes bitsets with bit0 meaning the least significant bit</p></td>
<td><p>0x09</p></td>
</tr>
<tr class="even">
<td><p>double</p></td>
<td><p>64 bit</p></td>
<td><p>IEEE754 floating point numbers</p></td>
<td><p>0x0B |-|</p></td>
</tr>
</tbody>
</table>

### Composed Types

In the following, composed data types are described.

#### Position Representations

**2DPosition (*ubyte* identifier: 0x01)**

A cartesian 2D position within the simulation network, described by two
double values (x and y coordinate).

| ubyte | double | double |
| :---: | :----: | :----: |
|   0   |   0    |   0    |

**3DPosition (*ubyte* identifier: 0x03)** A cartesian 3D position within
the simulation network, described by three double values (x, y and z
coordinates).

| ubyte | double | double | double |
| :---: | :----: | :----: | :----: |
|   0   |   0    |   0    |   0    |

**Road Map Position (*ubyte* identifier: 0x04)** Alternative position
description that is also used by [SUMO](../SUMO.md) in most cases.
*RoadId* identifies a road segment (edge), *Pos* describes the position
of the node in longitudinal direction (ranging from 0 to the road's
length). *LaneId* identifies the driving lane on the road segment. Lanes
are numbered sequentially from right to left starting with 0.

| ubyte | string | double | ubyte |
| :---: | :----: | :----: | :---: |
|   0   |   0    |   0    |   0   |

**Lon-Lat-Position (*ubyte* identifier: 0x00)** A position within the
simulation network in geo-coordinates, described by two double values
(longitude and latitude).

| ubyte | double | double |
| :---: | :----: | :----: |
|   0   |   0    |   0    |

**Lon-Lat-Alt-Position (*ubyte* identifier: 0x02)** A position within
the simulation network in geo-coordinates with altitude, described by
three double values (longitude, latitude, and altitude).

| ubyte | double | double | double |
| :---: | :----: | :----: | :----: |
|   0   |   0    |   0    |   0    |

#### Polygon (*ubyte* identifier: 0x06)

A sequence of 2D points, representing a polygon shape. *Length* is the
number of (x,y) points that make up the polygon.

| ubyte | ubyte | double | double |   |
| :---: | :---: | :----: | :----: | :-: |
|   0   |   0   |   0    |   0    | 0 |

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

| ubyte | ubyte | string | string | ubyte |   |
| :---: | :---: | :----: | :----: | :---: | :-: |
|   0   |   0   |   0    |   0    |   1   | 1 |

#### Color (*ubyte* identifier: 0x11)

A R,G,B,A-quadruple of unsigned byte (0-255).

| ubyte | ubyte | ubyte | ubyte | ubyte |
| :---: | :---: | :---: | :---: | :---: |
|   0   |   0   |   0   |   1   |   0   |