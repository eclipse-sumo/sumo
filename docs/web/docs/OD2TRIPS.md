---
title: OD2TRIPS
permalink: /OD2TRIPS/
---

# From 30.000 feet

**OD2TRIPS** imports O/D-matrices and splits them into single vehicle
trips.

  -
    **Purpose:** Conversion of O/D-matrices to single vehicle trips
    **System:** portable (Linux/Windows is tested); runs on command line
      - **Input (mandatory):**
        A) O/D-Matrix
        B) a set of districts
    **Output:** A list of vehicle trip definitions
    **Programming Language:** C++

# Usage Description

OD2TRIPS maps traffic that is defined via origin and destination zones
onto the edges of a network. For details, see
[Demand/Importing_O/D_Matrices](Demand/Importing_O/D_Matrices.md).

## Options

You may use a XML schema definition file for setting up a OD2TRIPS
configuration:
[od2tripsConfiguration.xsd](http://sumo.dlr.de/xsd/od2tripsConfiguration.xsd).

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
<td><p>Loads TAZ (districts; also from networks) from FILE(s)</p></td>
</tr>
<tr class="even">
<td><p><br />
</p></td>
<td><p>Loads O/D-files from FILE(s)</p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Loads O/D-matrix in Amitran format from FILE(s)</p></td>
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
<td><p><br />
</p></td>
<td><p>Writes trip definitions into FILE</p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Writes flow definitions into FILE</p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Writes probabilistic flow instead of evenly spaced flow; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Writes pedestrians instead of vehicles; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Writes persontrips instead of vehicles; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Does not save vtype information; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td></td>
</tr>
</tbody>
</table>

### Time

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
<td><p>Defines the begin time; Previous trips will be discarded; <em>default: <strong>0</strong></em></p></td>
</tr>
<tr class="even">
<td><p><br />
</p></td>
<td><p>Defines the end time; Later trips will be discarded; Defaults to the maximum time that SUMO can represent; <em>default: <strong>9223372036854774</strong></em></p></td>
</tr>
<tr class="odd">
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
<td><p><br />
</p></td>
<td><p>Scales the loaded flows by FLOAT; <em>default: <strong>1</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Spreads trips uniformly over each time period; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Always choose source and sink edge which are not identical; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Defines the name of the vehicle type to use</p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Defines the prefix for vehicle names</p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Uses STR as a timeline definition</p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Uses STR as a 24h-timeline definition; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Disable console output of current time step; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td></td>
</tr>
</tbody>
</table>

### Defaults

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
<td><p>Assigns a default depart lane; <em>default: <strong>free</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Assigns a default depart position</p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Assigns a default depart speed; <em>default: <strong>max</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Assigns a default arrival lane</p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Assigns a default arrival position</p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Assigns a default arrival speed</p></td>
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
<td><p>Continue on broken input; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
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

# Notes

The option --vtype only works when "--od-matrix-files" is used. For
"Amitran" format, this is given by the "id" property of "actorConfig".

-----

[Category:ApplicationDescription](Category:ApplicationDescription.md)