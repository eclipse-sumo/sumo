---
title: Purgatory/TopicVehicleRerouting
permalink: /Purgatory/TopicVehicleRerouting/
---

# Vehicle Rerouting

Within the next subchapter, we want to try to give some advices on how
investigations on vehicle rerouting should be prepared and evaluated.
Though most of these are based on prior work, any further ideas or
processes and other information is appreciated.

## Basic Description

The research descibed in here deals with methods for computing vehicle
routes, mainly in the case of unforeseenable events. Normally, we assume
traffic is in an "equlibrium state" - a route change does not yield in a
better performance of the road network. Nonetheless, the traffic may
change due to:

- accidents and resulting jams
- additional event traffic
- street closures
- changing destinations (new shopping center)

In such cases, certain edges of the road network will be either
completely unsuable or the time needed to pass them raises, either
directly, or because of the additional demand of vehicles that want to
pass them.

## Things to keep in Mind

- **The simulated traffic should be in equilibrium mode before the
  incident is introduced**<br>
  *Explanation*: Using fastest routes in an empty network, like
  obtained when using a simple Dijkstra (done when converting trips to
  routes via [DUAROUTER](../DUAROUTER.md)) is not realistic. In
  this case, all vehicles would use the fastest edges *in an empty
  network*, which would yield in jam situation on these edges. I such
  cases, it is easy to find a new best route during simulation - but
  the initial situation would be not realistic. Having a proper
  traffic assignment is needed for realistic evaluations.

## Evaluation Advices