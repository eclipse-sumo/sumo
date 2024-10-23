---
title: OpenStreetMap
---

*"[OpenStreetMap](https://www.openstreetmap.org/) is a free editable map
of the whole world. It is made by people like you."* (from
[<https://www.openstreetmap.org>](https://www.openstreetmap.org/)). This
page describes the conversion of files containing data from OpenStreetMap to
SUMO network files.

There are several ways to download the data from OpenStreetMap to a
file. Please read
[Networks/Import/OpenStreetMapDownload](../../Networks/Import/OpenStreetMapDownload.md)
to learn about them. For more information about the file format,
see [OpenStreetMap file](../../OpenStreetMap_file.md).

# 3-click scenario generation

By using the [osmWebWizard.py script](../../Tools/Import/OSM.md), a
complete scenario can be built quickly and comfortably. The network will
be imported with options and typemaps suitable for the selected traffic
modes. If more control is needed the options described below can be
used.

# Importing the Road Network

[netconvert](../../netconvert.md) can import OSM-files natively. The
corresponding option is called **--osm-files** {{DT_FILE}}\[,{{DT_FILE}}\]\* or **--osm** {{DT_FILE}}\[,{{DT_FILE}}\]\* for short.

The following call to [netconvert](../../netconvert.md) imports the
road network stored in "berlin.osm.xml" and stores the SUMO-network
generated from this data into "berlin.net.xml":

```
netconvert --osm-files berlin.osm.xml -o berlin.net.xml
```

OSM-data always uses WGS84 geo coordinates, which are automatically
transformed to UTM by netconvert (since sumo 0.11.1). Thus you only need
explicit projection parameters if a different projection is needed.
Refer to the [netconvert](../../netconvert.md) documentation for other
conversion options.

!!! note
    Several aspects of the imported network may have to be modified to suit your needs. Some of the relevant [netconvert](../../netconvert.md) options are described below. For more flexible alterations see [Tutorials/ScenarioGuide#Modifying_the_Network](../../Tutorials/ScenarioGuide.md#modifying_the_network).

The number of tiles given in both calls must match.

### Recommended [netconvert](../../netconvert.md) Options

```
 --geometry.remove --ramps.guess --junctions.join --tls.guess-signals --tls.discard-simple --tls.join --tls.default-type actuated
```

Rationale:

- \--geometry.remove : Simplifies the network (saving space) without
  changing topology
- \--ramps.guess : Acceleration/Deceleration lanes are often not
  included in OSM data. This option identifies  roads that likely have
  these additional lanes and adds them
- \--junctions.join : See [\#Junctions](#junctions)
- \--tls.guess-signals --tls.discard-simple --tls.join : See
  [\#Traffic_Lights](#traffic_lights)
- \--tls.default-type actuated : Default static traffic lights are defined without knowledge about traffic patterns and may work poorly in high traffic

### Countries with left-hand driving

!!! caution
    When importing networks with left-hand driving rules, the option **--lefthand** must be set.

### Recommended Typemaps

There are multiple degrees of freedom when importing data from OSM.
Sometimes, information such as speed limit is missing in the raw data
and must be inferred from the abstract type of the road (i.e. motorway).
Different simulation scenarios require different modes of traffic and
thus different parts of the traffic infrastructure to be imported. The
tool for making these choices are typemaps. SUMO provides
recommended typemaps in the folder {{SUMO}}/data/typemap/. They are explained
below.

- **osmNetconvert.typ.xml** default settings, appropriate for rural
  and motorway scenarios. This is used in the absence of
  user-specified types. All other typemaps are intended as *patches*
  to this typemap.
- **osmNetconvertUrbanDe.typ.xml** Changes default speeds to reflect
  typical urban speed limits (50km/h).
- **osmNetconvertPedestrians.typ.xml** Adds sidewalks for some edge
  types and sets permissions appropriate for pedestrian simulation.
- **osmNetconvertBicycle.typ.xml** Adapts the width of bicycle lanes.
- **osmNetconvertShips.typ.xml** Imports waterways and ferry routes.
  This typemap can be combined with any other typemap.
- **osmNetconvertRailUsage.typ.xml** Imports additional
  \[<https://wiki.openstreetmap.org/wiki/Key:usage> usage information
  for railways (main,branch,industrial,...). This typemap only works
  in combination with other typemaps.
- **osmNetconvertBidiRail.typ.xml**. Changes the default from
  uni-directional railroads to bi-directional railroads. This may be
  useful in some regions of the world where OSM contributors used
  this style of date representation. The use of this typemap supplants
  the older option **--osm.railway.oneway-default** {{DT_BOOL}}.
- **osmNetconvertAirport.typ.xml**. Imports aeroways (runway, taxiway, etc.)

!!! caution
    When specifying a typemap using the option **--type-files**, the defaults are not loaded. To achieve the desired types, the user should load the default typemap along with the desired modification (**--type-files <SUMO_HOME\>/data/typemap/osmNetconvert.typ.xml,<SUMO_HOME\>/data/typemap/osmNetconvertUrbanDe.typ.xml**) or create a fully specified typemap file themselves.

### Bicycle Traffic

Bicycle infrastructure can be imported using the option **--osm-bike-access**.
It will evaluate the [bicycle=yes/no tags](https://wiki.openstreetmap.org/wiki/Key:bicycle)
as well as oneway information for bicycles and trigger the addition of separate bike lanes.
If you want to modify the width of the bike lanes depending on the street type use the bicycle
type map mentioned above.

### Pedestrian Traffic

By default only footpaths (osm ways dedicated for pedestrian use) are imported.
To import all sidewalk related information, the option **--osm.sidewalks** can be set. Alternatively, sidewalks can be added heuristically via typemaps (see above) or [guessing-options](../../Simulation/Pedestrians.md#generating_a_network_with_sidewalks)

### Lane-To-Lane Connections

By default, lane-to-lane connections are guessed by [netconvert](../../netconvert.md) and only turning restrictions are loaded from OSM to influence connection generation. When setting option **--osm.turn-lanes**, the turn direction road markings from OSM are evaluated to guide connection generation.

!!! caution
    At roads where some lanes have turn markings and others do not, the unmarked lanes are interpreted as through-lanes. This may not be correct in all cases.

# Importing additional Polygons (Buildings, Water, etc.)

OSM-data not only contains the road network but also a wide range of
additional polygons such as buildings and rivers. These polygons can be
imported using [polyconvert](../../polyconvert.md) and then added to a
`sumo-gui`-configuration.

To interpret the OSM-data an additional *typemap*-file is required (the
example below is identical to {{SUMO}}/data/typemap/osmPolyconvert.typ.xml):

```xml
<polygonTypes>
  <polygonType id="waterway"                name="water"       color=".71,.82,.82" layer="-4"/>
  <polygonType id="natural"                 name="natural"     color=".55,.77,.42" layer="-4"/>
  <polygonType id="natural.water"           name="water"       color=".71,.82,.82" layer="-4"/>
  <polygonType id="natural.wetland"         name="water"       color=".71,.82,.82" layer="-4"/>
  <polygonType id="natural.wood"            name="forest"      color=".55,.77,.42" layer="-4"/>
  <polygonType id="natural.land"            name="land"        color=".98,.87,.46" layer="-4"/>

  <polygonType id="landuse"                 name="landuse"     color=".76,.76,.51" layer="-3"/>
  <polygonType id="landuse.forest"          name="forest"      color=".55,.77,.42" layer="-3"/>
  <polygonType id="landuse.park"            name="park"        color=".81,.96,.79" layer="-3"/>
  <polygonType id="landuse.residential"     name="residential" color=".92,.92,.89" layer="-3"/>
  <polygonType id="landuse.commercial"      name="commercial"  color=".82,.82,.80" layer="-3"/>
  <polygonType id="landuse.industrial"      name="industrial"  color=".82,.82,.80" layer="-3"/>
  <polygonType id="landuse.military"        name="military"    color=".60,.60,.36" layer="-3"/>
  <polygonType id="landuse.farm"            name="farm"        color=".95,.95,.80" layer="-3"/>
  <polygonType id="landuse.greenfield"      name="farm"        color=".95,.95,.80" layer="-3"/>
  <polygonType id="landuse.village_green"   name="farm"        color=".95,.95,.80" layer="-3"/>

  <polygonType id="tourism"                 name="tourism"     color=".81,.96,.79" layer="-2"/>
  <polygonType id="military"                name="military"    color=".60,.60,.36" layer="-2"/>
  <polygonType id="sport"                   name="sport"       color=".31,.90,.49" layer="-2"/>
  <polygonType id="leisure"                 name="leisure"     color=".81,.96,.79" layer="-2"/>
  <polygonType id="leisure.park"            name="tourism"     color=".81,.96,.79" layer="-2"/>
  <polygonType id="aeroway"                 name="aeroway"     color=".50,.50,.50" layer="-2"/>
  <polygonType id="aerialway"               name="aerialway"   color=".20,.20,.20" layer="-2"/>

  <polygonType id="shop"                    name="shop"        color=".93,.78,1.0" layer="-1"/>
  <polygonType id="historic"                name="historic"    color=".50,1.0,.50" layer="-1"/>
  <polygonType id="man_made"                name="building"    color="1.0,.90,.90" layer="-1"/>
  <polygonType id="building"                name="building"    color="1.0,.90,.90" layer="-1"/>
  <polygonType id="amenity"                 name="amenity"     color=".93,.78,.78" layer="-1"/>
  <polygonType id="amenity.parking"         name="parking"     color=".72,.72,.70" layer="-1"/>
  <polygonType id="power"                   name="power"       color=".10,.10,.30" layer="-1" discard="true"/>
  <polygonType id="highway"                 name="highway"     color=".10,.10,.10" layer="-1" discard="true"/>
  <polygonType id="railway"                 name="railway"     color=".10,.10,.10" layer="-1" discard="true"/>

  <polygonType id="boundary" name="boundary"    color="1.0,.33,.33" layer="0" fill="false" discard="true"/>
  <polygonType id="admin_level" name="admin_level"    color="1.0,.33,.33" layer="0" fill="false" discard="true"/>
</polygonTypes>
```

Using the typemap file *typemap.xml* the following call to
[polyconvert](../../polyconvert.md) imports polygons from OSM-data and
produces a Sumo-polygon file.

```
polyconvert --net-file berlin.net.xml --osm-files berlin.osm --type-file typemap.xml -o berlin.poly.xml
```

The created polygon file *berlin.poly.xml* can then be added to a
`sumo-gui` configuration:

```
 <configuration>
     <input>
         <net-file value="berlin.net.xml"/>
         <additional-files value="berlin.poly.xml"/>
     </input>
 </configuration>
```

## Railway-specific Objects

By using typemap [{{SUMO}}/data/typemap/osmPolyconvertRail.typ.xml]({{Source}}data/typemap/osmPolyconvertRail.typ.xml), numerous railway-related POIs such as

- stations
- switches
- stop positions
- platforms
- position markers

can be imported.

# Import Scripts

The helper script *osmGet.py* allows downloading a large area. The
resulting file called "<PREFIX\>.osm.xml" can then be imported using the
script *osmBuild.Py*. Both scripts are located in {{SUMO}}/tools.

The call is:

```
osmGet.py --bbox <BOUNDING_BOX> --prefix <NAME>
osmBuild.py --osm-file <NAME>.osm.xml  [--vehicle-classes (all|road|passenger)] [--type-file <TYPEMAP_FILE>] [--netconvert-options <OPT1,OPT2,OPT3>] [--polyconvert-options <OPT1,OPT2,OPT3>]
```

If "road" is given as parameter, only roads usable by road vehicles are
extracted, if "passenger" is given, only those accessible by passenger
vehicles are.

When using the option **--type-file** an additional output file with polygons of rivers
and buildings as well as Points of Interest (POIs) will be generated.
This can be loaded in [sumo-gui](../../sumo-gui.md) for additional
visualization. Useful typemap files can be found at {{SUMO}}/data/typemap/.

Additional options for [netconvert](../../netconvert.md) and
[polyconvert](../../polyconvert.md) can be supplied using the options **--netconvert-options**
and **-polyconvert-options**

!!! note
    By default *osmBuild.py* uses the [recommended options](../../Networks/Import/OpenStreetMap.md#recommended_netconvert_options) but a [netconvert-typemap](../../Networks/Import/OpenStreetMap.md#recommended_typemaps) must be specified manually.

Note that the scripts also support a secondary syntax for loading even
large areas by splitting them into multiple tiles and download requests.
In this case the calls look like this:

```
osmGet.py --bbox <BOUNDING_BOX> --prefix <NAME> --oldapi --tiles <INT>
osmBuild.py --oldapi-prefix <NAME --tiles <INT> [--vehicle-classes (all|road|passenger),ramps,tls] [--type-file <TYEPMAP_FILE>]
```

# Elevation Data

Incooperating z-coordinates in networks is still experimental so please
report anything odd.

## SRTM

When using option **--osm.elevation**, z-data is imported from [tags with `key="ele"`
](https://wiki.openstreetmap.org/wiki/Key:ele) in OSM nodes. Since this
tag is not yet in wide use, tools exist to overlay OSM data with
elevation data sources
(https://wiki.openstreetmap.org/wiki/Srtm_to_Nodes). When using the
osmosis-srtm plugin the option **tagName=ele** must be used since only the ['ele'
tag](https://wiki.openstreetmap.org/wiki/Key:ele) is evaluated and the
plugin would use the ['height'
tag](https://wiki.openstreetmap.org/wiki/Key:height) by default.

## Layer Information

When using option **--osm.layer-elevation**, z-data is imported from [tags with `key="layer"`
](https://wiki.openstreetmap.org/wiki/Key:layer) in OSM ways. Since this
data does not encode full elevation information, a heuristic is used to
interpret the given information. Manual correction may be necessary.

## Other data sources

Further options for importing elevation data are listed at the
[Elevation overview page](../../Networks/Elevation.md).

# Further Notes

## Junctions

In OpenStreetMap roads forming a single street and separated by, for
example, a lawn or tram line, are represented by two edges that are
parallel to each other. When crossing with another street, they form two
junctions instead of one. To merge such junctions into a single
junction, one can define which nodes to merge. See [Networks/Building
Networks from own XML-descriptions\#Joining
Nodes](../../Networks/PlainXML.md#joining_nodes)
and [netconvert](../../netconvert.md) documentation for usage details.

The [netconvert](../../netconvert.md) option **--junctions.join** applies a heuristic to
join these junction clusters automatically and is used by default when
using the *osmBuild.py* script described above. However, some junction
clusters are too complex for the heuristic and should be checked
manually (as indicated by the warning messages). To manually specify
joins for these junctions, see
[JoiningNodes](../../Networks/PlainXML.md#joining_nodes)
Also, sometimes the heuristic wrongly joins some junctions. These can be
excluded by giving them as a list to the option **--junctions.join-exclude** {{DT_IDList}}\[,{{DT_IDList}}\]\*.

When leaving junctions unjoined, there is a high risk of getting low
throughput, jams and even deadlocks due to the short intermediate edges
and the difficulty in computing proper traffic light plans for the
junction clusters.

## Traffic Lights

### Interpreting traffic light information in OSM

[netconvert](../../netconvert.md) prefers each intersection to be
represented by a single node with a single traffic light controller. To
achieve the former, see [\#Junctions](#junctions). To achieve
the latter some extra options are recommended. OSM often uses nodes
ahead of an intersection to represent the position of traffic light
signals. The actual intersection itself is then not marked as
controlled. To interpret these structures the option **--tls.guess-signals** and **--tls.guess-signals.dist** {{DT_FLOAT}} may be used.
To cover the cases where this heuristic fails, the options below may be
used to computed a joint tls plan for multiple nodes.
To identify the guessed signals in a network they all start with the prefix *GS_*
before the node id.

### Joining traffic lights

OSM does not have the possibility to assign several nodes to a single
traffic light. This means that near-by nodes, normally controlled by one
traffic light system are controlled by two after the network is
imported. It is obvious that traffic collapses in such areas if both
traffic lights are not synchronized. Better representation of the
reality can often be achieved by [joining nearby junctions into a single
junction](#junctions). However, if the junctions should stay
separate, it is possible to at least generate a joint controller by
setting the option **--tls.join**. For fine-tuning of joint traffic lights, the
attribute `tl` can be [customized for individual
nodes](#junctions). The joined traffic lights get the id
*joinedS_id0_id1* where *id0* and *id1* are the junction ids. If there are
more nodes in the join than given by **--max-join-ids** (default 4)
the id will be abbreviated to something like *joinedS_id0_id1_id2_id3_#5more*
(for a 9 node cluster).

If you want to let netconvert guess joined traffic lights at node clusters
which were not joined previously (e.g. by using **--junctions.join**) and which do
not have traffic lights assigned in the input, you can use the option **--tls.guess.joining**.
This is not recommended in general. The traffic lights joint by this method get the id
*joinedG_id0_id1* where *id0* and *id1* are the junction ids and the same abbreviation rules as above apply.

### Debugging missing traffic lights

Occasionally intersections that should be TLS-controlled are set to
uncontrolled in the exported *.net.xml*-file. This may either be due to
lack of data in OSM, or due to the invalid interpretation of that data
by [netconvert](../../netconvert.md). Either of the following steps
may be useful to diagnose the problem:

- run [netconvert](../../netconvert.md) **without** the options **--tls.discard-loaded --tls.discard-simple**
- run [polyconvert](../../polyconvert.md) with a type-file that
  contains `<polygonType id="highway" name="highway"` and then look at the generated POIs in
  [sumo-gui](../../sumo-gui.md#loading_shapes_and_pois). They should
  include all traffic light locations defined in the OSM file.

### Overriding the traffic light information

If the traffic light information embedded in the OSM file does not fit
your needs, you can strip it with **--osm.discard-tls** option in
[netconvert](../../netconvert.md) and then provide your own definition
in a separate
[\*.nod.xml](../../Networks/PlainXML.md#node_descriptions)
file in a second run of [netconvert](../../netconvert.md):

```
# 1. Import the OSM file to SUMO, discarding TLS information.
netconvert --osm-files berlin.osm.xml --output-file berlin-without-tls.net.xml \
   --osm.discard-tls

# 2. Set traffic light information.
netconvert --sumo-net-file berlin-without-tls.net.xml --node-files tls-controlled-nodes.nod.xml \
   --output-file berlin-with-tls.net.xml
```

where **tls-controlled-nodes.nod.xml** overwrites the type of node to
**"traffic_light"**. If the node already exists (which is usually the
case) you don't have to provide any information other than the node's ID
and new node type.

## Highway On- and Off-Ramps

OSM networks often lack additional lanes for highway on- and off-ramps.
They can be guessed via [netconvert](../../netconvert.md) using the **--guess-ramps**
option.

## Isolated Edges

When dealing with strictly vehicular scenarios it usually helps to add
the option

```
--remove-edges.isolated
```

To discard edges which have no predecessor and no successor edge.
However, this often causes the removal of railways or waterways which is
not desirable for multi-modal scenarios.

## Shared Space for Trams and Road Vehicles

In the OSM database, shared space for tram and road vehicles is often modelled with distinct elements that occupy the same space. When imported directly, this would allow those modes of traffic to ignore each other due to running on different edges.
To fix this, the option **--edges.join-tram-dist FLOAT** may be used. When this option is set (values between 1 and 2 are recommended), overlapping OSM elements will be converted to road lanes with shared permissions (indicated by a dark purple in [sumo-gui](../../sumo-gui.md#default_coloring)).


## Further way/edge attributes

When setting option **--output.street-names**, the 'name' attribute of every edge will be set to the street name defined in the OSM data.

When setting option **--osm.all-attributes**, all OSM tags of a way are exported as [generic params](../../Simulation/GenericParameters.md) of the edge.
If only a specific selection of tags should be imported, this can be set with option **--osm.extra-attributes**.

## Railway signals

By default, rail signals of category "main" and "combined" are imported. In case the region of interest doesn't provide the necessary level of detail it is possible to customize the import rules using option **--osm.railsignals**. The following values are supported

- *ALL*: imports all signals (key="railway" and value="signal")
- KEY=VALUE: imports all nodes with a tag where key is "railway:signal:KEY" and value is VALUE
- KEY=VALUE,KEY2=VALUE2,...: as above with multiple combinations
- KEY=: imports all nodes with a tag where key is "railway:signal:KEY" (also supports giving a list)
- *DEFAULT*: equivalent to option value "main=,combined="

# Editing OSM networks

## JOSM

*From George Dita, on 01.07.2009* [JOSM](https://josm.openstreetmap.de/)
can be used to edit OSM-data (i.e. for trimming a rectangular map and
deleting unwanted features). After you delete the part that does not
interest you, you have to alter the file using xmlstarlet which actually
deletes the nodes.

xmlstarlet can be used like this:

```
xmlstarlet ed -d "/osm/*[@action='delete']" < input.osm > output.osm
```

!!! caution
    Up to version 4279 of JOSM, nodes and ways created or modified by JOSM are assigned a negative ID. With each run of JOSM, these IDs are recalculated. Please do not rely on them in your SUMO files. If you decide not to upload your changes to OpenStreetMap, you can remove the minuses in the IDs, assure that IDs are unique and then safely refer to them in SUMO files.

## OSMOSIS

*From Christian Klotz, on 01.07.2009, tip by Christoph Sommmer*

The java tool osmosis (https://wiki.openstreetmap.org/index.php/Osmosis)
can be used to filter out unwanted features from an OSM-file. The
following command keeps motorways and motorway links while filtering out
everything else:

```
java -jar osmosis.jar --read-xml file="orginal.osm.xml" --way-key-value \
    keyValueList="highway.motorway,highway.motorway_link" \
    --used-node --write-xml file="filtered.osm.xml"
```
## osmfilter / osmconvert

[osmfilter](https://wiki.openstreetmap.org/wiki/Osmfilter) is a command line tool used to filter OpenStreetMap data files.
It can be used to preprocess the data and can save a lot of work for netconvert.
With this tool it is possible to import the top-level roads for a whole country within minutes of processing time.
It is recommended to use the associated tool [osmconvert](https://wiki.openstreetmap.org/wiki/Osmconvert) to convert the data into the '.o5m' format and back to speed up the work on large datasets. The tools can also be used to cut an osm file to a boundary box like this (be aware that the order of coordinates is lon,lat):
```
osmconvert -b=10.54,52.257,10.545,52.26 fullnet.osm.xml > myplace.osm.xml
```

# netconvert Details

## Road Types

When importing road networks, [netconvert](../../netconvert.md)
searches for the street type, encoded in OSM as a key/value-pair where
the key is either "*highway*", "*railway*" or "*waterway*". Only if such
a key occurs in the edge definition, the edge is imported (see also
below). The edge's type name is built from the found key/value pair by
building a name as: *<KEY\>*.*<VALUE\>*. Using this type name, the edge's
attributes are determined using a predefined map of type names to type
definitions. It is possible to override the default types with own type
definitions. This is documented in the article about the [SUMO edge type
file](../../SUMO_edge_type_file.md).

If no explicit type file is given, the default one located at
[{{SUMO}}/data/typemap/osmNetconvert.typ.xml]({{Source}}data/typemap/osmNetconvert.typ.xml)
is used. If you want to change the values to add pedestrian
infrastructure or have bidirectional railway edges you may want to load
additional type maps from
[{{SUMO}}/data/typemap/]({{Source}}data/typemap/).

!!! caution
    The values in those type maps were set-up ad-hoc and are not yet verified. It would be a great help if someone would revisit and improve them. Please let us know.

## Explicit Road Attributes

In case an edge contains the definition about the number of lanes
(key="*lanes*") or the allowed speed (key="*maxspeed*"), this
information is used instead of the according type's value. Also, the
per-edge information whether the edge is a one-way edge is read
(key="*oneway*"). In case the edge belongs to a roundabout
(key="*junction*" and value="*roundabout*"), it is also set as being a
one-way edge.

## Dismissing unwanted traffic modes

In most cases, tracks and edges which not may be crossed by motorised
traffic are not interesting for road traffic research. It is possible to
exclude these edges from being imported using the
[netconvert](../../netconvert.md)-option **--remove-edges.by-vclass** {{DT_STR}}\[,{{DT_STR}}\]\*.

For removing all edges which can not be used by passenger vehicles the
call must be extended by:

```
--remove-edges.by-vclass hov,taxi,bus,delivery,transport,lightrail,cityrail, \
  rail_slow,rail_fast,motorcycle,bicycle,pedestrian
```

For removing all edges which can not be used by road vehicles the call
must be extended by:

```
--remove-edges.by-vclass rail_slow,rail_fast,bicycle,pedestrian
```

## Relationship between OSM ids and SUMO-ids

For the most part, the relationship between OSM and .net.xml is simple:

- OSM node id "1234" translates into junction id "1234".
- OSM way id "5677" translates into multiple edge ids "5678\#0",
  "5678\#1", "5678\#2", "-5678\#0", "-5678\#1", "-5678\#2"

OSM ways are mostly bi-directional and cross multiple intersections. In
.net.xml they are split at each intersection with a running index \#n
and the edges with prefix '-' are those in the opposite direction.

By setting the option **--output.original-names**, each lane in the network will record the element
name from the input network that it is derived from in a child element:

```xml
<param key="origID" value="...."/>
```

Some OSM elements may not appear in the created network because they are
joined with other elements or converted to (unnamed) geometry points
along an edge (due to option **--geometry-remove**).
Thus, a sequence of ways (i.e. "42", "43", "57") without intersection, would be joined into a single SUMO edge using the id of the first way ("42").

Also, option [**--junctions.join**](#junctions) can lead to the removal of some ways and nodes. Multiple nodes ("1", "2") are joined into "cluster_1_2" and the edges that are within the cluster are converted into internal edges named ":cluster_1_2_INDEX".

## Warnings during Import

Several types of warnings and errors with different levels of severity
may be issued during OSM import.

| Message                                        | Explanation                                          | Recommended Action                     |
| ---------------------------------------------- | ---------------------------------------------------- | ----------------------------------------------- |
| Warning: Discarding unusable type ...  / Discarding unknown compound ... in type ...                  | Lists `<way>` types that are not mentioned in the type file                         | Can be safely ignored in most cases (unless the user edited the type-file)                      |
| Warning: The referenced geometry information (ref='...') is not known       | Unknown osm node references during import.                                   | Can be safely ignored in most cases (unless the user edited the OSM file)                       |
| Warning: Discarding way '...' because it has only 1 node(s)                 | Incomplete data in the OSM file (typically at the boundary of the data set). | Can be safely ignored in most cases (unless the user edited the OSM file)                       |
| Warning: Discarding unusable type "...." (first occurrence for edge "....") | Unknown edge types are ignored during import.                                | Ignore or provide an [Edge-type file](../../SUMO_edge_type_file.md) which contains that type. |
| Warning: Ignoring restriction relation ...                                  | Some data is missing within the OSM file.                                    | Ignore, because this relation most likely falls outside the boundaries of the road network.     |
| Warning: Direction of restriction relation could not be determined          | Some data is missing within the OSM file.                                    | Ignore, because this relation most likely falls outside the boundaries of the road network.     |

[Additional warnings are described
here](../../netconvert.md#warnings_during_import).

# Importing large Networks

It is possible to build SUMO-networks for very large areas (i.e. the
whole of Scotland) but some precautions should be taken:

- reduce the network size with the following options

```
--no-internal-links, --keep-edges.by-vclass passenger,
  --remove-edges.by-type highway.track,highway.services,highway.unsurfaced
```

- if you only need major roads you can reduce the network further by
  setting the option

```
--keep-edges.by-type
  highway.motorway,highway.motorway_link,highway.trunk,highway.trunk_link,highway.primary,highway.primary_link
```

- if your network is larger than \~500MB make sure you use the 64bit
  version of sumo and use a computer with plenty of RAM

# Importing Public Transport Data

netconvert and further SUMO tools can be used to import public transpot
data from OSM. The easiest way to do this is by using the
[OSM-Web-Wizard tool](../../Tutorials/OSMWebWizard.md). Performing the
import without the wizard is explained in
[Tutorials/PT_from_OpenStreetMap](../../Tutorials/PT_from_OpenStreetMap.md).

The following data is imported:

- Public transport stops
- Public transport lines that service these stops

Public transport schedules which are needed to make use of the above data
are generated with a user-defined service period based on a simulation
of the lines.

# Importing Sidewalks

By default (osmNetconvert.typ.xml), only dedicated edges for pedestrians will be built and all other roads (except motorway) will permit pedestrians on all lanes.
For a [pedestrian simulation](../../Simulation/Pedestrians.md#building_a_network_for_pedestrian_simulation), sidewalks are needed and pedestrians should be forbidden on most road lanes.

OSM data coverage and definition style for sidewalks varies by region which may require different import options to achieve adequate sidewalk coverage.

## Sidewalks from typemap

By adding a typemap such as {{SUMO_HOME}}/data/osmNetconvertPedestrians.typ.xml, sidewalks of a pre-configured width a added to a specific set of road types and all non-sidewalk lanes are forbidden for pedestrians.

OSM-sidewalk data will only be considered if it explicitly disables sidewalks on an edge.
This ensures good sidewalk coverage but may lead to double-sidewalks if OSM modellers have added the "sidewalks" as parallel foot paths edges.

## Sidewalks from OSM

By setting option **--osm.sidewalks**, all sidewalk data from OSM will be loaded. When combined with a typemap such as {{SUMO_HOME}}/data/osmNetconvertPedestrians.typ.xml, the typemap will only be used to configure sidewalk widths but no extra sidewalks will be added.

When **--osm.sidewalks** is set, by setting option **--osm.crossings** the "highway=crossing" tags will be loaded and crossings will be generated.

This definition style prevents double-sidewalks but may lead to missing sidewalks wherever OSM modellers did not add sidewalk information.

[osmWebWizard](../../Tutorials/OSMWebWizard.md) uses this style beginning with version 1.11.0.

# Importing OSM Data via Python/ Overpass API

Another way to get OSM data is to query via the Overpass API, e.g. with
python.

Python Example (Get OSM River Data from OSM in BBox)

```
   import requests
   import json
```

```
   def getData(lsouthern-most latitude, western-most longitude, northern-most latitude, eastern-most longitude):
     query = 'way["waterway"="river"]["ship"="yes"](bbox:%s, %s, %s, %s)' %(southern-most latitude, western-most longitude, northern-most latitude, eastern-most longitude)
     overpass_url = "https://overpass-api.de/api/interpreter"
     overpass_query = """
     [out:json][timeout:25];
     (
       """+query+""";
     );
     out body;
     >;
     out skel qt;
     """
     response = requests.get(overpass_url, params={'data': overpass_query})
     data = response.json()
     return data, query
```

Caution: If the data volume is too large, the overpass API aborts.

More information about the query syntax and a GUI for testing the queries
can be found on the website <https://overpass-turbo.eu/>.

# Importing other formats (pbf, o5m)

To use a [pbf](https://wiki.openstreetmap.org/wiki/PBF_Format) or [o5m](https://wiki.openstreetmap.org/wiki/O5m) network in SUMO, you first need to convert it to the OSM format. This can be done using [Osmconvert](https://wiki.openstreetmap.org/wiki/Osmconvert).

# Missing Descriptions

- TLS computation
- computation of lane-2-lane connections
- what is exactly imported (how edge attributes are determined)
- other traffic modes
- Network quality

# References

- <https://www.openstreetmap.org/> - the home site
- <https://www.openstreetmap.de/> - the German home site
- <https://wiki.openstreetmap.org/wiki/Map_features> - information about database attributes
