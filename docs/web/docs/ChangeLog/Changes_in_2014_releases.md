---
title: Changes in the 2014 releases (versions 0.20.0, 0.21.0 and 0.22.0)
---

## Version 0.22.0 (11.11.2014)

### Bugfixes

- Simulation
  - Vehicles with `departspeed="max"` now properly adapt their speed to vehicles ahead
        at insertion. This results in higher flows.
  - Teleporting vehicles will only be inserted if they fit fully
        onto the destination lane.
  - Fixed some inconsistencies in E3-Detector output (see [E3 Further_Notes](../Simulation/Output/Multi-Entry-Exit_Detectors_(E3).md#further_notes)).
  - Flows using attribute `probability` now correctly terminate when attribute `end` is given.
  - Fixed several bugs for traffic lights of type `actuated`.
  - Pedestrians using the model `striping` now always respect attribute `arrivalPos`
  - Fixed crash when computing pedestrians positions on short
        walkingAreas.
  - Fixed crash when using car following model `<carFollowing-IDM/>`
  - Fixed bug that was causing collisions at intersections.
  - Fixed bug that was causing collisions due to unsafe
        lane-changing decisions.
  - Fixed bug that was causing collisions due to unsafe insertion.
  - Fixed bug that was causing collisions due to unsafe re-insertion
        after teleporting.
  - Fixed bug that was causing *silent* collisions between vehicles
        on different edges. Previously this type of collision was not
        reported but visible in the gui.
  - Fixed bug that sometimes lead to inferior lane-change decisions
        in networks with vehicle class restrictions.
  - Fixed bugs that sometimes prevented vehicles from being inserted
        at the earliest opportunity.
  - Fixed bug that prevented vehicles from being inserted when their `departSpeed` was close to their `maximumSpeed` and using car following model `<carFollowing-IDM/>`.
  - Fixed bug that was causing unnecessary lane changes leading to
        oscillations.
  - Fixed bug that was degrading the cooperation between vehicles
        during lane changing.
  - Flows now always insert at least 1 vehicle for any positive
        repetition period (if the simulation time reaches the `begin`-time).
  - Fixed position caching which caused wrong positions and angles
        after lane changes #143
  - Fixed Bluetooth reentry see #1455
  - Fixed the usage of different random number generators for route
        file parsing and vehicle movement
  - Fixed rerouting close to junctions

- sumo-gui
  - Single-stepping the simulation with hotkey (<kbd>Ctrl</kbd> + <kbd>D</kbd>) is now
        working as smoothly as intended.
  - Changing simulation delay via mousewheel now works when the
        pointer is on top of the dial.
  - Vehicle coloring *by acceleration*, *by time gap* and *by offset
        from best lane* now correctly visualize negative values
  - Persons which are waiting or riding in a a vehicle now face the
        correct direction.
  - Fixed crash related to parking vehicles.
  - Corrected angle of parking vehicles.
  - Fixed bug where train carriages were drawn incorrectly.
  - The drawing size of laneAreaDetectors can now be scaled
        properly.

- netconvert
  - Street-sign-output now references the correct xsd file.
  - warnings are emitted if no proj support is available and
        projection is needed

- duarouter
  - Fixed crash when using options **--repair** and **--ignore-errors**.
  - Fixed crash when using option **--remove-loops**.
  - When using option **--remove-loops** the non-looping parts of the route are now
        kept unchanged as intended.
  - Progress indicator is now only shown if the **--end** option is used.
  - Fixed crash when loading `<flow>` with attribute `probability`.

- TraCI
  - Fixed crash when retrieving vehicle variable best lanes (id
        0xb2) while the vehicle is on an internal lane.
  - The command induction loop value ([last step vehicle data Command_0x17](../TraCI/Lane_Area_Detector_Value_Retrieval.md)))
        now returns the value **-1** for the *leaveTime* of a vehicle
        which has not yet left the detector (instead of returning the
        current time step as before).
  - Fixing connection retries in the python client

- Misc
  - Fixed includes to speed up compilation

### Enhancements

- Simulation
  - Whenever rerouting vehicles, a new route is only recorded when
        it differs from the previous route.
  - tripinfo-output now includes attribute `timeLoss` which holds the time
        loss compared to the maximum driving speed.
  - Added option **--tripinfo-output.write-unfinished** {{DT_BOOL}}. When this is set, tripinfo output will
        additionally be written for all vehicles that did not arrive at
        the end of the simulation.
  - Vehicles with the attribute `departLane="free"` now consider the free space on all
        candidate lanes instead of the number of vehicles. This results
        in higher flow.
  - Added a new departLane-value named `departLane="first"` where vehicles are inserted
        on the rightmost lane they are allowed to use; this is the new
        default insertion lane
  - Now supports parallel routing by using the option **--device.rerouting.threads** {{DT_INT}}
  - Added new routing algorithms 'astar' and 'CH' which are faster
        than the default 'dijkstra' algorithm.
  - `flow` elements with attributes `from` and `to` can now be loaded directly in the
        simulation and will automatically be routed according to the
        current traveltimes at `begin`.
  - Invalid stops now generate better error messages, see #1458
  - Added option **--vehroute-output.intended-depart** {{DT_BOOL}}. This allows to generate vehicle route files which
        reproduce the original behavior when using one shot routing. #1429
  - Added option **--weights.random-factor** {{DT_FLOAT}} to disturb edge weights when routing in the
        simulation. This allows modelling imperfect information, helps
        to avoid biases when multiple routes have the same length (i.e.
        in grid networks) and may be used to prevent jams on a single
        shortest path.

- sumo-gui
  - Now appending *.xml* suffix when saving viewport, view settings
        or decals
  - Added lane coloring *by loaded weights*. This colors by whatever
        attribute was set with option **--weight-attribute** {{DT_STR}}. The weight value is also shown
        in the parameter dialog.
  - Added lane coloring *by priority*. This uses the edge priorities
        used during network creation. The priority value is also shown
        in the parameter dialog.
  - Added junction coloring *by type*.
  - Added visualization options for drawing vehicles, persons, POIs,
        polygons and additional gui objects with constant size when
        zooming out.
  - The dialog for changing visualization settings is now resizable
        and will remember its size across application runs. All settings
        are now scrollable to allow work on smaller screens.
  - The attributes which can be used to customize lane colors can
        now also be used to scale their width. This is done in the view
        customization dialog unter 'Streets' -\> 'Scale width'.
  - Vehicle coloring *by time since lane change* now indicates the
        direction of the change by its color
  - Added new link state 'u' to encode red-yellow phase (shown as
        orange). Vehicles behave as if 'r' (red) was set. This linkstate
        may be used to indicates upcoming green phase according to
        [RiLSA](https://de.wikipedia.org/wiki/Richtlinien_f%C3%BCr_Lichtsignalanlagen).
  - Lane coloring by speed now uses more colors (the same as vehicle
        coloring by speed).

- netconvert
  - Roundabouts can now be specified via [plain-xml input](../Networks/PlainXML.md#roundabouts).
        They are also written to the *.edg.xml*-file when using option **--plain-output-prefix**.

- duarouter
  - Now supports parallel routing by using the option **--routing-threads** {{DT_INT}}
  - Added new routing algorithms 'astar' and 'CH' which are faster
        than the default 'dijkstra' algorithm.

- jtrrouter
  - Routes which loop back on themselves are no longer generated by
        default (can be enabled using option **--allow-loops**).

- TraCI
  - added method *vehicle.setSpeedMode()* to the python API
        ([Command 0x3b](../TraCI/Change_Vehicle_State.md)).
  - added method *areal.getLastVehicleIDs()* to the python API
        ([Command_0x12](../TraCI/Lane_Area_Detector_Value_Retrieval.md)).
  - added argument *extended* to method *lane.getLinks()* in the
        python API (default *False*). If it is set to *True*, the method
        returns all the information defined for
        ([Command_0x33](../TraCI/Lane_Value_Retrieval.md))
        instead of a subset.

- Tools
  - Added [randomTrips.py](../Tools/Trip.md#randomtripspy)
        option **--weights-output-prefix** {{DT_STR}} to generate weight files which can be used to visualize
        the edge probabilities for being source/destination/via in
        [sumo-gui](../sumo-gui.md).
  - Added [randomTrips.py](../Tools/Trip.md#randomtripspy)
        option **--weights-prefix** {{DT_STR}} which allows loading edge probabilities for being
        source/destination/via. The file format is the same as for
        option **--weights-output-prefix** {{DT_STR}} with missing edges defaulting to probability 0 and
        missing files defaulting to normal randomization.
  - Added [duaIterate.py](../Tools/Assign.md#duaiteratepy)
        option **--zip** to zip old iteration outputs using 7-zip
  - Added script [server.py](../Tools/Import/OSM.md#osmwebwizardpy)
        for "three-click" scenario generation (thanks to Jakob
        Stigloher)

- [marouter](../marouter.md)
  - Added new router which reads trips and writes routes by computing a macroscopic user assignment.

### Other

- Web presence moved to sumo.dlr.de (wiki at sumo.dlr.de/wiki, trac at
    sumo.dlr.de/trac)

- Documentation
  - Added [description of generic netconvert warnings](../netconvert.md#warnings_during_import).
  - Added [description of OSM-specific netconvert warnings](../Networks/Import/OpenStreetMap.md#warnings_during_import).
  - Added [description of Windows build configurations](../Installing/Windows_Build.md).

- duarouter
  - Now issuing warnings about repaired routes.


- sumo-gui
  - Tweaked right-of-way colors for types *stop* and *allway_stop*.
        [All colors are now documented.](../sumo-gui.md#right_of_way)

- Misc
  - The version string for each application now includes the flag
        'D' for the debug-build and flag 'I' for the internal build
  - The traceExporter.py script moved from {{SUMO}}/tools/bin to {{SUMO}}/tools
  - unittest compilation is automatically enabled when gtest is
        found (Linux only)


## Version 0.21.0 (11.06.2014)

### Bugfixes

- Simulation
  - Fixed infinite loop when tls-phases were shorter than the
    step-size
  - Fixed bug where vehicles would block the intersection when
    stopping at the very end of a lane due to a red light
  - Fixed lane-changing bug which could cause differing results
    between different compilers due to uninitialized memory.
  - The vehicle class `ignoring` now properly ignores edge permissions and is
    allowed to drive anywhere.
  - Time values are now checked on input whether they are in the
    correct range (less than 2147483 seconds)
  - Teleport over an edge which has a stop now removes the stop from
    the list of pending stops
  - the IDM and the IDMM car following model now stop closer to the
    halting line (in front of intersections), furthermore the moving
    average in the IDMM was repaired
  - Bluetooth device handles subsecond simulation correctly
  - Fixed check for slow lanes ahead of the depart position to allow
    for higher insertion speeds
  - Repaired rerouting of vehicles which are on an internal lane
  - Several fixes with teleporting over, on and onto induction loops
    and range detectors.
  - Fix to the computation of the expected time at which vehicles
    leave an intersection. This results in a minor increase in
    junction throughput.

- sumo-gui
  - The time shown in the gui now matches the time found in
    simulation outputs for the shown vehicle positions. Previously,
    the gui showed the time + 1 step.
  - Fixed crash when drawing persons
  - Patched building under macOS, thanks to Alex Voronov
  - lane coloring by netto occupancy no longer shows brutto
    occupancy (see [lane coloring](../sumo-gui.md#edgelane_visualisation_settings)).
  - drawing of simple vehicle shapes now always uses a distinct
    color for drawing details.
  - Fixed crash when closing the breakpoint editor
  - Fixed crash when loading viewsettings and changing them
    immediately.
- netconvert
  - when loading traffic light programs from *tllogic-files*, the
    node attribute *controlledInner* is now propperly preserved
  - guessed traffic light programs no longer contain duplicate
    consecutive phases
  - Fixed bug where option **ramps.guess** would sometimes fail to add necessary
    lanes.
  - Extremely short internal lanes are no longer split. Instead a
    warning is issued since these cases usually indicate an invalid
    junction geometry.
- dfrouter
  - removed option **--all-end-follower** as it's not longer needed and buggy
- duarouter
  - when using tazs (districts) they are now correctly attached to
    the network with an edge with travel time 0 (before it was 1)
- TraCI
  - subscriptions for VAR_LOADED_VEHICLES_IDS now work
  - when setting lane change mode to prevent all changes due to a
    certain change-motivation, motivations with lower priority are
    now correctly considered.
  - adding a vehicle with a negative depart position gives an error
    message (applies only to ADD not to ADD_FULL)
  - adding a vehicle with a depart time in the past gives an error
    instead of silently dropping the vehicle
  - vaporizing parked vehicles is now possible #1166
  - forcing a lane change for a stopped vehicle now correctly
    removes the current stop from the list of pending stops
- Tools
  - Fixed bug where sortRoutes.py would produce invalid XML (thanks
    to Pieter Loof for the patch)
  - Fixed bug in traceExporter.py when generating files for NS2
    which caused non-sequential IDs to be written.
- General
  - Fixed several divisions by zero and integer over- and underflows
    reported by the clang sanitizer
  - Trying to open existing files which do not have reading
    permissions now gives a correct error message
  - All tools now give error messages when an output socket is not
    any longer available on closing

### Enhancements

- Simulation
  - Added improved capabilities for [pedestrian simulation](../Simulation/Pedestrians.md)
    - Added option **--pedestrian.model** {{DT_STR}} with available values **nonInteracting** and
      **striping**. The most important features of the
      **striping** model (which is the new default are:
    - Pedestrian cross streets and interact with traffic (blocking
      vehicles, blocked by vehicles, reacting to traffic lights)
    - Pedestrians interact with each other (jam)
    - Pedestrians select their route dynamically to avoid red
      lights when multiple paths across an intersection are
      available.
    - Added options **--pedestrian.striping.stripe-width** {{DT_FLOAT}} and **--pedestrian.striping.dawdling** {{DT_FLOAT}} to configure the 'striping'-model

  - Added option **--lanechange.overtake-right** {{DT_BOOL}} to switch between german overtaking laws (default)
    where overtaking on the right on highways is prohibited and
    american overtaking laws
  - Vehicles which are already on a junction but have to wait for
    another vehicles (also on the junction) now always drive up to
    point where their path crosses with the blocking vehicle.
  - Added option **--random-depart-offset** {{DT_TIME}} to randomize departure times for all vehicles by
    applying a positive (uniform) random offset.
  - `flow` elements now support attribute `probability` for specifying flows with
    randomly distributed departures (following a binomial
    distribution). Each second a vehicle is inserted with the given
    probability (which is scaled according to the given **--step-length**).
  - Vehicles with a `vType` which has a specified `vClass` will now get appropriate
    default values for some of their parameters (i.e. busses will be
    longer than 5m by default). Refer to
    [Vehicle_Type_Parameter_Defaults](../Vehicle_Type_Parameter_Defaults.md)
    for details.
  - the option **--scale** {{DT_FLOAT}} can now also handle values \> 1 and inserts more
    vehicles in this case
  - added [HBEFA3](../Models/Emissions/HBEFA3-based.md)
    emission model
  - added [outputs](../Simulation/Output/index.md) conforming to the
    Amitran standards

- sumo-gui
  - the option **--no-warnings** {{DT_BOOL}} now also applies to the GUI message window
  - Added person coloring scheme *by selection* and *by angle*
  - All person modes now give a waitingTime
  - Person parameter dialog now includes attributes 'edge',
    'position', 'angle' and 'waitingTime'
  - Pedestrian crossings are now indicated by the typical
    zebra-pattern (light color indicates that pedestrians have
    priority, dark color means that vehicles have priority)
  - Showing permission code in lane parameter dialog, updated style
    color by permissions
  - Uncontrolled links (not part of a junction logic) are now shown
    with index -1
  - Now showing edge type in lane parameter dialog
  - Customizable drawing of ids for edges of type crossings and
    walkingarea
  - Added lane coloring scheme *by angle*
  - Lanes are now drawn with smooth shapes regardless of geometry
    (fixed white gaps at corners)

- netconvert
  - Added element `<crossing>` for '.con.xml' files. [This can be used to specify pedestrian crossings](../Networks/PlainXML.md#pedestrian_crossings)
  - Added attribute `sidewalkWidth` to `<type>` elements in [*.typ.xml* files](../SUMO_edge_type_file.md). If this is given, edges
    of this type will get an extra lane with `allow="pedestrian"` and the specified width.
  - Added options **--sidewalks.guess** {{DT_BOOL}}, **--sidewalks.guess.min-speed** {{DT_FLOAT}} and **--sidewalks.guess.max-speed** {{DT_FLOAT}} to trigger guessing of sidewalks
  - Added option **--crossings.guess** {{DT_BOOL}} to trigger guessing of pedestrian crossings
  - Added options **--tls.guess-signals** {{DT_BOOL}} and **--tls.guess-signals.dist** {{DT_FLOAT}} to interpret special traffic light nodes
    which surround an intersection as the signal positions for that
    intersection. This heuristic is useful for correctly importing a
    typical representation of traffic lights in OSM data.
  - Added attribute `shape` to `<node>` elements in [*.nod.xml* files](../Networks/PlainXML.md#node_descriptions).
    This allows to specify a custom shape for that node.
  - can write networks in the Amitran format

- duarouter
  - `<trip>` and `<flow>` elements now support attribute `via` for specifying intermediate
    edges to use during routing.
  - `vType` and `route` definitions can now be given in [*additional-files*](../sumo.md#format_of_additional_files) just like for
    [sumo](../sumo.md)
  - Pedestrian walks with attributes `from` and `to` are now routed and
    written with attribute `edges`.
  - the maximum (the average) speed factor are taken into account
    when calculating the minimum (the default) travel times
  - References to an unknown `vType` are now an error (which can be ignored
    with **--ignore-errors**). Previously, an unknown `vType` was silently copied to the
    output and treated like the default type during routing.

- polyconvert
  - OSM road shapes and traffic light positions can now be exported
    [by setting the appropriate entries in the typemap file.](../Networks/Import/OpenStreetMap.md#importing_additional_polygons_buildings_water_etc)

- od2trips
  - reads now OD matrices in Amitran format

- Tools
  - Added [randomTrips.py](../Tools/Trip.md#randomtripspy)
    option **--pedestrians** {{DT_BOOL}} to generate pedestrian walks
  - Added [randomTrips.py](../Tools/Trip.md#randomtripspy)
    option **--max-distance** {{DT_FLOAT}} to limit the straight-line distance of generated trips
  - Added [randomTrips.py](../Tools/Trip.md#randomtripspy)
    option **-a, --additional-files** {{DT_FILE}}[,{{DT_FILE}}\]\* which is passed to [duarouter](../duarouter.md)
  - Added [randomTrips.py](../Tools/Trip.md#randomtripspy)
    option **-i, --intermediate** {{DT_INT}} for generating trips with intermediate waypoints
  - Added [randomTrips.py](../Tools/Trip.md#randomtripspy)
    option **--maxtries** {{DT_INT}} to control the number of attempts for finding a trip
    which meet the given distance constraints
  - convert almost arbitrary XML outputs (with a given schema) to
    CSV and protobuf, see [Tools/Xml](../Tools/Xml.md)

### Other

- Simulation
  - Removed obsoleted **--incremental-dua-step** and **--incremental-dua-base** option (use **--scale** instead)

- Tools
  - Renamed [randomTrips.py](../Tools/Trip.md#randomtripspy)
    option for setting the id prefix of generated trips from **-t, --trip-id-prefix** to **--prefix**
  - Renamed [randomTrips.py](../Tools/Trip.md#randomtripspy)
    option for setting trip attributes from **-a, --trip-parameters** to **-t, --trip-attributes**

- Documentation
  - An explanation for vehicle insertion can be found
    [here](../Simulation/VehicleInsertion.md).
  - [Default parameters for different vehicle classes](../Vehicle_Type_Parameter_Defaults.md).
  - [Documentation for Calibrators](../Simulation/Calibrator.md). These simulation
    objects allow for dynamic adaption to traffic flow and speed at
    selected edges.
  - The obsolete Media Wiki, Trac and Wordpress instances at the
    SourceForge Hosted apps facility have been removed altogether

- Miscellaneous
  - Some
    [`vClass` definitions](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#abstract_vehicle_class)
    where added and others renamed. Old definitions will continue to
    work but some will cause deprecation warnings.
  - Reference [typemap files](../SUMO_edge_type_file.md) for
    importing edges and polygon data from OSM networks as well as
    from other formats can now be found at {{SUMO}}/data
  - SUMO builds now with [clang](https://clang.llvm.org/) as well
  - The shortcut **-p** for the option **--print-options** was removed because of possible
    name clashes


## Version 0.20.0 (11.03.2014)

### Bugfixes

- Simulation
  - Fixed bug in the computation of safe velocities. Sometimes the
    velocity was too high (causing collisions) but most times it was
    too low (degrading traffic flow).
  - Vehicles now move to the right lane after passing instead of
    staying on the passing lane. (see #1139)
  - Fixed simulation crash related to lane-changing.
  - Fixed bug where vehicles would no longer stop on the correct
    lane if they already stopped on the wrong lane.
  - Fixed bug which would cause collisions due to unsafe lane
    changes
  - Fixed crash when terminating the simulation while vehicles were
    performing continuous lane changing maneuvers (using option **--lanechange.duration**)
  - Fast vehicles can now be correctly inserted closely in front of
    slow vehicles #1176.
  - Rerouting fixed for vehicles which are on internal lanes when
    the rerouting is requested #1128.
- sumo-gui
  - Fixed crash when using the object locator #923 on some versions of
    Ubuntu.
  - Fixed bug where brake lights were shown even though the vehicle
    was not braking
  - Fixed bug where train carriages where sometimes drawn onto the
    wrong lane
  - The vehicle option 'select foes' now works correctly for foe
    vehicles on internal lanes
  - Fixed rare graphical glitch when drawing concave junction shapes
  - Fixed drawing of segmented vehicles (trains) which are longer
    than their departPos
- netconvert
  - Fixed bug which gave some links the wrong priority thus causing
    collisions (see #1096)
  - Fixed crash when specifying connections from/to edges which are
    removed during network building
  - OSM import no longer fails when encountering empty strings in
    places where numerical values are expected.
  - Fixed bug where permissions involving the vClasses **custom1**
    and **custom2** were not written correctly.
  - Fixed bug where connections from a lane were sometimes indexed
    in the wrong order.
  - Disconnected nodes can no longer be joined into a cluster when
    using the option **--junctions.join**.
  - Fixed (crash #1177) when using option **--ramps.guess**
  - option **--ramps.guess** no longer creates ramps if one of the involved
    connections is a turnaround
  - traffic lights which get new programs are no longer renamed (see #1157)
- DFRouter
  - Fixed crash/infinite loop when using option **--guess-empty-flows**. Use of this option
    is not (yet) recommended (see #1198)
- JTRRouter
  - Fixed handling of empty flows (no vehicles) together with a
    specified start time (see #1109)
- TraCI
  - instantInductionLoops are no longer included when calling
    calling traci.inductionloop.getIDList()
- Tools
  - Fixed bug which prevented traceExporter.py from running
  - Fixed bug which caused all speed values in NS2 output to be 0
  - Fixed bug which prevented traceExporter.py option **--penetration** {{DT_FLOAT}} from taking
    effect

### Enhancements

- Simulation
  - The summary of teleporting vehicles now lists all teleports
    according to their reason
  - Vehicles now perform an emergency braking maneuver to prevent
    driving across a red light. If this happens in a simulation it
    usually indicates that the length of the yellow-phase is to
    short in relation to the road speed and the braking
    capabilities. To enable driving across red lights, additional
    TraCI flags were added (see below).
  - Improved intersection model which results in less conflicts
    between vehicles with intersecting trajectories while on
    internal lanes.
  - Added a first version of the
    [PHEMlight](../Models/Emissions/PHEMlight.md) emission
    model (contribution of TU Graz)
  - Rerouting now keeps existing stops and adds new stops which are
    part of the new route even in cyclic routes
- sumo-gui
  - Reworked drawing of trains to have more realistic [carriage lengths](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#visualization).
    Also some busses are drawn with flexible segments now.
  - The vehicle parameter dialog now lists the maximum speed of the
    vehicle's type and the time headway to the vehicle's current
    leader
  - added vehicle coloring mode *by time headway*
  - Traffic light colors now distinguish between *green major* (code
    **G**) and *green minor* (code **g**) by coloring the minor
    state with a darker color.
  - The lane color scheme *by vClass* now gives fine-grained control
    over coloring by using the [permission bitmask](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#abstract_vehicle_class)
    to select the color.
  - Added menu option Edit-\>Select lanes which allow...-\>*vClass*.
    Together with the street-coloring option *by selection* this
    allows an easy overview of the road network permitted for a
    given *vClass*.
  - Added junction visualization option to disable drawing of
    junction shapes.
  - The vehicle coloring mode *by max speed* now factors in the
    speed limit on the current lane and the speedFactor of the
    vehicle. The same color thresholds as in coloring *by speed* are
    now used.
- netconvert
  - added option **--osm.elevation** {{DT_BOOL}} which imports elevation data from OSM input
    (default *false*).
  - Conflicts between connections originating from the same edge are
    now recognized. This can be used to model interaction between
    right turning vehicles and straight moving non-vehicular traffic
    on a lane further right.
  - improved the heuristic for determining junction clusters to be
    joined when using option **--junctions.join**. This results in fewer invalid joins.
    Candidate clusters for manual intervention are indicated.
- TraCI
  - added method `traci.simulation.convert3D()` which
    converts a road position to either x,y,z or lon,lat,alt
  - added additional bits to the traci command [Change Vehicle State/speed mode](../TraCI/Change_Vehicle_State.md) to
    configure the behavior at intersections. This can be used to
    make vehicles ignore right-of-way rules and also to force them
    to drive across a red light.
  - added optional argument 'flags' to traci.vehicle.setStop() in
    accordance with the [Vehicle modification API specification](../TraCI/Change_Vehicle_State.md)
  - the [API to Areal Lane Detectors was added](../TraCI/Lane_Area_Detector_Value_Retrieval.md);
    thanks to Mario Krumnow and Robbin Blokpoel
  - it is now possible to retrieve the leading vehicle within a
    specified range [Vehicle API specification](../TraCI/Vehicle_Value_Retrieval.md)
  - a vehicle may be added using a full specification (which is
    available via XML input)
- Tools
  - added tool [xml2csv.py](../Tools/Xml.md#xml2csvpy) which
    converts all of the XML-output files written by SUMO to a
    flat-file (CSV) format which can by opened with most
    spread-sheet software
  - added tool [csv2xml.py](../Tools/Xml.md#csv2xmlpy) for the
    opposite conversion, requiring an XML schema and also for
    conversion to Google's protocol buffers
    [xml2protobuf.py](../Tools/Xml.md#xml2protobufpy)
  - osm/osmGet.py now supports retrieving very large areas by
    bounding box using the option **--tiles** {{DT_INT}}. (previously this option was only
    supported together with option **--oldapi** which has severe size limitations)
  - added option **--gpx-output** {{DT_FILE}} to
    [traceExporter.py](../Tools/TraceExporter.md) which allows
    exporting vehicle trajactories as [GPX files](https://en.wikipedia.org/wiki/GPS_eXchange_Format)
  - added option **--poi-output** {{DT_FILE}} to
    [traceExporter.py](../Tools/TraceExporter.md) which allows
    exporting vehicle trajectories as
    [POIs](../Simulation/Shapes.md#poi_point_of_interest_definitions)
  - added option **--vclass** {{DT_STRING}} to
    [randomTrips.py](../Tools/Trip.md#randomtripspy) which
    ensures that generated trips only depart and arrive at edges
    permitted for *vclass*
  - [randomTrips.py](../Tools/Trip.md#randomtripspy) now
    recognizes the network fringe even if the network contains
    turn-around connections (An edge belongs to the fringe if it
    contains only incoming or outgoing connections excepting
    turnarounds). This is important when using the option **--fringe-factor** {{DT_FLOAT}} for
    generating plausible through-traffic in small networks.
  - added two [tools for visualising emission behavior](../Tools/Emissions.md)
  - added some [visualisation tools](../Tools/Visualization.md)

### Other

- TraCI
  - TraCI version is now 8
  - fixed [documentation of of command **slow down**](../TraCI/Change_Vehicle_State.md): It changes speed
    smoothly over the given number of milliseconds (in contrast to
    command **speed** which changes using maximum
    acceleration/deceleration).
- All Applications
  - Added option **--xml-validation** {{DT_STR}} with possible values *never*, *always* and *auto*
    defaulting to *auto*. This causes all input files to be
    validated against their respective schema, provided the
    schema-URL is declared in the toplevel XML-Tag (*always*
    requires the schema-URL to be present).
  - Added option **--xml-validation.net** {{DT_STR}} to all applications which read
    `.net.xml`-files which works like
    option **--xml-validation** {{DT_STR}} but applies only to sumo networks (default *never* since
    network validation is computationally expensive).
  - Most of the input and output files have an XML schema now, see
    <{{Source}}data/xsd>
