---
title: NETCONVERT
permalink: /NETCONVERT/
---

# From 30.000 feet

**NETCONVERT** imports digital road networks from different sources and
generates road networks that can be used by other tools from the
package.

  -
    **Purpose:** Road networks import and conversion
    **System:** portable (Linux/Windows is tested); runs on command line
    **Input (mandatory):** Definition of a road network
    **Output:** A generated SUMO-road network; optionally also other
    outputs
    **Programming Language:** C++

# Usage Description

[NETCONVERT](NETCONVERT.md) is a command line application. It
assumes at least one parameter - the combination of the name of the file
type to import as parameter name and the name of the file to import as
parameter value. So, for importing a network from OpenStreetMap one
could simply write:

`netconvert --osm my_osm_net.xml`

and for importing a VISUM-network:

`netconvert --visum my_visum_net.net`

In both cases, as no output name is given, the SUMO network generated
from the imported data is written into the file "net.net.xml". To write
the network into a different file, use the option . If you want to save
the imported VISUM-network into a file named "my_sumo_net.net.xml",
write:

`netconvert --visum my_visum_net.net -o my_sumo_net.net.xml`

Many further parameter steer how the network is imported and how the
resulting SUMO-network is generated.

# Supported Network Formats

## Import

[NETCONVERT](NETCONVERT.md) is able to import road networks from
the following formats:

  - ["SUMO native" XML
    descriptions](Networks/Building_Networks_from_own_XML-descriptions.md)
    (\*.edg.xml, \*.nod.xml, \*.con.xml, \*.tll.xml)
  - [OpenStreetMap](OpenStreetMap_file.md) (\*.osm.xml),
    including shapes (see [OpenStreetMap
    import](Networks/Import/OpenStreetMap.md))
  - [VISUM](Networks/Import/VISUM.md), including shapes and
    demands
  - [Vissim](Networks/Import/Vissim.md), including demands
  - [OpenDRIVE](Networks/Import/OpenDRIVE.md)
  - [MATsim](Networks/Import/MATsim.md)
  - [SUMO](Networks/Import/SUMO_Road_Networks.md) (\*.net.xml)
  - [Shapefiles](Networks/Import/ArcView.md) (.shp, .shx, .dbf),
    e.g. ArcView and newer Tiger networks
  - [Robocup Rescue League](Networks/Import/RoboCup.md),
    including shapes
  - [a DLR internal variant of Navteq's
    GDF](Networks/Import/DlrNavteq.md) (Elmar format)

## Export

[NETCONVERT](NETCONVERT.md) is able to export road networks in
the following formats:

  - [SUMO](Networks/Export#SUMO.md) (\*.net.xml)
  - ["SUMO native" XML descriptions](Networks/Export#Plain.md)
    (\*.edg.xml, \*.nod.xml, \*.con.xml, \*.tll.xml)
  - [OpenDRIVE](Networks/Export#OpenDRIVE.md)
  - [MATsim](Networks/Export#MATsim.md)
  - [a DLR internal variant of Navteq's
    GDF](Networks/Export#DlrNavteq.md) (Elmar format)
  - [a basic network view developed in the Amitran
    project](Networks/Export#Amitran.md)

# Further supported Data Formats

## Import

  - [Public transport
    stops](Simulation/Public_Transport#Bus_Stops.md). The main
    use of importing stop data is to update/add
    [access](Simulation/Public_Transport#Access_Lanes.md)
    information.

## Export

The following list ouf output is explained in more detail at
[Networks/Further_Outputs](Networks/Further_Outputs.md)

  - [Public transport
    stops](Simulation/Public_Transport#Bus_Stops.md). Currently,
    this can only be iported from
    [OSM](Tutorials/PT_from_OpenStreetMap#Initial_network_and_public_transit_information_extraction.md)
  - [public transport
    lines](Tutorials/PT_from_OpenStreetMap#Initial_network_and_public_transit_information_extraction.md)
  - Information on joined junctions
  - Street Signs (as POIs)
  - Street Names (embedded within the *.net.xml* output
  - [Parking Areas](Simulation/ParkingArea.md) (currently, only
    road-side parking can be imported from OSM\]\]
  - Railway topology output: A file for analyzing the topology of
    railway networks in regard to bi-directional track usage

# Options

You may use a XML schema definition file for setting up a NETCONVERT
configuration:
[netconvertConfiguration.xsd](http://sumo-sim.org/xsd/netconvertConfiguration.xsd).

## Configuration

All applications of the **SUMO**-suite handle configuration options the
same way. These options are discussed at [Basics/Using the Command Line
Applications\#Configuration
Files](Basics/Using_the_Command_Line_Applications#Configuration_Files.md).

<table>
<thead>
<tr class="header">
<th><p>Option</p></th>
<th><p>Description</p></th>
</tr>
</thead>
<tbody>
<tr class="odd">
<td><p><br />
</p></td>
<td><p>Loads the named config on startup</p></td>
</tr>
<tr class="even">
<td><p><br />
</p></td>
<td><p>Saves current configuration into FILE</p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Saves a configuration template (empty) into FILE</p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Saves the configuration schema into FILE</p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Adds comments to saved template, configuration, or schema; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td></td>
</tr>
</tbody>
</table>

## Input

<table>
<thead>
<tr class="header">
<th><p>Option</p></th>
<th><p>Description</p></th>
</tr>
</thead>
<tbody>
<tr class="odd">
<td><p><br />
</p></td>
<td><p>Read SUMO-net from FILE</p></td>
</tr>
<tr class="even">
<td><p><br />
</p></td>
<td><p>Read XML-node defs from FILE</p></td>
</tr>
<tr class="odd">
<td><p><br />
</p></td>
<td><p>Read XML-edge defs from FILE</p></td>
</tr>
<tr class="even">
<td><p><br />
</p></td>
<td><p>Read XML-connection defs from FILE</p></td>
</tr>
<tr class="odd">
<td><p><br />
</p></td>
<td><p>Read XML-traffic light defs from FILE</p></td>
</tr>
<tr class="even">
<td><p><br />
</p></td>
<td><p>Read XML-type defs from FILE</p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Reads public transport stops from FILE</p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Reads public transport lines from FILE</p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Reads polygons from FILE for embedding in network where applicable</p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Read shapefiles (ArcView, Tiger, ...) from files starting with 'FILE'</p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Read converted Navteq GDF data (unsplitted Elmar-network) from path 'FILE'</p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Read OSM-network from path 'FILE(s)'</p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Read OpenDRIVE-network from FILE</p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Read VISUM-net from FILE</p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Read VISSIM-net from FILE</p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Read RoboCup-net from DIR</p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Read MATsim-net from FILE</p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Read ITSUMO-net from FILE</p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Read heightmap from ArcGIS shapefile</p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Read heightmap from GeoTIFF</p></td>
</tr>
<tr class="odd">
<td></td>
<td></td>
</tr>
</tbody>
</table>

## Output

<table>
<thead>
<tr class="header">
<th><p>Option</p></th>
<th><p>Description</p></th>
</tr>
</thead>
<tbody>
<tr class="odd">
<td></td>
<td><p>Include license info into every output file; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Prefix which is applied to all output files. The special string 'TIME' is replaced by the current time.</p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Defines the number of digits after the comma for floating point output; <em>default: <strong>2</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Defines the number of digits after the comma for lon,lat output; <em>default: <strong>6</strong></em></p></td>
</tr>
<tr class="odd">
<td><p><br />
</p></td>
<td><p>Write time values as hour:minute:second or day:hour:minute:second rathern than seconds; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td><p><br />
</p></td>
<td><p>The generated net will be written to FILE</p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Prefix of files to write plain xml nodes, edges and connections to</p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Writes information about joined junctions to FILE (can be loaded as additional node-file to reproduce joins</p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Defines a prefix for edge and junction names</p></td>
</tr>
<tr class="even">
<td></td>
<td><p>The generated net will be written to FILE using Amitran format</p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>The generated net will be written to FILE using MATsim format</p></td>
</tr>
<tr class="even">
<td></td>
<td><p>The generated net will be written to FILE using OpenDRIVE format</p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>The generated net will be written to dlr-navteq files with the given PREFIX</p></td>
</tr>
<tr class="even">
<td></td>
<td><p>The network coordinates are written with the specified level of output precision; <em>default: <strong>2</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Street names will be included in the output (if available); <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Writes original names, if given, as parameter; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Writes street signs as POIs to FILE</p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Writes public transport stops to FILE</p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Writes public transport lines to FILE</p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Clean-up pt stops that are not served by any line; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Writes parking areas to FILE</p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Analyse topology of the railway network</p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Write shapes that are embedded in the network input and that are not supported by polyconvert (OpenDRIVE)</p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Builds parameterized curves whenever the angular change between straight segments exceeds FLOAT degrees; <em>default: <strong>1e-08</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td></td>
</tr>
</tbody>
</table>

## Projection

<table>
<thead>
<tr class="header">
<th><p>Option</p></th>
<th><p>Description</p></th>
</tr>
</thead>
<tbody>
<tr class="odd">
<td></td>
<td><p>Uses a simple method for projection; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Scaling factor for input coordinates; <em>default: <strong>1</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Rotation (clockwise degrees) for input coordinates; <em>default: <strong>0</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Determine the UTM zone (for a universal transversal mercator projection based on the WGS84 ellipsoid); <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Determine the DHDN zone (for a transversal mercator projection based on the bessel ellipsoid, "Gauss-Krueger"); <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Uses STR as proj.4 definition for projection; <em>default: <strong>!</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Inverses projection; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Convert from Gauss-Krueger to UTM; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Write geo coordinates in plain-xml; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td></td>
</tr>
</tbody>
</table>

## Processing

<table>
<thead>
<tr class="header">
<th><p>Option</p></th>
<th><p>Description</p></th>
</tr>
</thead>
<tbody>
<tr class="odd">
<td></td>
<td><p>vmax is parsed as given in km/h (some); <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Use YYYY-MM-DD date to determine the readiness of features under construction</p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Remove all z-data; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>If edge shapes do not end at the node positions, extend them; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Remaps alphanumerical IDs of nodes and edges to ensure that all IDs are integers; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Ensures that generated ids do not included any of the typed IDs from FILE (SUMO-GUI selection file format)</p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Removes vehicle class restrictions from imported edges; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Splits edges across geometry nodes; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="odd">
<td><p><br />
</p></td>
<td><p>Replace nodes which only define edge geometry by geometry points (joins edges); <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Ensure that the given list of edges is not modified</p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Ensure that the edges in FILE are not modified (Each id on a single line. Selection files from SUMO-GUI are also supported)</p></td>
</tr>
<tr class="even">
<td></td>
<td><p>splits geometry to restrict segment length; <em>default: <strong>0</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>reduces too similar geometry points; <em>default: <strong>-1</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Warn about edge geometries with an angle above DEGREES in successive segments; <em>default: <strong>99</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Warn about edge geometries with a turning radius less than METERS at the start or end; <em>default: <strong>9</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Straighten edge geometries to avoid turning radii less than geometry.min-radius; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Warn if the junction shape is to far away from the original node position; <em>default: <strong>20</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Warn if edges overlap by more than the given threshold value; <em>default: <strong>0</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Ignore overlapping edges if they are separated vertically by the given threshold.; <em>default: <strong>4</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Modify edge geometries to avoid overlap at junctions; <em>default: <strong>true</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Warn about edge geometries with a grade in % above FLOAT.; <em>default: <strong>10</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Smooth edge edge geometries with a grade in above the warning threshold.; <em>default: <strong>true</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Turn off normalizing node positions; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Adds FLOAT to net x-positions; <em>default: <strong>0</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Adds FLOAT to net y-positions; <em>default: <strong>0</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Flips the y-coordinate along zero; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Enable roundabout-guessing; <em>default: <strong>true</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Enable guessing of opposite direction lanes usable for overtaking; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Ensure that opposite edges have the same length; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Assumes left-hand traffic on the network; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Merges edges which connect the same nodes and are close to each other (recommended for VISSIM import); <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Modifies all edge speeds by adding FLOAT; <em>default: <strong>0</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Modifies all edge speeds by multiplying by FLOAT; <em>default: <strong>1</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Modifies all edge speeds to at least FLOAT; <em>default: <strong>0</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td></td>
</tr>
</tbody>
</table>

## Building Defaults

<table>
<thead>
<tr class="header">
<th><p>Option</p></th>
<th><p>Description</p></th>
</tr>
</thead>
<tbody>
<tr class="odd">
<td><p><br />
</p></td>
<td><p>The default number of lanes in an edge; <em>default: <strong>1</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>The default width of lanes; <em>default: <strong>-1</strong></em></p></td>
</tr>
<tr class="odd">
<td><p><br />
</p></td>
<td><p>The default speed on an edge (in m/s); <em>default: <strong>13.89</strong></em></p></td>
</tr>
<tr class="even">
<td><p><br />
</p></td>
<td><p>The default priority of an edge; <em>default: <strong>-1</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>The default width of added sidewalks; <em>default: <strong>2</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>The default width of a pedestrian crossing; <em>default: <strong>4</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>The default for disallowed vehicle classes</p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Whether junctions should be kept clear by default; <em>default: <strong>true</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>The default turning radius of intersections; <em>default: <strong>4</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>The default algorithm for computing right of way rules ('default', 'edgePriority'); <em>default: <strong>default</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td></td>
</tr>
</tbody>
</table>

## Tls Building

<table>
<thead>
<tr class="header">
<th><p>Option</p></th>
<th><p>Description</p></th>
</tr>
</thead>
<tbody>
<tr class="odd">
<td></td>
<td><p>Does not instatiate traffic lights loaded from other formats than plain-XML; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Does not instatiate traffic lights at geometry-like nodes loaded from other formats than plain-XML; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Interprets STR as list of junctions to be controlled by TLS</p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Interprets STR as list of junctions to be not controlled by TLS</p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Turns on TLS guessing; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Sets minimum value for the sum of all incoming lane speeds when guessing TLS; <em>default: <strong>69.4444</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Sets district nodes as tls-controlled; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Includes node clusters into guess; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Tries to cluster tls-controlled nodes; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Determines the maximal distance for joining traffic lights (defaults to 20); <em>default: <strong>20</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Do not control edges that lie fully within a joined traffic light. This may cause collisions but allows old traffic light plans to be used; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Interprets tls nodes surrounding an intersection as signal positions for a larger TLS. This is typical pattern for OSM-derived networks; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Distance for interpreting nodes as signal locations; <em>default: <strong>25</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Use INT as cycle duration; <em>default: <strong>90</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Use INT as green phase duration; <em>default: <strong>31</strong></em></p></td>
</tr>
<tr class="even">
<td><p><br />
</p></td>
<td><p>Defines smallest vehicle deceleration; <em>default: <strong>3</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Given yellow times are patched even if being too short; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Set INT as fixed time for yellow phase durations; <em>default: <strong>-1</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Set INT as fixed time for red phase duration at traffic lights that do not have a conflicting flow; <em>default: <strong>5</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Set INT as fixed time for intermediate red phase after every switch; <em>default: <strong>0</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Use FLOAT as threshold for allowing left-turning vehicles to move in the same phase as oncoming straight-going vehicles; <em>default: <strong>19.44</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Use INT as green phase duration for left turns (s). Setting this value to 0 disables additional left-turning phases; <em>default: <strong>6</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Use INT as minimum green duration for pedestrian crossings (s).; <em>default: <strong>4</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Use INT as clearance time for pedestrian crossings (s).; <em>default: <strong>5</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Use INT as green phase duration for pedestrian scramble phase (s).; <em>default: <strong>5</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>TLSs in STR will be shifted by half-phase</p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>TLSs in STR will be shifted by quarter-phase</p></td>
</tr>
<tr class="even">
<td></td>
<td><p>TLSs with unspecified type will use STR as their algorithm; <em>default: <strong>static</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Set phase layout four grouping opposite directions or grouping all movements for one incoming edge ['opposites', 'incoming']; <em>default: <strong>opposites</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Default minimum phase duration for traffic lights with variable phase length; <em>default: <strong>5</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Default maximum phase duration for traffic lights with variable phase length; <em>default: <strong>50</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td></td>
</tr>
</tbody>
</table>

## Ramp Guessing

<table>
<thead>
<tr class="header">
<th><p>Option</p></th>
<th><p>Description</p></th>
</tr>
</thead>
<tbody>
<tr class="odd">
<td></td>
<td><p>Enable ramp-guessing; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Guess on-ramps and mark acceleration lanes if they exist but do not add new lanes; <em>default: <strong>true</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Treat edges with speed &gt; FLOAT as no ramps; <em>default: <strong>-1</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Treat edges with speed &lt; FLOAT as no highways; <em>default: <strong>21.9444</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Use FLOAT as ramp-length; <em>default: <strong>100</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Tries to handle the given edges as ramps</p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Do not consider the given edges as ramps</p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Avoids edge splitting; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td></td>
</tr>
</tbody>
</table>

## Edge Removal

<table>
<thead>
<tr class="header">
<th><p>Option</p></th>
<th><p>Description</p></th>
</tr>
</thead>
<tbody>
<tr class="odd">
<td></td>
<td><p>Only keep edges with speed in meters/second &gt; FLOAT; <em>default: <strong>-1</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Remove edges in STR</p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Only keep edges in STR or those which are kept due to other keep-edges or remove-edges options</p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Only keep edges in FILE (Each id on a single line. Selection files from SUMO-GUI are also supported) or those which are kept due to other keep-edges or remove-edges options</p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Remove edges in FILE. (Each id on a single line. Selection files from SUMO-GUI are also supported)</p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Remove edges after joining; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Only keep edges which are located within the given boundary (given either as CARTESIAN corner coordinates &lt;xmin,ymin,xmax,ymax&gt; or as polygon &lt;x0,y0,x1,y1,...&gt;)</p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Only keep edges which are located within the given boundary (given either as GEODETIC corner coordinates &lt;lon-min,lat-min,lon-max,lat-max&gt; or as polygon &lt;lon0,lat0,lon1,lat1,...&gt;)</p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Only keep edges which allow one of the vclasss in STR</p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Remove edges which allow only vclasses from STR</p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Only keep edges where type is in STR</p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Only keep the INT largest weakly connected components; <em>default: <strong>0</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Remove edges where type is in STR</p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Removes isolated edges; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td></td>
</tr>
</tbody>
</table>

## Unregulated Nodes

<table>
<thead>
<tr class="header">
<th><p>Option</p></th>
<th><p>Description</p></th>
</tr>
</thead>
<tbody>
<tr class="odd">
<td></td>
<td><p>All nodes will be unregulated; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Do not regulate nodes in STR</p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Do not regulate district nodes; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td></td>
</tr>
</tbody>
</table>

## Junctions

<table>
<thead>
<tr class="header">
<th><p>Option</p></th>
<th><p>Description</p></th>
</tr>
</thead>
<tbody>
<tr class="odd">
<td></td>
<td><p>Omits internal links; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Disables building turnarounds; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Disables building turnarounds at tls-controlled junctions; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Disables building turnarounds at geometry-like junctions; <em>default: <strong>true</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Disables building turnarounds except at dead end junctions; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Disables building connections to left; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Joins junctions that are close to each other (recommended for OSM import); <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Determines the maximal distance for joining junctions (defaults to 10); <em>default: <strong>10</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Interprets STR as list of junctions to exclude from joining</p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Generate INT intermediate points to smooth out intersection corners; <em>default: <strong>5</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Generate INT intermediate points to smooth out lanes within the intersection; <em>default: <strong>5</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Generate longer intersections to allow for smooth s-curves when the number of lanes changes; <em>default: <strong>0</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Builds common edges for turning connections with common from- and to-edge. This causes discrepancies between geometrical length and assigned length due to averaging but enables lane-changing while turning; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Limits speed on junctions to an average lateral acceleration of at most FLOAT m/s^2); <em>default: <strong>5.5</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Do not limit turn speed for angular changes below FLOAT (degrees). The value is subtracted from the geometric angle before computing the turning radius.; <em>default: <strong>15</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Do not limit turn speed for angular changes below FLOAT (degrees) on railway edges. The value is subtracted from the geometric angle before computing the turning radius.; <em>default: <strong>35</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Warn about turn speed limits that reduce the speed of straight connections by more than FLOAT; <em>default: <strong>5</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Warn about turn speed limits that reduce the speed of turning connections (no u-turns) by more than FLOAT; <em>default: <strong>22</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Default radius for junctions that do not require wide vehicle turns; <em>default: <strong>1.5</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Forces rectangular cuts between lanes and intersections; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Allow driving onto a multi-lane road if there are foes on other lanes (at roundabouts); <em>default: <strong>true</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Allow driving onto a multi-lane road if there are foes on other lanes (everywhere); <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td></td>
</tr>
</tbody>
</table>

## Pedestrian

<table>
<thead>
<tr class="header">
<th><p>Option</p></th>
<th><p>Description</p></th>
</tr>
</thead>
<tbody>
<tr class="odd">
<td></td>
<td><p>Guess pedestrian sidewalks based on edge speed; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Add sidewalks for edges with a speed equal or below the given limit; <em>default: <strong>13.89</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Add sidewalks for edges with a speed above the given limit; <em>default: <strong>5.8</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Add sidewalks for edges that allow pedestrians on any of their lanes regardless of speed; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Do not guess sidewalks for the given list of edges</p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Guess pedestrian crossings based on the presence of sidewalks; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>At uncontrolled nodes, do not build crossings across edges with a speed above the threshold; <em>default: <strong>13.89</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Always build walking areas even if there are no crossings; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td></td>
</tr>
</tbody>
</table>

## Railway

<table>
<thead>
<tr class="header">
<th><p>Option</p></th>
<th><p>Description</p></th>
</tr>
</thead>
<tbody>
<tr class="odd">
<td></td>
<td><p>Discard all railway signal information loaded from other formats than plain-xml; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Repair topology of the railway network; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Allow bidiretional rail use wherever rails with opposite directions meet at a straight angle; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Make all rails usable in both direction; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>The search radius for finding suitable road accesses for rail stops; <em>default: <strong>150</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>The maximum roud accesses registered per rail stops; <em>default: <strong>5</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>The walking length of the access is computed as air-line distance multiplied by FLOAT; <em>default: <strong>1.5</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td></td>
</tr>
</tbody>
</table>

## Formats

<table>
<thead>
<tr class="header">
<th><p>Option</p></th>
<th><p>Description</p></th>
</tr>
</thead>
<tbody>
<tr class="odd">
<td></td>
<td><p>Skips the check for duplicate nodes and edges; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Imports elevation data; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Reconstruct (relative) elevation based on layer data. Each layer is raised by FLOAT m; <em>default: <strong>0</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Maximum grade threshold in % at 50km/h when reconstrucing elevation based on layer data. The value is scaled according to road speed.; <em>default: <strong>10</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Whether one-way roads should be spread to the side instead of centered; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>The default length of a public transport stop in FLOAT m; <em>default: <strong>25</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>The default length of a bus stop in FLOAT m; <em>default: <strong>15</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>The default length of a tram stop in FLOAT m; <em>default: <strong>25</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>The default length of a train stop in FLOAT m; <em>default: <strong>200</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Whether additional attributes shall be imported; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>The edge lengths given in the MATSIM-file will be kept; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>The lane number will be computed from the capacity; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Read edge ids from column STR</p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Read from-node ids from column STR</p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Read to-node ids from column STR</p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Read type ids from column STR</p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Read lane number from column STR</p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Read speed from column STR</p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Read (non-unique) name from column STR</p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Distance threshold for determining whether distinct shapes are connected (used when from-id and to-id are not available); <em>default: <strong>0</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Add the list of field names as edge params</p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Uses edge type defaults on problems; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Insert edges in both directions; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Guess the proper projection; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Structure join offset; <em>default: <strong>5</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Use FLOAT as default speed; <em>default: <strong>50</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Factor for edge velocity; <em>default: <strong>1</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Writes lanes without an explicit speed set; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Uses priorities from types; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Uses lane numbers from types; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Uses speeds from types; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Sets connector speed; <em>default: <strong>100</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Sets connector lane number; <em>default: <strong>3</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Excludes connectors; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Computes the number of lanes from the edges' capacities; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Prints all warnings, some of which are due to VISUM misbehaviour; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>The factor for flow to no. lanes conversion; <em>default: <strong>1800</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Imports all lane types; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Whether lane widths shall be ignored.; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>The geometry resolution in m when importing curved geometries as line segments.; <em>default: <strong>2</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Allow stop lines to be built beyond the start of the junction if the geometries allow so; <em>default: <strong>0</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>The minimum lane width for determining start or end of variable-width lanes; <em>default: <strong>1.8</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Import internal lane shapes; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td></td>
</tr>
</tbody>
</table>

## Report

<table>
<thead>
<tr class="header">
<th><p>Option</p></th>
<th><p>Description</p></th>
</tr>
</thead>
<tbody>
<tr class="odd">
<td><p><br />
</p></td>
<td><p>Switches to verbose output; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Prints option values before processing; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="odd">
<td><p><br />
</p></td>
<td><p>Prints this screen or selected topics; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td><p><br />
</p></td>
<td><p>Prints the current version; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="odd">
<td><p><br />
</p></td>
<td><p>Set schema validation scheme of XML inputs ("never", "auto" or "always"); <em>default: <strong>auto</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Set schema validation scheme of SUMO network inputs ("never", "auto" or "always"); <em>default: <strong>never</strong></em></p></td>
</tr>
<tr class="odd">
<td><p><br />
</p></td>
<td><p>Disables output of warnings; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td><p><br />
</p></td>
<td><p>Writes all messages to FILE (implies verbose)</p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Writes all non-error messages to FILE (implies verbose)</p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Writes all warnings and errors to FILE</p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Continue on broken input; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Continue on invalid connections; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Show errors in connections at parsing; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Continue on unknown edge types; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td></td>
</tr>
</tbody>
</table>

## Random Number

<table>
<thead>
<tr class="header">
<th><p>Option</p></th>
<th><p>Description</p></th>
</tr>
</thead>
<tbody>
<tr class="odd">
<td></td>
<td><p>Initialises the random number generator with the current system time; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Initialises the random number generator with the given value; <em>default: <strong>23423</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td></td>
</tr>
</tbody>
</table>

# Lefthand Networks

By default, NETCONVERT assumes networks to follow right-hand traffic
rules. When importing importing/building a network for a jurisdiction
wiht left-hand traffic, the option **--lefthand** must be set.

Note, that this also influences geometries if the edges which make up a
two-directional road are defined using the road-center-line as geometry.
In right-hand networks the lane shapes will be placed to the right of
the center line whereas in left-hand networks they will be placed to the
left. This means that for some edges, the same input geometries can be
used to build right-hand as well as left-hand networks. However, if the
edge geometries are defined using the center-line of the directional
edge (attribute \{\{XML|1=spreadType="center"/\>) then the geometries are
only useable for a specific type of jurisdiction (networks with the
wrong setting for **--lefthand** will look strange).

# Warnings during Import

Several types of warnings and errors with different levels of severity
may be issued during network building.

| Message                                                                                                   | Explanation                                                                                                                                      | Recommended Action                                                                                                                                                                                   |
| --------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------ | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| Error: Could not set connection ...                                                                       | A connection definition could not be applied because the lanes do not exist or do not share a common junction                                    | Check your input files                                                                                                                                                                               |
| Warning: Removed a road without junctions ...                                                             | Isolated road is removed from the network                                                                                                        |                                                                                                                                                                                                      |
| Warning: Not joining junctions ... because the cluster is too complex                                     | The heuristic for option  cannot handle a complex junction                                                                                       | Inspect the mentioned nodes and provide a [join-description](Networks/Building_Networks_from_own_XML-descriptions#Joining_Nodes.md) manually.                                                |
| Warning: Found angle of ... degrees                                                                       | An edge has an unusually sharp turn somewhere in the middle                                                                                      | Inspect the mentioned edge for correctness. Warning may be suppressed using option .                                                                                                                 |
| Warning: Warning: Found sharp turn with radius ...                                                        | An edge has an unusually sharp turn where entering or leaving an intersection                                                                    | Inspect the mentioned edge for correctness. Problems may be corrected automatically by setting option . Warning may be suppressed by setting option .                                                |
| Warning: Intersecting left turns at junction ...                                                          | The junction is very small so that left-turning vehicles from opposite directions obstruct each other                                            | Inspect the mentioned junction shape for correctness. Warning is not critical because the right-of-way model will be adapted to the conflict.                                                        |
| Warning: Lane '...' is not connected from any incoming edge at junction '...'.                            | The connection layout at the junction is suspicious                                                                                              | Inspect the mentioned junction's connections for correctness.                                                                                                                                        |
| Warning: Lane '...' is not connected to outgoing edges at junction '...'.                                 | The connection layout at the junction is suspicious                                                                                              | Inspect the mentioned junction's connections for correctness.                                                                                                                                        |
| Warning: Edge / Connection '...' has a grade of ...%.                                                     | There is a sharp jump in elevation in the edge or connection geometry                                                                            | [Inspect the z values for correctness](Networks/Elevation#Visualizing_Elevation_data.md)                                                                                                     |
| Warning: Speed of ... connection '...' reduced by ... due to turning radius of ... (length=... angle=...) | There is an unusually sharp turn relative to the road speed                                                                                      | Check edge speed limit and junction geometry for correctness. Warning may be suppressed by setting higher threshold via option .                                                                     |
| Warning: For node '...': could not compute shape.                                                         | The shape of the junction area could not be computed                                                                                             | Inspect the mentioned node and maybe [provide a custom node shape](Networks/Building_Networks_from_own_XML-descriptions#Node_Descriptions.md).                                               |
| Warning: Junction shape for '...' has distance ... to its given position.                                 | The shape and location of the junction area is suspicious.                                                                                       | Inspect the mentioned node and maybe [provide a custom node shape](Networks/Building_Networks_from_own_XML-descriptions#Node_Descriptions.md). Warning may be suppressed by setting option . |
| Warning: The traffic light '...' does not control any links; it will not be build.                        | The traffic light at the given node sits at a junction without connections or the node was removed altogether.                                   | Inspect the mentioned node if it still exists. Can usually be ignored.                                                                                                                               |
| Warning: Splitting vector close to end ...                                                                | A geometry computation somewhere deep in the code is suspicious. Often related to computation of internal lanes at intersections with bad shape. | Ignore for lack of sufficient information (or try to narrow down the problem by building smaller sections of the network).                                                                           |
| Warning: Splitting vector close to end ...                                                                | A geometry computation somewhere deep in the code is suspicious. Often related to computation of internal lanes at intersections with bad shape. | Ignore for lack of sufficient information (or try to narrow down the problem by building smaller sections of the network).                                                                           |
|                                                                                                           |                                                                                                                                                  |                                                                                                                                                                                                      |

-----

[Category:ApplicationDescription](Category:ApplicationDescription.md)