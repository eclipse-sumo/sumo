---
title: NETEDIT toolbar options
---

# NETEDIT toolbar options

The toolbar changes when you switch between the Network, Demand, and Data supermodes or select a different edit mode. Hover over a button to see what it controls. The **Edit** menu shows the options and shortcuts for the active mode. Switch to the relevant mode before enabling an option.

## Network supermode

### Choose whether clicks target edges or lanes

In [Inspect](editModesCommon.md#inspect), [Select](editModesCommon.md#select), and [Delete](editModesCommon.md#delete), enable **Clicks target lanes** in the toolbar to act on an individual lane. The **Edit** menu calls this option **Clicking should target lanes**. Turn it off to target the whole edge. In Inspect mode, hold <kbd>Shift</kbd> while clicking to temporarily reverse the current setting.

| Edit mode | Effect when lane targeting is enabled |
| --- | --- |
| Inspect | Open the clicked lane's attributes. |
| Select | Toggle selection of the clicked lane. |
| Delete | Remove the clicked lane when deleting individual elements. |

### Other mode-specific options

| Edit mode | Option | Effect |
| --- | --- | --- |
| Inspect, Select, Delete | **Show connections over junctions** | Display connections between lanes so that you can click them. |
| Select | **Selecting multiple edges automatically select their junctions** | Include the junctions when selecting multiple edges. |
| [Create Edges](editModesNetwork.md#create_edges) | **Create consecutive edges** | Use the end of one edge as the start of the next. |
| Create Edges | **Create an edge in the opposite direction** | Create a reverse edge for each edge you draw. |
| [Move](editModesCommon.md#move) | **Move elevation** | Change a junction's or geometry point's elevation by dragging it. |
| [Traffic Lights](editModesNetwork.md#traffic_lights) | **Clicking should apply state changes to all phases of TLS plan** | Apply a signal state change across the traffic light program. |

## Demand supermode

In Inspect mode, use **Show non-inspected demand elements** to control whether other demand remains visible while inspecting an element. Demand mode also provides **Show all person plans** and **Show all container plans** to display plans beyond the current selection.

## Data supermode

| Edit mode | Option | Effect |
| --- | --- | --- |
| Inspect, Select, Delete, [TAZ relation](editModesData.md#taz_relation_mode) | **Draw TAZRel from center** | Draw relation lines from zone centers. |
| Inspect, Select, Delete, TAZ relation | **Only draw TAZRel from**, **Only draw TAZRel to** | Show relations that originate or end in the chosen zone. |
| Inspect, TAZ relation | **Draw TAZ fill** | Fill zone polygons. |
