---
title: Changes in the 2017 releases (versions 0.29.0, 0.30.0, 0.31.0 and 0.32.0)
---

## Version 0.32.0 (19.12.2017)

### Bugfixes

- Simulation
  - Fixed collisions in the sublane model #3446, #3595, #3613
  - Fixed bug that was causing invalid behavior in the sublane model
    when used with option **--step-method.ballistic**. #3486
  - Fixed bug that was causing deadlocks after undercutting minimum
    gap. #3486
  - Fixed bug that was causing deadlocks at intersections. #3615
  - Option **--ignore-route-errors** now also applies to invalid (i.e. misordered) stop
    definitions. #3441
  - PHEMlight handles large acceleration values better #3390 and has
    updated emission values for new Diesel cars
  - `<stop>` definitions using attribute *until* that are used within a `<flow>` now
    shift the *until* times according to the offset between
    departure and flow *begin*. #1514
  - `<chargingStation>` attribute *chargeDelay* now accepts floating point values.
  - `<chargingStation>` attribute *chargeDelay* now works with subsecond simulation.
  - Vehicles passing a minor link with impatience 0 no longer force
    braking for prioritized vehicles. #3494
  - Fixed bug that was causing collisions between vehicles and
    pedestrians #3527
  - Fixed slow simulation when combining cars and ships in one
    simulation. #3528
  - Fixed collisions on junctions between vehicles coming from the
    same lane. #1006, #3536
  - Fixed failure to change lanes for speed gain in the sublane
    model. #3582
  - Fixed collision of a vehicle with itself. #3584
  - Several fixes in regard to intermodal routing. #3613, #3622, #3560, #3561, #3562

- sumo-gui
  - Fixed crash when simulating pedestrians. #3484
  - Coloring *by selection* is now working for pedestrian crossings. #3396
  - Options **--window-size** and **--window-pos** are now working when set in a configuration file. #3406
  - Vehicle blinkers that signal left or right turns now remain
    switched on while the vehicle is still on the intersection
    (requires networks to be rebuilt). #3478
  - Fixed invalid lane-change blinkers for near-zero lateral
    movements in the sublane model. #3612
  - Fixed invalid vehicle angle when using the sublane model. #3609
  - Networks created with simple projection can now be shown. #3467
  - Fixed invalid *duration factor* in network parameters for
    sub-second simulation. #3600

- polyconvert
  - Fixed handling of XML special characters when exporting
    arbitrary text via option **--all-attributes**. #3447

- netconvert
  - Fixed crash when importing Vissim networks.
  - Fixed bug that was causing invalid signal plans when loading a
    .net.xml file and removing edges from an intersection with
    pedestrian crossings (the link indices for crossings were
    re-assigned but the signal plan was left unmodified creating a
    mismatch).
  - No longer writing pedestrian crossings with length 0 (minimum
    length is 0.1m).
  - Parameters (i.e. those for actuated traffic lights) are no
    longer lost when importing *.net.xml* files or *plain-xml*
    files. #3343
  - Fixed bug that was causing invalid networks to be generated when
    additional lanes were placed to the right of a sidewalk. #3503
  - Fixed bug that was causing invalid networks to be generated when
    nodes without connections were part of a joined traffic light #3715
  - Defining pedestrian crossings for [Pedestrian Scramble](https://en.wikipedia.org/wiki/Pedestrian_scramble) is
    now supported. #3518
  - Custom traffic light plans for pedestrian crossings are no
    longer modified. #3534
  - Fixed invalid traffic light plans at pedestrian crossings for
    node type *traffic_light_right_on_red*. #3535
  - Fixed invalid right of way rules at node type
    *traffic_light_right_on_red* that could cause deadlock. #3538
  - Networks with intersections that are very close to each other
    can now be re-imported. #3585
  - Edges that do not have connections are now correctly represented
    in plain-xml output. #3589
  - Fixed invalid geometry in opendrive-output for lefthand
    networks. #3678
  - Fixed invalid road markings in opendrive-output.

- netedit
  - Fixed bug that was causing pedestrian crossings to remain
    uncontrolled at traffic light controlled intersections. #3472

    !!! caution
        Regression in 0.31.0. As a workaround, networks that were saved with netedit 0.31.0 can be repaired by calling *netconvert -s bugged.net.xml -o fixed.net.xml* or simply reopened and saved with a fresh nightly build of netedit.

  - Options **--window-size** and **--window-pos** are now working when set in a configuration file. #3406
  - Fixed crash when setting linkIndex. #3642

- duarouter
  - Fixed invalid public transport routing if the last vehicle
    departs before the person enters the simulation. #3493

- TraCI
  - Fixed bug in *traci.trafficlights.setLinkState*.
  - Fixed bug in *traci.vehicle.getDrivingDistance* related to
    internal edges. #3553
  - Fixed bug in *traci.vehicle.getDistance* related to looped
    routes. #3648
  - Fixed bug in *traci.simulation.getDistance2D* and
    *traci.simulation.getDistanceRoad* related to internal edges. #3554
  - Command *load* no longer fails when there are too many arguments
    or long file paths. #3599
  - Fixed bug in *traci.vehicle.changeLane* when using the sublane
    model. #3634

- Tools
  - Fixed bug that would trigger an infinite loop in
    [flowrouter.py](../Tools/Detector.md#flowrouterpy).
  - [ptlines2flows.py](../Tutorials/PT_from_OpenStreetMap.md)
    fixes:
    - missing stops no longer result in crashing
    - fixed invalid *until* times when multiple lines use the same
      stop
  - emissionsDrivingCycle now uses the slope values from the correct
    time step when forward calculation of acceleration is enabled
  - [generateTurnDefs.py](../Tools/Misc.md#generateturndefspy)
    now writes interval information. Thanks to Srishti Dhamija for
    the patch. #3712

### Enhancements

- Simulation
  - Added option **--collision.mingap-factor** to control whether collisions are registered when
    the vehicle *minGap* is violated. With the default value of 1.0
    minGap must always be maintained. When setting this to 0 only
    *physical* collisions are registered. #1102
  - Added new [junction model parameters](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#junction_model_parameters) #3148:
    - *jmIgnoreFoeProb, jmIgnoreFoeSpeed* can be used to configure
      right-of-way violations.
    - *jmSigmaMinor* allows configuring driving imperfection
      (dawdling) while passing a minor link.
    - *jmTimegapMinor* configures the minimum time gap when
      passing a minor link ahead of a prioritized vehicle.
    - *jmDriveAfterRedTime* and *jmDriveRedSpeed* allow
      configuring red-light violations depending on the duration
      of the red phase.
  - Added new
    [laneChangeModel-attribute](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#lane-changing_models)
    *lcLookaheadLeft* to configure the asymmetry between strategic
    lookahead when changing to the left or to the right. #3490
  - Added new
    [laneChangeModel-attribute](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#lane-changing_models)
    *lcSpeedGainRight* to configure the asymmetry between
    thrhesholds when changing for speed gain to the left or to the
    right. #3497
  - [Electric vehicles](../Models/Electric.md) can now be used
    for emission-model (electricity) output by setting `emissionClass="Energy/unknown"`
  - Tripinfo-output for pedestrians now includes *routeLength,
    duration* and *timeLoss*. #3305
  - [duration-log.statistics](../Simulation/Output/index.md#aggregated_traffic_measures)-output
    now informs about person rides. #3620
  - Vehicles that end their route with a stop on a parkingArea
    (arrivalPos is within the parkingArea bounds) will be assigned a
    new destination after [rerouting to another parkingArea](../Simulation/Rerouter.md#rerouting_to_an_alternative_parking_area)
    (previously they would drive to the original parkingArea edge
    after finishing their stop). #3647
  - Rerouters now support the attribute `timeThreshold` which makes their
    activation dependent on on a minimum amount of accumulated
    waiting time. #3669
  - Simulation step length has been decoupled from the action step
    length, which is the vehicle's interval of taking decisions.
    This can be configured globally via the option
    '--default.action-step-length', or per vehicle via the parameter
    'actionStepLength'.

- sumo-gui
  - Transparency is now working for all objects.
  - Junction parameters can now be inspected.
  - Upcoming stops are now shown in the vehicle parameter window and
    also in the network when selecting *show current route*. #3679

- netconvert
  - When using option **--numerical-ids** together with option **--output.original-names**, the original IDs of
    all renamed nodes and edges are written to `<param>` elements with key
    *origId*. #3246
  - connections now support the attribute *speed* to set a custom
    (maximum) speed on intersections. #3460
  - connections now support the attribute *shape* to set a custom
    shape. #2906
  - crossings now support the attribute *shape* to set a custom
    shape. #2906
  - The [new element `<walkingArea>`](../Networks/PlainXML.md#walking_areas)
    can now be used in *con.xml* files to define custom walking area
    shapes. #2906
  - Added options **--osm.stop-output.length.bus**, **--osm.stop-output.length.train**, **--osm.stop-output.length.tram** to set appropriate default stop lengths for
    different modes of traffic (in conjunction with option **--ptstop-output**).
  - Added options **--osm.all-attributes** {{DT_BOOL}} which can be used to import additional edge
    parameters such as *bridge*, *tunnel* and *postcode*.
  - Parallel lanes of connecting roads are now written as a single
    road in opendrive-output. #2700

- netedit
  - Additional objects (i.e. detectors) as well as POIs and Polygons
    can now be located based on their ID. #3069
  - Connection and Crossing shapes can now be edited visually. #3464
  - Object types such as edges or polygons can now be locked against
    selection modification. #3471
  - The traffic light index of controlled connections can now be
    edited in *Inspect Mode*. #2627
  - Added button to traffic light mode for cleaning up unused states
    from a traffic light plan.

- duarouter
  - Vehicles and flows which are considered public transport (have
    the line attribute) are now only routed if an additional option **--ptline-routing**
    is given. #2824
  - route alternative output (*.rou.alt.xml*) now contains costs for
    pedestrian stages. #3491

- Tools
  - [osmWebWizard.py](../Tools/Import/OSM.md#osmwebwizardpy)
    can now import public transport (activated by a checkbox on the
    settings tab). If pedestrians are imported as well they may
    elect to use public transport to shorten their walks.
  - added new tool
    [filterDistrics.py](../Tools/District.md#filterdistrictspy)
    to generate district (TAZ) files that are valid for a given
    vehicle class
  - [traceExporter.py](../Tools/TraceExporter.md) can now built
    a direct socket connection to sumo and can filter fcd output for
    regions and times.
  - [flowrouter.py](../Tools/Detector.md#flowrouterpy)
    improvements:
    - route and flow ids now include source and target edge ids
      for better readability. #3434
    - turn-around flow can now be limited using the new option **--max-turn-flow** {{DT_INT}}
  - Added new tool [**tls_csvSignalGroup.py**](../Tools/tls.md#tls_csvsignalgroupspy) for importing traffic light definitions from csv input. The input format aims to be similar to the        representation used by traffic engineers. Thanks
    to Mirko Barthauer for the contribution.

- TraCI
  - return value of trafficlights.getControlledLinks is now a list
    of lists (of links) for the C++ client as well
  - python client now supports the whole API for
    *vehicle.setAdaptedTraveltime* and *vehicle.setEffort*
    (resetting custom values or setting with default time range) by
    using default arguments.

    !!! note
        The order of parameters had to be changed to allow this. Old code will still work but trigger a warning.

### Other

- The SUMO license changed to the [Eclipse Public License Version 2](https://eclipse.org/legal/epl-v20.html)
- The SUMO build process now supports CMake. It is likely that version
0.32.0 will be the last one shipping Visual Studio solutions. Please
have a look at
[Windows CMake](../Installing/Windows_Build.md#manual_cmake_configuration) for
information on how to build SUMO on Windows with CMake. There are
also helper scripts in preparation at [{{SUMO}}/tools/build]({{Source}}tools/build) for instance `tools/build/buildMSVS15Project.py`.

- Simulation
  - **chargingstations-output** now writes times as seconds rather
    than milliseconds.
  - Default value of option **--pedestrian.striping.stripe-width** changed to *0.64* (previously 0.65).
    This allows vehicles with default width to pass a pedestrian on
    a road with default width.
  - preliminary version of [libsumo](../Libsumo.md) is
    available for experimental building of your own apps using SUMO
    as a "library" (calling its functions directly without TraCI)

- sumo-gui
  - default font changed to [Roboto](https://fonts.google.com/specimen/Roboto)
  - Removed OpenGL visualisation option *Antialias*
  - E3-Entry and -Exit detectors are now drawn in darker color to better distinguish them from traffic lights. #3378

- netconvert
  - The element `<customShape>` is no longer supported. Instead `<connection>` and `<crossing>` support the
    *shape* attribute. To set a custom shape for walkingAreas, the
    new element `<walkingArea>` may be used.

- TraCI
  - TraCI version is now 17

- Documentation
  - Documented [simulation object right-click menus](../sumo-gui.md#object_properties_right-click-functions)
  - Described [visualization of edgeData files](../sumo-gui.md#visualizing_edge-related_data)


## Version 0.31.0 (14.09.2017)

### Bugfixes

- Simulation
  - [Sublane-model](../Simulation/SublaneModel.md)
    - Lane changing to clear the overtaking lane ([motivation
      *keepRight*](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#lane-changing_models))
      is now working properly. #3106
    - Fixed error that prevented violating right-of-way rules in
      the sublane-model. #3140
    - Fixed bug that was preventing speed adaptations for
      strategic changing. #3183
    - Fixed error that prevented changing for speed gain #3107, #3186
    - Insertion with `departPosLat="random" departPos="last"` is now working. #3191
    - Fixed bug that could cause deadlocks on an intersection #3189
    - Fixed collisions #3195, #3213, #3050, #2837
    - Fixed invalid angles when lane-changing at low speeds or low
      step-length. #3220
    - Fixed oscillation. #3333
    - Fixed too-late changing for speed gain when approaching a
      slow leader. #3375
    - Fixed bug that was causing sublane-changing despite speed
      loss. #3372

  - Lane-changing
    - Fixed behavior problems in regard to the rule that prohibits
      overtaking on the right under some circumstances (by default
      this is prohibited in free-flowing motorway traffic).
      Vehicles now avoid overtaking whenever braking is possible
      and they overtake on the left instead if there is a third
      lane. #1214, #2547, #3115, #3379
    - Fixed time loss due to late overtaking in some situations. #2126
    - Fixed invalid overtaking to the left. #3337

  - Persons
    - Attribute *arrivalPos* is no longer ignored for person `<ride>`
      elements. #2811
    - Fixed crash when pedestrian routes contain disallowed edges #3248 #3381
    - Fixed collision at prioritized crossings because pedestrians
      ignored some vehicles #3393
  - Calibrators now respect the option **--ignore-route-errors**. #3089
  - `departLane="allowed"` no longer selects an invalid departLane on multimodal edge. #3065
  - Acquired waiting time of vehicles is now properly loaded from a
    simulation state. #2314
  - Fixed output of meso calibrator (regression in 0.30.0). #3132
  - Modified meanTimeLoss output of [lane area (e2)
    detector](../Simulation/Output/Lanearea_Detectors_(E2).md#generated_output).
    Semantics is now average time loss \*per vehicle\* #3110
  - Fixed invalid stop state and invalid position of vehicles that
    cannot resume from parking due to blocking traffic. #3153
  - Fixed erroneous calculation of occupancy in meandata output for
    vehicles being only partially on the corresponding lane. #153
  - Fixed collision detection of junctions (some collisions were not
    registered). #3171
  - Options **--collision.stoptime** is now working when collisions are detected on a
    junction (using Option **--collision.check-junctions**). #3172
  - FCD-output now contains z-data if the network includes elevation
    information. #3191
  - Fixed crash when loading invalid `<busStop>` definition with `<access>` element. #3214
  - Fixed invalid errors on loading stops on looped routes. #3231
  - Fixed crash on invalid `<e2Detector>` definition. #3233
  - Fixed crash on saving and loading simulation state in
    conjunction with vehroute-output. #3237, #3238
  - Fixed too high density values in meandata output. #3265
  - Fixed invalid *routeLength* in tripinfo-output. #3355

- sumo-gui
  - Fixed visual glitch when drawing vehicles with multiple
    carriages as raster images. #3049
  - Fixed crash when reloading a simulation after editing the
    network. #3059
  - Fixed crash when using invalid routes in calibrator. #3060
  - The number of nodes listed in the network parameter dialog no
    longer includes internal nodes. #3118
  - Fixed crash when multiple vehicles start and end parking on the
    same edge #3152
  - All parking vehicles and empty parking spaces of a `<parkingArea>` are now
    accessible via right-click. #3169
  - Fixed rendering position of `<busStop>, <containerStop>, <chargingStation>, <parkingArea>` on curved roads. #3200
  - Fixed drawing position of vehicles with lateral offset and of
    passengers if a vehicle is on a very short lane (also affects
    fcd-output). #3249
  - Corrected drawing of sublane borders in case the lane width is
    not a multiple of the lateral-resolution.
  - Fixed wrong occupancy values in Parameter Window for short
    lanes.
  - [Pre-configured screenshots](../sumo-gui.md#screenshots)
    are now taken at the correct time regardless of simulation
    speed. #1340
  - Fixed visual glitches when drawing waiting pedestrians, parking
    vehicles and parkingAreas in left-hand networks. #3382
  - The list of additional simulation objects no longer includes
    POIs and polygons (they have their own locator lists). #3384

- netconvert
  - **--ptstop-output** now exports stop lanes with the appropriate vClass. #3075
  - **--ptstop-output** now exports stop lanes in the correct road direction #3101, #3212
  - Fixed invalid geo-reference when loading lefthand *.net.xml*
    files #3198
  - Fixed bug that was causing an error when patching a *.net.xml*
    file with a *.tll.xml* file along with other
    connection-affecting patches. #2868
  - Fixed insufficient precision of internal lane elevation in
    OpenDrive output.
  - Fixed overly long yellow duration in generated tls plans. #3239
  - Fixed invalid lengths of internal turning lanes. As a side
    effect lane-changing is not possible any more while on these
    lanes. The old behavior can be enabled by setting option **--junctions.join-turns** {{DT_BOOL}}. #2082.
  - Variable phase durations are no longer lost when importing from
    *.net.xml* or *.tll.xml* files.
  - Information about edges without connections is no longer lost
    when exporting with option **--plain-output-prefix**. #3332
  - Fixed invalid edge shape after importing a *.net.xml* file with
    custom node shape. #3385
  - Fixes related to importing OpenDRIVE networks
    - Fixed error when loading `<roadMark><type>`-data. #3163
    - Fixed crash when loading OpenDRIVE networks with attribute
      *pRange*. #3164
    - Fixed invalid traffic lights. #3203
    - Fixed invalid connections when importing OpenDRIVE networks
    - Fixed invalid internal-lane speed when importing OpenDRIVE
      networks or setting lane-specific speeds. #3240

- netedit
  - Fixed rendering slowdown (regression in 0.30.0) #3167
  - Fixed error when loading pois with attributes *lane* and *pos*
    (regression in 0.30.0) #3199
  - Fixed crash when trying to filter selection of additionals by ID
    (regression in 0.30.0). #3068
  - The z-Coordinate of junctions is now properly displayed again in
    inspect mode (regression in 0.30.00). #3134
  - The z-Coordinate of junctions is no longer reset to 0 when
    moving them with *move mode*. #3134
  - busStop and chargingStation elements with negative *startPos* or
    *endPos* can now be loaded.
  - Undo now restores the selection status of deleted additionals. #3073
  - Loading color schemes is now working (only schemes saved by
    netedit are valid). #2936
  - Fixed invalid geo-reference when editing lefthand networks #3198
  - The cycle time is now always shown for selected traffic lights
    in tls-mode. #3206
  - Output precision set in the Options dialog now take effect. #3219
  - When selecting edges (or lanes) that allow a specific vehicle
    class, edges (and lanes) with `allow="all"` are now matched. #3168
  - Fixed crash when <ctrl\>-clicking on invalid lanes in
    connection-mode.
  - Fixed invalid edge shape after setting a custom node shape. #3276
  - Function *replace by geometry node* now preserves connections,
    crossings and custom geometry endpoints. If the function is
    disabled, the reason is shown in the menu. #3287
  - Joining junctions now always preserves edge endpoints. #3257
  - Fixed invalid network after deleting traffic light and a
    junction with pedestrian crossings. #3346
  - Fixed crash when joining tls. #3365
  - The junction visualization option *Show lane to lane
    connections* now takes effect.

- duarouter
  - Fixed bugs that were causing intermodal routing failures. #3119, #3226
  - Fixed invalid `speedFactor` output when specifying both `speedFactor` and `speedDev` in the input
    files. #3121
  - Fixed (almost) infinite loop when specifying `<flow>` without *end*. #3225
  - Fixed handling of *departPos* and *arrivalPos* for persons. #3246

- marouter
  - Fixed crash due to error in matrix parsing. #3366

- TraCI
  - After sending command *traci.load()* the simulation now keeps
    running until sending *traci.close()* instead of terminating
    when there are no more vehicles or the end time is reached. #3048
  - Vehicle state change retrieval (*simulation.getDepartedIDList,
    simulation.getArrivedNumber, ...*) are now working after sending
    command *traci.load()*. #3051
  - Vehicle commands *getDistance* and *getDrivingDistance* now
    return correct values when the current vehicle edge or the
    target edge are junction-internal edges. #2321
  - Fixed invalid lane occupancy values when calling moveToXY. #3185
  - *traci.load()* is now working if the previous simulation had
    errors. #3204
  - Invalid edge ids in *traci.vehicle.setRoute()* no longer cause
    crashing. #3205
  - Fixed *moveToXY* mapping failures. #3263
  - Person context subscriptions are now working. #3304
  - Fixed invalid behavior after canceling stop. #3348
  - Fixed freeze when calling *gui.screenshot* #3362

- Tools
  - [netdiff.py](../Tools/Net.md#netdiffpy) now correctly
    handles junctions that had their *radius* or *z* attributes
    changed to the (unwritten) default value.
  - [randomTrips.py](../Tools/Trip.md#randomtripspy) now
    correctly handles **--trip-attributes** that contain a space in the value (i.e.
    *modes*). #3117
  - Fixed bug that was causing insufficient flow when using
    [flowrouter.py](../Tools/Detector.md#flowrouterpy). #3285
  - Fixed [flowrouter.py](../Tools/Detector.md#flowrouterpy)
    crash. #3356

### Enhancements

- Simulation
  - Behavior at intersections can now be configured with new
    [junction model
    parameters](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#junction_model_parameters). #3148
  - Emergency vehicles (`vClass="emergency"`) may always overtake on the right.
  - The default car following model can now be specified on the
    command line with **--carfollow.model**. #3142
  - Routing with **--routing-algorithm astar** is now working efficiently when using [traffic assignment zones](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#traffic_assignement_zones_taz). #3144
  - [Lanechange-output](../Simulation/Output/Lanechange.md) now
    includes the *type* of the changing and the longitudinal gaps on
    the target lane. #3156
  - Stops on internal lanes may now be defined.
  - Aggregate trip information generated via option **--duration-log.statistics** now includes
    vehicles that were still running at simulation end if the option
    **--tripinfo-output.write-unfinished** is also set. #3209
  - Vehicles now react to pedestrians on the same lane. #3242
  - Pedestrians now react to vehicles that are blocking their path. #3138
  - Collisions between vehicles and pedestrians are now detected
    when setting the option **--collision.check-junctions**.
  - Pedestrian `<walk>`s may now be defined using attribute *route*. #3302
  - Summary-output now includes mean vehicle speed (absolute and
    relative) as well as the number of halting vehicles. #3312
  - Pedestrian statistics are now included in the [aggregated traffic measures](../Simulation/Output/index.md#aggregated_traffic_measures). #3306
  - Tripinfo-output now includes additional attributes for persons
    and containers stages (*depart, waitingTime, duration, vehicle,
    arrivalPos, actType*). #3305
  - Added new lanechangeModel parameter *lcAccelLat* to model
    lateral acceleration in the sublane model. #3371

- sumo-gui
  - All `<param>` values of simulation objects (i.e. TLS) can now be
    inspected. #3098
  - Calibrators can now be defined for specific lanes not just for
    the whole edge. #2434

    !!! caution
        To obtain the old behavior, calibrators must use the attribute `edge`. When using the attribute `lane` the new behavior is activated.

  - When using the [Sublane model](../Simulation/SublaneModel.md), the lateral offset
    of left and right vehicle side as well as the rightmost and
    leftmost sublane are listed in the vehicle parameter dialog.
  - Added button for calibrating lane/edge colors to the current
    value range. #2464
  - [POIs](../Simulation/Shapes.md#poi_point_of_interest_definitions)
    which are defined using attributes *lane* and *pos* now accept
    the optional attribute *posLat* for specifying lateral offset
    relative to the lane. #3339. These attributes are automatically added
    as [generic parameters](../Simulation/GenericParameters.md)
    retrievable via TraCI.
  - Added option **--tracker-interval** {{DT_FLOAT}} to configure the aggregation interval of value
    tracker windows. Previously this was fixed at 1s. Now it
    defaults to the **--step-length** value.

- netconvert
  - [<split\>-definitions](../Networks/PlainXML.md#road_segment_refining)
    now support the attribute *id* to specify the id of the newly
    created node. Two-way roads can be split with the same node by
    using the same id in two split definitions. #3192
  - Variable lane widths are now taken into account when importing
    OpenDrive networks. The new option **--opendrive.min-width** {{DT_FLOAT}} is used to determine which
    parts of a lane are not usable by the vehicles (*default 1.8m*). #3196
  - Added option **--tls.red.time** {{DT_TIME}} for building a red phase at traffic lights that do
    not have a conflicting stream (i.e. roads with a a pedestrian
    crossing in a network that is not meant for pedestrian
    simulation). The new default value is 5 seconds. #1748
  - Added option **--tls.allred.time** {{DT_TIME}} for building all-red phases after every yellow
    phase. #573
  - Added option **--walkingareas** to allow [generation of walkingareas](../Simulation/Pedestrians.md#walkingareas) in
    networks without pedestrian crossings. #3253
  - Added option **--opposites.guess.fix-lengths** {{DT_BOOL}} to ensure that opposite lane information can be
    set for curved roads. (see
    [Simulation/OppositeDirectionDriving\#Limitations](../Simulation/OppositeDirectionDriving.md#limitations)).
  - Custom lane shapes [can now be defined](../Networks/PlainXML.md#lane-specific_definitions). #2250
  - Added options **--tls.min-dur** {{DT_TIME}} and **--tls.max-dur** {{DT_TIME}} for defining the time range of non-static
    traffic lights. #3329
  - The option **--proj.scale** now accepts arbitrary floats and can be used to
    scale the output network. #3351

- netedit
  - [POIs and Polygons](../Netedit/index.md#pois_and_polygons) can
    now be defined with a new editing mode. #1667
  - Minimum and maximum phase duration for actuated traffic lights
    can now be defined. #831
  - Added button for calibrating lane/edge colors to the current
    value range. #2464

- TraCI
  - Support for multiple clients. #3105
  - Added function *vehicle.getAccumulatedWaitingTime* to retrieve
    the waiting time collected over the interval **--waiting-time-memory**. #999
  - Added many value retrieval functions to the C++ client. Thanks
    to Raphael Riebl for the patch\!
  - New vehicle types can be created dynamically (by duplicating
    existent). -\> *traci.vehicletype.copy()* #3211
  - Added function *person.rerouteTraveltime* to [reroute pedestrians](../TraCI/Change_Person_State.md#command_0xce_change_person_state). #3352
  - Rerouting-device [period can now be set for individual vehicles.
    assumed edge travel times can be set globally.](../TraCI/Change_Vehicle_State.md#supported_device_parameters) #3097
  - Rerouting-device [period and assumed edge travel times can now be retrieved.](../TraCI/Vehicle_Value_Retrieval.md#supported_device_parameters) #3097

- duarouter
  - Routing with **--routing-algorithm astar** is now working efficiently when using [traffic
    assignment zones](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#traffic_assignement_zones_taz). #3144
  - Stops on internal lanes are now supported. #3174
  - Pedestrian `<walk>`s may now be defined using attribute *route*. #3302

- dfrouter
  - Added option **--randomize-flows** for randomizing the departure times of generated
    vehicles.

- od2trips
  - Added option **--pedestrians** for generating pedestrian demand rather than
    vehicles. #3331
  - Added option **--persontrips** for generating [intermodal traffic demand](../Specification/Persons.md#persontrips). #3331

- Tools
  - [osmWebWizard.py](../Tools/Import/OSM.md#osmwebwizardpy)
    now supports location search. #2582
  - [osmWebWizard.py](../Tools/Import/OSM.md#osmwebwizardpy)
    now generates scenarios with actuated traffic lights
  - [osmWebWizard.py](../Tools/Import/OSM.md#osmwebwizardpy)
    now generates traffic with more realistic speed distribution
  - flowrouter.py now supports [specifying route restrictions to
    resolve ambiguities](../Tools/Detector.md#ambiguity).
  - When setting
    [randomTrips.py](../Tools/Trip.md#randomtripspy) option **--vehicle-class**,
    vType attributes from option **-t** are recognized and written to the
    generated vType. #3335

### Other

- SUMO now uses C++11
- specifying the car following model as nested element in a vType is
now deprecated
- trips without ids are deprecated
- router options are now more consistent with simulation options
  - use -a for additional files
  - use -r or --route-files for all kinds of route input (trips,
    flows, routes, alternatives)
  - the old options --flows, --trips, -- alternatives are deprecated
  - The network argument for
    [routeStats.py](../Tools/Routes.md#routestatspy) is now
    optional and set with option **-n**.
- The option **proj.shift** which used to be an alias for **--proj.scale** is no longer supported.
These option were used to set a scaling factor by negative powers of
ten but now **--proj.scale** sets the scaling factor directly (the previous value
**5** now corresponds to **1e-5**)
- default *detector-gap* for [actuated traffic
lights](../Simulation/Traffic_Lights.md#based_on_time_gaps) is
now 2.0s. #3340
- default *minGapLat* value (used by the [sublane
model](../Simulation/SublaneModel.md) is now 0.6m (down from
1.0m) to better match observations.
- Documentation
  - Added description of [automatically generated traffic light
    programs](../Simulation/Traffic_Lights.md#automatically_generated_tls-programs).
  - Added Tutorial for [importing public transport data from
    OSM](../Tutorials/PT_from_OpenStreetMap.md).
  - Extended page on [Safety-related
    topics](../Simulation/Safety.md)
  - Added overview page for
    [Geo-Coordinates](../Geo-Coordinates.md)
- TraCI
  - TraCI version is now 16


## Version 0.30.0 (02.05.2017)

### Bugfixes

- Simulation
  - Random pedestrian decelerations (configured via option **--pedestrian.striping.dawdling**) are now
    working. #2851
  - Loading state-files with vehicles that stop at a `<busStop>` is now
    working. #2914
  - Fixed collisions when using the sublane model. #2295, #2924, #2154, #3005
  - Various fixes to lateral distance keeping in the sublane model.
  - Vehicles now longer drive beyond the road borders when using the
    sublane model. #2988
  - Fixed crash when loading saved stated with an arriving vehicle. #2938
  - Option **--load-state.offset** now properly applies to vehicles departing in the future
    that are part of the loaded state (i.e. due to having been
    loaded from an additional file before saving). This was a
    regression in version 0.29.0. #2949
  - Simulation behaviour is no longer affected by randomly equipping
    vehicles with devices that only generate outputs. #2977
  - vType-attribute `laneChangeModel` is no longer ignored (was silently replaced
    with "default", since version 0.28.0)
  - Fixed deadlock when setting vehicle attributes *arrivalSpeed*
    and *arrivalPos* both to 0. #2995
  - Fixed issue where a lane-change was blocked for invalid reasons
    causing deadlock. #2996
  - When using `departLane="best"`, the look-ahead distance is now limited to 3000m for
    determining suitable insertion lanes. #2998
  - loading state now writes tls states correctly #1229

- netconvert
  - Various fixes to junction-shape computation. #2551, #2874, #1443
  - Ramp-guessing (option **--ramps.guess**) no longer identifies sharply turning
    roads as motorway ramps.
  - Fixed invalid right-of-way rules at junctions with type
    *traffic_light_right_on_red* when importing a *net.xml* file
    or editing with [netedit](../Netedit/index.md). #2976
  - Networks built with option **--junctions.scurve-stretch** now retain their shape when imported
    again. #2877
  - Networks [imported from VISUM](../Networks/Import/VISUM.md)
    no longer round node positions to meters. #3001
  - Loading *.tll* files from a network that includes node types `rail_crossing` or `rail_signal`
      is now working. #2999
  - Fixed crash when applying a `<split>` to an edge within a roundabout. #2737
  - Fixed infinite loop when importing some OpenDRIVE networks. #2918

- netedit
  - [vClass-specific speed limits](../Networks/PlainXML.md#vehicle-class_specific_speed_limits)
    are no longer lost when saving a network. #2508
  - The lane shapes induced by option **--rectangular-lane-cut** are no longer lost when
    editing a network
  - Deleting whole edges is working again (regression in 0.29.0). #2883
  - Deleting edges and afterwards adding edges no longer creates
    node type "unregulated" (without right-of-way rules). #2882
  - Fixed visual glitches when opening left-hand networks. #2890
  - Fixed various crashes. #2902, #3010, #3026, #2969
  - Fixed coloring of green-verge lanes. #2961

- TraCI
  - Fixed mapping failures when calling *moveToXY*.
  - Vehicles that are moving outside the road network due to
    *moveToXY* calls now return the correct position and angle.
  - Function *vehicle.getSpeedWithoutTraCI* now correctly returns
    the current speed if the vehicle is not being influenced.
  - Fixed crash when adding and erasing persons in the same step. #3014

- Tools
  - Restored Python3.0 compatibility for sumolib and traci
    (regression in 0.29.0)
  - carFollowing child-elements of vType element are now included in
    route2trips.py output. #2954
  - Fixed OSM Web Wizard problems with spaces in SUMO_HOME path. #2939
  - [cutRoutes.py](../Tools/Routes.md#cutroutespy) now writes
    two independent routes instead of one containing edges not
    contained in the reduced network. #3011

### Enhancements

- Simulation
  - [vClass-specific speed limits](../Networks/PlainXML.md#vehicle-class_specific_speed_limits)
    can now be loaded from an [*additional-file*](../sumo.md#format_of_additional_files). #2870
  - Refactored implementation of [E2 detectors](../Simulation/Output/Lanearea_Detectors_(E2).md).
    These may now be defined to span over a sequence of lanes,
    XML-attribute `cont` is deprecated. #1491, #1877, #2773, #2871
  - Added [traffic light type "delay_based"](../Simulation/Traffic_Lights.md#based_on_time_loss)
    which implements an alternative algorithm to type "actuated".
  - Added option **--time-to-impatience** {{DT_TIME}} which defines the waiting time until driver
    impatience grows from 0 to 1. Formerly this was tied to the
    value of **--time-to-teleport**. #2490
  - [lanechange-output](../Simulation/Output/Lanechange.md) now
    includes the lateral gap to the closest neighbor. #2930
  - [attribute `speedFactor`](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#speed_distributions)
    can use normal distributions with optional cutoff to define the
    distribution of vehicle speeds #2925
  - [Traffic light related outputs](../Simulation/Output/Traffic_Lights.md) have now
    consistent camelCase XML tags.
  - Added option **collision.stoptime** {{DT_TIME}} which lets vehicles stop for a time after
    experiencing a collision before the action set via **--collision.action** takes place. #1102
  - [Electric vehicles](../Models/Electric.md) are now
    initialized with a maximum capacity of 35kWh and and a half full
    battery by default (before, the default was 0 which always made
    it necessary to define this).
  - **--vehroute-output** now includes additional vehicle attributes such as *departLane*
    and *departSpeed* to facilitate scenario replaying.
  - Increased maximum possible insertion flow when using
    *departLane* values *free,allowed* or *best* on multi-lane
    edges. #3000
  - Improvements to lateral-gap keeping when using the [sublane model](../Simulation/SublaneModel.md):
    - Vehicles now attempt to equalize left and right gaps if
      there is insufficient lateral space #2932
    - The semantics of attribute `minGapLat` where changed to define the
      desired gap at 50km/h and do not grow beyond that speed
      (before that threshold was at 100km/h)
    - Vehicles now ignore follower vehicles behind the midpoint of
      their own length in regard to lateral gap keeping.
  - Added option **--max-num-teleports** {{DT_INT}} which can abort the simulation after a number of
    teleports is exceeded. #3003
  - Added option **--chargingstations-output** {{DT_FILE}} which generates output for
    [chargingStations](../Models/Electric.md#charging_station_output). #2357

- sumo-gui
  - Added parameters
    *minGapLat,maxSpeedLat,latAlignment,boardingDuration,loadingDuration,car
    follow model* to the vType-parameter window.
  - Added parameter *acceleration* to the vehicle-parameter window.
  - Added option *Show type parameter dialog* to the person context
    menu. #2973
  - When running the simulation with option **--duration-log.statistics**, the average travel
    speed of completed trips is shown in the network parameter
    dialog.
  - For long-running simulations the time displays can now show
    elapsed days. #2889

- netconvert
  - Networks imported from
    [DlrNavteq](../Networks/Import/DlrNavteq.md)-format now
    process *prohibited_manoeuvres* and *connected_lanes* input
    files. #738
  - Edge types are now imported from a *.net.xml* file. #2508
  - Added option **--opendrive.curve-resolution** {{DT_FLOAT}} for setting the level of detail when importing
    road geometries from parmeterized curves. #2686
  - Node shape computation (especially stop line position) can now
    be influenced by setting edge geometries that do not extend to
    the node position.
    - To deal with ambiguous stop line information in OpenDrive
      networks, the new option **--opendrive.advance-stopline** {{DT_FLOAT}} may now now be used to affect the
      heuristic that computes stop line positioning based on the
      border between roads and connecting roads.
  - Bus stops can now be imported from OpenStreetMap using the new
    option **--ptstop-output** {{DT_FILE}}. #2933
  - Lanes within a network that have no incoming connection and
    edges that have no outgong connections are now reported. #2997
  - Specific lanes can now be deleted [via loaded *.edg.xml* files](../Networks/PlainXML.md#deleting_edges_or_lanes)
    (to ensure that connections are kept as intended). #3009

- netedit
  - The junction visualization options *show link junction index*
    and *show link tls index* are now working #2955
  - When creating or moving edge and junction geometry, positions
    can now be [restricted to a regular grid](../Netedit/index.md#background_grid) (i.e. multiples of
    100). #2776
  - [Custom edge geometry endpoints](../Netedit/index.md#specifying_the_complete_geometry_of_an_edge_including_endpoints)
    values can now be entered in inspect mode. #2012

- TraCI
  - It is now possible to reload the simulation with new options by
    sending the [load command](../TraCI/Control-related_commands.md#command_0x01_load).
  - Added *vehicle.setMaxSpeed* and *vehicle.getMaxSpeed* to the C++
    client. Thanks to Raphael Riebl for the patch.
  - Added *vehicle.changeTarget* to the C++ client.
  - To allow vehicles to run a red light, speedmode *7* can now be
    used instead of *14*. This is much safer as it avoids rear-end
    collisions.
  - Vehicles can now stop at a named ParkingArea or ChargingStation.
    The methods *traci.vehicle.setParkingAreaStop,
    traci.vehicle.setChargingStationStop* were added to the python
    client to simplify this. #2963
  - vehicle function *moveToXY* now supports the special angle value
    *traci.constants.INVALID_DOUBLE_VALUE*. If this is set, the
    angle will not be factored into the scoring of candidate lanes
    and the vehicle will assume the angle of the best found lane.
    For vehicles outside the road network, the angle will be
    computed from the old and new position. #2262
  - Vehicles now support [retrieval of battery device parameters and retrieval of riding persons and containers as well as retrieval of laneChangeModel parameters](../TraCI/Vehicle_Value_Retrieval.md#device_and_lanechangemodel_parameter_retrieval_0x7e)
    using the *vehicle.getParameter* function.
  - Vehicles now support [setting of battery device parameters and laneChangeModel parameters](../TraCI/Change_Vehicle_State.md#setting_device_and_lanechangemodel_parameters_0x7e)
    using the *vehicle.setParameter* function.
  - Added [sublane-model related](../Simulation/SublaneModel.md) vehicle functions
    *getLateralLanePosition, getMaxSpeedLat, getMinGapLat,
    getLateralAlignment, setMaxSpeedLat, setMinGapLat,
    setLateralAlignment, changeSublane*. #2216, #2001
  - Added [sublane-model related](../Simulation/SublaneModel.md) vehicletype
    functions *getMaxSpeedLat, getMinGapLat, getLateralAlignment,
    setMaxSpeedLat, setMinGapLat, setLateralAlignment*. #2216
  - Function *edge.getLastStepPersonIDs* now includes persons riding
    in a vehicle which are on that edge. #2865
  - The TraCI python client now supports
    [StepListeners](../TraCI/Interfacing_TraCI_from_Python.md#adding_a_steplistener).
  - The lane-changing choices of the laneChange model can now be
    retrieved (with and without TraCI influence) using [command *change lane information 0x13*](../TraCI/Vehicle_Value_Retrieval.md).

- Miscellaneous
  - Improved routing efficiency of
    [sumo](../sumo.md), [duarouter](../duarouter.md) and
    [marouter](../marouter.md) when using option **--routing-algorithm astar**.

- Tools
  - [randomTrips.py](../Tools/Trip.md#randomtripspy) now
    supports the option **--flows** {{DT_INT}} to generate a number of random flows
    instead of individual vehicles.
  - [routeStats.py](../Tools/Routes.md#routestatspy) now
    supports generating statistics on departure time by setting the
    option **--attribute depart**.
  - [tls_csv2SUMO.py](../Tools/tls.md#tls_csv2sumopy) can now
    take arbitrary strings as index and has improved signal group
    handling, thanks to Harald Schaefer
  - more tools (including osmWebWizard) are python3 compatible

### Other

- Documentation
  - The [TraCI command documentation](../TraCI.md#traci_commands) now includes
    links to the corresponding python functions for each command.
  - New [overview page on safety-related behavior](../Simulation/Safety.md)
  - The [Quick Start tutorial](../Tutorials/quick_start.md) now
    describes how to create an network with
    [netedit](../Netedit/index.md)

- TraCI
  - TraCI version is now 15
  - some TraCI constants have been renamed
    - CMD_SIMSTEP2 to CMD_SIMSTEP
    - \*AREAL_DETECTOR\* to \*LANEAREA\*
    - \*MULTI_ENTRY_EXIT_DETECTOR\* TO \*MULTIENTRYEXIT\*

- Miscellaneous
  - The compile-option **--disable-internal-lanes** was removed. Simulation without internal
    lanes is still possible using either the netconvert option **--no-internal-links** or
    the simulation option **--no-internal-links**
  - The compile-option **--enable-memcheck** and the corresponding nvwa package were
    removed. Checking for memory leaks should be done using the
    clang build or valgrind.


## Version 0.29.0 (16.02.2017)

### Bugfixes

- Simulation
  - Fixed emergency braking and collisions related to
    [opposite-direction driving](../Simulation/OppositeDirectionDriving.md). #2652
  - Fixed crashing related to [opposite-direction driving](../Simulation/OppositeDirectionDriving.md). #2664
  - Fixed implausible behavior related to [opposite-direction driving](../Simulation/OppositeDirectionDriving.md). #2665
  - Fixed error where vehicles could not stop at the end of a lane
    for numerical reasons. #2670
  - [Generic parameters](../Simulation/GenericParameters.md) of
    vehicles and vehicle types are now handled when [saving and loading simulation state](../Simulation/SaveAndLoad.md). #2690
  - Stopped and parking vehicles are now handled when [saving and loading simulation state](../Simulation/SaveAndLoad.md). #1301
  - All vehicle parameters and vehicle stops are now handled when
    [saving and loading simulation state](../Simulation/SaveAndLoad.md). #2720
  - Fixed extreme vehicle angles during lane changing when using the
    [sublane model](../Simulation/SublaneModel.md). #2741
  - Fixed error when defining flows without vehicles. #2823
  - Fixed issue where the heuristic to prevent junction blocking
    caused a vehicle to block itself. #2834
  - Fixed bug that was causing pedestrian collisions. #2840
  - The last vehicle from a flow is no longer missing if the flow
    duration is not a multiple of the *period* parameter. #2864

- netconvert
  - Fixed errors when exporting elevation data to OpenDRIVE. #2641
  - Fixed format conformity issues when exporting OpenDRIVE
    networks. #2673, #2674, #2682
  - Networks exported in the [DlrNavteq format](../Networks/Export.md#dlrnavteq) are now written
    with **--numerical-ids** by default.
  - Fixed crash when importing OpenDrive and using [edge-removal options](../netconvert.md#edge_removal). #2685
  - Improved connection guessing at roads with a lane reduction.
  - Fixed crash when specifying a
    [`<split>`](../Networks/PlainXML.md#road_segment_refining)-element
    for a roundabout-edge. #2737
  - The option **--geometry.max-segment-length** is now working when importing OSM data. #2779
  - Improved heuristic for option **osm.layer-elevation** to reduce unrealistic up-and-down
    geometries.
  - Fixed steep grades near intersections in networks with
    3D-geometry. #2782
  - Geometry fixes for output in [DlrNavteq format](../Networks/Export.md#dlrnavteq). #2785, #2786

- netedit
  - Fixed crash when doing undo/redo connection changes at a newly
    created junction. #2662
  - Fixed slow operation when switching between move-mode and other
    modes in a large network.
  - Fixed slow operation when move junctions in large networks. #2699
  - Selecting objects by [matching against attributes](../Netedit/index.md#match_attribute) now works on
    windows. #2675
  - Fixed crash when setting custom geometry endpoints. #2693
  - Fixed shortcuts #2694
  - Fixed crash when using option **--numerical-ids**.
  - Fixed visualization errors when using **--offset.disable-normalization *false* **
  - When adding sidewalks to selected lanes, pedestrians are now
    automatically forbidden on the other lanes. #2708
  - Fixed an inconsistency with the definition of SUMO Time in
    Inspector mode. #2625

- sumo-gui
  - Fixed bug that was causing interface lag when right-clicking in
    networks with detailed geometry.
  - Configuring the visualisation of generated induction loops for
    [actuated traffic lights](../Simulation/Traffic_Lights.md#actuated_traffic_lights)
    is now working. #2639
  - Fixed crash when coloring lanes by occupancy. #2850

- MESO
  - when using option **--meso-tls-penalty** the maximum flow at controlled intersections
    is now scaled according to fraction of available green time. #2753
  - Fixed various issues related to inconsistent handling of net and
    gross time gaps. This was causing exaggerated speeds. The option
    **--meso-taujf** {{DT_TIME}} now corresponds to the net time-gap (as do **--meso-taufj**, **--meso-tauff**). It's default
    value has been reduced from *2.0* to *1.73* correspondingly
    (reproducing the old behavior at 100km/h).

- MESO-GUI
  - Fixed invalid coloring of the gap between lanes. #1428

- duarouter
  - The default vehicle class is now *passenger* (as in the
    simulation). Note, that non-passenger classes such as trains now
    need an explicit type definition to be able to use rail edges. #2829
  - The last vehicle from a flow is no longer missing if the flow
    duration is not a multiple of the *period* parameter. #2864
  - Various fixes to [intermodal routing functionality](../IntermodalRouting.md). #2852, #2857

- TraCI
  - Fixed crash when trying to reroute vehicles before their
    departure.
  - Fixed inefficiency when calling *vehicle.rerouteTravelTime()*
    multiple times per simulation step (edge weights are now updated
    at most once per step).
  - Various fixes to *vehicle.moveToXY* mapping behavior.
  - unsubscribing now works in the python client. #2704
  - The C++ client now properly closes the simulation when calling
    *close()*
  - Subscriptions to 2D-Positions are now working in the C++ client
  - Deleting vehicles that have not yet departed is now working. #2803
  - [Brake lights and blinkers](../TraCI/Vehicle_Signalling.md)
    can now be set for the current simulation step. #2804
  - Vehicle function *move to XY* now works for edges with custom
    length. #2809
  - Vehicle function *move to XY* now actually uses the 'edgeID' and
    'lane index' parameters to resolve ambiguities. It uses the
    'origID' parameter if set and the lane id otherwise.
  - Function *edge.adaptTravelTime(begin=... end=...)* is now
    working as expected with times in seconds. #2808

- Tools
  - [netdiff.py](../Tools/Net.md#netdiffpy) now correctly
    handles changes to `spreadType` and other optional attributes. #2722
  - [sumolib.net](../Tools/Sumolib.md) now returns consistent
    values for *edge.getShape()*: the center-line of all lanes.
    Previously, the raw edge shape used in netconvert was returned.
  - [randomTrips.py](../Tools/Trip.md#randomtripspy) now works
    correctly when giving the options **--via, --validate** at the same time. #2821

- Miscellaneous
  - The [special sub-string *TIME*](../Basics/Using_the_Command_Line_Applications.md#writing_files)
    in output file names is now working on Windows.
  - Various fixes concerning the binary XML format #2650, #2651

### Enhancements

- Simulation
  - Added definition of [parking areas](../Simulation/ParkingArea.md) and functionality for
    [rerouting in search of a free parking space](../Simulation/Rerouter.md#rerouting_to_an_alternative_parking_area).
    Many thanks to Mirco Sturari for this contribution.
  - Added new output option **--stop-output** {{DT_FILE}} for generating [output on vehicle stops (for public transport, logistics, etc.)](../Simulation/Output/StopOutput.md)
  - Vehicles that are inserted with `departPos="last"` now depart at the end of the
    lane when there are no leader vehicles. This reduces the overall
    variance in depart positions. #2571
  - The **--step-log-output** now includes statistics on time spend in TraCI functions. #2698
  - When a [vClass is specified for a vehicle type](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#vehicle_emission_classes),
    this information is used to [assign a default emissionClass](../Vehicle_Type_Parameter_Defaults.md) from
    the [HBEFA3 model](../Models/Emissions/HBEFA3-based.md)
    (Before all vehicles had class
    ["HBEFA2/P_7_7"](../Models/Emissions/HBEFA-based.md) by
    default, even bicycles). The default class for passenger
    vehicles is now "HBEFA3/PC_G_EU4".
  - Output files of the
    [MultiEntryExitDetector](../Simulation/Output/Multi-Entry-Exit_Detectors_(E3).md)
    now include *timeLoss* information. #2703
  - Output files of the
    [LaneAreaDetector](../Simulation/Output/Lanearea_Detectors_(E2).md)
    now include *timeLoss* information. #2703
  - Added option **--load-state.remove-vehicles ID1\[,ID2\]\*** for removing vehicles from a loaded state. #2774
  - Pedestrians now use the *speedDev* attribute of their type to
    vary their walking speed. #2792
  - [tripinfo-output](../Simulation/Output/TripInfo.md#generated_output)
    now includes the attribute *speedFactor* which may have been
    chosen randomly when loading the vehicle.
  - Output precision is automatically increased whenever simulating
    with step-lengths below 10ms. #2816
  - E1 detectors now support the attribute *vTypes* for collecting
    type specific measurements. The attribute *splitByType* is no
    longer supported.

- sumo-gui
  - Added new option **--window-pos** {{DT_INT}},{{DT_INT}} for specifying the initial window placement.
  - Added new button to the object locator dialog that allows
    toggling selection status. #2687
  - Parking vehicles are now listed in the vehicle locator dialog.
    This can be disabled using a new menu option. Teleporting
    vehicles can optionally be listed as well #2750
  - Vehicles can now be colored *by time loss* (accumulated over the
    whole route). The time loss is also shown in the vehicle
    parameter dialog.

- MESO
  - Added new option **--meso-minor-penalty** {{DT_TIME}} for applying a time penalty when passing a
    minor link. This may be used to model slow down on approach or
    even intersection delays when running without **--meso-junction-control**. #2640
  - When using the option **--meso-tls-penalty** {{DT_FLOAT}} to model mesoscopic traffic lights, the
    maximum capacity of edges is now reduced according to the
    proportion of green-time and cycle time at controlled
    intersection. #2753

- MESO-GUI
  - The number of queues for the current segment is now shown in the
    edge parameter dialog
  - The latest headway for the current segment is now shown in the
    edge parameter dialog

- netconvert
  - Pedestrian rail crossings are now working. #2654
  - Added new option **--geometry.check-overlap** {{DT_FLOAT}} for finding edges with overlapping lanes. This
    is typically a sign of faulty inputs. The accompanying option **--geometry.check-overlap.vertical-threshold** {{DT_FLOAT}}
    filters out edges that are separated vertically by at least the
    given value (default *4.0*). #2648
  - The options **--keep-edges.explicit, --keep-edges.input-file** now allow for white-listing in combination with
    other removal options (edges are kept if listed explicitly or if
    any other keep-condition is met). #2680
  - Added new option **--osm.oneway-spread-right** {{DT_BOOL}} for setting the default
    [spreadType](../Networks/PlainXML.md#edge_descriptions)
    to *right* for one-way edges (i.e. motorways).
  - The
    [`<split>`](../Networks/PlainXML.md#road_segment_refining)-element
    now supports the new attributes *idBefore, idAfter* to determine
    the ids of the newly created edges. #2731
  - Added new option **--reserved-ids** {{DT_FILE}} for loading a selecting of node and edge ids
    that shall be avoided in the output network.
  - When importing networks from
    [DlrNavteq](../Networks/Import/DlrNavteq.md) format, the
    new option **--construction-date YYYY-MM-DD** can now be used to interpret the readiness of roads
    under construction.
  - Networks exported to
    [DlrNavteq](../Networks/Export.md#dlrnavteq) format now
    contain additional information: bridge/tunnel information, *form
    of way*, ZIP code, prohibited_manoeuvres, connected_lanes
  - Warnings are now issued when steep grades are present in the
    road network. The warning threshold can be configured using the
    new option **geometry.max-grade** {{DT_FLOAT}}.
  - <laneOffset\> data is now imported from OpenDrive networks

- netedit
  - Added new option **--window-pos** {{DT_INT}},{{DT_INT}} for specifying the initial window placement.
  - new [hotkeys](../Netedit/index.md#hotkeys) implemented. #2694
  - New icons for edit modes. #2612
  - Added new button to the object locator dialog that allows
    toggling selection status. #2687

- od2trips
  - Added new option **--flow-output.probability** {{DT_BOOL}} for generating probabilistic flows instead of
    evenly spaced flows. Thanks to Dominik Buse for the patch.

- duarouter
  - Added option **persontrip.walkfactor** {{DT_FLOAT}} to account for pedestrian delays in [intermodal routing](../IntermodalRouting.md#intermodal_cost_function). #2856


- TraCI
  - Added functions *person.getStage, person.getRemainingStages*,
    *person.getVehicle* and *person.getEdges* to the API, the python
    client and the C++ client. #1595
  - Added functions *person.add*, *person.appendWalkingStage*,
    *person.appendDrivingStage*, *person.appendWaitingStage*,
    person.removeStage'', *person.removeStages, person.setColor,
    person.setLength, person.setHeight, person.setWidth,
    person.setMinGap, person.setType* and *person.setSpeed* to the
    API, the python client and the C++ client. #2688
  - The python client now supports *vehicle.getPosition3D*.
  - Added the functions *vehicle.getLine* and *vehicle.setLine* to
    the python client and the C++ client [(to be used for public transport)](../Specification/Persons.md#riding) #2719
  - Added the functions *vehicle.getVia* and *vehicle.setVia* to the
    python client and the C++ client (affects subsequent rerouting
    calls) #2729
  - Added the functions *polygon.getFilled* and *polygon.setFilled*
    to the python client

- Tools
  - [route_departOffset.py](../Tools/Routes.md#route_departoffset)
    now supports additional options for departure time modification
    based on arrival edge. #2568
  - [sumolib.net](../Tools/Sumolib.md) now supports the new
    function *edge.getRawShape()* to retrieve the shape used by
    netconvert. #2742

- Miscellaneous
  - All applications now support the option **--precision** {{DT_INT}} to specify the output
    precision as number of decimal places for floating point output.
    For lon/lat values this is configured separately using option **--precision.geo** {{DT_INT}}.

### Other

- Documentation
  - Added description of the [Object Locator](../sumo-gui.md#selecting_objects)-menu
  - Added documentation of [visualizing road access permissions](../sumo-gui.md#road_access_permissions)
  - Added documentation of [crossings](../Netedit/index.md#crossings) in netedit
  - Added documentation for the [MESO-model](../Simulation/Meso.md#model_description)
  - Extended documentation of the [sublane-model](../Simulation/SublaneModel.md)
  - Added [TraCI performance information](../TraCI.md#performance)
  - The [TraCI command reference](../TraCI.md#traci_commands) now includes links to the corresponding python method(s).
  - Fixed inconsistencies in [TraCI command reference](../TraCI.md#traci_commands) (mostly methods that
    were available in the python client but not yet documented in
    the wiki).

- Simulation
  - A warning is now issued when trying to define a vehicle with
    vClass=*pedestrian* as this vClass should only be used for
    persons. #2830

- TraCI
  - TraCI version is now 14

- Binary format
  - SBX has now version number 2 #2651

- Miscellaneous
  - [sumo-gui](../sumo-gui.md) and
    [netedit](../Netedit/index.md) now remember their last window position