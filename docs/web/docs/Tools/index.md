---
title: Tools
---

Tools can be found in the SUMO-distribution under {{SUMO}}/tools. Most of
them are tiny - they were written for a certain purpose and worked well
under certain input, but may be not verified for other cases.

The tools are divided into the following topics:

- *[assign](Assign.md)* - traffic assignment tools
- *[build](Build.md)* - tools used for code styling and
by the building subsystems
- *[detector](Detector.md)* - some tools for dealing
with real life induction loop data
- *[turn-data](Turns.md)* - some tools with turning counts and turning
  ratio data
with real life induction loop data
- *[district](District.md)* - tools for handling traffic
analysis zones (TAZ) aka districts
- *import* - additional tools to import data from a variety of formats
  - *[GTFS](Import/GTFS.md)* - some helpers for using GTFS data
  - *[MATSim](Import/MATSim.md)* - some helpers for
    using MATSim data
  - *[OpenDRIVE](Import/OpenDRIVE.md)* - some helpers for
    using OpenDRIVE data
  - *[OSM](Import/OSM.md)* - some helpers for
    accessing/using OpenStreetMap data
  - *[Vissim](Import/VISSIM.md)* - some helpers for
    using VISSIM data
  - *[Visum](Import/VISUM.md)* - some helpers for
    using VISUM data
- *[net](Net.md)* - tools for working with networks
(mainly SUMO-networks)
- *projects* - additional tools used by our projects
- *[route](Routes.md)* - tools for working with routes
- *[shapes](Shapes.md)* - tools for working with shapes
(PoIs and polygons)
- *[tls](tls.md)* - tools for traffic light systems
- *[traci](../TraCI/Interfacing_TraCI_from_Python.md)* - Python
implementation of the TraCI interface
- *[traceExporter](TraceExporter.md)* - generate
mobility traces / trajectories / orbits / gps tracks in various
formats
- *[trip](Trip.md)* - Trip generation and modification
without [od2trips](../od2trips.md)
- *[visualization](Visualization.md)* - graphical
evaluation of SUMO-outputs
- *[sumolib](Sumolib.md)* - python library for working
with sumo networks, simulation output and miscellaneous utility
functions
- *[output](Output.md)* - python tools for working with
sumo output files
- *[emissions](Emissions.md)* - tools for computing and
visualizing emissions
- *[xml](Xml.md)* - python tools for converting xml and
xml schema related utilities
- *[devel](devel.md)* - tools for developers
- *[miscellaneous](Misc.md)* - tools that do not fit the
above categories

!!! caution
    Most tools were written for Python 2.7 and still require this version. With the exception of sumolib and traci, Python 3.0 compatibility is not assured. We are still working on it.
