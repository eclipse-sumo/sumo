---
title: Shapes
---

# circlePolygon.py

Generate circular polygons with custom radius, and number of vertices.

```
<SUMO\>/tools/shape/circlePolygon.py X,Y,RADIUS,NUMVERTICES [x2,y2,radius2,points2] ....
```

- <X\>: Polygon center X
- <Y\>: Polygon center Y
- <RADIUS\>: Polygon radius
- <NUMVERTICES\>: number of vertices

# CSV2polyconvertXML.py

Converts a given CSV-file that contains a list of pois to an XML-file that may be read by POLYCONVERT

```
<SUMO\>/tools/shape/CSV2polyconvertXML.py <CSV_FILE> <OUTPUT_FILE> <VALUENAME>[,<VALUENAME>]*
```

- <CSV_FILE\>: CSV document to be converted
- <OUTPUT_FILE\>: POIs compatible with POLYCONVERT

# pois2inductionLoops.py

Converts a PoIs within a given PoI-file which are located on lanes into
induction loop detectors. Each PoI is replicated to cover all lanes of
the road. The detectors are named "<POINAME\>__l<LANE_INDEX\>".

```
<SUMO\>/tools/shape/pois2inductionLoops.py <NET> <POIS> <OUTPUT>
```

- <NET\>: The net to use for retrieving lane numbers
- <POIS\>: The file containing the PoIs
- <OUTPUT\>: The file to write the detectors into

The tool uses the <SUMO\>/tools/lib/sumopoi.py library.

# poi_alongRoads.py

Spatial distribute of POIs along given edges on a given network.

So far POIs are situated in the middle on all edges without regard to
the type of the edge (street, junction). Edges may be given in arbitrary
order, connected edges are found automatically. Therefore: crossing
chains of connected edges are not allowed -\> this needs two different
runs of this script. Output is written in file 'pois.add.xml'

```
<SUMO\>/tools/shape/poi_alongRoads.py <NET> <EDGE_ID>[,<EDGE_ID>]* <DISTANCE>
```

- <NET\>: The net to use for retrieving the geometry
- <EDGE_ID\>\[,<EDGE_ID\>\]\*: The edges to go along
- <DISTANCE\>: Distance between PoIs

Edges are separated with comma and without spaces in between. The
distance between POIs may be any positive real number PoIs are stored
with type="default", color="1,0,0", and layer="0".

The tool uses the <SUMO\>/tools/lib/sumonet.py library.

# poi_at_stops.py

Spatial distribute of POIs along given edges on a given network.

Generates a PoI-file containing a PoI for each Stop from the given net.

```
<SUMO\>/tools/shape/poi_at_stops.py <NET> <STOPS>
```

- <NET\>: The network
- <STOPS\>: The stops to read stop positions from

PoIs are stored with type="default", color="1,0,0", and layer="0".

The tool uses the <SUMO\>/tools/lib/sumonet.py library.

# poi_atTLS.py

Spatial distribute of POIs along given edges on a given network.

Generates a PoI-file containing a PoI for each tls controlled
intersection from the given net.

```
<SUMO\>/tools/shape/poi_atTLS.py <NET> [nojoin]
```

- <NET\>: The net to read traffic light (tls) positions from
- \[nojoin\]: If "nojoin" is given, PoIs will be built on all nodes
  covered by traffic lights, otherwise, if a traffic light spans over
  multiple intersections, only one PoI will be built for this traffic
  lights, at the center of all intersections controlled by this
  traffic light

PoIs are stored with type="default", color="1,0,0", and layer="0".

The tool uses the <SUMO\>/tools/lib/sumonet.py library.

# poly2edgedata.py

Transform polygons with params into edgedata with attributes.
For each polygon a unique edge is selected that gives the best geometrical match.

```
<SUMO\>/tools/shape/poly2edgedata.py <POLYGONS> -n NETFILE -o OUTPUT_EDGEDATA
```

Further options:

- **--split-attributes** and **--nosplit-attributes** support the use case where a single polygon applies to both directions of a road and related data must be split for the forward and reverse edge (i.e. traffic counts).
- **--filter ATTR,MIN,MAX** supports filtering out polygons where ATTR is not in range [MIN,MAX]
- **--radius**: maximum matching radius (default 20)
- **--min-length**: avoid matching to short edges (default 10)
- **--angle-tolerance**: avoid matching if edge and shape angle are too different (default 20 degrees)

!!! note
    polygons can be obtained from OSM, shapefiles or geojson with the help of [polyconvert](../polyconvert.md)

## patches
Option **--patchfile** loads instructions to modify the matching in special cases.
The following syntax is supported in the patch file (one patch per line):

```
# lines starting with '#' are ignored as comments
# rev overrides the reverse edge of EDGEID to be REVEDGEID
rev EDGEID REVEDGEID
# edg overrides the edge to assign for POLYID to be EDGEID
edg POLYID EDGEID
# dat overrrides the data attribute ATTR for POLYID to take on value VALUE
dat POLYID ATTR VALUE
```

any ID or VALUE may bet set to 'None' to signify that

- a reverse edge should not be assigned
- a polygon should not be mapped
- data should be ignored

