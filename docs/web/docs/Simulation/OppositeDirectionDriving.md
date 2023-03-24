---
title: Opposite Direction Driving
---

# Opposite-Direction-Driving

This page describes simulations which are enabled for for overtaking
through the opposite-direction edge. To enable this feature, the network
must contain additional information regarding the adjacency of
opposite-direction edges.

# Building a Network for opposite-direction-driving

The enable overtaking through the opposite direction lane, the
*.net.xml* file must contain `<neigh>` elements that specify adjacency. The
options for including this information are described below.

## Heuristically

By setting the [netconvert](../netconvert.md) option **--opposites.guess** {{DT_BOOL}}, opposite
edges are identified heuristically.

## Visually

Lane attribute 'opposite' can be set directly in [netedit](../Netedit/elementsNetwork.md#lanes)

## Explicitly

Adjacency can be declared explicitly in an *.edg.xml* file [as described
here](../Networks/PlainXML.md#neighboring_opposite-direction_lanes).
Explicit definitions take precedence over heuristic computation of
adjacency. To modify an existing network, an *.edg.xml* file can be [loaded as a patch](../Tutorials/ScenarioGuide.md#modifying_the_network)

# Model Description

Vehicles may overtake through the opposite direction lane to improve
their travel speed. The decision to perform such a maneuver is subject
to additional checks in regard to

- oncoming vehicles
- the assumed duration for overtaking the leading vehicle(s)
- the presence of free space at the end of the column being overtaken
  (maximum look-ahead is currently fixed at 150m and overtaking may
  only take place if free space is found before that threshold).
- the priority of intersections that may be passed during overtaking
  (only straight prioritized links may be used for opposite-direction
  overtaking)
- The required safety-buffers can be calibrated using the vehicle type
  attribute *lcOpposite*

Vehicles may also stop on the opposite side by defining a `<stop>` with a lane index that lies to the left of the leftmost forward direction lane (i.e. index 3  if the forward lane has lane indices 0,1,2.

# Limitations

- Visibility of oncoming traffic and the preceding vehicles due to
  road topology and occluding vehicles is not taken into account.
- Neighboring lane information can only be set if both edges have the
  same length. As a workaround the length of the edges may be set to a
  custom value (which may be different from the geometrical length).
  This can be automated by setting the option **--opposites.guess.fix-lengths**.
- Opposite-direction-driving is not compatible with the [sublane
  model](../Simulation/SublaneModel.md). before version 1.9.0
