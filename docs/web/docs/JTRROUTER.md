---
title: JTRROUTER
permalink: /JTRROUTER/
---

# From 30.000 feet

**JTRROUTER** computes routes that may be used by
[SUMO](SUMO.md) based on traffic volumes and junction turning
ratios.

  -
    **Purpose:** Building vehicle routes from demand definitions using
    junction turning percentages
    **System:** portable (Linux/Windows is tested); runs on command line
      - **Input (mandatory):**
        A) a road network as generated via
        [NETCONVERT](NETCONVERT.md) or
        [NETGENERATE](NETGENERATE.md), see [Building
        Networks](SUMO_User_Documentation#Network_Building.md)
        B) a demand definition, see [Demand
        Modelling](SUMO_User_Documentation#Demand_Modelling.md)
        C) Junction turning definitions
    **Output:** [Definition of Vehicles, Vehicle Types, and
    Routes](Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md)
    usable by [SUMO](SUMO.md)
    **Programming Language:** C++

# Usage Description

JTRROUTER is made for [routing based on
turn-ratios](Demand/Routing_by_Turn_Probabilities.md).

## Options

You may use a XML schema definition file for setting up a JTRROUTER
configuration:
[jtrrouterConfiguration.xsd](http://sumo.dlr.de/xsd/jtrrouterConfiguration.xsd).

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
<td><p>Use FILE as SUMO-network to route on</p></td>
</tr>
<tr class="even">
<td><p><br />
</p></td>
<td><p>Read additional network data (districts, bus stops) from FILE(s)</p></td>
</tr>
<tr class="odd">
<td><p><br />
</p></td>
<td><p>Read sumo routes, alternatives, flows, and trips from FILE(s)</p></td>
</tr>
<tr class="even">
<td><p><br />
</p></td>
<td><p>Read turning ratios from FILE(s)</p></td>
</tr>
<tr class="odd">
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
<td><p>Write generated routes to FILE</p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Write used vehicle types into separate FILE</p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Keep vTypeDistribution ids when writing vehicles and their types; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Write exit times (weights) for each edge; <em>default: <strong>false</strong></em></p></td>
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
<td><p>Assume input is unsorted; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td><p><br />
</p></td>
<td><p>Load routes for the next number of seconds ahead; <em>default: <strong>200</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Disable (junction) internal links; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>generate random departure times for flow input; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Prune the number of alternatives to INT; <em>default: <strong>5</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Remove loops within the route; Remove turnarounds at start and end of the route; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Tries to correct a false route; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Tries to correct an invalid starting edge by using the first usable edge instead; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Tries to correct an invalid destination edge by using the last usable edge instead; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Interpolate edge weights at interval boundaries; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Apply the given time penalty when computing routing costs for minor-link internal lanes; <em>default: <strong>1.5</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Use origin and destination zones (districts) for in- and output; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Aggregate routing queries with the same origin; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>The number of parallel execution threads used for routing; <em>default: <strong>0</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Routes are cut off when the route edges to net edges ratio is larger than FLOAT; <em>default: <strong>2</strong></em></p></td>
</tr>
<tr class="even">
<td><p><br />
</p></td>
<td><p>Use STR as default turn definition; <em>default: <strong>30,50,20</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Use STR as list of sink edges</p></td>
</tr>
<tr class="even">
<td><p><br />
</p></td>
<td><p>Whether all edges are allowed as sink edges; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="odd">
<td><p><br />
</p></td>
<td><p>Ignore road restrictions based on vehicle class; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Allow to re-use a road; <em>default: <strong>false</strong></em></p></td>
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
<td><p>Assigns a default depart lane</p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Assigns a default depart position</p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Assigns a default depart speed</p></td>
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
<td><p>Defaults will override given values; <em>default: <strong>false</strong></em></p></td>
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
<td><p>Continue if a route could not be build; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Defines how often statistics shall be printed; <em>default: <strong>-1</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Disable console output of route parsing step; <em>default: <strong>false</strong></em></p></td>
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

-----

[Category:ApplicationDescription](Category:ApplicationDescription.md)