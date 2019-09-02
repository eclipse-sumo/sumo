---
title: MAROUTER
permalink: /MAROUTER/
---

# From 30.000 feet

**MAROUTER** computes a macroscopic [user
assignment](Demand/Dynamic_User_Assignment#Introduction.md)
(routes) to be loaded into [SUMO](SUMO.md) from various inputs
(Origin-Destination-Matrices, trip files or route files).

  -
    **Purpose:** Macroscopic route assignment
    **System:** portable (Linux/Windows is tested); runs on command line
      - **Input (mandatory):**
        A) a road network as generated via
        [NETCONVERT](NETCONVERT.md) or
        [NETGENERATE](NETGENERATE.md), see [Building
        Networks](SUMO_User_Documentation#Network_Building.md)
        B) O/D matrix
    **Output:** [Definition of Vehicles, Vehicle Types, and
    Routes](Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md)
    usable by [SUMO](SUMO.md)
    **Programming Language:** C++

# Usage Description

MAROUTER computes a user assignment (routes) with a configurable
assignment method (using option ):

  - **incremental**
  - **UE**: user equilibrium
  - **SUE**: stochastic user equilibrium

MAROUTER uses a hard-coded capacity-constraint function based on speed
limit, lane number and edge priority to compute traveltimes and flows
based on density. For details, see functions
*capacityConstraintFunction* and *getCapacity* in .

MAROUTER reads OD matrices and creates route files. These route files
contain route distributions between traffic assignment zones. Each route
in the distribution is a fully specified list of network edges.

## Microscopic Outputs

One of the main usages of MAROUTER is to compute a
[Demand/Dynamic_User_Assignment](Demand/Dynamic_User_Assignment.md).
To this end it generates  definitions with fully specified routes that
can be loaded into [SUMO](SUMO.md) and
[SUMO-GUI](SUMO-GUI.md).

## Macroscopic Outputs

MAROUTER can create macroscopic traffic data for all network edges by
using the option . This data contains *travelTime, speed*, *entered*
(the number of vehicles that entered the edge) and *flowCapacityRatio*
(the proportion of total capacity used). These values can either be
written aggregated for the whole day (default) or split into intervals
according to the time-granularity of the input data ().

This output can [visualized in
SUMO-GUI](SUMO-GUI#Visualizing_edge-related_data.md) to
understand how the different traffic measures change over time.

## Options

You may use a XML schema definition file for setting up a MAROUTER
configuration:
[marouterConfiguration.xsd](http://sumo.dlr.de/xsd/marouterConfiguration.xsd).

### Configuration

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
<td><p>Read additional network data (districts, bus stops) from FILE</p></td>
</tr>
<tr class="odd">
<td><p><br />
</p></td>
<td><p>Loads O/D-files from FILE(s)</p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Loads O/D-matrix in Amitran format from FILE(s)</p></td>
</tr>
<tr class="odd">
<td><p><br />
</p></td>
<td><p>Read sumo-routes or trips from FILE(s)</p></td>
</tr>
<tr class="even">
<td><p><br />
</p></td>
<td><p>Read network weights from FILE(s)</p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Read lane-based network weights from FILE(s)</p></td>
</tr>
<tr class="even">
<td><p><br />
</p></td>
<td><p>Name of the xml attribute which gives the edge weight; <em>default: <strong>traveltime</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>The travel time influence of prior intervals; <em>default: <strong>0</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Parameter key(s) defining source (and sink) taz</p></td>
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
<td><p>Write flow definitions with route distributions to FILE</p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Write used vehicle types into separate FILE</p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Does not save vtype information; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Writes edge loads and final costs into FILE</p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Writes complete distance matrix into FILE</p></td>
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
<td><p>Defines the time interval when aggregating single vehicle input; Defaults to one hour; <em>default: <strong>3600</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Prune the number of alternatives to INT; <em>default: <strong>5</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Interpolate edge weights at interval boundaries; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Expand weights behind the simulation's end; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Select among routing algorithms ['dijkstra', 'astar', 'CH', 'CHWrapper']; <em>default: <strong>dijkstra</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Aggregate routing queries with the same origin for different vehicle types; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>The number of parallel execution threads used for routing; <em>default: <strong>0</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Aggregation period for the given weight files; triggers rebuilding of Contraction Hierarchy; <em>default: <strong>3600</strong></em></p></td>
</tr>
<tr class="odd">
<td><p><br />
</p></td>
<td><p>Scales the loaded flows by FLOAT; <em>default: <strong>1</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Defines the name of the vehicle type to use</p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Defines the prefix for vehicle flow names</p></td>
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
<td><p>Keep traffic flows of all time slots in the net; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Choose a assignment method: incremental, UE or SUE; <em>default: <strong>incremental</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Use FLOAT as tolerance when checking for SUE stability; <em>default: <strong>0.001</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Use left-turn penalty FLOAT to calculate link travel time when searching routes; <em>default: <strong>0</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Use INTEGER as the number of paths needed to be searched for each OD pair at each iteration; <em>default: <strong>1</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Penalize existing routes with FLOAT to find secondary routes; <em>default: <strong>1</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Use FLOAT as the upper bound to determine auxiliary link cost; <em>default: <strong>0.5</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Use FLOAT as the lower bound to determine auxiliary link cost; <em>default: <strong>0.15</strong></em></p></td>
</tr>
<tr class="even">
<td><p><br />
</p></td>
<td><p>maximal number of iterations for new route searching in incremental and stochastic user assignment; <em>default: <strong>20</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>maximal number of inner iterations for user equilibrium calcuation in the stochastic user assignment; <em>default: <strong>1000</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Choose a route choice method: gawron, logit, or lohse; <em>default: <strong>logit</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Use FLOAT as Gawron's beta; <em>default: <strong>0.3</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Use FLOAT as Gawron's a; <em>default: <strong>0.05</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Save routes with near zero probability; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Only reuse routes from input, do not calculate new ones; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Use FLOAT as (c-)logit's beta for the commonality factor; <em>default: <strong>0.15</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Use FLOAT as (c-)logit's gamma for the commonality factor; <em>default: <strong>1</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Use FLOAT as (c-)logit's theta; <em>default: <strong>0.01</strong></em></p></td>
</tr>
<tr class="even">
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
<td><p>Continue if a route could not be build; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td></td>
</tr>
</tbody>
</table>

### Random Number

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