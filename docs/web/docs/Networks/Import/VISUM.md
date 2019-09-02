---
title: Networks Import VISUM
permalink: /Networks/Import/VISUM/
---

[NETCONVERT](NETCONVERT.md) can import native
[VISUM](http://www.ptvag.com/traffic/software/visum/)-network files.
Their extension is ".net". If you do not have a file with this
extension, but a ".ver"-file only, you have to generate the ".net"-file
using [VISUM](http://www.ptvag.com/traffic/software/visum/) by exporting
it from the loaded version description (".ver"-file).

The option to load a
[VISUM](http://www.ptvag.com/traffic/software/visum/) ".net"-file into
[NETCONVERT](NETCONVERT.md) in order to convert it into a
SUMO-network is named  or  for short. So, the following call to
[NETCONVERT](NETCONVERT.md) imports the road network stored in
"my_visum_net.net" and stores the SUMO-network generated from this
data into "my_sumo_net.net.xml":

`netconvert --visum my_visum_net.net -o my_sumo_net.net.xml`

# Import Coverage

The following table shows which information is parsed from a given VISUM
network.

**Information [NETCONVERT](NETCONVERT.md) reads from VISUM
networks**

<table>
<thead>
<tr class="header">
<th><p>Table name</p></th>
<th><p>Imported attributes</p></th>
<th><p>Description</p></th>
</tr>
</thead>
<tbody>
<tr class="odd">
<td><p>VSYS</p></td>
<td><p>VSysCode (CODE)<br />
VSysMode (TYP)</p></td>
<td><p>Traffic modes</p></td>
</tr>
<tr class="even">
<td><p>STRECKENTYP</p></td>
<td><p>Nr<br />
v0-IV (V0IV)<br />
Rang<br />
Kap-IV (KAPIV)</p></td>
<td><p>Edge types</p></td>
</tr>
<tr class="odd">
<td><p>KNOTEN</p></td>
<td><p>Nr<br />
XKoord<br />
YKoord</p></td>
<td><p>Nodes</p></td>
</tr>
<tr class="even">
<td><p>BEZIRK</p></td>
<td><p>Nr<br />
NAME (unused later)<br />
Proz_Q<br />
Proz_Z<br />
XKoord<br />
YKoord</p></td>
<td><p>Districts</p></td>
</tr>
<tr class="odd">
<td><p>STRECKE (STRECKEN)</p></td>
<td><p>Nr<br />
VonKnot (VonKnotNr)<br />
NachKnot (NachKnotNr)<br />
Typ (TypNr)<br />
Einbahn</p></td>
<td><p>Edges</p></td>
</tr>
<tr class="even">
<td><p>ANBINDUNG</p></td>
<td><p>BezNr<br />
KnotNr<br />
Proz<br />
t0-IV<br />
Typ<br />
Richtung</p></td>
<td><p>District connections</p></td>
</tr>
<tr class="odd">
<td><p>ABBIEGEBEZIEHUNG (ABBIEGER)</p></td>
<td><p>VonKnot (VonKnotNr)<br />
UeberKnot (UeberKnotNr)<br />
NachKnot (NachKnotNr)<br />
VSysCode (VSYSSET)</p></td>
<td><p>Edge Connections</p></td>
</tr>
<tr class="even">
<td><p>STRECKENPOLY</p></td>
<td><p>VonKnot (VonKnotNr)<br />
NachKnot (NachKnotNr)<br />
INDEX<br />
XKoord<br />
YKoord</p></td>
<td><p>Edge geometries</p></td>
</tr>
<tr class="odd">
<td><p>FAHRSTREIFEN</p></td>
<td><p>KNOTNR<br />
STRNR<br />
FSNR<br />
RICHTTYP<br />
LAENGE</p></td>
<td><p>Lane descriptions</p></td>
</tr>
<tr class="even">
<td><p>LSA (SIGNALANLAGE)</p></td>
<td><p>Nr<br />
Umlaufzeit (UMLZEIT)<br />
StdZwischenzeit (STDZWZEIT)<br />
PhasenBasiert</p></td>
<td><p>Traffic lights</p></td>
</tr>
<tr class="odd">
<td><p>KNOTENZULSA (SIGNALANLAGEZUKNOTEN)</p></td>
<td><p>KnotNr<br />
LsaNr</p></td>
<td><p>Nodes-&gt;TLS</p></td>
</tr>
<tr class="even">
<td><p>LSASIGNALGRUPPE (SIGNALGRUPPE)</p></td>
<td><p>Nr<br />
LsaNr<br />
GzStart (GRUENANF)<br />
GzEnd (GRUENENDE)<br />
GELB</p></td>
<td><p>Signal groups</p></td>
</tr>
<tr class="odd">
<td><p>ABBZULSASIGNALGRUPPE (SIGNALGRUPPEZUABBIEGER)</p></td>
<td><p>SGNR (SIGNALGRUPPENNR)<br />
LsaNr<br />
VonKnot / VONSTRNR<br />
NachKnot / NACHSTRNR<br />
UeberKnot (UeberKnotNr)<br />
LsaNr</p></td>
<td><p>Edge connections-&gt;TLS</p></td>
</tr>
<tr class="even">
<td><p>LSAPHASE (PHASE)</p></td>
<td><p>Nr<br />
LsaNr<br />
GzStart (GRUENANF)<br />
GzEnd (GRUENENDE)</p></td>
<td><p>Signal phases</p></td>
</tr>
<tr class="odd">
<td><p>LSASIGNALGRUPPEZULSAPHASE</p></td>
<td><p>PsNr<br />
LsaNr<br />
SGNR</p></td>
<td><p>Signal groups-&gt;phases</p></td>
</tr>
<tr class="even">
<td><p>FAHRSTREIFENABBIEGER</p></td>
<td><p>KNOT (KNOTNR)<br />
VONSTR (VONSTRNR)<br />
NACHSTR (NACHSTRNR)<br />
VONFSNR<br />
NACHFSNR</p></td>
<td><p>Lane-to-lane descriptions</p></td>
</tr>
</tbody>
</table>

Well, basically that's all for network import, isn't it? Well, actually
not. In the following, the basic possibilities and further advices and
tricks for working with these is described.

## Lane Number

[VISUM](http://www.ptvag.com/traffic/software/visum/) does not work with
the lane numbers of streets, instead, the streets' capacities are used.
This means that under circumstances the informationabout the edges' lane
numbers are missing. Still, one can try to obtain the lane number from
the given edges' capacities. An approximation is:

`LANE_NUMBER = MAXIMUM_FLOW / CAPACITY_NORM`

The value of CAPACITY_NORM is controlled via the option  (default:
1800).

In the case the "*ANZFAHRSTREIFEN*" (lane numbers) field within the
net's "*STRECKEN*" (edges)-table is empty, and also the types do not
hold any information about the according edges' lane numbers, this
conversion is done automatically. Still, we also had
[VISUM](http://www.ptvag.com/traffic/software/visum/) nets in our hands,
where a capacity was given, but the lane number field was set to 1 for
all edges. In this case, one has to force
[NETCONVERT](NETCONVERT.md) to ignore the lane number attribute
and use the capacity. This is done by using the option
**--visum.recompute-laneno**.

## Dealing with Connectors

[VISUM](http://www.ptvag.com/traffic/software/visum/) as a macroscopic
tool, does not regard single vehicles. As one consequence,
[VISUM](http://www.ptvag.com/traffic/software/visum/) uses "connections"
through which traffic is fed from the districts into the network.

The [effects on using such connectors within a microscopic simulations
should be described at a different
page](MacroDemandAndMicroSim.md). Here, we want to show the
possibilities to change the connector attributes using
[NETCONVERT](NETCONVERT.md).

# See also

[NETCONVERT](NETCONVERT.md) is able to guess some information
which is sometimes missing in imported networks. Below, you may find
links to further information of interest.

  - Most [VISUM](http://www.ptvag.com/traffic/software/visum/) networks
    do not contain definitions of traffic lights positions; Still,
    [NETCONVERT](NETCONVERT.md) is able to [guess tls
    positions](NETCONVERT_GuessingTlsPositions.md) and to [guess
    tls programs](TCONVERT_GuessingTlsPrograms.md).
  - Also, we have not seen a
    [VISUM](http://www.ptvag.com/traffic/software/visum/) network where
    on- and off-ramps where available for highways.
    [NETCONVERT](NETCONVERT.md) is able to [guess on- and
    off-ramps](NETCONVERT_GuessingRamps.md).
  - In addition to the network, further descriptions of [lane-to-lane or
    edge-to-edge connections](NETCONVERT_SettingConnections.md)
    may be read.

Other possibilities of [NETCONVERT](NETCONVERT.md), such as
projection of geo-coordinates, should not apply when working with
[VISUM](http://www.ptvag.com/traffic/software/visum/) networks.

## Importing other data from VISUM

[VISUM](http://www.ptvag.com/traffic/software/visum/) uses O/D-matrices
as a demand descriptions. There is some further information on
[Demand/Importing O/D
Matrices](Demand/Importing_O/D_Matrices.md).

# References

  - [PTV AG's
    VISUM-page](http://www.ptvag.com/software/transportation-planning-traffic-engineering/software-system-solutions/visum/visum/)
    (06.09.2011)

# Missing

  - Attributes of connectors
  - Importing POIs/Shapes
  - what is exactly imported (how edge attributes are determined)
  - other traffic modes
  - Network quality