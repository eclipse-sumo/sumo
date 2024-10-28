---
title: Elevation
---

# Including Elevation Data in a Network

Elevation data can be imported from the following sources

- directly from network input
  - from [OSM (with some tricks)](../Networks/Import/OpenStreetMap.md#elevation_data)
  - from [OpenDRIVE](../Networks/Import/OpenDRIVE.md) or [Shapefile](../Networks/Import/ArcView.md)

- by applying extra data from supplemental files
  - from a shapefile mesh by using the [netconvert](../netconvert.md) option **--heightmap.shapefiles**
  - from a greyscale height-map using [netconvert](../netconvert.md) option **--heightmap.geotiff**.
  - from [*edg.xml files*](../Networks/PlainXML.md#edge_descriptions) as part of the shape specification

- by shifting geometry points and junctions along the z-axis in [netedit move mode](../Netedit/editModesCommon.md#changing_elevation).
- by generating an abstract network with z-level noise using [netgenerate](../netgenerate.md) option **--perturb-z**

# Related Topics

## Retrieving Elevation data via [TraCI](../TraCI.md)

Current 3D-Position can be retrieved for persons and vehicles.

## Visualizing Elevation data

[sumo-gui](../sumo-gui.md) and [netedit](../Netedit/index.md) allow
coloring edges by:

- elevation at the start of the edge
- elevation at the start of each straight-line geometry segment for
each edge
- inclination of the whole edge
- inclination of each straight-line geometry segment for each edge

To calibrate the color range to the elevations found in the network, the *Recalibrate Rainbow* within the gui settings dialog button may be used.

Furthermore, there is an "extra" version of [sumo-gui](../sumo-gui.md) [available with support for 3D-Visualization](../Downloads.md) which can be used to visualize elevation profiles as well.

## Models that use Elevation Data

- [Electric vehicle model](../Models/Electric.md)
- [PHEMLight emission model](../Models/Emissions/PHEMlight.md)
- [carFollowModel="KraussPS"](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#car-following_models)
