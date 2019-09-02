---
title: NETGENERATE
permalink: /NETGENERATE/
---

# From 30.000 feet

**NETGENERATE** generates abstract road networks that may be used by
other SUMO-applications.

  -
    **Purpose:** Abstract road network generation
    **System:** portable (Linux/Windows is tested); runs on command line
    **Input (mandatory):** Command line parameter
    **Output:** A generated SUMO-road network; optionally also other
    outputs
    **Programming Language:** C++

# Usage Description

The usage is described at
[Networks/Abstract_Network_Generation](Networks/Abstract_Network_Generation.md)

## Options

You may use a XML schema definition file for setting up a NETGENERATE
configuration:
[netgenerateConfiguration.xsd](http://sumo.dlr.de/xsd/netgenerateConfiguration.xsd).

### Configuration

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

### Grid Network

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
<td><p>Forces NETGEN to build a grid-like network; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>The number of junctions in both dirs; <em>default: <strong>5</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>The length of streets in both dirs; <em>default: <strong>100</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>The number of junctions in x-dir; Overrides --grid-number; <em>default: <strong>5</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>The number of junctions in y-dir; Overrides --grid-number; <em>default: <strong>5</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>The length of horizontal streets; Overrides --grid-length; <em>default: <strong>100</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>The length of vertical streets; Overrides --grid-length; <em>default: <strong>100</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>The length of streets attached at the boundary; 0 means no streets are attached; <em>default: <strong>0</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td></td>
</tr>
</tbody>
</table>

### Spider Network

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
<td><p>Forces NETGEN to build a spider-net-like network; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>The number of axes within the net; <em>default: <strong>13</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>The number of circles of the net; <em>default: <strong>20</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>The distances between the circles; <em>default: <strong>100</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Omit the central node of the network; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td></td>
</tr>
</tbody>
</table>

### Random Network

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
<td><p>Forces NETGEN to build a random network; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Describes how many times an edge shall be added to the net; <em>default: <strong>2000</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Defines the probability to build a reverse edge; <em>default: <strong>1</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>The maximum distance for each edge; <em>default: <strong>250</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>The minimum distance for each edge; <em>default: <strong>100</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>The minimum angle for each pair of (bidirectional) roads; <em>default: <strong>0.785398</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>The number of tries for creating each node; <em>default: <strong>50</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Probability for roads to continue at each node; <em>default: <strong>0.95</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Probability for a node having exactly 1 neighbor; <em>default: <strong>0</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Probability for a node having exactly 2 neighbors; <em>default: <strong>0</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Probability for a node having exactly 3 neighbors; <em>default: <strong>10</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Probability for a node having exactly 4 neighbors; <em>default: <strong>10</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Probability for a node having exactly 5 neighbors; <em>default: <strong>2</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Probability for a node having exactly 6 neighbors; <em>default: <strong>1</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Draw lane numbers randomly from [1,default.lanenumber]; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Draw edge priority randomly from [1,default.priority]; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Place nodes on a regular grid with spacing rand.min-distance; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td></td>
</tr>
</tbody>
</table>

### Output

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
<td></td>
<td><p>The Ids of generated nodes use an alphanumerical code for easier readability when possible; <em>default: <strong>true</strong></em></p></td>
</tr>
<tr class="odd">
<td><p><br />
</p></td>
<td><p>The generated net will be written to FILE</p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Prefix of files to write plain xml nodes, edges and connections to</p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Writes information about joined junctions to FILE (can be loaded as additional node-file to reproduce joins</p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Defines a prefix for edge and junction names</p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>The generated net will be written to FILE using Amitran format</p></td>
</tr>
<tr class="even">
<td></td>
<td><p>The generated net will be written to FILE using MATsim format</p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>The generated net will be written to FILE using OpenDRIVE format</p></td>
</tr>
<tr class="even">
<td></td>
<td><p>The generated net will be written to dlr-navteq files with the given PREFIX</p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>The network coordinates are written with the specified level of output precision; <em>default: <strong>2</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Street names will be included in the output (if available); <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Writes original names, if given, as parameter; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Writes street signs as POIs to FILE</p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Builds parameterized curves whenever the angular change between straight segments exceeds FLOAT degrees; <em>default: <strong>1e-08</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td></td>
</tr>
</tbody>
</table>

### Processing

Normally, both [NETCONVERT](NETCONVERT.md) and
[NETGENERATE](NETGENERATE.md) translate the read network so that
the left- and down-most node are at coordinate (0,0). The options
--offset.x and --offset.y allow to disable this and to apply different
offsets for both the x- and the y-axis. If there are explicit offsets
given, the normalization is disabled automatically (thus there is no
need to give --offset.disable-normalization if there is at least one of
the offsets given).

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
<td><p>Generate INT left-turn lanes; <em>default: <strong>0</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Set the length of generated turning lanes to FLOAT; <em>default: <strong>20</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Apply random spatial pertubation in x direction according the the given distribution; <em>default: <strong>0</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Apply random spatial pertubation in y direction according the the given distribution; <em>default: <strong>0</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Apply random spatial pertubation in z direction according the the given distribution; <em>default: <strong>0</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Remaps alphanumerical IDs of nodes and edges to ensure that all IDs are integers; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Ensures that generated ids do not included any of the typed IDs from FILE (SUMO-GUI selection file format)</p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Warn about edge geometries with a grade in % above FLOAT.; <em>default: <strong>10</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Smooth edge edge geometries with a grade in above the warning threshold.; <em>default: <strong>true</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Turn off normalizing node positions; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Adds FLOAT to net x-positions; <em>default: <strong>0</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Adds FLOAT to net y-positions; <em>default: <strong>0</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Flips the y-coordinate along zero; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Enable roundabout-guessing; <em>default: <strong>true</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Enable guessing of opposite direction lanes usable for overtaking; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Ensure that opposite edges have the same length; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Assumes left-hand traffic on the network; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Merges edges which connect the same nodes and are close to each other (recommended for VISSIM import); <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td></td>
</tr>
</tbody>
</table>

### Building Defaults

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
<td><p><br />
</p></td>
<td><p>[traffic_light|priority|right_before_left|traffic_light_right_on_red|priority_stop|allway_stop|...] Determines junction type (see wiki/Networks/PlainXML#Node_types)</p></td>
</tr>
<tr class="even">
<td></td>
<td></td>
</tr>
</tbody>
</table>

### Tls Building

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
<td><p>Includes node clusters into guess; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Tries to cluster tls-controlled nodes; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Determines the maximal distance for joining traffic lights (defaults to 20); <em>default: <strong>20</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Do not control edges that lie fully within a joined traffic light. This may cause collisions but allows old traffic light plans to be used; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Use INT as cycle duration; <em>default: <strong>90</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Use INT as green phase duration; <em>default: <strong>31</strong></em></p></td>
</tr>
<tr class="odd">
<td><p><br />
</p></td>
<td><p>Defines smallest vehicle deceleration; <em>default: <strong>3</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Given yellow times are patched even if being too short; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Set INT as fixed time for yellow phase durations; <em>default: <strong>-1</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Set INT as fixed time for red phase duration at traffic lights that do not have a conflicting flow; <em>default: <strong>5</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Set INT as fixed time for intermediate red phase after every switch; <em>default: <strong>0</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Use FLOAT as threshold for allowing left-turning vehicles to move in the same phase as oncoming straight-going vehicles; <em>default: <strong>19.44</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Use INT as green phase duration for left turns (s). Setting this value to 0 disables additional left-turning phases; <em>default: <strong>6</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Use INT as minimum green duration for pedestrian crossings (s).; <em>default: <strong>4</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Use INT as clearance time for pedestrian crossings (s).; <em>default: <strong>5</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Use INT as green phase duration for pedestrian scramble phase (s).; <em>default: <strong>5</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>TLSs in STR will be shifted by half-phase</p></td>
</tr>
<tr class="even">
<td></td>
<td><p>TLSs in STR will be shifted by quarter-phase</p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>TLSs with unspecified type will use STR as their algorithm; <em>default: <strong>static</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Set phase layout four grouping opposite directions or grouping all movements for one incoming edge ['opposites', 'incoming']; <em>default: <strong>opposites</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Default minimum phase duration for traffic lights with variable phase length; <em>default: <strong>5</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Default maximum phase duration for traffic lights with variable phase length; <em>default: <strong>50</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td></td>
</tr>
</tbody>
</table>

### Edge Removal

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
<td><p>Only keep edges which are located within the given boundary (given either as CARTESIAN corner coordinates &lt;xmin,ymin,xmax,ymax&gt; or as polygon &lt;x0,y0,x1,y1,...&gt;)</p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Only keep edges which are located within the given boundary (given either as GEODETIC corner coordinates &lt;lon-min,lat-min,lon-max,lat-max&gt; or as polygon &lt;lon0,lat0,lon1,lat1,...&gt;)</p></td>
</tr>
<tr class="even">
<td></td>
<td></td>
</tr>
</tbody>
</table>

### Unregulated Nodes

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

### Junctions

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
<td><p>Generate INT intermediate points to smooth out intersection corners; <em>default: <strong>5</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Generate INT intermediate points to smooth out lanes within the intersection; <em>default: <strong>5</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Generate longer intersections to allow for smooth s-curves when the number of lanes changes; <em>default: <strong>0</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Builds common edges for turning connections with common from- and to-edge. This causes discrepancies between geometrical length and assigned length due to averaging but enables lane-changing while turning; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Limits speed on junctions to an average lateral acceleration of at most FLOAT m/s^2); <em>default: <strong>5.5</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Do not limit turn speed for angular changes below FLOAT (degrees). The value is subtracted from the geometric angle before computing the turning radius.; <em>default: <strong>15</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Do not limit turn speed for angular changes below FLOAT (degrees) on railway edges. The value is subtracted from the geometric angle before computing the turning radius.; <em>default: <strong>35</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Warn about turn speed limits that reduce the speed of straight connections by more than FLOAT; <em>default: <strong>5</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Warn about turn speed limits that reduce the speed of turning connections (no u-turns) by more than FLOAT; <em>default: <strong>22</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Default radius for junctions that do not require wide vehicle turns; <em>default: <strong>1.5</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Forces rectangular cuts between lanes and intersections; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Allow driving onto a multi-lane road if there are foes on other lanes (at roundabouts); <em>default: <strong>true</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Allow driving onto a multi-lane road if there are foes on other lanes (everywhere); <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td></td>
</tr>
</tbody>
</table>

### Pedestrian

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

### Report

All applications of the **SUMO**-suite handle most of the reporting
options the same way. These options are discussed at [Basics/Using the
Command Line Applications\#Reporting
Options](Basics/Using_the_Command_Line_Applications#Reporting_Options.md).

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
<td></td>
</tr>
</tbody>
</table>

### Random Number

All applications of the **SUMO**-suite handle randomisation options the
same way. These options are discussed at [Basics/Using the Command Line
Applications\#Random Number
Options](Basics/Using_the_Command_Line_Applications#Random_Number_Options.md).

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

-----

[Category:ApplicationDescription](Category:ApplicationDescription.md)