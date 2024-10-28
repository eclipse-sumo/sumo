---
title: Changes in the 2015 releases (versions 0.23.0, 0.24.0 and 0.25.0)
---

## Version 0.25.0 (07.12.2015)

### Bugfixes

- Simulation
  - Fixed crashing and deadlocks when performing [routing in the simulation based](../Demand/Automatic_Routing.md) on
    [districts](../Demand/Importing_O/D_Matrices.md#describing_the_taz). #1938
  - Fixed bug that was hindering lane-changes due to invalid
    cooperative speed adaptations. #1971
  - Fixed bug that was causing erratic lane changes when using
    subsecond simulation. #1440
  - Fixed bug that was causing erratic lane changes in front of
    intersections. #1856
  - Fixed right-of-way in regard to vehicles that were driving
    across the same intersection twice. #2023
  - Vehicles waiting to enter a roundabout no longer yield to other
    vehicles outside the roundabout. #1847
  - Pedestrians no longer walk past their specified arrival
    position. #1780
  - Fixed asymmetrical pedestrian behavior when walking to a
    *busStop*. Now they always walk to the middle of the busstop
    rather than to its *endPos*. #1562

- sumo-gui
  - Fixing crash when selecting *Show all routes* from the vehicle
    menu.
  - When loading a gui-settings-file from the *View Settings*
    dialog, the delay value is now correctly applied.
  - Fix bug that caused giant circles to appear when exaggerating
    the width of lanes with short geometry segments.
  - Vehicle names are now drawn for vehicles that occupy multiple
    edges. #1960
  - Fixed drawing of link indices, link rules, lane markings and bus
    stops for left-hand networks.
  - Fixed error when reloading a network with `<param>`-elements. #1979

- netconvert
  - Fixed missing connections in multi-modal networks. #1865
  - Fixed bug that caused invalid pedestrian crossings to be
    generated after importing a *.net.xml* file. #1907
  - Fixed bug that caused pedestrian crossings to change their
    priority when importing a *.net.xml* file. #1905
  - Fixed geometry bug when building pedestrian crossings and
    walkingareas for left-hand networks.
  - Fixed invalid network after deleting edges at a joined traffic
    light with controlled pedestrian crossing. #1902
  - Motorway ramps are no longer guessed if the lane permissions
    indicate that the edge is not a motorway. #1894
  - Motorway ramps are no longer guessed at roundabouts. #1890
  - Fixed some cases when roads where invalidly guessed to be
    roundabouts. #1933
  - When importing a *.net.xml* file the resulting network is no
    longer modified due to automatic joining of edges that connect
    the same nodes. #1930
  - When importing a *.net.xml* file with pedestrian crossings and
    setting option **--no-internal-links**, the crossings and walkingareas are removed from
    the resulting network. #1730
  - Several fixes in regard to OpenDrive networks:
    - Fixed missing `<laneLink>` elements when exporting networks as
      [OpenDrive](../Networks/Import/OpenDRIVE.md). #1973
    - Now successfully importing [OpenDrive networks](../Networks/Import/OpenDRIVE.md) with
      dead-end edges. #1692
    - Fixed imprecise geometry when importing
      [OpenDrive](../Networks/Import/OpenDRIVE.md) networks.
    - Fixed imprecise geometry when exporting networks as
      [OpenDrive](../Networks/Import/OpenDRIVE.md). #2031
    - Fixed invalid geometry of lanes within intersections when
      exporting networks as
      [OpenDrive](../Networks/Import/OpenDRIVE.md).
  - Fixed crash when specifying inconsistent [tllogic input](../Networks/PlainXML.md#traffic_light_program_definition). #2010
  - When patching an existing network with *.nod.xml* file, existing
    traffic light programs are now preserved unless changes are
    specified explicitly.
  - No longer patching loaded traffic light programs for new
    crossings if they already have the correct state size.
  - When importing a *.net.xml* which was built for left-hand
    traffic, the resulting network will also be built for left-hand
    traffic. #1880
  - When importing a *.net.xml*, generated networks will retain the
    same value of **--junctions.corner-detail**) as the input network. #1897
  - Fixed invalid geo-referencing in left-hand networks. #2020
  - Traffic lights that control multiple intersections no longer
    create unsafe right-of-way rules. The edges that lie within the
    traffic light are now controlled according to the appropriate
    right of way (This does not necessarily model physical traffic
    signals but reflects the behavior of drivers). #812
    - Old signal plans for these *joined* traffic lights no longer
      work for new networks since more link states need to be
      defined. The option **--tls.uncontrolled-within** {{DT_BOOL}} was added to build networks that are
      compatible with old-style signal plans. Note, that this may
      create unsafe intersections, causing collisions.

- netedit
  - When renaming an edge, the lane IDs are now updated as well. #1867
  - Fractional widths can now be set when inspecting edges.
  - Modifying traffic light plans which control multiple nodes is
    now working. #2009

- duarouter
  - Fixed invalid error when compiled without the FOX library. #1956

- od2trips
  - Option **--begin** is now working. #1889

- MESO-GUI
  - Loading of edge-scaling schemes from a *gui-settings-file* is
    now working.
  - The front of each vehicle queue is now drawn at the start of its
    segment.

- TraCI
  - Fixed bug that prevented the [C++ TraCI library](../TraCI/C++TraCIAPI.md) from functioning. #2007
  - [Vehicle command](../TraCI/Change_Vehicle_State.md) *set speed* can now be used in conjunction with *move to VTD*. #1984
  - When using [Vehicle command](../TraCI/Change_Vehicle_State.md) *move to VTD*,
    the speed is set according to the covered distance where this is
    deemed plausible (the value of *set speed* overrides this).

- Tools
  - [osmWebWizard.py](../Tools/Import/OSM.md) no longer fails
    when encountering path names with space-characters in them.
  - [osmWebWizard.py](../Tools/Import/OSM.md) no longer nests
    output directories when generating multiple scenarios.

### Enhancements

- Simulation
  - [Vehicle types](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#abstract_vehicle_class)
    now have `vClass="passenger"` by default. Earlier, the default was `ignoring` which would let
    the vehicles drive along footpaths and railways.
  - [Zipper merging](https://en.wikipedia.org/wiki/Merge_%28traffic%29) is
    now supported (see netconvert enhancement below). #1292
  - Added person statistics to [verbose output](../Simulation/Output/index.md#commandline_output_verbose). #1898
  - Now warning about jammed pedestrians.
  - Now warning about pedestrians "collisions".
  - Now warning about traffic lights where one link never gets a green light.
  - It is now possible to [modify the offset of an existing tls program](../Simulation/Traffic_Lights.md#modifying_existing_tls-programs)
    without loading a completely new `tlLogic`-definition. #1940
  - Added option **--duration-log.statistics** {{DT_BOOL}} which causes average vehicle trip data to be
    [printed in verbose mode](../Simulation/Output/index.md#commandline_output_verbose)
    (average route length, travel time and time loss, ...) for quick
    evaluation of a scenario. #1911
  - The option **--max-num-vehicles** {{DT_INT}} can now be used to maintain a constant number of
    vehicles in the network. Vehicle insertions are delayed whenever
    this number would be exceeded. Previously this option would
    terminate the simulation when the number was exceeded. To avoid
    a large number of delayed vehicles it is recommended to use the
    option **--max-depart-delay** {{DT_TIME}}. #1958
  - Traffic detectors which are generated for [actuated traffic lights now support additional parameters](../Simulation/Traffic_Lights.md#type_actuated)
    to allow writing output files the same way as [regular detectors](../Simulation/Output/Induction_Loops_Detectors_(E1).md). #1839
  - Angles in simulation output and TraCI results now conform to
    *Navigational Standards* with *0* pointing towards the North and
    *90* pointing due East. #1372

- netedit
  - [netedit](../Netedit/index.md) is now open. Have fun.
  - Int and float options can now be set in the
    *Processing-\>Options* dialog.
  - Added many lane- and junction-coloring modes already known from
    [sumo-gui](../sumo-gui.md#edgelane_visualisation_settings). #1756
  - Pedestrian crossings are now supported when editing traffic
    light plans.
  - Attributes of pedestrian crossings can now be modified.
  - Added context-menu option for removing intermediate geometry
    points from selected edges. #1913
  - Vehicle class permissions can now be edited via check-boxes
    instead of typing all class names. #1909
  - Individual lanes and selections of lanes can now be deleted when
    unchecking *Select edges*. #1895
  - A lane (or a selections of lanes) can now be duplicated by
    selecting *Duplicate lane* from the context menu. #1896
  - *Selection Mode* now allows [additional operators](../Netedit/editModesCommon.md#select) when matching against a
    non-numerical attribute. #1923
  - Added new option *Copy edge name to clipboard* to the lane
    popup-menu.
  - Junction attribute *keepClear* is now supported.
  - Custom junction shapes can now be drawn by selecting *Set custom
    shape* from the junction popup-menu. This will create a
    [modifiable shape outline. The popup-menu of this outline allows saving, discarding and simplifying the shape.](../Netedit/neteditPopupFunctions.md#modifiable_poly)
  - Added *reload* menu option. #2029
  - When editing traffic light plans, [states can now be set for multiple links and multiple phases at the same time](../Netedit/editModesNetwork.md#traffic_lights).

- sumo-gui
  - Persons can now be tracked by selecting *Start Tracking* from
    the context menu. #1779
  - The current route of pedestrians can now be shown by selecting
    *Show Current Route* from the context menu. #1906
  - Error messages can now by clicked for jumping to the referenced
    simulation object (i.e. a teleporting vehicle). #900
  - Added person statistics to network parameter dialog. #1898
  - Added new menu option *Edit-\>Open in netedit* for opening the
    current network (at the current location) in
    [netedit](../Netedit/index.md).
  - Added new option *Copy edge name to clipboard* to the lane
    popup-menu.
  - Added new options *Close edge* and *Close lane* to the lane
    popup-menu. This will force vehicles (with an assigned vClass)
    to wait until the corresponding lanes have been reopened (also
    via popup-menu).
  - Added new option *Add rerouter* to the lane popup-menu. This
    will make vehicles recompute their route when entering that
    edge.
  - The size and color of link indices can now be customized (old
    [gui settings files](../sumo-gui.md#changing_the_appearancevisualisation_of_the_simulation)
    may have to be updated).
  - Average trip data (for completed vehicle trips) is now available
    in the network parameter dialogue when running with option **--duration-log.statistics**.
  - Added new junction visualization option *draw
    crossings/walkingareas*. #1899
  - Vehicles can now be colored *by depart delay* (the differences
    between desired and actual depart time). Depart delay was also
    added to the vehicle parameter dialog. #1970
  - Junction shapes are no longer drawn when their color is set to
    fully transparent.
  - The network version is now shown in the network parameter
    dialog.

- netconvert
  - [Zipper merging](https://en.wikipedia.org/wiki/Merge_%28traffic%29) is
    now supported via the new [node type *zipper*](../Networks/PlainXML.md#node_types). #1292
  - [Right-turn-on-red](https://en.wikipedia.org/wiki/Right_turn_on_red)
    is now supported via the new [node type *traffic_light_right_on_red*](../Networks/PlainXML.md#node_types). #1287
  - Importing *.inpx* [VISSIM networks](../Networks/Import/Vissim.md) is now supported.
    Thanks to the [AIT](https://www.ait.ac.at/) for their
    contribution.
  - The positioning and presence of [internal junctions](../Networks/SUMO_Road_Networks.md#internal_junctions)
    can now be [customized with the new connection attribute `contPos`](../Networks/PlainXML.md#explicitly_setting_which_edge_lane_is_connected_to_which). #2016
  - The maximum number of connections per junction was raised from
    64 to 256.
  - Added options **--geometry.remove.keep-edges.explicit** {{DT_STR}} and **--geometry.remove.keep-edges.input-file** {{DT_FILE}} to exclude edges from being modified when
    using option **--geometry.remove**. #1929
  - When specifying [multiple connections from the same edge to the same target lane](../Networks/PlainXML.md#multiple_connections_from_the_same_edge_to_the_same_target_lane),
    safe right-of-way rules are now established among the
    conflicting connections. #1859
  - Added options **--tls.cycle.time** {{DT_TIME}} to ensure that heuristically generated traffic
    light plans have a fixed cycle length. The new default is *90*
    (s) which will have no effect on most 4-arm intersections but
    will cause different timings for controlled 3-arm intersections
    and other types. #1942
  - Added options **--edges.join** {{DT_BOOL}} to select whether edges that connect the same
    nodes (and have similar geometry) shall be joined into an edge
    with multiple lanes. The new default is *false* (before, this
    heuristic was always active). #1930
  - Street names are now imported form
    [OpenDrive](../Networks/Import/OpenDRIVE.md).
  - Now including sumo edge-ids in [OpenDrive export](../Networks/Further_Outputs.md#opendrive_road_objects)
    if option **--output.original-names** is given (as `<userData sumoId="sumo_edge_id"/>`).
  - Now using more lane types in [OpenDrive export](../Networks/Further_Outputs.md#opendrive_road_objects).
  - Added option **--default.junctions.radius** {{DT_FLOAT}} for setting the default `radius` of
    [nodes](../Networks/PlainXML.md#node_descriptions). #1987
  - Added option **--tls.left-green.time** {{DT_TIME}} to configure the default duration for the
    dedicated left-turn phase. A value of 0 disables building this
    phase. #1949
  - Added option **--sidewalks.guess.exclude** {{DT_STR}} to prevent guessing a sidewalk for the given list
    of edges.
  - Added option **--junctions.internal-link-detail** {{DT_INT}} for setting the number of geometry points for
    lanes within intersections (Before, this was hard-coded as *5*). #1988
  - Added option **--crossings.guess.speed-threshold** {{DT_FLOAT}}. This allows setting the speed threshold above
    which crossings will not be generated at uncontrolled nodes
    (before this was hard-coded to *13.89*m/s). #1819

- polyconvert
  - Added option **--fill** {{DT_BOOL}} to control whether polygons are filled by default
    or not. #1858
  - Added option **--shapefile.fill** {{DT_STR}} to override the fill state when importing
    shapefiles. Allowed values are **\[auto|true|false\]**.

- MESO-GUI
  - It is now possible to select individual vehicles, to examine
    their parameters, track them and show their route just like for
    [sumo-gui](../sumo-gui.md). #1932
  - Vehicles can now be located as in
    [sumo-gui](../sumo-gui.md). #1487

- Tools
  - [traceExporter.py](../Tools/TraceExporter.md) now supports
    exporting traces of persons when using the new option **--persons** {{DT_BOOL}}.
  - Added option **--ignore-gaps** {{DT_BOOL}} for
    [traceExporter.py](../Tools/TraceExporter.md). When this is
    set vehicles will not be destroyed until the end of the fcd-file
    even when disappearing for a few simulation steps..
  - The [osmWebWizard.py](../Tools/Import/OSM.md#osmwebwizardpy)
    import script is now more robust in finding a suitable output
    directory and recovering from errors.
  - Added new tool
    [tlsCoordinator.py](../Tools/tls.md#tlscoordinatorpy)
    which coordinates traffic lights in a network for a given
    traffic demand in order to create green waves for many vehicles.
  - Connection objects from networks parsed via
    [Sumolib](../Tools/Sumolib.md) can now return
    *getTLLinkIndex* as well as *getJunctionIndex*. #2048

- TraCI
  - Added function *simulationStep()* to the [C++ TraCI library](../TraCI/C++TraCIAPI.md).

### Other

- Documentation
  - Online documentation of
    [TraCI4J](https://github.com/egueli/TraCI4J) can now be found at traci4j-javadoc (outdated link removed)
  - The section on [Additional netconvert outputs was completely rewritten](../Networks/Further_Outputs.md).
  - Added a [new page on XML Validation](../XMLValidation.md)
  - Added [documentation for the C++ TraCI API client](../TraCI/C++TraCIAPI.md)
  - Added documentation on [route probe detectors](../Simulation/Output/RouteProbe.md) (which was
    quite overdue).
  - Expanded documentation on
    [Calibrators](../Simulation/Calibrator.md) in regard to
    their mechanism for generating new vehicles.
  - Added [overview page for simulation routing](../Simulation/Routing.md).

- Simulation
  - Attribute names of the [energy model (battery device, charging stations)](../Models/Electric.md) were updated to bring
    them in line with naming conventions. Refer to the documentation
    for new attribute names.

- netconvert
  - Network version is now **0.25**. New features that justify the
    version change are:
    - New linkstate *Z* and junction type *zipper* for zipper
      merging.
    - New junction type *traffic_light_right_on_red*. To
      accommodate this type of junction, linkstate 's' (stop) is
      now allowed in traffic light plans.
    - Multiple connections to the same lane do not result in a
      warning any more. (The conflict is resolved using zipper
      merging or priority right of way)
    - New network attributes *lefthand*, *junctionCornerDetail*
      and *junctionLinkDetail*
    - Note, that the network version should have been updated in
      0.23.0 due to the introduction of ships but this was
      forgotten.

- sumo-gui
  - The visualization options *Show internal edge name* and *Show
    crossing and walkingarea name* were moved from the *Streets*-tab
    to the *Junctions*-tab.

- Tools
  - *extractRouteEdges.py* was removed as the functionality is now
    fulfilled by
    [*route2sel.py*](../Tools/Routes.md#route2selpy).
  - [server.py](../Tools/Import/OSM.md) was renamed to
    [osmWebWizard.py](../Tools/Import/OSM.md) and now resides
    directly within the *tools* folder.

- Misc
  - *start-commandline-bat* now adds python (python 2.7 in its
    default install location) and {{SUMO}}/tools to the path.
  - Simplified runner script of
    [Tutorials/TraCI4Traffic_Lights](../Tutorials/TraCI4Traffic_Lights.md)


## Version 0.24.0 (02.09.2015)

### Bugfixes

- Simulation
  - Fixed default arrivalPos when loading `<trip>` or `<flow>` elements using
    attributes `from, to` directly into SUMO. #1739
  - Fixed crash when specifying consecutive walks for the same
    person. #1781
  - Fixed bug that caused pedestrians to get too close to each
    other. #1769
  - Fixed crash and other bugs when using option **--lanechange.duration**. #1152, #1795, #1796, #1797
  - Fixed bug that sometimes caused the rear end of vehicles to be
    placed on the wrong lane after lane-changing. #1804
  - Rerouters where closed edges are disabled for specific vehicle
    classes now cause these vehicles to wait until the closing ends
    if the destination edge is closed.
  - Vehicles with a `<stop>` in their route are now being overtaken if there
    is sufficient space. This was causing problems when modelling
    parked vehicles on a multi-lane road.
  - Fixed bug that resulted in invalid routes when routing at
    simulation time (at intersections where a required connection
    originates from a prohibited lane). #1861
- sumo-gui
  - Fixed centering of names on objects (a large mismatch was
    visible for persons).
  - Polygons are no longer drawn when setting their
    size-exaggeration to 0.
- duarouter
  - Custom car following model specification is now preserved in the
    route output. #1832
  - Fixed bug when using option **--remove-loops** where the start/destination edge
    changed needlessly.
  - Vehicles may now depart/arrive at any *TAZ* regardless of their
    vehicle class.
- netconvert
  - When adding crossings to a *.net.xml* without internal links,
    the output network will be built with internal links. #1729
  - Fixed bug where duplicate crossings between the same pair of
    walkingareas where sometimes build when using option **--crossings.guess**. #1736
  - Fixed bugs where loading a *.net.xml* file and removing edges
    would lead to an invalid network. #1742, #1749, #1753
  - Permissions are no longer lost when guessing ramps. #1777
  - Defining pedestrian crossings at a node with `type="traffic_light_unregulated"` now works. #1813
  - Fixed invalid TLS-plans when loading a *.net.xml* file with
    TLS-controlled pedestrian crossings.
  - Connections between lanes with incompatible vehicle classes are
    no longer generated. #1630
  - Fixed generation of connections at intersections with dedicated
    bicycle lanes (symptoms were invalid connections and missing
    connections).
  - Modifying lane permissions in a network with crossings, so that
    crossings are no longer valid, is now working (invalid crossings
    are removed with a warning).
  - Fixed bug that caused invalid lane lengths when building with
    option **--no-internal-links** (near intersections with sharp angles).
  - Fixed some invalid clusters when using option **--junctions.join**.
- TraCI
  - Fixed python API for [function *move to VTD* (0xb4)](../TraCI/Change_Vehicle_State.md). #1720
  - Fixed subscriptions for `<laneAreaDetector>`
  - Vehicle command *move to* (0x5c) can now be used to forcefully
    insert vehicles which have not yet entered the network. #1809
  - Parking vehicles now return a reasonable position (and other
    values which do not depend on being on a lane). #1809
  - Fixed screenshots for Linux. #1341
  - The close command returns now a little later to have more data
    written to files (still not completely flushed though, see #1816)
- netedit
  - Fixed bug where unselected objects were wrongly selected after
    undoing deletion.
  - No longer creating invalid network when loading and saving a
    network with split crossings.
- Tools
  - [randomTrips.py](../Tools/Trip.md#randomtripspy) no longer
    attempts to find a fringe based on edge-direction when using
    option **--pedestrians**. #1737
- All Applications
  - Fixed invalid paths when using option **--output-prefix** {{DT_STR}} and loading a
    configuration file in a subdirectory. #1793

### Enhancements

- Simulation
  - Simulation of [electric vehicles is now supported](../Models/Electric.md) with a new model for
    energy consumption and battery charging.
  - Maximum time that may be simulated increased from 24 days to 290
    million years. #1728
  - [`<walk>`-definitions](../Specification/Persons.md#walking) now support
    and `departPos="random"` and `arrivalPos="random"`.
  - **--fcd-output** now includes z-data if the network has elevation. #1751
  - [`<vTypeProbe>`](../Simulation/Output/VTypeProbe.md) output now
    includes z-data if the network has elevation.
  - Device [assignment by `<param>`](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#devices)
    can now be used to override device assignment by option **--device.<DEVICENAME\>.probability**.
  - Added new `vType`-attribute `carFollowModel` which can be used instead of a child
    element when declearing the car following model. #663
  - Vehicles which do not have a route and cannot find one on
    insertion get discarded when **--ignore-route-errors** is given. #1825
  - **--vehroute-output** now includes the route length. #1790

- sumo-gui
  - The view can now be moved and zoomed via [keyboard shortcuts](../sumo-gui.md#keyboard_shortcuts). #298
  - Lanes which disallow passenger cars (i.e. paths and service
    roads) now have a distinct shade of grey. The color can be
    customized in the gui-settings dialog.
  - Vehicles now activate their blinker during continuous lane
    change manoeuvres.
  - The types of POIs and Polygons can now be displayed via *View
    Settings*. #1803
  - The `personNumber,containerNumber,personCapacity,containerCapacity` information for a vehicle is now shown in the parameter
    dialog. #1617
  - The GUI asks at simulation end whether all files and windows
    should be closed
- netconvert
  - added option **--remove-edges.input-file** {{DT_FILE}} which works as an analogue to **--keep-edges.input-file**. Giving either
    option a selection file (where edge ids are prefixed with
    **edge:** as argument is now supported.
  - added option **--ramps.unset** {{DT_STR}} which works as an analogue to **--ramps.set** and prevents edges
    from being treated as on- or off-ramps. #1763
  - Now importing signalized pedestrian crossings from
    [OSM](../Networks/Import/OpenStreetMap.md) (`<tag k="crossing" v="traffic_signals"/>`).
  - added new option **--default.junctions.keep-clear** {{DT_BOOL}} and new attributes `<node keepClear="..."/> <connection keepClear="..."/>` [for allowing drivers to drive onto an intersection despite the risk of blocking it for cross-traffic](../Simulation/Intersections.md#netconvert_options_for_allowing_drivers_to_drive_onto_an_intersections). #1290
  - pedestrian crossings may be removed using the [new attribute `discard`](../Networks/PlainXML.md#pedestrian_crossings).
  - when splitting an edge, the new node is not removed by option **--geometry.remove**
    even when speed and lane count remain unchanged. #1842
  - Connections from and to sidewalks are only generated when also
    building pedestrian crossings since they are superfluous
    otherwise. When using pedestrian model *nonInteracting* these
    connections are not used (pedestrians *jump* across
    intersections between any two sidewalks) and when using model
    *striping*, crossings are mandatory.
  - Bicycle lanes are now imported from [OSM (when using the appropriate typemap)](../Networks/Import/OpenStreetMap.md#recommended_typemaps). #1289
  - Bus lanes are now imported from [OSM](../Networks/Import/OpenStreetMap.md). #1682
  - Improved control over edge types (typemaps) when importing from
    [OSM](../Networks/Import/OpenStreetMap.md). All defaults
    can now be overridden in a transparent manner and it's easier to
    add additional modes of traffic than ever before [(see documentation)](../Networks/Import/OpenStreetMap.md#recommended_typemaps).
  - Option **--lefthand** now works for generating networks with left-hand
    traffic. Thanks to Andrea Fuksova for suggesting the
    double-mirroring technique.
  - Edge types can now be used to [define *vClass*-specific speed limits](../Networks/PlainXML.md#vehicle-class_specific_speed_limits). #1800
  - Additional attributes are now supported to specify the node that
    is generated when [splitting an edge](../Networks/PlainXML.md#road_segment_refining). #1843

- duarouter
  - option **--ignore-errors** now also applies if no input trips/routes are loaded.
  - Added option **--bulk-routing** {{DT_BOOL}} to improve routing speed when many (similar)
    vehicles depart at the same time from the same location. The
    time aggregation can be controlled using option **--route-steps** {{DT_TIME}} The *bulkstar*
    routing algorithm is now obsolete and no longer supported. #1792
  - Added options **--repair.from** {{DT_BOOL}}, **--repair.to** {{DT_BOOL}} which attempts to repair invalid source or
    destination edges in the route input.
- TraCI
  - added [command to retrieve persons on an edge](../TraCI/Edge_Value_Retrieval.md). Also added the
    corresponding method *traci.edge.getLastStepPersonIDs()* to the
    [python API](../TraCI/Interfacing_TraCI_from_Python.md). #1612
  - added method *traci.vehicle.getStopState()* to the [python API](../TraCI/Interfacing_TraCI_from_Python.md) [(command 0xb5)](../TraCI/Vehicle_Value_Retrieval.md). Also added
    convenience methods *traci.vehicle.isStopped(),
    traci.vehicle.isStoppedParking,
    traci.vehicle.isStoppedTriggered()*.
  - added [command to retrieve the next edge of a walking person](../TraCI/Person_Value_Retrieval.md). Also added the
    corresponding method *traci.person.getNextEdge()* to the [python API](../TraCI/Interfacing_TraCI_from_Python.md). This can
    be used to implement pedestrian-actuated traffic lights.
  - Added named constant *traci.vehicle.DEPART_SPEED_RANDOM*. This
    corresponds to `<vehicle departSpeed="random" .../>`.
  - Added variable retrieval functions for
    [`<laneAreaDetector>`](../TraCI/Lane_Area_Detector_Value_Retrieval.md):
    lane, position, and length. Also added corresponding functions
    to the [python API](../TraCI/Interfacing_TraCI_from_Python.md)
  - added [command to retrieve the index of the vehicles edge within its route](../TraCI/Vehicle_Value_Retrieval.md). Also
    added the corresponding method *traci.vehicle.getRouteIndex()*
    to the [python API](../TraCI/Interfacing_TraCI_from_Python.md). #1823
  - extended method *traci.vehicle.setStop(...)* to optionally
    include the startPos of a stop.

- polyconvert
  - When importing OSM data, POIs are now raised above the layer of
    polygons and roads by default to make them always visible. #1771
  - Added option **--poi-layer-offset** {{DT_FLOAT}} to control the layer of pois relative to polygons
    (especially in cases where they have the same type).
  - **--offset** {{DT_FLOAT}} options are now respected even when used together with a network.

- netedit
  - The view can now be moved and zoomed via [keyboard shortcuts](../sumo-gui.md#keyboard_shortcuts).
  - Added menu option for replacing junctions by geometry points. #1754
  - Geometry points of parallel edges can now be moved
    simultaneously when both edges are selected.
  - option **--gui-settings-file** is now supported.

- od2trips
  - can now choose only differing sources and sinks #1837

<!-- end list -->

- Tools
  - made edgesInDistricts.py aware of vClasses (and use the sumolib
    for parsing)
  - added [randomTrips.py](../Tools/Trip.md#randomtripspy)
    option **--binomial** {{DT_INT}} for achieving binomially distributed arrival rates.
  - added [randomTrips.py](../Tools/Trip.md#randomtripspy)
    option **--validate** {{DT_BOOL}} for generating trips with validated connectivity. #1783
  - [randomTrips.py](../Tools/Trip.md#randomtripspy) now
    supports attributes for `<person>` and `<walk>` definitions when using option **--trip-attributes**.
  - [netcheck.py](../Tools/Net.md#netcheckpy) now supports
    discovering reverse connectivity by using option **--destination**
  - [netcheck.py](../Tools/Net.md#netcheckpy) now supports
    edge permissions by using option **--vclass**
  - [netcheck.py](../Tools/Net.md#netcheckpy) now supports
    writing an edge selecting for every (weakly) connected component
    when using option **--selection-output**
  - [netcheck.py](../Tools/Net.md#netcheckpy) now outputs
    additional statistics in regard to the disconnected components.
    Thanks to Gregory Albiston for the patch.
  - added new tool
    [districts2poly.py](../Tools/District.md#districts2polypy)
    for visualizing districts.
  - added new tool
    [route2sel.py](../Tools/Routes.md#route2selpy) for
    creating an edge selection from a route file.
  - added new tool
    [edgeDataDiff.py](../Tools/Output.md#edgedatadiffpy) for
    comparing two traffic scenarios via their [edgeData output](../Simulation/Output/Lane-_or_Edge-based_Traffic_Measures.md).
  - [tls_csv2SUMO.py](../Tools/tls.md) now supports multiple
    definitions in a single input file. Thanks to Thomas Lockhart
    for the patch.
  - OSM-scenario-generator script
    [server.py](../Tools/Import/OSM.md) now supports additional
    modes of traffic.
  - *traceExporter.py* now supports filtering to a bounding box. #1774

### Other

- Scenarios
  - An updated version of the [TAPASCologne scenario](../Data/Scenarios/TAPASCologne.md) can be found
    on [the sourceforge download page](https://sourceforge.net/projects/sumo/files/traffic_data/scenarios/TAPASCologne).
    This updates the network to the latest OSM data and
    [netconvert](../netconvert.md) version.
  - An updated version of the [Bologna scenarios](../Data/Scenarios.md#bologna) can be found on
    [the sourceforge download page](https://sourceforge.net/projects/sumo/files/traffic_data/scenarios/Bologna_small/).
    This contains minor network fixes and contains a new pedestrian
    version of the *acosta*-scenario
- Documentation
  - Added new tutorial
    [Tutorials/TraCIPedCrossing](../Tutorials/TraCIPedCrossing.md)
    which shows how to build a pedestrian-actuated traffic light
    controller.
  - Documented [TraCI function *move to VTD* (0xb4)](../TraCI/Change_Vehicle_State.md)
  - cleaned XML schema concerning person capacity and person number
    definition
  - Documented [tllogic files](../Networks/PlainXML.md#traffic_light_program_definition)
  - Documented current state of [bicycle simulation](../Simulation/Bicycles.md), [train simulation](../Simulation/Railways.md) and [waterway simulation](../Simulation/Waterways.md).
  - Added description of [intersection dynamics](../Simulation/Intersections.md).
- TraCI
  - TraCI version is now 10
  - The named constant *traci.vehicle.DEPART_MAX* is now named
    *traci.vehicle.DEPART_SPEED_MAX*. This corresponds to `<vehicle departSpeed="max" .../>`.
- Tools
  - GDAL 2.0 is now supported. Thanks to Thomas Lockhart for the
    patch.
  - osmBuild.py and [server.py](../Tools/Import/OSM.md) no
    longer use option **--geometry.remove-isolated** to avoid removing rivers and railways.
- Misc
  - The scripts *randomTrips.py* and *route2trips.py* moved from
    {{SUMO}}/tools/trip to {{SUMO}}/tools
  - The [OSM typemaps](../Networks/Import/OpenStreetMap.md#recommended_typemaps)
    now disallow vClass *tram* and *ship* where appropriate
  - The tool *sumoplayer* has been removed because it became
    obsolete with the introduction of **--fcd-output** and *traceExporter.py*. #1651
  - The Win64 binaries no longer have a 64 suffix.
  - Error reporting on opening files got a little more verbose.
  - Whitespace in filenames is handled a little bit better


## Version 0.23.0 (31.03.2015)

### Bugfixes

- Simulation
  - fixed bugs that caused multiple pedestrians to enter the same
    spot when using the *striping* model. #1506
  - fixed crash when vehicles with `vClass="ignoring"` attempt to drive across
    sidewalks and walkingareas.
  - fixed invalid `departDelay` in **--tripinfo-output** when using flows with attribute `probablity`. #1482
  - fixed rerouter interval ends which are now exclusive like all
    other intervals
  - fixed bug that caused invalid exit times when vehicles were
    teleported while on an intersection. #1520
  - fixed invalid estimation of the time and speed when reaching an
    intersection which could cause collisions. #1531 #1533
  - explicitly setting option **--weight-attribute** *traveltime* now properly fills the
    edge-weight container (i.e. for visualization). #1541
  - fixed invalid `routeLength` in **--tripinfo-output** when vehicles did not finish their route. #1539
  - fixed invalid `routeLength` in **--tripinfo-output** when using networks with internal lanes. #443
  - fixed bugs that were causing collisions. #1549, #1551, #1553, #1575, #1591
  - fixed bug that was causing vehicles to brake prematurely when
    turning left on a prioritized road. #1566
  - fixed crash when pedestrians were using the same edge twice in a
    row. #1555
  - fixed crash when pedestrians were given a disconnected route. #1578.
    Now disconnected routes are an error which is recoverable (by
    *teleporting*) using option **--ignore-route-errors**
  - pedestrians with a disconnected trip now properly reach their
    destination (if the option **--ignore-route-errors** is given). #1581
  - corrected reading the PHEMLIGHT_PATH environment variable
  - simulation now properly terminates when loading a `<trip>` with a `from`-edge
    that is shorter than the vehicle. #1559
  - vehicles no longer collide (visually) within an intersection
    when using the
    [`endOffset` attribute](../Networks/PlainXML.md#edge_descriptions). #970
  - fixed crash when attempting to load an additional tls program
    with unused phase states. #1641
  - fixed crash when specifying duplicate person ids. #1650

- netconvert
  - when adding sidewalks via
    [type-map](../SUMO_edge_type_file.md) and a sidewalk
    already exists, the edge now remains unchanged.
  - fixed bug where some generated pedestrian crossings at
    T-intersections had a permanent red light. #1363
  - fixed bug where right-turning vehicles had the green light
    during the left-turn phase when pedestrian crossings where
    present.
  - the width of adjacent sidewalks is now taken into account when
    computing the width of a walkingarea.
  - fixed bug where an invalid network was generated when declaring
    a pedestrian crossing at a dead-end node. #1366
  - fixed bug in computation of *walkingarea* shape. #1257
  - fixed bug in connection computation when some connections were
    explicitly deleted (also applied to some OSM turn-restrictions). #1457
  - changing offset or algorithm type with *.tll.xml* file for TLS
    loaded from a *.net.xml* file without changing the phases now
    works. #1207
  - when loading `<split>` elements along with a *.net.xml* file, existing
    connections are preserved whenever possible. #1353
  - Fixed 3D-geometry of internal lanes at elevated junctions. #1526
  - Fixed invalid connections when generating ramps. #1529
  - Fixed invalid geometry when generating ramps. #1535
  - fixed bug where user defined connections were ignored at
    intersections with one incoming and one outgoing edge. #1552
  - fixed crash when loading a sumo net with pedestrian crossings
    and removing edges via options. #1557
  - fixed errors when building pedestrian crossings. #1582
  - fixed invalid connections at edges with sidewalks. #1586
  - fixed positioning of internal junctions at turnarounds when
    using the
    [`endOffset` attribute](../Networks/PlainXML.md#edge_descriptions). #1589
  - when importing from OSM, compound types now honor the setting `discard="true"` of the individual component types.
  - fixed junction shape when using lanes with custom width. #1604
  - user-defined node shapes are now correctly imported from
    *.net.xml* files. #1450
  - lane-specific settings are no longer lost when applying an
    *.edg.xml* patch file to an existing network definition. #1609
  - fixed some of the bugs that were causing inappropriate node
    shapes.
  - several fixes to the placement of internal junctions (waiting
    positions on the intersection for selected connections)
    - removed some internal junctions that were not necessary
    - repositioned internal junctions so that the waiting vehicles
      do not overlap with foe vehicles #1611
    - moved internal junctions at TLS-controlled nodes further
      towards the junction center (previously they respected foe
      vehicles which would never get the green light at the same
      time)
  - fixed invalid junction logic (right-of-way rules) when incoming
    edges had uncommon angles. #1632 #1462
  - generate tls logic now matches the underlying edge priorities.
    The streams that would have priority if the node type was
    "priority" now always have a common green phase. #1642
  - the attribute `controlledInner` is now correctly exported to *plain-xml* output
    when importing a *.net.xml* where edges within a joined TLS are
    controlled. #1264
  - fixed bug that was causing the wrong flow to receive the right
    of way at priority junctions. #1689
- [MESO](../Simulation/Meso.md)
  - fixed crash when updating vehicle routes while running with **--meso-junction-control**.
    see #1502
  - fixed angle and position of vehicle ids in the GUI
  - fixed crashes when using various outputs. #1505
  - fixed bug leading to exaggerated emissions. #1504
  - vehicles are now drawn correctly on curving lanes. #758
  - vehicle size exaggeration now working. #1406
- sumo-gui
  - Fixed crash when using a vehicle parameter window while the
    vehicle disappears. #1534
  - Coloring by loaded edge weight now correctly reflects changes
    over time.
  - Edges/Lanes set to transparent (alpha = 0) are no longer drawn
    when zoomed out.
  - The view-settings dialog can no longer become hidden when
    switching between displays with a different resolution. #1512
  - Traffic-light *show phase* dialog no longer crashes when loading
    very long phases. #1451
  - fixed visualization of closed edges in rerouters
  - Lane coloring *by inclination* now correctly shows the
    direction. #1526
  - The checkbox Streets-\>'show rails' in the gui-settings dialog
    can now be used to toggle the visualization style of railways.
  - The edge width exaggeration factor is now properly saved to and
    loaded from a
    [gui-settings-file](../sumo-gui.md#configuration_files).
  - fixed gui glitch when zooming with transparent lanes in view.
  - now drawing vehicles on transparent lanes.
  - Selecting the *Center* action from a popup-menu immediately
    centers the view on that object (instead of delaying until the
    next window update). #1593
  - when an error occurs during the initial route loading, reloading
    the simulation will now work after the user corrects the error
    in the input files. #1508
- duarouter
  - no longer building invalid routes when the connectivity among
    edges differs between vClasses. #303
  - fixed bug that caused unreasonable detours in pedestrian routes
    (also affected routing in the simulation). #1556
- all routers
  - Unknown vehicle type in `<flow>` element can now be ignored with option **--ignore-errors**.
- Tools
  - [cutRoutes.py](../Tools/Routes.md#cutroutespy) now sorts
    unmodified departure times correctly.
  - [server.py](../Tools/Import/OSM.md#osmwebwizardpy) no longer
    creates disconnected routes. Also, trains will no longer be
    generated on inappropriate edges.
  - [sort_routes.py](../Tools/Routes.md#sort_routespy) now
    sorts correctly when using option **--big**. #1603

### Enhancements

- Simulation
  - Pedestrians now have [appropriate default values for length, minGap and width](../Vehicle_Type_Parameter_Defaults.md).
    These values can be changed by using a `vType`.
  - Improved realism of pedestrian dynamics on walkingareas when
    using model *striping*. [Pedestrians at arbitrary angles now properly avoid each other](../Simulation/Pedestrians.md#model_striping).
  - Added option **--pedestrian.striping.jamtime** {{DT_TIME}} to control behavior of jammed pedestrians. By
    default, pedestrians will now start squeezing through the jam
    after being stopped for that time. As a consequence, pedestrian
    scenarios now always terminate.
  - Added option **--output-prefix** {{DT_FILE}} to automatically add a prefix to all output files.
    The special string *TIME* is replaced with the current time.
  - Rerouters can now set permitted vehicle classes on closed lanes,
    see #1518
  - A warning is now issued when a loaded network contains *unsafe*
    connections. #447
  - A new car-following class based on the original Krauß was added
    that that takes into account the maximum acceleration ability
    computed by PHEMlight
  - Pedestrian routing now also [works on networks without pedestrian infrastructure (walkingareas)](../Simulation/Pedestrians.md#building_a_network_for_pedestrian_simulation). #1564
  - Added option **--netstate-dump.precision** {{DT_INT}} to control the precision (number of digits after
    the dot) of vehicle positions and speeds when using option **--netstate-dump**.
  - Added vehicle class `ship`.
  - option **--ignore-route-errors** now also handles errors which prevent the vehicle from
    being inserted into the network. #1661
  - Added option **--device.rerouting.output** {{DT_FILE}} to export the travel times which are used for
    dynamic rerouting during every adaptation interval. #1663

- sumo-gui
  - Can now color and scale lanes *by average speed* and by *by
    average relative speed* (relative to the speed limit).
  - Added option for drawing the path across the current walkingarea
    to the person popup menu.
  - Can now color pedestrians randomly.
  - When drawing persons as triangles, the direction they are facing
    is now indicated (it was ambiguous for equilateral triangles).
  - The lane menu now shows the height under the cursor as well as
    the offset (*pos*).
  - Can now color lanes *by height at segment start* and by *by
    inclination at segment start*. This allows investigating the
    3D-geometry in detail.
  - [Added `param`](../Simulation/Traffic_Lights.md#parameters)
    *show-detectors* that allows showing detectors of *actuated*
    traffic lights in the GUI.
  - When drawing streets with exaggerated width, the edge in one
    direction no longer overlaps with the opposite direction. #1540
  - Now setting window title when loading config or net at
    application start.
  - Added menu for locating persons. #1276
  - The `line` information for a vehicle is now shown below the vehicle id
    and in the parameter dialog. #1563
  - The network parameter dialog now includes some statistics on the
    number of nodes and edges as well as the total road length. #1367
  - Added guiShape `ship`.
  - Now drawing waterways (lanes with `allow="ship"`) in a distinct style.
  - Added menu option for loading an [*additional-file*](../sumo.md#format_of_additional_files) with
    [shapes](../Simulation/Shapes.md).
  - Improved gui shapes for two-wheeled vehicles.
  - Markings for bike lanes are now drawn at road intersections
    (bike lanes are lanes which only allow vClass *bicycle*).
  - Added options for saving the current viewport and simulation
    delay along with the current visualisation settings. #1625

- netconvert
  - read heightmaps from GeoTIFFs for instance [SRTM data](https://opendem.info/download_srtm.html) #1495
  - when adding sidewalks via
    [typemap](../SUMO_edge_type_file.md) or **--sidewalks.guess**, permissions on
    all other lanes are now automatically set to disallow
    pedestrians. #1507
  - sidewalks can now be added for specific edges by adding the
    attribute `sidewalkWidth="<width_in_m>"` to `<edge>`-elements in *.edg.xml*-files. This automatically
    remaps any existing connections. #1580
  - when using pedestrian crossings and green verges (lanes which
    disallow all vClasses), the crossing now starts at the side of
    the ride instead of the green verge. #1583
  - Added option **--sidewalks.guess.from-permissions** {{DT_BOOL}}. This adds sidewalks for edges that allow
    pedestrians on any of their lanes regardless of speed. #1588
  - Can now import waterways from OSM ([needs a custom type-map]({{Source}}data/typemap/osmNetconvertShips.typ.xml)).
  - The junction size can now be customized [using the new `<node>`-attribute `radius`](../Networks/PlainXML.md#node_descriptions) #1605
  - Added option **--junctions.corner-detail** {{DT_INT}} which can be used to generate rounded intersection
    corners. #1606
  - user-defined shapes for internal lanes, crossings and
    walkingareas are now supported via the new `customShape`-element in *.con.xml* files #1607
  - right-turning road vehicles will now wait within the
    intersection when yielding to straight-going bicycles. #1255

- netgenerate
  - can now generate networks with unregulated intersections. #1558

- polyconvert
  - added option **--output.plain-geo** {{DT_BOOL}} for writing POIs and polygons with
    geo-coordinates. #1608

- netedit
  - can now visualize railways the same way as sumo-gui. #1542
  - can now disable drawing of junction shapes via gui-settings.
  - can now load all types of shapes from the menu.
  - can now undo/redo bulk selection/deselection of objects. #1614
  - added option for auto-selecting an edges nodes.
  - can now modify the nodes of an edge via the attribute dialog. #1662
  - the attribute panel is now updated on undo/redo/recompute. #696
  - when creating a reverse edge with spread type *center*, the new
    edge is shifted sideways and given new endpoints.
  - can now move the view in 'Create Edge'-mode by holding <kbd>Ctrl</kbd>

- Tools
  - [sort_routes.py](../Tools/Routes.md#sort_routespy) now
    supports sorting persons and also copies additional elements
    (such as `vType` to the output).
  - [cutRoutes.py](../Tools/Routes.md#cutroutespy) can now
    handle nested stops in a vehicle definition.
  - added [routeStats.py](../Tools/Routes.md#routestatspy) for
    computing statistics on route length.
  - added [traceExporter.py](../Tools/TraceExporter.md) option **--orig-ids** {{DT_BOOL}}
    for keeping the original vehicle IDs instead of a running index.
    For some output formats such as *NS2* this only works if the
    vehicle IDs are numeric to begin with.
  - `route2trips.py` now preserves `vType` elements.
  - [server.py](../Tools/Import/OSM.md#osmwebwizardpy) now starts
    with 'real world' gui settings and initial delay of 20. Also,
    the trip generation settings have been tweaked for realism (i.e.
    shorter pedestrian trips, longer train routes)

- TraCI
  - Several simulation entities support [named string parameters](../Simulation/GenericParameters.md) in their
    XML-definitions. These can now be set and retrieved [via TraCI](../TraCI/GenericParameters.md) and via the [python library](../TraCI/Interfacing_TraCI_from_Python.md) for the
    following entities: Edge, Lane, Person, Vehicle, VehicleType,
    PoI, Polygon, Route, TrafficLight
  - Added [person API methods](../TraCI/Person_Value_Retrieval.md) to sumo and to
    the [python library](../TraCI/Interfacing_TraCI_from_Python.md).

### Other

- Documentation
  - Added overview on [Randomness in the Simulation](../Simulation/Randomness.md)
  - Added overview on [Vehicle speeds](../Simulation/VehicleSpeed.md)
  - The feature for passing arbitrary sumo options to
    [duaIterate.py](../Demand/Dynamic_User_Assignment.md) is
    now mentioned when calling **--help**.
  - The attribute `endOffset` which can be used to move the stop line when
    [defining networks](../Networks/PlainXML.md#edge_descriptions)
    is now documented. It has been available since version 0.13.0.
  - Documented recommended options when [importing OSM networks](../Networks/Import/OpenStreetMap.md)
  - Documented TraCI retrieval of *waiting time* for vehicles,
    persons, edges and lanes
- TraCI
  - consolidated the message IDs for ArealDetectors
  - TraCI version is now 9
- Miscellaneous
  - netconvert option **--map-output** was removed since it was neither working nor
    deemed useful enough to repair it.
  - added warning about invalid geo-coordinates in netconvert and
    polyconvert input
