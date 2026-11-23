---
title: elementsNetwork
---

# Network Elements

## Junctions

[Junctions](../Networks/PlainXML.md#node_descriptions), (also referred to as *Nodes*), represent intersections. A SUMO junction is equivalent to a Vertex in graph theory.

![](../images/GNEJunction.png)
Example of different junctions

## Edges

In the SUMO-context, [edges](../Networks/PlainXML.md#edge_descriptions) represent roads or streets. Note that edges are unidirectional. It's equivalent to a edge in a graph theory.

![](../images/GNEEdge.png)
An edge in the Inspector

## Lanes

Each SUMO edge is composed of a set of [lanes](../Networks/PlainXML.md#lane-specific_definitions) (At least one).
Use [Inspect mode](editModesCommon.md#inspecting_lanes) to edit lane attributes.

![](../images/GNELane.png)
A lane in the Inspector

## Connections

[Connections](../Networks/PlainXML.md#connection_descriptions) describe how incoming and outgoing edges of junctions are connected (for example to prohibit left-turns at some junctions.

![](../images/GNEConnection.png)
Example of connection between a source lane and a target lane

## Traffic Lights

A [traffic light program](../Networks/PlainXML.md#traffic_light_program_definition) defines the phases of a traffic light.

![](../images/GNETLS.png)
Example of Traffic light
