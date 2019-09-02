---
title: Sumo at a Glance
permalink: /Sumo_at_a_Glance/
---

## About

"**S**imulation of **U**rban **MO**bility", or "SUMO" for short, is an
open source, microscopic, multi-modal traffic simulation. It allows to
simulate how a given traffic demand which consists of single vehicles
moves through a given road network. The simulation allows to address a
large set of traffic management topics. It is purely microscopic: each
vehicle is modelled explicitly, has an own route, and moves individually
through the network. Simulations are deterministic by default but there
are various options for [introducing
randomness](Simulation/Randomness.md).

If you download the SUMO package, you will note that it contains further
applications besides SUMO. These applications are used to import/prepare
road networks and demand data for being used in SUMO, see [\#Included
Applications](#included_applications) for a more verbose
list.

## Features

- Includes all applications needed to prepare and perform a traffic
simulation (network and routes import, DUA, simulation)
- Simulation
  - Space-continuous and time-discrete vehicle movement
  - Different vehicle types
  - Multi-lane streets with lane changing
  - Different right-of-way rules, traffic lights
  - A fast openGL graphical user interface
  - Manages networks with several 10.000 edges (streets)
  - Fast execution speed (up to 100.000 vehicle updates/s on a 1GHz
    machine)
  - Interoperability with other application at run-time
  - Network-wide, edge-based, vehicle-based, and detector-based
    outputs
  - Supports person-based inter-modal trips
- Network Import
  - Imports VISUM, Vissim, Shapefiles, OSM, RoboCup, MATsim,
    OpenDRIVE, and XML-Descriptions
  - Missing values are determined via heuristics
- Routing
  - Microscopic routes - each vehicle has an own one
  - Different Dynamic User Assignment algorithms
- High portability
  - Only standard C++ and portable libraries are used
  - Packages for Windows main Linux distributions exist
- High interoperability through usage of XML-data only
- Open source ([EPL](https://eclipse.org/legal/epl-v20.html))

## Usage Examples

Since 2001, the SUMO package has been used in the context of several
national and international research
[projects](Other/Projects.md). The applications included:

- traffic lights evaluation
- route choice and re-routing
- evaluation of traffic surveillance methods
- [simulation of vehicular communications](Topics/V2X.md)
- traffic forecast

## Included Applications

The package includes:

| Application Name                                    | Short Description                                         |
| --------------------------------------------------- | --------------------------------------------------------- |
| [SUMO](SUMO.md)                             | The microscopic simulation with no visualization; command line application                                         |
| [SUMO-GUI](SUMO-GUI.md)                     | The microscopic simulation with a graphical user interface                                                        |
| [NETCONVERT](NETCONVERT.md)                 | Network importer and generator; reads road networks from different formats and converts them into the SUMO-format   |
| [NETEDIT](NETEDIT.md)                       | A graphical network editor.                                                                                  |
| [NETGENERATE](NETGENERATE.md)               | Generates abstract networks for the SUMO-simulation                    |
| [DUAROUTER](DUAROUTER.md)                   | Computes fastest routes through the network, importing different types of demand description. Performs the DUA      |
| [JTRROUTER](JTRROUTER.md)                   | Computes routes using junction turning percentages                                                               |
| [DFROUTER](DFROUTER.md)                     | Computes routes from induction loop measurements                                                     |
| [MAROUTER](MAROUTER.md)                     | Performs macroscopic assignment                                                            |
| [OD2TRIPS](OD2TRIPS.md)                     | Decomposes O/D-matrices into single vehicle trips                                       |
| [POLYCONVERT](POLYCONVERT.md)               | Imports points of interest and polygons from different formats and translates them into a description that may be visualized by [SUMO-GUI](SUMO-GUI.md) |
| [ACTIVITYGEN](ACTIVITYGEN.md)               | Generates a demand based on mobility wishes of a modeled population                                |
| [EMISSIONSMAP](Tools/Emissions.md)          | Generates an emission map                                                                   |
| [EMISSIONSDRIVINGCYCLE](Tools/Emissions.md) | Calculates emission values based on a given driving cycle                           |
| [Additional Tools](Tools.md)           | There are some tasks for which writing a large application is not necessary. Several solutions for different problems may be covered by these tools.            |

Several parties have extended the SUMO package during their work and
submitted [their code](Contributed.md). If not being a part of
the release, these contributions are usually not tested frequently and
may be outdated. The following contributions are included.

| Application Name                                                                      | Short Description                   |
| ------------------------------------------------------------------------------------- | --------------------------------------------------------------- |
| [TraCI4Matlab](http://de.mathworks.com/matlabcentral/fileexchange/44805-traci4matlab) | A Matlab interface for connecting and extending information via [TraCI](TraCI.md) Andres Acosta                                                               |
| [TraaS](TraCI/TraaS.md)                                                       | A SOAP(webservice) interface for connecting and extending information via [TraCI](TraCI.md) by Mario Krumnow, Also a Java TraCI client library.               |
| [LiSuM](Tools/LiSuM.md)                                                       | LiSuM is a middleware that couples [LISA+](https://www.schlothauer.de/en/software-systems/lisa/) and SUMO to execute real-world traffic control software within SUMO. |

## History

The development of SUMO started in the year 2000. The major reason for
the development of an open source, microscopic road traffic simulation
was to support the traffic research community with a tool with the
ability to implement and evaluate own algorithms. The tool has no need
for regarding all the needed things for obtaining a complete traffic
simulation such as implementing and/or setting up methods for dealing
with road networks, demand, and traffic controls. By supplying such a
tool, the DLR wanted to i) make the implemented algorithms more
comparable by using a common architecture and model base, and ii) gain
additional help from other contributors.

## Software design criteria

Two major design goals are approached: the software shall be fast and it
shall be portable. Due to this, the very first versions were developed
to be run from the command line only - no graphical interface was
supplied at first and all parameter had to be inserted by hand. This
should increase the execution speed by leaving off slow visualisation.
Also, due to these goals, the software was split into several parts.
Each of them has a certain purpose and must be run individually. This is
something that makes SUMO different to other simulation packages where,
for instance, the dynamical user assignment is made within the
simulation itself, not via an external application like here. This split
allows an easier extension of each of the applications within the
package because each is smaller than a monolithic application that does
everything. Also, it allows the usage of faster data structures, each
adjusted to the current purpose, instead of using complicated and
ballast-loaded ones. Still, this makes the usage of SUMO a little bit
uncomfortable in comparison to other simulation packages. As there are
still other things to do, we are not thinking of a redesign towards an
integrated approach by now.

## Contributors and Participants

<table>
<thead>
<tr class="header">
<th><p>Org.</p></th>
<th><p>Name</p></th>
<th><p>Topics / Contribution</p></th>
</tr>
</thead>
<tbody>
<tr class="odd">
<td><figure>
<img src="images/Zaik_small.gif" alt="Zaik_small.gif" />
</figure></td>
<td><p>Christian Rössel</p></td>
<td><p>Initial microsimulation core; initial detectors implementation</p></td>
</tr>
<tr class="even">
<td rowspan="11"><figure>
<img src="images/Dlr_small.gif" title="dlr_small.gif" alt="" />
</figure></td>
<td><p>Peter Wagner</p></td>
<td><p>Models, organisation, spiritual lead</p></td>
</tr>
<tr class="odd">
<td><p>Daniel Krajzewicz</p></td>
<td><p>Everything</p></td>
<td></td>
</tr>
<tr class="even">
<td><p>Julia Ringel</p></td>
<td><p>Traffic Light &amp; WAUT Algorithms</p></td>
<td></td>
</tr>
<tr class="odd">
<td><p>Eric Nicolay</p></td>
<td><p>Everything</p></td>
<td></td>
</tr>
<tr class="even">
<td><p>Michael Behrisch</p></td>
<td><p>Everything</p></td>
<td></td>
</tr>
<tr class="odd">
<td><p>Yun-Pang Wang</p></td>
<td><p>User Assignment</p></td>
<td></td>
</tr>
<tr class="even">
<td><p>Danilot Teta Boyom</p></td>
<td><p>Vehicular Communication Model (removed from the source)</p></td>
<td></td>
</tr>
<tr class="odd">
<td><p>Sascha Krieg</p></td>
<td></td>
<td></td>
</tr>
<tr class="even">
<td><p>Lena Kalleske</p></td>
<td></td>
<td></td>
</tr>
<tr class="odd">
<td><p>Laura Bieker</p></td>
<td><p>Tests, Python scripts</p></td>
<td></td>
</tr>
<tr class="even">
<td><p>Jakob Erdmann</p></td>
<td><p>network import, <a href="NETEDIT.html" title="wikilink">NETEDIT</a></p></td>
<td></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Andreas Gaubatz</p></td>
<td></td>
</tr>
<tr class="even">
<td></td>
<td><p>Maik Drozdzynski</p></td>
<td></td>
</tr>
<tr class="odd">
<td rowspan="3"><p>Uni Lübeck</p></td>
<td><p>Axel Wegener</p></td>
<td><p>TraCI initiator</p></td>
</tr>
<tr class="even">
<td><p>Thimor Bohn</p></td>
<td><p>TraCI</p></td>
<td></td>
</tr>
<tr class="odd">
<td><p>Friedemann Wesner</p></td>
<td><p>TraCI</p></td>
<td></td>
</tr>
<tr class="even">
<td></td>
<td><p>Felix Brack</p></td>
<td></td>
</tr>
<tr class="odd">
<td></td>
<td><p>Tino Morenz</p></td>
<td></td>
</tr>
<tr class="even">
<td rowspan="4"><p><img src="images/Uni_erlangen.png" title="fig:uni_erlangen.png" width="149" alt="uni_erlangen.png" /><br />
<br />
<img src="images/Uibk-small.png" title="fig:uibk-small.png" width="64" alt="uibk-small.png" /><br />
<br />
<img src="images/Logo_cmu.png" title="fig:logo_cmu.png" width="100" alt="logo_cmu.png" /><br />
<br />
<img src="images/Logo_ucla.png" title="fig:logo_ucla.png" width="100" alt="logo_ucla.png" /></p></td>
<td><p>Christoph Sommer</p></td>
<td><p>TraCI merge with <a href="http://veins.car2x.org/">Veins</a>, Subscription Interface, Misc.</p></td>
</tr>
<tr class="odd">
<td><p>David Eckhoff</p></td>
<td><p>TraCI, deterministic simulation behavior</p></td>
<td></td>
</tr>
<tr class="even">
<td><p>Falko Dressler</p></td>
<td><p>TraCI</p></td>
<td></td>
</tr>
<tr class="odd">
<td><p>Tobias Mayer</p></td>
<td><p>Traffic model abstraction, IDM model port</p></td>
<td></td>
</tr>
<tr class="even">
<td><p>HU Berlin</p></td>
<td><p>Matthias Heppner</p></td>
<td><p>Unittests</p></td>
</tr>
<tr class="odd">
<td rowspan="3"><figure>
<img src="images/Tum-logo.png" title="tum-logo.png" alt="" />
</figure></td>
<td><p>Piotr Woznica</p></td>
<td><p><a href="ACTIVITYGEN.html" title="wikilink">ACTIVITYGEN</a></p></td>
</tr>
<tr class="even">
<td><p>Walter Bamberger</p></td>
<td><p>Development of <a href="ACTIVITYGEN.html" title="wikilink">ACTIVITYGEN</a> as a base for the evaluation of trust scenarios in VANETs. The work is part of the project <a href="http://www.ldv.ei.tum.de/fidens/">Fidens: Trust between Cooperative Systems</a> featuring trusted probabilistic knowledge processing in vehicular networks.</p></td>
<td></td>
</tr>
<tr class="odd">
<td><p>Matthew Fullerton</p></td>
<td></td>
<td></td>
</tr>
<tr class="even">
<td><p>IIT Bombay, India</p></td>
<td><p>Ashutosh Bajpai</p></td>
<td><p>randomDepart.py, a python script to generate the real traffic pattern by exponential Distribution.</p></td>
</tr>
<tr class="odd">
<td><figure>
<img src="images/Torino_small.gif" title="torino_small.gif" alt="" />
</figure></td>
<td><p>Enrico Gueli</p></td>
<td><p><a href="http://sourceforge.net/projects/traci4j/">TraCI4J</a></p></td>
</tr>
<tr class="even">
<td></td>
<td><p>Leontios Papaleontiou</p></td>
<td><p><a href="Contributed/SUMO_Traffic_Modeler.html" title="wikilink">Contributed/SUMO Traffic Modeler</a></p></td>
</tr>
<tr class="odd">
<td><figure>
<img src="images/Wroclaw_university_small.jpg" title="Wroclaw_university_small.jpg" alt="" />
</figure></td>
<td><p>Karol Stosiek</p></td>
<td><p>Documentation, network building</p></td>
</tr>
</tbody>
</table>