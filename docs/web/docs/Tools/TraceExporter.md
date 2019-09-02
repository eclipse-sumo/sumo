---
title: Tools/TraceExporter
permalink: /Tools/TraceExporter/
---

# From 30.000 feet

*traceExporter.py* converts and filters SUMO [fcd
output](Simulation/Output/FCDOutput.md) to different "trace
file" formats: OMNET, Shawn, ns2/ns3, PHEM.

  -
    **Purpose:** trace file conversion/generation
    **System:** portable (Linux/Windows is tested); runs on command line
    **Input (mandatory):** SUMO fcd-output
    **Output:** One or more "trace file(s)" and other complementary
    files
    **Programming Language:** Python

# Introduction

Different applications read "vehicular traces", files containing mainly
positions of vehicles over time.
[traceExporter.py](Tools/TraceExporter.md) generates such files
by converting SUMO's [fcd
output](Simulation/Output/FCDOutput.md) into several supported
formats. Following files can be generated, sorted by application:

  - OMNET: mobility-traces (.xml)
  - Shawn: snapshot-files (.xml)
  - ns2/ns3: trace-files, activity-files, and mobility-files (all
    \*.tcl)
  - PHEM: .dri-files, .str-files, .fzp-files, .flt-files
  - GPSDAT csv traces (id,date,x,y,status,speedKMH)
  - [GPX](http://en.wikipedia.org/wiki/GPS_eXchange_Format)
  - [POIs](Simulation/Shapes#POI_.28Point_of_interest.29_Definitions.md)
    (can visualize GPS distortions and sampling in
    [SUMO-GUI](SUMO-GUI.md))
  - [fcd output](Simulation/Output/FCDOutput.md) to restrict the
    file to certain regions or time intervals

# Installation

[traceExporter.py](Tools/TraceExporter.md) comes with SUMO and
is located in /tools/. Python must be installed.

# Usage Description

You convert a given [fcd output](Simulation/Output/FCDOutput.md)
into a format using a command like this:

`traceExporter.py --fcd-input myFCDoutput.xml --omnet-output myOMNETfile.xml`

In the case given above, a file for [OMNET](http://www.omnetpp.org/)
will be built.

In the following, the command line options are described.

## Input Options

[traceExporter.py](Tools/TraceExporter.md) often needs only the
[fcd output](Simulation/Output/FCDOutput.md) as input. In some
cases, the network used while obtaining the [fcd
output](Simulation/Output/FCDOutput.md) must be given.
[traceExporter.py](Tools/TraceExporter.md) gives an error
message if the wanted output needs additional files, such as the
network, but no such file is specified. The options for defining the
input files are:

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
<td><p>The fcd-output - file to convert (numeric value is interpreted as port to listen on)</p></td>
</tr>
<tr class="even">
<td><p><br />
</p></td>
<td><p>The network file used; Mainly used for exporting the network</p></td>
</tr>
<tr class="odd">
<td></td>
<td></td>
</tr>
</tbody>
</table>

All outputs which need to read the input only once support reading from
a socket. To do so just give a number as file name and the script will
open a port to listen on. Afterwards you can start sumo with the
corresponding port as output, e.g. traceExporter.py --fcd-input 43214
... needs to be followed by sumo --fcd-output localhost:43214. This
should work at least with OMNET, Shawn, GPSDAT, GPX, POIs and fcd
filtering.

## Output Options

In the following, the output options of
[traceExporter.py](Tools/TraceExporter.md) are given, divided by
the application,

### PHEM

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
<td><p>Generates a PHEM .dri-file named </p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Generates a PHEM .str-file named ; note that the SUMO network used during the simulation must be given using the  option</p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Generates a PHEM .fzp-file named </p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Generates a PHEM .flt-file named </p></td>
</tr>
<tr class="odd">
<td></td>
<td></td>
</tr>
</tbody>
</table>

### OMNET

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
<td><p>Generates a OMNET mobility trace named </p></td>
</tr>
<tr class="even">
<td></td>
<td></td>
</tr>
</tbody>
</table>

### Shawn

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
<td><p>Generates a Shawn snaphot file named </p></td>
</tr>
<tr class="even">
<td></td>
<td></td>
</tr>
</tbody>
</table>

### ns2/ns3

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
<td><p>Generates a ns2 configuration file named </p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Generates a ns2 activity file named </p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Generates a ns2 mobility file named </p></td>
</tr>
<tr class="even">
<td></td>
<td></td>
</tr>
</tbody>
</table>

### GPSDAT

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
<td><p>Generates a GPSDAT file named </p></td>
</tr>
<tr class="even">
<td></td>
<td></td>
</tr>
</tbody>
</table>

### GPX

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
<td><p>Generates a GPX file named </p></td>
</tr>
<tr class="even">
<td></td>
<td></td>
</tr>
</tbody>
</table>

### PoI

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
<td><p>Generates a SUMO PoI file named </p></td>
</tr>
<tr class="even">
<td></td>
<td></td>
</tr>
</tbody>
</table>

### fcd

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
<td><p>Reads filter settings from </p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Extra comments to include in fcd file</p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Modify the vehicle type of all written vehicles to </p></td>
</tr>
<tr class="even">
<td></td>
<td></td>
</tr>
</tbody>
</table>

The filter file is a simple csv format giving
begin;end;bounding_box;outfile where the bounding box is x1,y1,x2,y2
defining the lower left and the upper right corner of the area to
extract. The coordinate format needs to fit the input format (so if the
input has geo coordinates the bounding box needs to have geo coordinates
as well). It is currently not possible to give the same output file for
different filter lines.

Example filter file:

`0;50;500,400,600,500;out.xml`
`50;100;550,450,600,500;out2.xml`

## Processing Options

Several options allow to fine-tune the processing.

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
<td><p>Whether persons should be included in the trace; <em>default: False</em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>A float between 0 and 1, defining the probability of a vehicle to be included in the output; <em>default: 1</em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>A float defining a random offset applied on the vehicle positions before exporting them; <em>default: 0</em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Defines the bounding box as 'xmin,ymin,xmax,ymax'</p></td>
</tr>
<tr class="odd">
<td><p><br />
</p></td>
<td><p>Defines the time sampling, if given; only every  time step will be exported.</p></td>
</tr>
<tr class="even">
<td><p><br />
</p></td>
<td><p>Time steps before  are not exported.</p></td>
</tr>
<tr class="odd">
<td><p><br />
</p></td>
<td><p>Time steps after  are not exported. Please note that some issues are known, here.</p></td>
</tr>
<tr class="even">
<td><p><br />
</p></td>
<td><p>The randomizer seed to use.</p></td>
</tr>
<tr class="odd">
<td><p><br />
</p></td>
<td><p>The base date in seconds since the epoch (1970-01-01 00:00:00), the default is the current local time.</p></td>
</tr>
<tr class="even">
<td></td>
<td></td>
</tr>
</tbody>
</table>

## Additional Options

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
<td><p>Shows a help screen.</p></td>
</tr>
<tr class="even">
<td></td>
<td></td>
</tr>
</tbody>
</table>

# See Also

  - This [tutorial on using
    TraceExporter](Tutorials/Trace_File_Generation.md)
  - [working around bugs in version
    0.19.0](FAQ#traceExporter.py_fails_to_work_properly_in_verson_0.19.0.md)