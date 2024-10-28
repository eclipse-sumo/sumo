---
title: Export
---

# Supported export formats

## SUMO

The default output format, when using no other options. While being
partly similar in structure to the plain format below, this file is not
meant to be edited by hand since there are complex dependencies between
the various parts. The name of the file to
write the network into can be given using the option **--output-file** {{DT_FILE}}.
**--sumo-output** {{DT_FILE}} and **--output** {{DT_FILE}} are
synonyms. If any of the other outputs below is active the SUMO
network will only be generated if the **--output** option is set.

## Plain

Parsed node and edge definitions may be saved into XML-files which
have the same format as the ones used for importing XML-networks as
described in
[Networks/Building_Networks_from_own_XML-descriptions](../Networks/PlainXML.md).
This shall ease processing of networks read from other formats than XML.
The option **--plain-output** {{DT_FILE}} lets [netconvert](../netconvert.md) and
[netgenerate](../netgenerate.md) generate four files named
"{{DT_FILE}}.nod.xml", "{{DT_FILE}}.edg.xml". "{{DT_FILE}}.con.xml",
and "{{DT_FILE}}.tll.xml" which contain the previously imported nodes, edges,
connections, and traffic light logics, respectively. If the original network contained
edge type information a fifth file with the name "{{DT_FILE}}.typ.xml"
will be generated.

The edge file contains the list of previously read edges and each edge
will have the information about the edge's id, the allowed velocity, the
number of lanes, and the from/to - nodes stored. Geometry information is
stored only if the imported edge has a shape, meaning that it is not
only a straight connection between the from/to-nodes. The lane spread
type and the basic edge type are only saved if differing from defaults
("right" and "normal", respectively). Additionally, if one of the lanes
prohibits/allows vehicle classes, this information is saved, too (see
also "Defining allowed Vehicle Types").

## OpenDRIVE

To write the imported/generated network as a
[OpenDRIVE](../Networks/Import/OpenDRIVE.md) file (version 1.4),
use the option **--opendrive-output** {{DT_FILE}}. The extension for OpenDRIVE networks is usually
".xodr".

Some notes:

- The feature is currently under implementation
- `road` - the normal ones
  - the road `type` is always set to
    "`town`" for the complete street
  - the lane `type` is set to either
    *biking, sidewalk, tram, none* or *driving* according to the
    edge permissions.
  - `link`
    - The road is always connected to the nodes it is outgoing
      (`predecessor`) / incoming
      (`successor`) from/to
  - `planView`
    - the geometry is given as lines and paramPoly3
  - no road widenings are modeled - if the number of lanes changes,
    the road changes
  - `elevationProfile` is written if the
    network contains 3D geometries
  - `lateralProfile` does not contain
    relevant information
  - the roads are always unidirectional, this means only the center
    lane and the right lanes are given
  - `objects` and
    `signals` do not contain relevant
    information

Recommended options

- **--junctions.scurve-stretch 1.0**. This elongates junction shapes to allow for smooth transitions
  (values around *1.0* can be used to reduced or increase stretching)

- **--output.original-names**. This records the edge IDs from the corresponding *.net.xml* within `<userData sumoID="..."/>` elements as children of the `<road>`

### Embedding Road Objects

To include road objects in the generated *xodr*-output, the following
conditions must be met:

- a polygon file is loaded with `<poly>` and `<poi>` elements
- either:
  - edges include the [generic
  parameter](../Simulation/GenericParameters.md) `<param key="roadObjects" value="POLY_D1 POLY_ID2 ... POLY_IDK"/>`
  - option **--opendrive-output.shape-match-dist FLOAT** is set to automatically match loaded polygons and POIs to the closest network edge

Such edges will receive the polygon objects with the indicated IDs as
road objects


## MATSim

To write the imported/generated network as a MATSim file, use the option
**--matsim-output** {{DT_FILE}}. The extension for MATSim networks is usually ".xml".

Please note that the capacity is computed by multiplying an edge's lane
number with the capacity norm:

```
MAXIMUM_FLOW = LANE_NUMBER * CAPACITY_NORM
```

The value of CAPACITY_NORM is controlled via the option **--capacity-norm** {{DT_FLOAT}} (default: 1800).


## DlrNavteq

The **--dlr-navteq-output** {{DT_FILE}} generates a links, a nodes and a traffic lights file matching extraction version 6.5 by default. The version can be set using the option **--dlr-navteq.version** {{DT_STR}}. A version of 7 or larger will trigger the output of additional attributes such as maximum height and width.


## Amitran

The Amitran network format consists of a single XML file conforming to
the schema at <https://sumo.dlr.de/xsd/amitran/network.xsd>. The option **--amitran-output** {{DT_FILE}}
writes the data to a file with the following format

```xml
<?xml version="1.0" encoding="utf-8"?>

<network xmlns:xsi="https://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="http://sumo.dlr.de/xsd/amitran/network.xsd">
   <node id="0" type="rightBeforeLeft"/>
   <node id="1" type="priority"/>
   ...
   <link id="0" from="1" to="0" roadClass="4" length="136448" speedLimitKmh="20" laneNr="1"/>
   ...
</network>
```

where all values are integers (the length is in units of 0.01m, the road
class is a functional road class in the Navteq sense)


# Converting to other GIS formats

## (Q)GIS / GeoJSON
Conversion of .net.xml file with [python tool net2geojson](../Tools/Net.md#net2geojsonpy)

## KML
Conversion of .net.xml file with [python tool net2kml](../Tools/Net.md#net2kmlpy)



<div style="border:1px solid #909090; min-height: 35px;" align="right">
<span style="float: right; margin-top: -5px;"><a href="https://web.archive.org/web/20191005024529/https:/ec.europa.eu/research/fp7/index_en.cfm"><img src="../images/FP7-small.gif" alt="Seventh Framework Programme"></a>
<a href="https://web.archive.org/web/20180309093847/https://amitran.eu/"><img src="../images/AMITRAN-small.png" alt="AMITRAN project"></a></span>
<span style="">This part of SUMO was developed, reworked, or extended within the project
<a href="https://web.archive.org/web/20180309093847/https://amitran.eu/">"AMITRAN"</a>, co-funded by the European Commission within the <a href="https://web.archive.org/web/20191005024529/https:/ec.europa.eu/research/fp7/index_en.cfm">Seventh Framework Programme</a>.</span></div>
