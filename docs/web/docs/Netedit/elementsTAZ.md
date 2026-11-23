---
title: elementsTAZ
---

# TAZ elements

A [traffic analysis zone (TAZ)](../Demand/Importing_O/D_Matrices.md#describing_the_taz) groups edges that can serve as trip origins and destinations. Each member edge has a source weight for departures and a sink weight for arrivals. The polygon shows the zone on the map.

## Creating a zone

1. Select **Network** supermode (<kbd>F2</kbd>) and **TAZ** mode ![](../images/icon_modetaz.png) (<kbd>Z</kbd>).
2. Leave **edges within** enabled to add the edges inside the polygon as sources and sinks. Clear it to start with an empty zone.
3. Click **Start drawing** or press <kbd>Enter</kbd>. Click at least three points on the map to mark the vertices. Hold <kbd>Shift</kbd> and click to remove the last vertex.
4. Click **Stop drawing** or press <kbd>Enter</kbd> again to finish. NETEDIT closes the polygon. Click **Abort drawing** or press <kbd>Esc</kbd> to discard an unfinished zone.

## Editing edges and weights

Click a zone in **TAZ** mode to edit its edges. The **Membership** control changes what happens when you click an edge.

- **Toggle** adds or removes the clicked edge. Set **New source** and **New sink** before adding an edge to give it custom weights.
- **Keep** selects an existing member edge for editing. Hold <kbd>Shift</kbd> and drag with the left mouse button to select multiple edges. Enter **Source** and **Sink** weights in **Selection Statistics**.

Click **Confirm changes** to apply the edits or **Cancel changes** to discard them. Use [Inspect mode](editModesCommon.md#inspect) to edit zone attributes such as its name and color.

## Saving zones

Choose **File → Additionals and Shapes → Save Additionals** (<kbd>Ctrl</kbd>+<kbd>Shift</kbd>+<kbd>A</kbd>) to write the zones and their sources and sinks to an additional file. Use **Load Additionals...** in the same submenu to open an existing file.

Use [TAZ relation mode](editModesData.md#taz_relation_mode) in **Data** supermode to work with data for trips between zones.
