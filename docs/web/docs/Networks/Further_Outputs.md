---
title: Further Outputs
---

[netconvert](../netconvert.md),
[netgenerate](../netgenerate.md), and [netedit](../Netedit/index.md)
allow to generate additional output files besides writing a SUMO network
file. They will be presented in the following.

If no other output option is given, [netconvert](../netconvert.md)
and [netgenerate](../netgenerate.md) will write the result of
network import/generation as a SUMO network file into "net.net.xml".

Additional network formats such as MATSim and OpenDRIVE can also be generated.
This is described in the [network export documentation](Export.md).

## Public Transport Stops

The option **--ptstop-output** {{DT_FILE}} causes an {{AdditionalFile}} to be written that contains `<busStop/>` elements for the
imported network. These can be loaded directly into
[sumo](../sumo.md) or further modified with
[netedit](../Netedit/index.md).

## Public Transport Lines

The option **--ptline-output** {{DT_FILE}} causes a data file to be written that contains information
on public transport lines. These can be
[processed further](../Tutorials/PT_from_OpenStreetMap.md#finding_feasible_stop-to-stop_travel_times_and_creating_pt_schedules)
to generate public transport schedules for simulating public transport
and intermodal traffic. This is done automatically when using the
[osmWebWizard tool](../Tutorials/OSMWebWizard.md)

The ptline data format is described below:

```xml
<ptLines>
    <ptLine id="0" name="M2: Alexanderplatz to Heinersdorf" line="M2"
      type="tram" period="1200" completeness="0.11">
        <busStop id="-1615531689" name="S+U Alexanderplatz/Dircksenstraße"/>
        <busStop id="30732068" name="Memhardstraße"/>
    </ptLine>
  ...
</ptLines>
```

The above describes a public transport line which serves two stops with
a **period** of 1200 seconds. The line name as well as the stop names
are optional and only serve to enhance human reader comprehension. The
*tram* **type** is one of the recognized public transport types from OSM
(other allowed types are
*train,subway,light_rail,monorail,trolleybus,aerialway,ferry*). The
**line** attribute will be used in the simulation when distinguishing
public transport lines. The optional **completeness** attribute serves to
inform the human reader that the stops only describe 11% of the complete
line. Incomplete lines are typical when importing only a part of the
complete public transport network.

## Information on Joined Junctions

The option **--junctions.join-output** {{DT_FILE}} causes a file to be written that specifies the junctions
which were joined (usually due to option **--junctions.join**). The resulting output file is
suitable for loading with the **--node-files** option.

## Street Signs

The option **--street-sign-output** {{DT_FILE}} causes a file with
[POIs](../Simulation/Shapes.md#poi_point_of_interest_definitions)
to be written. These POIs encode the type of street signs that are
encountered on each edge and can be loaded as {{AdditionalFile}} in
[sumo-gui](../sumo-gui.md). Currently used sign types are:

- priority
- yield
- stop
- allway_stop
- right_before_left

## Parking Areas

Currently, importing road-side [parking
areas](../Simulation/ParkingArea.md) from OSM is supported by
setting the option **----parking-output** {{DT_FILE}}

## OpenDRIVE road objects

When loading an OpenDRIVE file, [embedded road objects can be imported
as well.](../Networks/Import/OpenDRIVE.md#road_objects)
They will be written to a file specified by option **--polygon-output**.

## VISUM Districts

When loading a visum network (**--visum-file**) the option **--taz-output** can be used to export embedded district information.

## Railway Topology

The option **--railway.topology.output** causes a file for analyzing the topology of railway networks
to be written. This is useful when analyzing network problems in regard
to bi-directional track usage and to evaluate the effect of option **--railway.topology.repair**.

## Additional Information within the output file

The option **--output.street-names** {{DT_BOOL}} ensures that street names from suitable input networks such
as [OSM](../Networks/Import/OpenStreetMap.md) or
[OpenDRIVE](../Networks/Import/OpenDRIVE.md) are included in the
generated *.net.xml* file.

When reading or writing OpenDRIVE networks, the option **--output.original-names** {{DT_BOOL}} [writes
additional data for mapping between sumo-ids and OpenDRIVE-ids into the
generated
networks](../Networks/Import/OpenDRIVE.md#referencing_original_ids).
