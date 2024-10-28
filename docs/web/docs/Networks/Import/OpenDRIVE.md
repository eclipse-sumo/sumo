---
title: OpenDRIVE
---

For reading [OpenDRIVE networks](https://www.opendrive.org/), give
[netconvert](../../netconvert.md) the option **--opendrive-files** {{DT_FILE}}\[,{{DT_FILE}}\]\* or **--opendrive** {{DT_FILE}}\[,{{DT_FILE}}\]\* for short.

```
netconvert --opendrive myOpenDriveNetwork.xodr -o mySUMOnetwork.net.xml
```

[netconvert](../../netconvert.md) can also [write OpenDrive
networks](../../Networks/Further_Outputs.md#opendrive_road_objects).

# User Options

## Defining Lane Types to Import

OpenDRIVE allows to assign a lane to an abstract (not pre-defined)
class. Some of the lanes can be used by vehicles, some represent
non-usable building structures, such as curbs. When reading OpenDRIVE
files, [netconvert](../../netconvert.md) determines whether and how a
lane shall be imported by looking into pre-defined and/or loaded [edge
types](../../Networks/PlainXML.md#type_descriptions).
Several known lane types are pre-defined, and shown in the following
table together with their defaults. They can be overwritten using [edge
types](../../Networks/PlainXML.md#type_descriptions)
of same name. The following attributes are retrieved from the [edge
types](../../Networks/PlainXML.md#type_descriptions)
if not set explicitly by the lane within the read OpenDRIVE file:

- width
- maximum allowed speed
- allowed vehicle classes

<center>

**Lane types [netconvert](../../netconvert.md) interprets as driving
lanes**

</center>

| type\@lane  | imported | default speed \[km/h\] | default width \[m\] | allowed vehicle classes |
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

All other types are not imported.

As a conclusion, if you wish to import lanes of type
"`border`", you have to additionally load a
[edge
types](../../Networks/PlainXML.md#type_descriptions)
file like this:

```xml
<types>

  <type id="border" priority="0" numLanes="1" speed="1.39"/>

</types>
```

## Pre-Defined type maps

The following edge-type files are available in {{SUMO_HOME}}/data/typemap for OpenDRIVE import:

- **opendriveNetconvert.typ.xml**: default typemap for importing automotive networks. This will be used if option **--type-files** is not set.
- **opendriveNetconvertBicycle.typ.xml**: can be combined with default typemap to also import bicycle lanes
- **opendriveNetconvertPedestrians.typ.xml**: can be combined with default typemap to also import sidewalks

# Import Process

## Dealing with Lane Sections

OpenDRIVE edges may contain one or more "lane sections". As OpenDRIVE
lane sections may differ in number of lanes for any of the directions,
the importer has to split the imported edge at all lane sections. The
resulting edges' IDs are built from the original edge ID and the offset
of the lane section within the edge (lane section's
`s`-value), divided by a dot ('.') when using the option **--opendrive.position-ids**. As an
example, the first lane section of the edge '100' is named '100.0.00'.
Assuming the edge has a further lane section starting at
`s`=100, a further edge named '100.100.00'
would be built.

The importer checks whether the lane sections are given in the right
order (increasing `s`-value) and resorts them,
if not. Additionally, lane sections that are very short, this means the
`s`-value is near (POSITION_EPS, 0.1 m) to
the last one, are removed. We have seen this only in automatically
generated (scanned) networks.

## Dealing with Speed Changes

OpenDRIVE allows to define changes of the allowed velocity along a lane.
As SUMO road networks do not support such a feature, the imported
OpenDRIVE edge is split at the positions the speed of a lane changes.
Internally, this is handled by building additional lane section. As a
result, the IDs of the edges are built the same way it would be done for
lane sections - the `s`-value of the new lane
section is composed by the original lane section's
`s`-value plus the speed change's
`sOffset`-value.

## Handling of geometry

Geometry in OpenDRIVE takes the form of parametric curves (arcs, spirals and splines). These are all sampled with a configurable precision (**--opendrive.curve-resolution** {{DT_FLOAT}}) to produce polylines in the .net.xml. Junction shapes are not encoded in OpenDRIVE. They are generated based on points where the shapes of normal roads and connecting roads meet.

## Handling of traffic light signaling

OpenDRIVE considers traffic lights by the physical aspect of the signals, their position and the lanes they control. Optionally, signals can be grouped in a `<controller>` element.
The car signals are imported into SUMO by default and organised into one traffic light per intersection. If the option **--opendrive.signal-groups** {{DT_FLOAT}} is set to true, netconvert will try to build the same signal groups in SUMO
as given by the OpenDRIVE `<controller>` items. **This can generate invalid SUMO signal programs (minor "g" vs. major green "G", see [signal state](../../Simulation/Traffic_Lights.md#signal_state_definitions)) or fail due to complex signal group settings.** Currently, signals for cyclists and pedestrians are not imported.

## Referencing original IDs

When using the option **--output.original-names**, each lane will receive a `<param origID="EDGE_ID LANE_INDEX"/>` which can be used with
the [TraCI function moveToVTD](../../TraCI/Change_Vehicle_State.md).

Furthermore, when exporting to OpenDrive with option **--opendrive-output** and using option **--output.original-names**,
each `<road>`-element will get an additional element which references the
corresponding edge id in the *.net.xml* file:

```xml
<userData sumoId="sumo_edge_id"/>
```

## Ignoring misplaced traffic signals

The OpenDrive standard is difficult to follow and especially older versions like 1.4 had some problems. Up to this version, there was no proper way to separate the physical position (pole) of a traffic signal from the logical position (the stop line). Starting from OpenDrive 1.5, the physical position of signals can be defined explicitly. Although the standard underlines that the road the traffic signal is placed on is the one it should control, in some OpenDrive 1.4 networks the signal is placed on the sidewalk (its physical position). For OpenDrive 1.4 networks only, the option **--opendrive.ignore-misplaced-signals** allows to skip the signals defined out of "driving" lanes.

# Road Objects

By setting the option **--polygon-output** {{DT_FILE}}, any road objects present in the input are
exported as [loadable shapes](../../Simulation/Shapes.md). If the
option **--proj.plain-geo true** is set and the input network is geo-referenced, generated shapes
will be written with geo-coordinate as well.

# Supported Versions and Features

[netconvert](../../netconvert.md) aims to support as many features as possible across OpenDRIVE versions.
Version 1.4 should generally be supported.
You can check the feature support per version (and request new featuers) in our issue tracker:

- [OpenDrive 1.4 features](https://github.com/eclipse-sumo/sumo/issues/6694)
- [OpenDrive 1.5 features](https://github.com/eclipse-sumo/sumo/issues/6695)
- [OpenDrive 1.6 features](https://github.com/eclipse-sumo/sumo/issues/8901)
