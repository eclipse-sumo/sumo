---
title: Import
---

[netconvert](../../netconvert.md) allows to import road networks from
different third-party formats. By now, the following formats are
supported:

- OpenStreetMap databases, see
  [Networks/Import/OpenStreetMap](../../Networks/Import/OpenStreetMap.md)
- PTV VISUM (a macroscopic traffic simulation package), see
  [Networks/Import/VISUM](../../Networks/Import/VISUM.md)
- PTV VISSIM (a microscopic traffic simulation package), see
  [Networks/Import/Vissim](../../Networks/Import/Vissim.md)
- OpenDRIVE networks, see
  [Networks/Import/OpenDRIVE](../../Networks/Import/OpenDRIVE.md)
- MATsim networks, see
  [Networks/Import/MATsim](../../Networks/Import/MATsim.md)
- ArcView-data base files, see [Importing ArcView networks (shapefiles)](../../Networks/Import/ArcView.md)
- Elmar Brockfelds unsplitted and splitted NavTeq-data, see
  [Networks/Import/DlrNavteq](../../Networks/Import/DlrNavteq.md)
- RoboCup Rescue League folders, see
  [Networks/Import/RoboCup](../../Networks/Import/RoboCup.md)

In most of these cases, [netconvert](../../netconvert.md) needs only
two parameter: the option named as the source application/format
followed by the name of the file to convert and the name of the output
file (using the **--output-file** option). In the case, a VISUM network shall be
imported, the following code will convert it into a SUMO-network:

```
netconvert --visum=MyVisumNet.inp --output-file=MySUMONet.net.xml
```

The import can be influenced using [further netconvert options](../../Networks/Further_Options.md).

The native format comes in the variant of
[*plain-xml*](../../Networks/PlainXML.md#node_descriptions)
which is designed as a simple input format for
[netconvert](../../netconvert.md) and in *.net.xml* which is the
output format of [netconvert](../../netconvert.md) and which is
enriched with heuristically derived data such as right-of-way rules and
junction geometry. Both formats can be converted into each other without
data loss.

# Supported Standards and Versions

The table below summarizes external infrastructure, demand, output, and co-simulation standards supported across SUMO:

| Standard / Format | Import | Export | Supported Versions & Notes |
| --- | --- | --- |
| [ASAM OpenDRIVE](OpenDRIVE.md) | `--opendrive-files` | [`--opendrive-output`](../Further_Outputs.md#opendrive_road_networks) | Full 1.4 support; partial 1.5 and 1.6 import (`<junction type="virtual">` unsupported); export writes OpenDRIVE 1.4 (`revMajor="1" revMinor="4"`). |
| [OpenStreetMap XML](OpenStreetMap.md) | `--osm-files` | No | OSM API v0.6 XML (`.osm`, `.osm.xml`, `.osm.xml.gz`). |
| [MATSim Network](MATsim.md) | `--matsim-files` | [`--matsim-output`](../Further_Outputs.md#matsim_road_networks) | `network_v1.dtd` XML. |
| [ESRI Shapefile / OGR](ArcView.md) | `--shapefile-prefix` | No | Vector road networks via GDAL/OGR. |
| [GeoTIFF DEM](../Elevation.md) | `--heightmap.geotiff` | No | Elevation rasters (SRTM, NASADEM, Copernicus DEM, ASTER) via GDAL; also supported as [sumo-gui](../../sumo-gui.md) backgrounds. |
| [PTV VISUM](VISUM.md) / [PTV VISSIM](Vissim.md) | `--visum-file`, `--vissim-file` | No | Legacy `.inp` and VISSIM `.inpx` XML network import (partial coverage on newer VISSIM releases). |
| [DLR NavTeq](DlrNavteq.md) | `--dlr-navteq-prefix` | [`--dlr-navteq-output`](../Further_Outputs.md#dlrnavteq_road_networks) | Version 6.5 export by default (`--dlr-navteq.version`). |
| [GTFS](../../Tutorials/GTFS.md) | `gtfs2pt.py` | No | Static GTFS public transport schedules and shapes mapped onto SUMO networks. |
| [Amitran](../../Simulation/Output/AmitranOutput.md) | `od2trips --amitran-files` | `--amitran-output`, `<edgeData>` | Amitran OD matrices, vehicle trajectories, and link traffic measures. |
| [FMI](../../Developer/FMI.md) | Co-simulation | `sumo-fmi` shared library | Functional Mock-up Interface (FMI) 2.0 for Co-Simulation (`fmi2Functions`); FMI 3.0 is not yet implemented. |
| ASAM OpenSCENARIO / CityGML | Unsupported | Unsupported | No native importer or exporter in SUMO; scenario coupling relies on external co-simulation runners via [TraCI](../../TraCI/index.md) or OpenDRIVE export. |
