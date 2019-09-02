---
title: Purgatory/ReroutingApproaches
permalink: /Purgatory/ReroutingApproaches/
---

There are different approaches to give new routes to a vehicle while the
simulation is running. This page will summarize them. The categorization
is twofold: internal vs. external and direct route setting vs.
(Dijkstra) shortest path finding.

## Internal

SUMO supports a rerouter which is located on an edge for three different
application scenarios:

- forbidding an edge (all vehicles passing the rerouter are no longer
  allowed to drive on the specified edge and will look for a new
  shortest route to their destination)
- giving a distribution of new destinations (the vehicle will choose
  the shortest route to the destination drawn from the distribution)
- giving a distribution of new routes (the vehicle will draw a route
  from the distribution)

*Edge weights?* Furthermore internal devices such as the C2C-device
support rerouting even with different edge weights for each vehicle.

## External

The TraCI interface supports setting an explicit route as well as
shortest path route finding (also with vehicle specific weights).

## Route setting

This is of course the most flexible approach. The drawback is that
either the set of routes to choose from or the net has to be known to
the one setting the route. Especially external applications would need
to parse the SUMO net (or an equivalent) not only for the
node/edge/connection information but also for forbidden vehicle types
etc. If this information can change over time the external app has to be
synchronized with the simulation, which is highly non-trivial.

## Shortest paths

SUMO has a flexible Dijkstra shortest path finder inside which can be
used to find new routes. The edge weight used to find the shortest path
is usually the quotient of edge length and maximum speed (minimum travel
time) or the average travel time (*averaged over ...?*) but may be
altered individually for each vehicle (*globally as well?*). The
implementation is considerably fast to allow even for multiple
reroutings of each vehicle during the simulation without measurable
performance losses. The drawback is that not every route choice can be
modeled via an appropriate edge cost, think for instance of minimizing
left turns (*If we could assign weights to internal edges, we could
model this\!?*) Furthermore the shortest path algorithm does not allow
for negative edge weights (*or is it flexible enough to fail on negative
cycles only?*)

## Actions and Actors

Route computation requires following actions:

- determination of edge weights (by measuring/extrapolation)
- storing of edge weights
- triggering a route computation
- route computation (algorithms for using stored values, weighting,
  and routes computation)
- route acceptance

**Table 1: possible actors**

| Action                         | Actor(s)           |
|--------------------------------|--------------------------------------------------------------------------------|
| determination of edge weights  | <li>vehicle (own travel time, received travel times, ...)</li><li>edge (mean travel time or other values, ...)</li><li>control center (reported jams, obtained travel times, ...)</li> |
| storing of edge weights        | <li>vehicle (own knowledge, as within obtaining knowledge via V2V)</li><li>edge (mean values)</li><li>center (same as edge?)</li>    |
| triggering a route computation | <li>vehicle itself (after obtaining a weight for own route)</li><li>center (via radio broadcast)</li>      |
| route computation              | <li>vehicle</li><li>external application</li>   |
| route acceptance               | vehicle                  |