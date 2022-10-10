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
Example of edge. Note that the box "select edge" is **checked**

## Lanes

Each SUMO edge is composed of a set of [lanes](../Networks/PlainXML.md#lane-specific_definitions) (At least one).
To inspect and edit lane attributes, either SHIFT+click on a lane or toggle the corresponding menu bar button ton control whether inspect clicks should target edges (default) or lanes (shortcut ALT+5).

![](../images/GNELane.png)   
Example of lane. Note that the box "select edge" is **unchecked**

## Connections

[Connections](../Networks/PlainXML.md#connection_descriptions) describe how incoming and outgoing edges of junctions are connected (for example to prohibit left-turns at some junctions.

![](../images/GNEConnection.png)   
Example of connection between a source lane and a target lane

## Traffic Lights

A [traffic light program](../Networks/PlainXML.md#traffic_light_program_definition) defines the phases of a traffic light.

![](../images/GNETLS.png)   
Example of Traffic light
