---
title: Networks Import OpenDRIVE
permalink: /Networks/Import/OpenDRIVE/
---

For reading [OpenDRIVE networks](http://www.opendrive.org/), give
[NETCONVERT](NETCONVERT.md) the option  or  for short.

`netconvert --opendrive myOpenDriveNetwork.xodr -o mySUMOnetwork.net.xml`

[NETCONVERT](NETCONVERT.md) can also [write OpenDrive
networks](Networks/Further_Outputs#OpenDRIVE_Road_Networks.md).

# User Options

## Defining Lane Types to Import

OpenDRIVE allows to assign a lane to an abstract (not pre-defined)
class. Some of the lanes can be used by vehicles, some represent
non-usable building structures, such as curbs. When reading OpenDRIVE
files, [NETCONVERT](NETCONVERT.md) determines whether and how a
lane shall be imported by looking into pre-defined and/or loaded [edge
types](Networks/Building_Networks_from_own_XML-descriptions#Type_Descriptions.md).
Several known lane types are pre-defined, and shown in the following
table together with their defaults. They can be overwritten using [edge
types](Networks/Building_Networks_from_own_XML-descriptions#Type_Descriptions.md)
of same name. The following attributes are retrieved from the [edge
types](Networks/Building_Networks_from_own_XML-descriptions#Type_Descriptions.md)
if not set explicitly by the lane within the read OpenDRIVE file:

  - width
  - maximum allowed speed
  - allowed vehicle classes

<center>

**Lane types [NETCONVERT](NETCONVERT.md) interprets as driving
lanes**

</center>

| type@lane  | imported | default speed \[km/h\] | default width \[m\] | allowed vehicle classes |
| ---------- | -------- | ---------------------- | ------------------- | ----------------------- |
| driving    | x        | 80                     | 3.65                | all                     |
| stop       | x        | 80                     | 3.65                | all                     |
| mwyEntry   | x        | 80                     | 3.65                | all                     |
| mwyExit    | x        | 80                     | 3.65                | all                     |
| special1   | x        | 80                     | 3.65                | all                     |
| parking    | x        | 5                      | 3.65                | all                     |
| sidewalk   | \-       | \-                     | \-                  | \-                      |
| border     | \-       | \-                     | \-                  | \-                      |
| shoulder   | \-       | \-                     | \-                  | \-                      |
| none       | \-       | \-                     | \-                  | \-                      |
| restricted | \-       | \-                     | \-                  | \-                      |
|            |          |                        |                     |                         |

All other types are not imported.

As a conclusion, if you wish to import lanes of type
"<span class="inlxml">border</span>", you have to additionally load a
[edge
types](Networks/Building_Networks_from_own_XML-descriptions#Type_Descriptions.md)
file like this:

    <types>

       <type id="border" priority="0" numLanes="1" speed="1.39"/>

    </types>

# Import Process

## Dealing with Lane Sections

OpenDRIVE edges may contain one or more "lane sections". As OpenDRIVE
lane sections may differ in number of lanes for any of the directions,
the importer has to split the imported edge at all lane sections. The
resulting edges' IDs are built from the original edge ID and the offset
of the lane section within the edge (lane section's
<span class="inlxml">s</span>-value), divided by a dot ('.'). As an
example, the first lane section of the edge '100' is named '100.0.00'.
Assuming the edge has a further lane section starting at
<span class="inlxml">s</span>=100, a further edge named '100.100.00'
would be built.

The importer checks whether the lane sections are given in the right
order (increasing <span class="inlxml">s</span>-value) and resorts them,
if not. Additionally, lane sections that are very short, this means the
<span class="inlxml">s</span>-value is near (POSITION_EPS, 0.1 m) to
the last one, are removed. We have seen this only in automatically
generated (scanned) networks.

## Dealing with Speed Changes

OpenDRIVE allows to define changes of the allowed velocity along a lane.
As SUMO road networks do not support such a feature, the imported
OpenDRIVE edge is split at the positions the speed of a lane changes.
Internally, this is handled by building additional lane section. As a
result, the IDs of the edges are built the same way it would be done for
lane sections - the <span class="inlxml">s</span>-value of the new lane
section is composed by the original lane section's
<span class="inlxml">s</span>-value plus the speed change's
<span class="inlxml">sOffset</span>-value.

## Referencing original IDs

When using the option , each lane will receive a  which can be used with
the [TraCI function moveToVTD](TraCI/Change_Vehicle_State.md).

Furthermore, when exporting to OpenDrive with option  and using option ,
each -element will get an additional element which references the
corresponding edge id in the *.net.xml* file:

<userData sumoId="sumo_edge_id"/>

# Road Objects

By setting the option , any road objects present in the input are
exported as [loadable shapes](Simulation/Shapes.md). If the
option  is set and the input network is geo-referenced, generated shapes
will be written with geo-coordinate as well.