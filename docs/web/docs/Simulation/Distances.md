---
title: Distances
---

# Introduction
A Sumo simulation network uses multiple coordination system

1. x,y,z: coordinates in meters (Cartesian coordinates)
2. lon,lat,z: geo-coordinates + elevation in meters (typically WGS84) if the network is geo-referenced
3. edgeID, laneIndex, lanePosition: The lanePosition is the driving distance from the start of the edge (road) in meters

For the simulation behavior of the vehicles, only coordinate system 3 is used
whereas the other systems are for visualization and output.

# Length-Geometry-mismatch

By default, the length of a straight edge corresponds to the euclidean distance
between its start and end positions (x,y).

However, lengths of edges and lanes can be different from the euclidean
distance (referred to as a **length-geometry-mismatch**) and there are several reasons why this occurs.


[sumo-gui](../sumo-gui.md#edgelane_visualisation_settings) and [netedit](../Netedit/index.md) can be configured to color lanes by ''by given length/geometrical length'' which highlights the mismatch between both distance values.

## Short Lanes
Lanes with a geometrical length of 0 have an assigned minimum length of 0.1m.

## Lane curvature
By design, all lanes of an edge are assigned the same length value (corresponding to driving distance and maximum lanePosition).
The assigned length is set to the average length of all lanes of that edge.
For curved edges, this introduces a mismatch between geometrical length and driving distance because outer
lanes in a curve are longer than inner lanes.

The main architectural reason for enforcing the same length for all lanes is the great simplification this brings to lane-changing computation.

!!! note
    since version 1.9.0 internal lanes for multi-lane turning connections (linkDir != 's') permit different lengths. Lane changing is forbidden while a vehicle is on such lanes.

## No-internal-links (meso)
When running a simulation with option **--no-internal-links** or **--mesosim**
(which implies **--no-internal-links**) then [vehicles will "jump" across
intersections](Intersections.md#internal_links).
This discontinuity obviously introduces a mismatch between driving distance and
x,y coordinates.

When the network was built with [netconvert option](../netconvert.md) **--no-internal-links** then all edges in the network are made artificially longer to ensure that the total driving distance through the network matches the euclidean distance (averaged over the whole route). As a consequence, each edge has a length-geometry mismatch

## Abstract networks
In some types of simulation it is beneficial to define edges with
length-geometry-mismatch. For example, railway networks with switches and parallel tracks are much easier to understand when the
geometrical lengths are reduced. To keep the driving-lengths at their realistic values, each edge is given a custom length value.

# Vehicle lengths in sumo-gui
By default, vehicle lengths are scaled according to [length-geometry mismatch](#length-geometry-mismatch). This corresponds to visual shortening when looking from above at a vehicle that is driving on a step include (the visual horizontal length is shorter than total road length). To prevent scaling, the vehicle gui option 'scale length with geometry' can be disabled.

!!! note
    Until version 1.9.0 Vehicle lengths were not affected by length-geometry-mismatch. This means, vehicles may have seemed to violate minimum gaps or even collide if they were driving on an edge where the (driving) length is longer than the geometrical (euclidean) length.
