---
title: Tools Emissions
permalink: /Tools/Emissions/
---

SUMO includes some small-sized tools and tests that help to develop and
evalue the implemented [emission
models](Topics/Environmental_Issues#Models.md). These tools are
presented in the following.

# emissionsMap

This tool generates matrices of emissions for a given range of
velocities, accelerations, and slopes, given the vehicle's emission
class mainly.

<table>
<tbody>
<tr class="odd">
<td><figure>
<img src="P_7_6_CO2.png" title="P_7_6_CO2.png" width="300" alt="" /><figcaption>P_7_6_CO2.png</figcaption>
</figure></td>
<td><p>The tool obtains parameter that describe the ranges and the emission classes to generate. If the option  is set, emissionsMap iterates over all available emission classes. In this case, the option  should denote a folder the emission maps shall be written into. If  is not set,  should name the file to generate and the emission class to write the map for must be defined using the option . If PHEMlight shall be used, one has to define the path to the CEP-files using the option .</p>
<p>The tool is located in /bin.</p>
<p>The image shows the visualisation of CO<sub>2</sub> emission of the <a href="Models/Emissions/HBEFA-based" title="wikilink">HBEFA v2.1-based</a> "P_7_6" emission class.</p></td>
</tr>
<tr class="even">
<td></td>
<td></td>
</tr>
</tbody>
</table>

## Options

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
<td><p>If set, maps for all available emissions are written</p></td>
</tr>
<tr class="even">
<td><p><br />
</p></td>
<td><p>Defines the name of the emission class to generate the map for</p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Defines the minimum velocity boundary of the map to generate (in [m/s]); <em>default: <strong>0</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Defines the maximum velocity boundary of the map to generate (in [m/s]); <em>default: <strong>50</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Defines the velocity step size (in [m/s]); <em>default: <strong>2</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Defines the minimum acceleration boundary of the map to generate (in [m/s^2]); <em>default: <strong>-4</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Defines the maximum acceleration boundary of the map to generate (in [m/s^2]); <em>default: <strong>4</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Defines the acceleration step size (in [m/s^2]); <em>default: <strong>.5</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Defines the minimum slope boundary of the map to generate (in [°]); <em>default: <strong>-10</strong></em></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Defines the maximum slope boundary of the map to generate (in [°]); <em>default: <strong>10</strong></em></p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Defines the slope step size (in [°]); <em>default: <strong>1</strong></em></p></td>
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
<td><p><br />
</p></td>
<td><p>Defines the file (or the path if  was set) to write the map(s) into</p></td>
</tr>
<tr class="even">
<td></td>
<td></td>
</tr>
</tbody>
</table>

The format of the output is a CSV-like with **;** as the column
separator:

<speed>`;`<acceleration>`;`<slope>`;`<Pollutant-Type>`;`<Polutant-Value-in-mg/s-or-ml/s>

### Emissions

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
<td><p>Determines where to load <a href="Models/Emissions/PHEMlight" title="wikilink">PHEMlight</a> definitions from.</p></td>
</tr>
<tr class="even">
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
<td><p><br />
</p></td>
<td><p>Prints the help screen</p></td>
</tr>
<tr class="odd">
<td></td>
<td></td>
</tr>
</tbody>
</table>

# emissionsDrivingCycle

This tools computes emissions given a timeline of speeds/accelerations
for a single vehicle. The driving cycle definition must be given using
the option . It must be a ';'-separated .csv-file with the following
columns:

  - time (in \[s\]); must start at zero and be increasing in steps of 1
    s
  - velocity (in \[m/s\] unless  is set, see below)
  - acceleration (in \[m/s^2\]); this column may be omitted or
    recalculated using the option
  - optional: slope (in \[°\]); if given, one has to enable it using the
    option

The tool generates a ';'-separated .csv file which contains the
following columns:

  - time (in \[s\])
  - velocity (in \[m/s\])
  - acceleration (in \[m/s^2\])
  - slope (in \[°\])
  - CO emissions (in \[mg/s\])
  - CO2 emissions (in \[mg/s\])
  - HC emissions (in \[mg/s\])
  - PMx emissions (in \[mg/s\])
  - NOx emissions (in \[mg/s\])
  - fuel consumption (in \[ml/s\])
  - electricity consumption (in \[Wh/s\])

Additionally, the sums of the generated emissions / the consumed fuel
are printed on the screen.

The tool is located in /bin.

## Options

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
<td><p>Defines the file to read the driving cycle from.</p></td>
</tr>
<tr class="even">
<td><p><br />
</p></td>
<td><p>Defines the file to read the trajectory from either in netstate or in Amitran format.</p></td>
</tr>
<tr class="odd">
<td><p><br />
</p></td>
<td><p>Defines for which emission class the emissions shall be generated.</p></td>
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
<td><p><br />
</p></td>
<td><p>If set, the acceleration is computed instead of being read from the file.</p></td>
</tr>
<tr class="even">
<td><p><br />
</p></td>
<td><p>If set, the first line of the read file is skipped.</p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>If set, the given speed is interpreted as being given in km/h.</p></td>
</tr>
<tr class="even">
<td></td>
<td><p>If set, the fourth column is read and used as slope (in [°]).</p></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Sets a global slope (in [°]) that is used if the file does not contain slope information.</p></td>
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
<td><p><br />
</p></td>
<td><p>Defines the file to write the emission cycle results into.</p></td>
</tr>
<tr class="even">
<td></td>
<td></td>
</tr>
</tbody>
</table>

### Emissions

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
<td><p>Determines where to load <a href="Models/Emissions/PHEMlight" title="wikilink">PHEMlight</a> definitions from.</p></td>
</tr>
<tr class="even">
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
<td><p>When set, the tool does not report anything; <em>default: <strong>false</strong></em></p></td>
</tr>
<tr class="even">
<td><p><br />
</p></td>
<td><p>Prints the help screen</p></td>
</tr>
<tr class="odd">
<td></td>
<td></td>
</tr>
</tbody>
</table>

-----

[Category:ApplicationDescription](Category:ApplicationDescription.md)