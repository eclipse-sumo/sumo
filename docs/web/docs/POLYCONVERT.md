---
title: POLYCONVERT
permalink: /POLYCONVERT/
---

# From 30.000 feet

**POLYCONVERT** imports geometrical shapes (polygons or points of
interest) from different sources, converts them to a representation that
may be visualized using [SUMO-GUI](SUMO-GUI.md).

  -
    **Purpose:** Polygon and POI import, conversion, and projection
    **System:** portable (Linux/Windows is tested); runs on command line
    **Input (mandatory):** polygons or pois
    **Output:** SUMO-shape file
    **Programming Language:** C++

## Features

  - Imports OSM, VISUM, Elmar, XML, ArcView shape files
  - per-type import settings
  - projections using a given proj.4-definition or via a matching
    network
  - Writes [simulation shape files](Simulation/Shapes.md) usable
    within [SUMO-GUI](SUMO-GUI.md) and [SUMO](SUMO.md)

# Usage Description

[POLYCONVERT](POLYCONVERT.md) is able to import shapes from
different file types. Normally, for importing data of a certain type,
the type name is used as option name and the value indicates the
position of the file. So

`polyconvert --visum mynet.net -o converted.poi.xml`

imports from a VISUM-net file.

## Options

You may use a XML schema definition file for setting up a POLYCONVERT
configuration:
[polyconvertConfiguration.xsd](http://sumo.dlr.de/xsd/polyconvertConfiguration.xsd).

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

### Input

[POLYCONVERT](POLYCONVERT.md) is capable to apply different
attributes to the imported shapes in dependence of their "type". Not all
imported formats have a type information. When using shape files, for
example, all instances of an artifact type are normally stored in a
distinct shape file.

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
<td><p>Loads SUMO-network FILE as reference to offset and projection</p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Reads polygons from FILE assuming they're coded in DLR-Navteq (Elmar)-format</p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Reads pois from FILE+ assuming they're coded in DLR-Navteq (Elmar)-format</p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Reads polygons from FILE assuming it's a Visum-net</p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Reads pois and shapes from FILE assuming they're coded in XML</p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Reads pois from FILE+ assuming they're coded in OSM</p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>The type will be made of the key-value - pair; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>The id will be set from the given 'name' attribute; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>If FLOAT &gt;= 0, assemble one polygon from all ways of a relation if they all connect with gaps below FLOAT; <em>default: <strong>-1</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Reads shapes from shapefiles FILE+</p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Guesses the shapefile's projection; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Defines in which column the id can be found</p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>A running number will be used as id; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Extract all additional columns as params; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>[auto|true|false]. Forces the 'fill' status to the given value. Default 'auto' tries to determine it from the data type</p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Reads types from FILE</p></td>
</tr>
<tr class="odd">
<td></td>
<td></td>
</tr>
</tbody>
</table>

### Output

All imported shapes that have not been discarded are written into a file
which has to be defined using .

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
<td><p>Write generated polygons/pois to FILE</p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Write generated polygons/pois to a dlr-tdp file with the given prefix</p></td>
</tr>
<tr class="even">
<td></td>
<td></td>
</tr>
</tbody>
</table>

### Projection

One of the major uses of [POLYCONVERT](POLYCONVERT.md) is to
apply a projection on the read shapes. Normally, one wants the shapes to
be aligned in accordance to a previously imported road network. In this
case, the network should be given using . But it is also possible to use
a different projection. In any case, if the read coordinates shall be
changed,  must be given.

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
<td><p>Write geo coordinates in output; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td></td>
</tr>
</tbody>
</table>

### Pruning

Sometimes, shapes cover a much larger area than the network. In order to
reduce the amount of data, one can force
[POLYCONVERT](POLYCONVERT.md) to prune the imported data on the
network's or a given boundary. Read shapes which are completely outside
this boundary are discarded in these cases.

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
<td><p>Enables pruning on net boundaries; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Uses STR as offset definition added to the net boundaries; <em>default: <strong>0,0,0,0</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Uses STR as pruning boundary</p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Items in STR will be kept though out of boundary</p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Items with names in STR will be removed</p></td>
</tr>
<tr class="even">
<td></td>
<td></td>
</tr>
</tbody>
</table>

### Processing

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
<td><p>Adds FLOAT to net x-positions; <em>default: <strong>0</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Adds FLOAT to net y-positions; <em>default: <strong>0</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Imports all attributes as key/value pairs; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Continue on broken input; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Adds FLOAT to the layer value for each poi (i.e. to raise it above polygons); <em>default: <strong>0</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td></td>
</tr>
</tbody>
</table>

### Building Defaults

When importing shapes for which no type-dependent attributes have been
given, the following default values are used which can be changed on the
command line.

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
<td><p>Sets STR as default color; <em>default: <strong>0.2,0.5,1.</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Sets STR as default prefix</p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Sets STR as default type; <em>default: <strong>unknown</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Fills polygons by default; <em>default: <strong>true</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Sets FLOAT as default layer; <em>default: <strong>-1</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Sets default action to discard; <em>default: <strong>false</strong></em></p></td>
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
<td><p>Prints this screen; <em>default: <strong>false</strong></em></p></td>
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

# See Also

  - [Using additional polygons and POIs within the
    Simulation](Simulation/Shapes.md)
  - [Importing shapes from
    OpenStreetMap](Networks/Import/OpenStreetMap#Importing_additional_Polygons_.28Buildings.2C_Water.2C_etc..29.md)
  - [Using configuration
    files](Basics/Using_the_Command_Line_Applications#Configuration_Files.md)

-----

[Category:ApplicationDescription](Category:ApplicationDescription.md)