---
title: Purgatory/Mobility-related commands
permalink: /Purgatory/Mobility-related_commands/
---

# Mobility-related commands

## Command 0x11: Set Maximum Speed

| integer |  float   |
| :-----: | :------: |
| NodeId  | MaxSpeed |

The vehicle identified by *NodeId* limits its speed to an individual
maximum of *MaxSpeed*.

When the command is called with a negative value for *MaxSpeed*, the
individual speed of a vehicle is reset to its original value.

## Command 0x12: Stop Node

| integer |   position   | float  |     integer     |
| :-----: | :----------: | :----: | :-------------: |
| NodeId  | StopPosition | Radius | WaitTime \[ms\] |

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
command. Please note that *Radius* has an epsilon of 0.1m.

## Command 0x14: Slow Down

| integer |  float   |       integer        |
| :-----: | :------: | :------------------: |
| NodeId  | MinSpeed | TimeIntervall \[ms\] |

Tells the node identified by *NodeId* to reduce its driving speed down
to *MinSpeed* within the given *TimeIntervall*. The speed reduction will
be linear. This command is provided to simulate different methods of
slowing down, like instant breaking or coasting.

## Command 0x13: Change Lane

| integer | byte |   integer   |
| :-----: | :--: | :---------: |
| NodeId  | Lane | Time \[ms\] |

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

## Command 0x30: Change Route

| integer | string |   double   |
| :-----: | :----: | :--------: |
| NodeId  | RoadId | TravelTime |

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

## Command 0x31: Change Destination

| integer | string |
| :-----: | :----: |
| NodeId  | RoadId |

The destination of the vehicle identified by *NodeId* is changed to a
new road segment (*RoadId*). The simulation continues after a route to
the new target has been calculated.