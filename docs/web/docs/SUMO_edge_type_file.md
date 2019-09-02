---
title: SUMO edge type file
permalink: /SUMO_edge_type_file/
---

A **SUMO edge type file** assigns default values for certain attributes
to types of roads. It configures [NETCONVERT](NETCONVERT.md)
when importing, for example, [SUMO XML
descriptions](Networks/Building_Networks_from_own_XML-descriptions.md),
[OpenStreetMap files](OpenStreetMap_file.md) or [ArcView
files](Networks/Import/ArcView.md). The type configuration is
saved in an XML structure. It is always possible to load multiple type
maps and redefining types partly or completely when loading an
additoinal file.

This article introduces the file format first. After that, it discusses
the application of the SUMO edge type file on certain types of map files
and collects reasonable templates for these map files. *You are very
welcome to provide additional templates for various contexts or to start
a discussion about existing templates.*

## Syntax

The XML root element is called *types*. It contains a collection of
*type* elements. A type element represents a record with the following
attributes:

| Attribute Name | Value Type  | Description                                                                                                                                                                                                                 |
| -------------- | ----------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **id**         | id (string) | The name of the road type. This is the only mandatory attribute. For OpenStreetMap data, the name could, for example, be *highway.trunk* or *highway.residential*. For ArcView data, the name of the road type is a number. |
| allow          | string      | List of allowed vehicle classes. The classes are space separated (e.g. "pedestrian bicycle").                                                                                                                               |
| disallow       | string      | List of not allowed vehicle classes. The classes are space separated too.                                                                                                                                                   |
| discard        | bool        | If "yes", edges of that type are not imported. This parameter is optional and defaults to false.                                                                                                                            |
| numLanes       | int         | The number of lanes on an edge. This is the default number of lanes per direction.                                                                                                                                          |
| oneway         | bool        | If "yes", only the edge for one direction is created during the import. (This attribute makes no sense for SUMO XML descriptions but, for example, for OpenStreetMap files.)                                                |
| priority       | int         | A number, which determines the priority between different road types. NETCONVERT derives the right-of-way rules at junctions from the priority. The number starts with one; higher numbers represent more important roads.  |
| speed          | float       | The default (implicit) speed limit in m/s.                                                                                                                                                                                  |
| sidewalkWidth  | float       | The default width for added sidewalks (defaults to -1 which disables extra sidewalks).                                                                                                                                      |

The names of the SUMO vehicle classes are:

  - By ownership : private, public_transport, public_emergency,
    public_authority, public_army, vip, ignoring,
    By size : passenger (a “normal” car), hov, taxi, bus, delivery,
    transport, lightrail, cityrail, rail_slow, rail_fast, motorcycle,
    bicycle, pedestrian.

So an example of a SUMO edge type file is:

    <types>
        <type id="highway.motorway" priority="13" numLanes="2" speed="44.0" />
        <type id="highway.residential" priority="4" numLanes="1" speed="13.889" />
    </types>

## Templates for OpenStreetMap

The SUMO edge type file defines default values for some tags, which are
taken if no explicit value is given for a certain road in a
OpenStreetMap file. So the OpenStreetMap tags *maxspeed*, *lanes* and
*oneway* override these defaults. If the map is tagged correctly, these
tags should only be omitted if their value equals an implicitly assumed
value. In this section, SUMO edge type files are collected that contain
these implicit values of OpenStreetMap for various contexts. The
following OpenStreetMap pages help to find out the implicit values for
the following attributes:

  - allow/disallow : [Map
    features](http://wiki.openstreetmap.org/wiki/Map_Features), [OSM
    tags for routing/Access
    restrictions](http://wiki.openstreetmap.org/wiki/OSM_tags_for_routing/Access-Restrictions).
    numLanes : [Map
    features](http://wiki.openstreetmap.org/wiki/Map_Features), [Editing
    standards and
    conventions](http://wiki.openstreetmap.org/wiki/Editing_Standards_and_Conventions),
    [Key:lanes](http://wiki.openstreetmap.org/wiki/Key:lanes),
    [Key:highway](http://wiki.openstreetmap.org/wiki/Key:highway) and
    [Tagging scheme by
    country](http://wiki.openstreetmap.org/wiki/Tagging).
    priority : [Map
    features](http://wiki.openstreetmap.org/wiki/Map_Features).
    oneway :
    [Key:oneway](http://wiki.openstreetmap.org/wiki/Key:oneway).
    speed :
    [Key:maxspeed](http://wiki.openstreetmap.org/wiki/Key:maxspeed),
    [OSM tags for
    routing/Maxspeed](http://wiki.openstreetmap.org/wiki/OSM_tags_for_routing/Maxspeed),
    [Key:source:maxspeed](http://wiki.openstreetmap.org/wiki/Key:source:maxspeed)
    and
    [Key:traffic_sign](http://wiki.openstreetmap.org/wiki/Key:traffic_sign).
    Wikipedia lists the [Speed limits by
    country](http://en.wikipedia.org/wiki/Speed_limits_by_country).

The right values are hard to find. Because they depend at least partly
on the legislation (maxspeed, for example); the values vary from country
to country. Further context information, like the fact whether a road is
inside or outside a city border, may determine the defaults as well. In
the following, SUMO edge type files for various contexts are given,
using the country as the main ordering criterion. The lists and files
contain only way types as they correspond to SUMO edges; node and area
types (like highway=services) are not considered. *You are very welcome
to provide additional templates for various contexts or to start a
discussion about existing templates.*

### Germany

In Germany, the legislation for the speed limit distinguishes roads
inside from those outside city borders, roads with a central barrier
separation from those without, and special road like living streets and
pedestrian areas. There is no direct connection to the importance of a
road. However in OpenStreetMap, a motorway is supposed to have such a
central barrier separation per default
[1](http://wiki.openstreetmap.org/wiki/Tag:highway%3Dmotorway). As a
consequence, the speed value only varies between rural and urban
contexts but not between road types, except for motorways, living
streets and pedestrian areas. The two SUMO edge type files are given
next, for the detailed values have a look at the files. References are
mentioned in the table below. The rural type map is the default mapping
when converting OpenStreetMap networks without an explicit typemap.

  -
  -

<table>
<thead>
<tr class="header">
<th><p>id</p></th>
<th><p>Comments</p></th>
</tr>
</thead>
<tbody>
<tr class="odd">
<td><p>highway.motorway</p></td>
<td><p>Autobahn. No legislative speed limit: 44.4 is just a reasonable default. <a href="http://wiki.openstreetmap.org/wiki/Tag:highway%3Dmotorway">2</a> is a bit opposite to <a href="http://wiki.openstreetmap.org/wiki/DE:Tag:highway%3Dmotorway">3</a> regarding oneway tagging.</p></td>
</tr>
<tr class="even">
<td><p>highway.motorway_link</p></td>
<td><p>Links are usually subordinate to the road that leads to the trunk (e.g. a secondary or tertiary road)</p></td>
</tr>
<tr class="odd">
<td><p>highway.trunk</p></td>
<td><p>Similar to Autobahn <a href="http://wiki.openstreetmap.org/wiki/Tag:highway%3Dtrunk">4</a> <a href="http://wiki.openstreetmap.org/wiki/DE:Tag:highway%3Dtrunk">5</a>. Lanes, maxspeed and oneway should be tagged explicitely.</p></td>
</tr>
<tr class="even">
<td><p>highway.trunk_link</p></td>
<td><p>Links are usually subordinate to the road that leads to the trunk (e.g. a secondary or tertiary road)</p></td>
</tr>
<tr class="odd">
<td><p>highway.primary</p></td>
<td><p>Bundesstraße <a href="http://wiki.openstreetmap.org/wiki/Tag:highway%3Dprimary">6</a> <a href="http://wiki.openstreetmap.org/wiki/DE:Tag:highway%3Dprimary">7</a></p></td>
</tr>
<tr class="even">
<td><p>highway.primary_link</p></td>
<td><p>Links are usually subordinate to the road that leads to the trunk (e.g. a secondary or tertiary road)</p></td>
</tr>
<tr class="odd">
<td><p>highway.secondary</p></td>
<td><p>Land-, Staats-, or a major Kreisstraße <a href="http://wiki.openstreetmap.org/wiki/Tag:highway%3Dsecondary">8</a> <a href="http://wiki.openstreetmap.org/wiki/DE:Tag:highway%3Dsecondary">9</a></p></td>
</tr>
<tr class="even">
<td><p>highway.secondary_link</p></td>
<td><p>Links are usually subordinate to the road that leads to the trunk (e.g. a tertiary road)</p></td>
</tr>
<tr class="odd">
<td><p>highway.tertiary</p></td>
<td><p>Kreisstraße, major Gemeindeverbindungsstraße or minor Staats-/Landesstraße <a href="http://wiki.openstreetmap.org/wiki/Tag:highway%3Dtertiary">10</a> <a href="http://wiki.openstreetmap.org/wiki/DE:Tag:highway%3Dtertiary">11</a></p></td>
</tr>
<tr class="even">
<td><p>highway.tertiary_link</p></td>
<td><p>Links are usually subordinate to the road that leads to the trunk (e.g. a tertiary road).</p></td>
</tr>
<tr class="odd">
<td><p>highway.unclassified</p></td>
<td><p>Below tertiary, still interconnecting, not in a residential area <a href="http://wiki.openstreetmap.org/wiki/Tag:highway%3Dunclassified">12</a> <a href="http://wiki.openstreetmap.org/wiki/Tag:highway%3Dresidential">13</a> <a href="http://wiki.openstreetmap.org/wiki/DE:Tag:highway%3Dunclassified">14</a> <a href="http://wiki.openstreetmap.org/wiki/DE:Tag:highway%3Dresidential">15</a></p></td>
</tr>
<tr class="even">
<td><p>highway.residential</p></td>
<td><p>Accessing a residential area, should probably not be outside a town <a href="http://wiki.openstreetmap.org/wiki/Tag:highway%3Dresidential">16</a> <a href="http://wiki.openstreetmap.org/wiki/DE:Tag:highway%3Dresidential">17</a></p></td>
</tr>
<tr class="odd">
<td><p>highway.living_street</p></td>
<td><p>Verkehrsberuhigter Bereich (walking speed). If you leave that street, you have to give way <a href="http://wiki.openstreetmap.org/wiki/Tag:highway%3Dliving_street">18</a></p></td>
</tr>
<tr class="even">
<td><p>highway.pedestrian</p></td>
<td><p>Fußgängerzone. Pedestrians only. (Bicycles should be pushed.) <a href="http://wiki.openstreetmap.org/wiki/Tag:highway%3Dtrack">19</a> <a href="http://wiki.openstreetmap.org/wiki/DE:Tag:highway%3Dpedestrian">20</a></p></td>
</tr>
<tr class="odd">
<td><p>highway.service</p></td>
<td><p>No explicit regulatory speed limit. Usually you have to give way when leaving such a street as it is not part of the road network with moving traffic. <a href="http://wiki.openstreetmap.org/wiki/Tag:highway%3Dservice">21</a></p></td>
</tr>
<tr class="even">
<td><p>highway.services</p></td>
<td><p>Rest area with additional services like a gas station. Usually you have to give way when leaving such a street as it is not part of the road network with moving traffic. <a href="http://wiki.openstreetmap.org/wiki/Tag:highway%3Dservices">22</a></p></td>
</tr>
<tr class="odd">
<td><p>highway.bus_guideway</p></td>
<td><p>For busses only. <a href="http://wiki.openstreetmap.org/wiki/Tag:highway%3Dbus_guideway">23</a></p></td>
</tr>
<tr class="even">
<td><p>highway.track</p></td>
<td><p>For agricultural traffic, usually not used by normal vehicles. <a href="http://wiki.openstreetmap.org/wiki/Tag:highway%3Dtrack">24</a></p></td>
</tr>
<tr class="odd">
<td><p>highway.path</p></td>
<td><p>Usually “not intended for motor vehicles”. <a href="http://wiki.openstreetmap.org/wiki/Tag:highway%3Dpath">25</a></p></td>
</tr>
<tr class="even">
<td><p>highway.cycleway</p></td>
<td><p>“Access is only allowed for bikes” in Germany. <a href="http://wiki.openstreetmap.org/wiki/Tag:highway%3Dcycleway">26</a></p></td>
</tr>
<tr class="odd">
<td><p>highway.footway</p></td>
<td><p>For designated footpaths in Germany. (Bicycles must be tagged explicitely.) <a href="http://wiki.openstreetmap.org/wiki/Tag:highway%3Dfootway">27</a> <a href="http://wiki.openstreetmap.org/wiki/DE:Tag:highway%3Dfootway">28</a></p></td>
</tr>
<tr class="even">
<td><p>highway.bridleway</p></td>
<td><p>For pedestrians, horses and sometimes bicycles. <a href="http://wiki.openstreetmap.org/wiki/Tag:highway%3Dbridleway">29</a></p></td>
</tr>
<tr class="odd">
<td><p>highway.steps<br />
highway.step<br />
highway.stairs</p></td>
<td><p>Steps are usually for pedestrians. <a href="http://wiki.openstreetmap.org/wiki/Tag:highway%3Dsteps">30</a></p></td>
</tr>
<tr class="even">
<td><p>railway.rail</p></td>
<td><p>Typical full sized trains</p></td>
</tr>
<tr class="odd">
<td><p>railway.tram</p></td>
<td><p>Trams</p></td>
</tr>
<tr class="even">
<td><p>railway.light_rail</p></td>
<td><p>S-Bahn <a href="http://wiki.openstreetmap.org/wiki/Tag:railway%3Dlight_rail">31</a></p></td>
</tr>
<tr class="odd">
<td><p>railway.subway</p></td>
<td><p>U-Bahn <a href="http://wiki.openstreetmap.org/wiki/Tag:railway%3Dsubway">32</a></p></td>
</tr>
<tr class="even">
<td><p>railway.preserved</p></td>
<td><p>Historic trains <a href="http://wiki.openstreetmap.org/wiki/Map_Features">33</a> <a href="http://wiki.openstreetmap.org/wiki/Tag:railway%3Dpreserved">34</a></p></td>
</tr>
</tbody>
</table>

### Templates for additional traffic modes in OpenStreetMap

There are additional templates which should be loaded when importing
pedestrian infrastructure (sidewalks), bike lanes, ship movements and
special railways with some comments below:

  -
  -
  -
  -
If your railways have two tracks but are mapped with a single way in
OSM, you may wish to load:

| id                    | Comments                                                                                                                                                                                         |
| --------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| railway.narrow_gauge | Smaller gauge. Assigned it to the SUMO class lightrail. [35](http://wiki.openstreetmap.org/wiki/Tag:railway%3Dnarrow_gauge)\]                                                                    |
| railway.monorail      | “Trains run on one single rail”. Assigned it to the SUMO class lightrail. [36](http://wiki.openstreetmap.org/wiki/Tag:railway%3Dmonorail)                                                        |
| railway.funicular     | “Cable driven inclined railways”. Assigned it to the SUMO class cityrail. [37](http://wiki.openstreetmap.org/wiki/Map_Features) [38](http://wiki.openstreetmap.org/wiki/Tag:railway%3Dfunicular) |

Only few data is available about the railway types. The most important
document might be the [Eisenbahn-Bau- und Betriebsordnung
(EBO)](http://www.gesetze-im-internet.de/bundesrecht/ebo/gesamt.pdf),
but even that did not help me much. So the given values are mostly
personal opinion, except if a reference is given. More references are
very welcome.

## Templates for OpenDrive

The default type map is .

There are additional templates which should be loaded when importing
pedestrian infrastructure (sidewalks) and/or bike lanes:

  -
  -