---
title: Purgatory/Commands
permalink: /Purgatory/Commands/
---

This page details the [TraCI](../TraCI.md) commands. Please see
there for a general introduction.

## Control-related commands

### Command 0x01: Simulation Step (double **TargetTime**, ubyte **PositionType**)

The network simulator sends this command repetitively, triggering the
traffic simulator to simulate the next simulation step up to target time
(which is the actual simulation time of the network simulator plus one
simulation step). Furthermore, the simulation times are kept
synchronized.
A *status* response and (optionally) a collection of *Move Node*
commands (see below) is returned. All responses are sent as one TraCI
message.

#### Response 0x80: Move Node (integer **NodeId**, double **TargetTime**, position **Position**)

When node movement information has been requested in the *Simulation
Step* command (by setting PositionType to a value other 0), performing a
simulation step will trigger a series of 'Move Node'' responses. For
each moving vehicle, the network simulator receives a *Move Node*
response as replie to the *Simulation Step* command. The network
simulator must convert these movement information into linear movements
to make sure that each node reaches its *Position* at *TargetTime*. The
*Position* representation is interpreted according to the given
*PositionType* in the *Simulation Step* command.

### Command 0x7F: Close ()

Tells TraCI to close the connection to any client, stop simulation and
shut down sumo.

## Mobility-related commands

### Command 0x11: Set Maximum Speed (integer **NodeId**, float **MaxSpeed**)

The vehicle identified by *NodeId* limits its speed to an individual
maximum of *MaxSpeed*.
When the command is called with a negative value for *MaxSpeed*, the
individual speed of a vehicle is reset to its original value.

### Command 0x12: Stop Node (integer **NodeId**, position **StopPosition**, float **Radius**, double **WaitTime**)

Use this command to stop a vehicle at a predetermined position. Whenever
the vehicle given by *NodeId* reaches *StopPosition*, it sets its
maximum speed to 0 and waits for the amount of time specified by
*WaitTime*. After that, it continues on its way.

Though *StopPosition* can be given in any position format, the *Road
Map* format should be preferred, as it is used internally by sumo. For
any other format, sumo will compute the nearest road map position and
stop the vehicle there.
In any case, a *Stop Node* command containing the final road map
position used by sumo to stop the vehicle is added to the response
command.

### Command 0x14: Slow Down (integer **NodeId**, float **MinSpeed**, double **TimeIntervall**)

Tells the node identified by *NodeId* to reduce its driving speed down
to *MinSpeed* within the given *TimeIntervall*. The speed reduction will
be linear. This command is provided to simulate different methods of
slowing down, like instant breaking or coasting.

### Command 0x13: Change Lane (integer **NodeId**, byte **Lane**, float **Time**)

The vehicle identified by *NodeId* tries to move to another *Lane* for
the duration of *Time*. (Lanes are numbered sequentially starting with 0
at the right most lane)
After *Time* has expired, the constraint is removed. It's not guaranteed
that the vehicle will actually change the lane, e.g. the defined route
my require it to hold the lane or change to another one. If the vehicle
passes a junction while on a fixed lane, it gets fixed to the
corresponding lane on the succeeding road segment. If such a lane
doesn't exist, the constraint is removed and all lanes may be used again
by the vehicle.

### Command 0x30: Change Route (integer **NodeId**, string **RoadId**, double **TravelTime**)

Usually, each road within a simulation has an assigned travel time based
on its length and allowed speed. To allow for dynamic rerouting, this
command assigns an individual *TravelTime* to a specific road segment
identified by *RoadId*.
The set travel time is only visible to the appointed vehicle (*NodeId*).
Setting a negative travel time will restore a typical travel time as
mentioned above for the road segment.
Before the simulation continues, a new route is calculated for the
vehicle.

**Note**: this command conflicts with routes that contain one or more
edges of the road network multiple times. When rerouting a vehicle, SUMO
determines a direct path between the vehicle's current edge and the
target edge (end of route), thus the original route may be truncated\!

### Command 0x31: Change Target (integer **NodeId**, string **RoadId**)

The destination of the vehicle identified by *NodeId* is changed to a
new road segment (*RoadId*). The simulation continues after a route to
the new target has been calculated.

## Subscription-related commands

### Command 0x61: Subscribe Lifecycles (ubyte **Domain**)

Subsequent creation/destruction of an object in *Domain* will trigger an
**Object Creation**/**Object Destruction** response, respectively.

#### Response 0x63: Object Creation (ubyte **Domain**, integer **DomainId**)

Sent after an object *DomainId* was created in a *Domain* the client
previously subscribed to using a **Subscribe Lifecycles** command.

#### Response 0x64: Object Destruction (ubyte **Domain**, integer **DomainId**)

Sent after an object *DomainId* was destroyed in a *Domain* the client
previously subscribed to using a **Subscribe Lifecycles** command.

### Command 0x62: Unsubscribe Lifecycles (ubyte **Domain**)

Cancels a lifecycle subscription previously initiated with a **Subscribe
Lifecycles** command.

### Command 0x65: Subscribe Domain (ubyte **Domain**, ubyte **VariableCount**, ubyte **Variable1**, ubyte **ValueType1**, ...)

Subsequent update of an object in *Domain* will trigger an **Update
Object** response.

#### Response 0x67: Update Object (ubyte **Domain**, integer **DomainId**, ValueType1 **Value1**, ...)

Sent after an object *DomainId* was updated in a *Domain* the client
previously subscribed to using a **Subscribe Domain** command. The
response will contain all variables specified with the last
subscription, using the exact order and type given.

### Command 0x66: Unsubscribe Domain (ubyte **Domain**)

Cancels a domain subscription previously initiated with a **Subscribe
Domain** command.

## Environment-related commands

### Command 0x71: Position Conversion (Position **Position**, ubyte **PositionId**)

Use this command to convert between the different position types used by
TraCI. The specified *Position* is converted into the requested format
given through *PositionId*. The result is the closest position that was
found to match the given one.

The converted value is returned within a PositionConversion command that
is added to the response message.

### Command 0x72: Distance Request (Position **Position1**, Position **Position2**, ubyte **Flag**)

To obtain information about distances on the road network, the
*DistanceRequest* command is available. The distance between *Position1*
and *Position2* is determined, whereas the value of *Flag* has the
following meaning:

- 0x00: the air distance between the two positions is computed
- 0x01: the driving distance is computed

The result is returned within a *DistanceRequest* response (s. below).

#### Response 0x72: Distance Request (ubyte **Flag**, float **Distance**)

Each *DistanceRequest* command is, in addition to a successful *Status*
command, answered by this response. It holds the requested *Distance*
and a *Flag* to indicate whether the result is air or driving distance.

### Command 0x73: Scenario (ubyte **Flag**, ubyte **Domain**, integer **DomainId**, ubyte **Variable**, ubyte **ValueType**, ValueType **Value**)

*Scenario* is used to obtain all kinds of parameters about the
simulation and may be used as a read or write command, indicated by
*Flag*:

- 0x00: *get*: the *Variable* is read and will not be altered, the data field *Value* is not used (except if stated otherwise)
- 0x01: *set*: the *Variable* is overwritten by the given *Value*

A Scenario is divided into domains (*Domain*) which represent different
aspects of the simulation world; *DomainId* addresses a certain object
within such a domain. The id is a numerical value that may reach from 0
to the total number of domain objects -1. The different parameters that
can be get or set are identified by *Variable* and may be domain
specific. *ValueType* indicates the data type that the addressed
parameter consists of, thus *Value* (if any) must have the same type.

Read-messages are responded by a *Scenario* command that contains the
read *Value* and its type. The requested *Variable* will be returned in
the specified data type if possible (e.g. position formats), otherwise
the correct type is chosen by SUMO.

<center>
**Overview of Domains and Variables**
</center>

<table style="border-collapse:collapse;border-spacing:0" class="tg"><tr><th style="font-family:Arial, sans-serif;font-size:14px;font-weight:normal;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:left;vertical-align:middle"></th><th style="font-family:Arial, sans-serif;font-size:14px;font-weight:bold;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:middle" colspan="5">Domain:</th></tr><tr><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:left;vertical-align:middle"></td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;font-weight:bold;text-align:center;vertical-align:middle">road map (id 0x00)</td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;font-weight:bold;text-align:center;vertical-align:top">vehicle (id 0x01)</td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;font-weight:bold;text-align:center;vertical-align:top">traffic light (id 0x02)</td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;font-weight:bold;text-align:center;vertical-align:top">point of interest (id 0x03)</td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;font-weight:bold;text-align:center;vertical-align:top">polygon (id 0x04)</td></tr><tr><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;font-weight:bold;text-align:center;vertical-align:middle" rowspan="20">read<br>only<br>variables</td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:middle">shape (id 0x09)</td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:top">count max (id 0x0A)</td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:top">actual phase (id 0x05)</td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:top"></td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:top">shape (id 0x09)</td></tr><tr><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:middle"></td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:top">equipped count (id 0x0B)</td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:top">next phase change (id 0x06)</td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:top"></td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:top"></td></tr><tr><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:middle"></td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:top">equipped count max (id 0x0C)</td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:top"></td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:top"></td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:top"></td></tr><tr><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:middle"></td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:top">speed (id 0x04)</td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:top"></td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:top"></td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:top"></td></tr><tr><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:middle"></td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:top">air distance (id 0x10)</td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:top"></td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:top"></td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:top"></td></tr><tr><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:middle"></td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:top">driving distance (id 0x11)</td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:top"></td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:top"></td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:top"></td></tr><tr><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:middle"></td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:top">route (id 0x0E)</td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:top"></td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:top"></td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:top"></td></tr><tr><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:middle"></td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:top">allowed speed (id 0x0F)</td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:top"></td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:top" colspan="2">type (id 0x07)</td></tr><tr><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:middle"></td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:top">angle (id 0x13)</td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:top"></td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:top"></td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:top"></td></tr><tr><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:middle"></td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:top">CO2 emissions (id 0x20)</td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:top"></td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:top"></td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:top"></td></tr><tr><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:middle"></td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:top">CO emissions (id 0x21)</td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:top"></td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:top"></td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:top"></td></tr><tr><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:middle"></td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:top">HC emissions (id 0x22)</td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:top"></td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:top"></td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:top"></td></tr><tr><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:middle"></td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:top">PMx emissions (id 0x23)</td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:top"></td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:top"></td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:top"></td></tr><tr><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:middle"></td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:top">NOx emissions (id 0x24)</td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:top"></td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:top"></td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:top"></td></tr><tr><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:middle"></td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:top">fuel consumption (id 0x25)</td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:top"></td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:top"></td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:top"></td></tr><tr><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:middle"></td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:top">noise emission (id 0x26)</td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:top"></td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:top"></td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:top"></td></tr><tr><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:middle">net boundaries(id 0x03)</td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:top" colspan="4">position (id 0x02)</td></tr><tr><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:middle" colspan="5">count(id 0x01)</td></tr><tr><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:middle" colspan="5">name(id 0x0D)</td></tr><tr><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:middle" colspan="5">external ID (id 0x12)</td></tr><tr><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:center;vertical-align:middle" colspan="5">simulation time (id 0x14)</td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align:left;vertical-align:top"></td></tr></table>

**Domain: road map (id 0x00)**

- **name** (id 0x0D): Returns the internal sumo id string of the road
  that is referenced by the given object id. Data type: *string*.
- **external ID** (id 0x12): Returns the numerical object id used by
  TraCI for the domain object identified by a given sumo id string.
  The *Value* parameter is used to pass the id string. Data type:
  *integer*.
- **net boundaries** (id 0x03): The boundary box of the simulation
  network. Data type: *Boundary Box*.
- **count** (id 0x01): The overall number of roads (=MSEdges) in the
  roadmap. Data type: *integer*.
- **shape** (id 0x09): The course of a road, defined at least by its
  start- and end-point. Data type: *Polygon*.

**Domain: vehicle (id 0x01)**

- **name** (id 0x0D): Returns the sumo internal id string of the
  vehicle that is referenced by the given object id. Data type:
  *string*.
- **external ID** (id 0x12): Returns the numerical object id used by
  TraCI for the domain object identified by a given sumo id string.
  The *Value* parameter is used to pass the id string. Data type:
  *integer*.
- **count** (id 0x01): Total number of active vehicles within the
  simulation at the current simulation step. Data type: *integer*.
- **count max** (id 0x0A): Upper bound for the total number of
  vehicles in the simulation. Data type: *integer*.
- **equipped count** (id 0x0B): Total number of vehicles that can be
  accessed via TraCI at the current simulation step. Data type:
  *integer*.
- **equipped count max** (id 0x0C): Upper bound for the total number
  of vehicles that can be accessed via TraCI in the simulation. Data
  type: *integer*.
- **position** (id 0x02): The position of a certain vehicle within the
  simulation network. Data type: *Position*.
- **speed** (id 0x04): Speed of a certain vehicle at the current
  simulation time. Data type: *float*.
- **route** (id 0x0E): The desired route of a vehicle. May change
  during simulation runtime, e.g., due to the ChangeRoute command.
  Data type: *string*.
- **allowed speed** (id 0x0F): Maximum allowed speed of a certain
  vehicle on the current lane. Data type: *float*.
- **air distance** (id 0x10): Air distance from a vehicles position to
  another position that is passed by the *Value* parameter. Data type:
  *float*.
- **driving distance** (id 0x11): Driving distance from a vehicles
  position to another position that is passed by the *Value*
  parameter. Data type: *float*.
- **CO2 emissions** (id 0x20): Vehicle's CO2 emissions during this
  time step. Data type: *float*.
- **CO emissions** (id 0x21): Vehicle's CO emissions during this time
  step. Data type: *float*.
- **HC emissions** (id 0x22): Vehicle's HC emissions during this time
  step. Data type: *float*.
- **PMx emissions** (id 0x23): Vehicle's PMx emissions during this
  time step. Data type: *float*.
- **NOx emissions** (id 0x24): Vehicle's NOx emissions during this
  time step. Data type: *float*.
- **fuel consumption** (id 0x25): Vehicle's fuel consumption during
  this time step. Data type: *float*.
- **noise emission** (id 0x26): Noise generated by the vehicle. Data
  type: *float*.

**Domain: traffic light (id 0x02)**

- **name** (id 0x0D): Returns the sumo internal id string of the
  traffic light that is referenced by the given object id. Data type:
  *string*.
- **external ID** (id 0x12): Returns the numerical object id used by
  TraCI for the domain object identified by a given sumo id string.
  The *Value* parameter is used to pass the id string. Data type:
  *integer*.
- **count** (id 0x01): Overall number of traffic lights within the
  simulation. Data type: *integer*.
- **position** (id 0x02): The position of a certain traffic light
  within the network. Data type: *3DPosition*.
- **actual phase** (id 0x05): For a junction that is controlled by
  traffic lights, the current phase of each individual light at the
  current simulation time is reported. Data type: *Traffic Light Phase
  List*.
- **next phase change** (id 0x06): For a junction that is controlled
  by traffic lights, the next phase that each individual light will
  switch to is reported. Data type: *Traffic Light Phase List*.

**Domain: point of interest (id 0x03)**

- **name** (id 0x0D): Returns the sumo internal id string of the point
  of interest that is referenced by the given object id. Data type:
  *string*.
- **external ID** (id 0x12): Returns the numerical object id used by
  TraCI for the domain object identified by a given sumo id string.
  The *Value* parameter is used to pass the id string. Data type:
  *integer*.
- **count** (id 0x01): Overall number of points of interest (poi)
  within the simulation. Data type: *integer*.
- **position** (id 0x02): The position of a poi within the network.
  Data type: *3DPosition*.
- **type** (id 0x07): The type assigned to a certain poi. Data type:
  *String*.

**Domain: polygon (id 0x04)**

- **name** (id 0x0D): returns the sumo internal id string of the
  polygon that is referenced by the given object id. Data type:
  *string*.
- **external ID** (id 0x12): Returns the numerical object id used by
  TraCI for the domain object identified by a given sumo id string.
  The *Value* parameter is used to pass the id string. Data type:
  *integer*.
- **count** (id 0x01): overall number of polygons within the
  simulation. Data type: *integer*.
- **position** (id 0x02): the position of a polygon within the
  network. Data type: *3DPosition*.
- **type** (id 0x07): the type assigned to a certain polygon. Data
  type: *String*.
- **shape** (id 0x09): the actual shape (sequence of points) of a
  certain polygon. Data type: *Polygon*.

## Traffic Lights

### Command 0x41: Get Traffic Light Status (integer **TrafficLightId**, double **IntervalStart**, double **IntervalEnd**)

The traffic lights of the junction specified by *TrafficLight-Id* are
asked for their state transitions that will occur within a given time
interval defined by *IntervalStart* and *IntervallEnd*. All traffic
light switches within this interval are then reported as a series of
*Traffic Light Switch* responses (s. below).

#### Response 0x91: Traffic Light Switch (double **SwitchTime**, string **PrecedingEdge**, float **PositionOnPrecedingEdge**, string **SucceedingEdge**, ubyte **NewPhase**, double **YellowTime**)

This response is sent for each switch in a traffic lights phase that
occurs within the time interval given by the command *Get Traffic Light
Status*. The *NewPhase* that the traffic light will switch to is
reported along with the time of the switch (*SwitchTime*) and the
duration of the yellow phase that precedes the phase switch
(*YellowTime*).
A switch is reported for each pair of *PrecedingEdge* and
*SucceedingEdge* that is affected by it. Additionally, the traffic
light's *PositionOnPrecedingEdge* is given.

## Induction Loop Value Retrieval

### Command 0xa0: Get Induction Loop Variable (ubyte **Variable**, string **InductionLoopID**)

Asks for the value of a certain variable of the named [induction loop
(e1 detector)](../Simulation/Output/Induction_Loops_Detectors_(E1).md). The value returned is the
state of the asked variable/value within the last simulation step.
Please note that for asking values from your [induction
loops](../Simulation/Output/Induction_Loops_Detectors_(E1).md) with TraCI, you have to:

- define them
- set their frequency to 1s

The following variable values can be retrieved, the type of the return
value is also shown in the table.

<center>
**Overview Retrievable Induction Loop Variables**
</center>

| Variable                        | ValueType | Description                                                                                                      |
| ------------------------------- | --------- | ---------------------------------------------------------------------------------------------------------------- |
| id list (0x00)                  | string\*  | Returns a list of ids of all known induction loops within the scenario (the given Induction Loop ID is ignored)  |
| last step vehicle number (0x10) | integer   | Returns the number of vehicles that were on the named induction loop within the last simulation step \[\#\]      |
| last step mean speed (0x11)     | float     | Returns the mean speed of vehicles that were on the named induction loop within the last simulation step \[m/s\] |
| last step vehicle ids (0x12)    | string\*  | Returns the list of ids of vehicles that were on the named induction loop in the last simulation step            |

#### Response 0xb0: Induction Loop Variable (ubyte **Variable**, string **InductionLoopID**, ubyte **ValueType**, ValueType **Value**)

The respond to a **"Command Get Induction Loop Variable"**. You can find
[some further description on induction loops](../Simulation/Output/Induction_Loops_Detectors_(E1).md)

## Multi-Entry/Multi-Exit Detectors Value Retrieval

### Command 0xa1: Get MeMeDetector Variable (ubyte **Variable**, string **DetectorID**)

Asks for the value of a certain variable of the named
[multi-entry/multi-exit (e3) detector](../Simulation/Output/Multi-Entry-Exit_Detectors_(E3).md). The
value returned is the state of the asked variable/value within the last
simulation step. Please note that for asking values from your
[multi-entry/multi-exit detectors](../Simulation/Output/Multi-Entry-Exit_Detectors_(E3).md) with
TraCI, you have to:

- define them
- set their frequency to 1s

The following variable values can be retrieved, the type of the return
value is also shown in the table.

<center>
**Overview Retrievable Multi-Entry/Multi-Exit Detector Variables**
</center>

| Variable                        | ValueType | Description                                                                                                                                |
| ------------------------------- | --------- | ------------------------------------------------------------------------------------------------------------------------------------------ |
| id list (0x00)                  | string\*  | Returns a list of ids of all known multi-entry/multi-exit detectors within the scenario (the given DetectorID is ignored)                  |
| last step vehicle number (0x10) | integer   | Returns the number of vehicles that have been within the named multi-entry/multi-exit detector within the last simulation step \[\#\]      |
| last step mean speed (0x11)     | float     | Returns the mean speed of vehicles that have been within the named multi-entry/multi-exit detector within the last simulation step \[m/s\] |
| last step vehicle ids (0x12)    | string\*  | Returns the list of ids of vehicles that have been within the named multi-entry/multi-exit detector in the last simulation step            |

#### Response 0xb1: MeMeDetector Variable (ubyte **Variable**, string **DetectorID**, ubyte **ValueType**, ValueType **Value**)

The respond to a **"Command Get MeMeDetector Variable"**. You can find
[some further description on multi-entry/multi-exit
detectors](../Simulation/Output/Multi-Entry-Exit_Detectors_(E3).md)

## Traffic Lights Value Retrieval

### Command 0xa2: Get Traffic Lights Variable (ubyte **Variable**, string **TrafficLightsID**)

Asks for the value of a certain variable of the named traffic light. The
value returned is the state of the asked variable/value within the last
simulation step. The following variable values can be retrieved, the
type of the return value is also shown in the table.

| Variable                            | ValueType       | Description                                                                                                                                                              |
| ----------------------------------- | --------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| id list (0x00)                      | string\*        | Returns a list of ids of all known traffic lights within the scenario (the given Traffic Lights ID is ignored)                                                           |
| state (light/priority tuple) (0x20) | string          | Returns the named tl's state as a tuple of light definitions from rRgGyYoO, for red, green, yellow, off, where lower case letters mean that the stream has to decelerate |
| state (phase/brake/yellow) (0x21)   | string\*3       | Returns three strings that describe the tl's current state, the first green y/n, the second for brake y/n, the third for yellow y/n<br><br>**Note:** Currently, the in the light/priority the controlled links are given in a reversed order. The order is a point that must be discussed.                                      |
| complete definition (0x25)          | compound object | Returns the complete traffic light program, structure described under data types<br><br>**Note:** Currently, the in the light/priority the controlled links are given in a reversed order. The order is a point that must be discussed.                                                                                         |
| controlled lanes (0x26)             | string list     | Returns the list of lanes which are controlled by the named traffic light                                                                                                |
| controlled links (0x27)             | compound object | Returns the links controlled by the traffic light, sorted by the signal index and described by giving the incoming, outgoing, and via lane                               |
| current phase (0x28)                | int             | Returns the index of the current phase in the current program                                                                                                            |
| current program (0x29)              | string          | Returns the id of the current program                                                                                                                                    |

**Overview Retrievable Traffic Lights Variables**

#### Response 0xb2: Traffic Lights Variable (ubyte **Variable**, string **TrafficLightID**, ubyte **ValueType**, ValueType **Value**)

The respond to a **"Command Get Traffic Lights Variable"**.

### Structure of compound object traffic light program:

If you request a complete traffic light program, the compound object is
structured as following. Attention, each part is fowarded by a byte
which represents its data type, except "length".

| integer |  type + integer  |  logic  | ... |  logic  |
| :-----: | :--------------: | :-----: | :-: | :-----: |
| Length  | Number of logics | logic 1 | ... | logic n |

**Logic:**

| type + string | type + integer | type + compound |   type + integer    |  type + integer  |  phase  | ... |  phase  |
| :-----------: | :------------: | :-------------: | :-----------------: | :--------------: | :-----: | :-: | :-----: |
|     SubID     |      Type      |  SubParameter   | Current phase index | Number of phases | Phase 1 | ... | Phase n |

Type and SubParameter aren't currently implemented therefore they are 0.

**Phase:**

| type + integer | type + integer | type + integer | type + stringList |
| :------------: | :------------: | :------------: | :---------------: |
|    Duration    |   Duration 1   |   Duration 2   | Phase definition  |

Duration 1 and 2 are currently not implemented.The phase definition
contains three strings that describe the tl's state, the first green
y/n, the second for brake y/n, the third for yellow y/n

### Structure of compound object controlled links:

If you request the list of links, the compound object is structured as
following. Attention, each part is fowarded by a byte which represents
its data type, except "length".

|          integer           |       controlled links       | ... |        controlled links        |
| :------------------------: | :--------------------------: | :-: | :----------------------------: |
| Length (number of signals) | links controlled by signal 0 | ... | links controlled by signal n-1 |

**Controlled links:**

|            int             | stringlist | ... | stringlist |
| :------------------------: | :--------: | :-: | :--------: |
| number of controlled links |   link 0   | ... |  link n-1  |

Each link is described by a string list with three entries: the lane
which is incoming into the junction, the lane which is outgoing from the
junction, and the lane across the junction (junction-internal). Empty
strings indicate missing lanes.

## Command 0xc2: Change Traffic Lights State (ubyte **Variable**, string **TrafficLightsID**, ubyte **ValueType**, ValueType **Value**)

Changes the state of a traffic light. Because it is possible to change
different values of a traffic light, the given value may have different
types. The following values can be changed, the type of the value to
given is also shown in the table.

<center>
**Overview Changeable Traffic Lights Variables**
</center>

| Variable                          | ValueType | Description                                                                       |
| --------------------------------- | --------- | ------------------------------------------------------------------------------------------------------------------------------------- |
| phase index (0x22)                | integer   | Sets the phase of the traffic light to the given. The given index must be valid for the current program of the traffic light, this means it must be between 0 and the number of phases known to the current program of the tls - 1.                                                                                                                                                   |
| state (phase/brake/yellow) (0x21) | string\*3 | Sets the phase definition to the given. The traffic light switches to "online" mode, this means that the phase can only be changed via traci, no automatic phase change takes place (in fact, the traffic light gets only one phase which takes 1 second). In order to let the traffic light work normally, one has to change the program back to a valid one. The index is set to 0. |
| program (0x23)                    | string    | Switches the traffic light to the given program. No WAUT algorithm is used, the program is directly instantiated. The index of the traffic light stays the same as before.                                                                                                                                                                                                            |
| phase duration (0x24)             | integer   | Sets the remaining duration of the current phase in seconds.                                                                                                     |