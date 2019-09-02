---
title: Networks/Elevation
permalink: /Networks/Elevation/
---

# Including Elevation Data in a Network

Elevation data can be imported from the following sources 

- directly from network input
  - from [OSM (with some tricks)](../Networks/Import/OpenStreetMap.md#elevation_data)
  - from [OpenDRIVE](../Networks/Import/OpenDRIVE.md) or [Shapefile](../Networks/Import/ArcView.md)

- by applying extra data from supplemental files
  - from a shapefile mesh by using the [NETCONVERT](../NETCONVERT.md) option **--heightmap.shapefiles**
  - from a greyscale height-map using [NETCONVERT](../NETCONVERT.md) option **--heightmap.geotiff**.
  - from [*edg.xml files*](../Networks/PlainXML.md#edge_descriptions) as part of the shape specification


# Related Topics

## Retrieving Elevation data via [TraCI](../TraCI.md)

Current 3D-Position can be retrieved for persons and vehicles.

## Visualizing Elevation data

[SUMO-GUI](../SUMO-GUI.md) and [NETEDIT](../NETEDIT.md) allow
coloring edges by:

- elevation at the start of the edge
- elevation at the start of each straight-line geometry segment for
each edge
- inclination of the whole edge
- inclination of each straight-line geometry segment for each edge

To calibrate the color range to the elevations found in the network, the *Recalibrate Rainbow* within the gui settings dialog button may be used.

Furthermore, [SUMO-GUI](../SUMO-GUI.md) is [available with support for 3D-Visualization](https://sumo.dlr.de/daily/sumo-msvc12extrax64-git.zip) which can be used to visualize elevation profiles as well. 

## Models that use Elevation Data

- [Electric vehicle model](../Models/Electric.md)
- [PHEMLigh emission model](../Models/Emissions/PHEMlight.md)
- [carFollowModel="KraussPS"](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#car-following_models)