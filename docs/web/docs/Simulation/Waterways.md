---
title: Waterways
---

# Waterway Simulation

This page describes simulations of (inland) waterways in SUMO.

!!! caution
    The simulation of waterways is a developing subject and still carries some difficulties. These are discussed below.

# Approaches to ship modelling

Currently, no exclusive movement model for ships is implemented. Instead
the existing models for vehicle movement need to be re-purposed. By
setting `guiShape="ship"`, vehicles are drawn in a more appropriate shape.

# Building a network for waterway simulation

Waterways can be imported from
[OSM](../Networks/Import/OpenStreetMap.md) by adding the type map
[osmNetconvertShips.typ.xml]({{Source}}data/typemap/osmNetconvertShips.typ.xml).
They can also be explicitly specified by setting `allow="ship"`.

# Overtaking

To allow overtaking, it is recommended to build a network that enables [opposite direction driving](OppositeDirectionDriving.md).

If a linear body of water is wide enough to accomodate more than two ships, it can either be modelled by

- defining multiple lanes per direction
- defining a single wide lane per direction and enabling the [sublane model](SublaneModel.md#sublane-model)

## Known Problems

- Since the right-of-way rules for ships are more complex than those
  for road vehicles, waterway intersections are set to `uncontrolled` by default.
- Reversing direction of ships is [currently not modelled](https://github.com/eclipse-sumo/sumo/issues/15510)
- All movements must be modelled by edges. Freely navigating a large body of water is not possible (unless [TraCI](../TraCI.md) is used).
