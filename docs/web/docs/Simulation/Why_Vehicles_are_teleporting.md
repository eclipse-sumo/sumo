---
title: Why Vehicles are teleporting
---

When running a simulation, one may encounter the following warning:

```
Warning: Teleporting vehicle '...'; waited too long, lane='...', time=....
```

What does it mean?

# Reasons

The following circumstances may force the simulation to "teleport" a
vehicle:

- the vehicle stood too long in front of an intersection (message:
  "*...'; waited too long, lane='...*")
- the vehicle has collided with his leader (message: "*...';
  collision, lane='...*")

## Waiting too long, aka Grid-locks

In the case a vehicle is standing at the first position in front of an
intersection, SUMO counts the number of steps the vehicle's velocity
stays below 0.1m/s. These steps are the "waiting time". In the case the
vehicle moves with a larger speed, this counter is reset. In the case
the vehicle waited longer than a certain threshold value (default 300
seconds), the vehicle is assumed to be in grid-lock and teleported onto
the next free edge on its route. If there is enough space on the subsequent edge of
the vehicle's route, the vehicle will be directly assigned to the respective
available space. Vehicle insertion will use depart method "free" (anywhere on the lane with the least traffic)
and attempt to insert the vehicle with it's maximum allowed speed.
The insertion space must allow for all necessary safety gaps of the vehicle itself and it's follower vehicle, though the speed may be reduced if it helps with insertion.

If the vehicle cannot be inserted immediately,it will be held outside the road network in a special 'teleporting-buffer' and (virtually) traverse the next edge with the average speed of that edge (minimum  m/s). After this virtual travel time has passend, the next attempt at re-inserting the vehicle into the network is made one edge further along it's route. This procedure repeats until the vehicle has been re-inserted or the end of the route is reached.
In the latter case, the vehicle is removed from the simulation.
While the vehicle is in the teleporting buffer it remains invisble.

A simple illustration about how congestion
resolves stepwise is shown below.
![grafik](https://user-images.githubusercontent.com/26454969/121517586-d900b100-c9ef-11eb-943a-69e410814bcf.png)


The threshold value can be configure
using the option **--time-to-teleport** {{DT_INT}} which sets the time in seconds. If the value is not
positive, teleporting due to grid-lock is disabled. Note that for
vehicles which have a stop as part of their route, the time spent
stopping is not counted towards their waiting time.

There are different reasons why a vehicle cannot continue with its
route. Every time a vehicle teleports due to grid-lock one of the
following reasons is given:

- **wrong lane**: The vehicle is stuck on a lane which has no
  connection to the next edge on its route.
- **yield** The vehicle is stuck on a low-priority road and did not
  find a gap in the prioritized traffic
- **jam** The vehicle is stuck on a priority road and there is no
  space on the next edge.
  
Related options are

- **--time-to-teleport.highways** (teleport earlier when stuck on the wrong lane of a road with speed above 19.167 m/s
- **--time-to-teleport.disconnected** (teleport earlier when the route is disconnected)

Unfortunately, grid-locks are rather common in congested simulation
scenarios. You can solve this only by [improving traffic flow, either by
correcting junction priorities, traffic light timings or the traffic
demand (route
files)](../FAQ.md#the_simulation_has_lots_of_jamsdeadlocks_what_can_i_do).

Also, besides plain grid-locks, the imperfection of the lane-change
model sometimes leads to a situation where two vehicles try to get to
the other lane, and each vehicle is blocking the other one. The
simulation behaves as described earlier. There are two common causes of
this:

- [Motorway weaving sections (combined on- and off-ramps). This can be
  solved with additional
  connections](../Simulation/Motorways.md#combined_on-off-ramps)
- Vehicles inserted on the wrong lane close to an intersection with
  turning lanes. This is best solved by setting the vehicle attribute `departLane="best"`
  to ensure that vehicles are inserted on the correct lane for
  following their route.

## Collisions

By default, SUMO uses a collision-free model. However, due to bugs,
network problems or deliberate configuration,
[collisions may occur](../Simulation/Safety.md#collisions). The default behavior of SUMO is to immediately teleport the rear vehicle onto the next edge of it's route (or remove it, when already on it's final edge). This behavior can be [configured to avoid or delay teleporting](../Simulation/Safety.md#collisions). 

To avoid collisions, observe the simulation
[sumo-gui](../sumo-gui.md) at the location and time of the
collision. Check if some of the [that may cause a collision are present
in your network](../Simulation/Safety.md#deliberately_causing_collisions).

# What is happening while a vehicle teleports

A teleported vehicle is removed from the network. It is then moved along
its route, but no longer being on the street. It is reinserted into the
network as soon as this becomes possible. While being teleported, the
vehicle is moved along its route with the average speed of the edge it
was removed from or - later - it is currently "passing". The vehicle is
reinserted into the network if there is enough place to be placed on a
lane which allows to continue its drive. Consequently a vehicle may
teleport multiple times within one simulation.
