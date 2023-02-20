---
title: Changes in the 2016 releases (versions 0.26.0, 0.27.0, 0.27.1 and 0.28.0)
---

## Version 0.28.0 (01.11.2016)

### Bugfixes

- Simulation
  - Fixed bug that was causing insufficient overtaking on a
    single-lane road when using sublane simulation (most noticeable
    with small step-lengths). #2518
  - Fixed invalid count of insertion-backlog for flows. #2535
  - Added check for possibility to stop at scheduled stops during
    insertion. #2543
  - Fixed invalid timeLoss values in
    [tripinfo-output](../Simulation/Output/TripInfo.md) (last
    step was counted twice for some vehicles).
  - Fixed bug that would prevent a vehicle from reaching the end of
    it's route after rerouting while being teleported. #2554
  - Fixed erroneous detector output for a special situation. #2556
  - Fixed computation of energy loss for subsecond simulation. #2559
  - Fixed crash when using
    [closing-rerouters](../Simulation/Rerouter.md#closing_a_street)
    that prevent vehicles from departing after being loaded (related
    to #2561).
  - Fixed invalid permissions when using multiple
    [closing-rerouters](../Simulation/Rerouter.md#closing_a_street)
    that modify permissions of the same edge #2560.
  - Fixed crash when using the [sublane model](../Simulation/SublaneModel.md) with varying lane
    widths. #2585
  - The option **--ignore-errors** now properly ignores `vClass`-related errors. #2148
  - Vehicles are now properly angled during lane-changing in
    [sublane-simulation](../Simulation/SublaneModel.md). #2525

- MESO
  - Tripinfo-output now has appropriate `timeLoss` values. #2570

- netconvert
  - Fixed invalid lane permissions when setting lane-specific
    attributes without specifying lane-specific permissions. #2515
  - Fixed invalid elevation values at junctions when importing
    OpenDRIVE networks.
  - Fixed crash when importing OpenDRIVE networks.
  - Fixed invalid internal-lane geometries when exporting OpenDRIVE
    networks. #2562
  - Fixed various specification conformity issues when exporting
    OpenDRIVE networks.
  - Option **--street-sign-output** now only generates signs at *real* intersections.
  - Fixed error when loading a *.net.xml* file and patching lane
    numbers with an *.edg.xml* file. #2459

- sumo-gui
  - The visulization option *show lane borders* is now working.
  - Fixed crash when user loads an empty E3 detector #2249

- netedit
  - Geometry nodes are no longer drawn when set to full transparency
    (alpha = 0).
  - Geometry nodes are now colored for selected edges.
  - All textures are now loaded instead of a white square or a
    netedit crash. #2594 #2476 #2597

- dfrouter
  - Vehicles now always depart with the measured speeds. A vehicle
    type with non-zero `speedDev` is used to allow patching the
    [vehicle-specific speed factor](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#speed_distributions)
    in [sumo](../sumo.md). #2510

- TraCI
  - Vehicles added via method add() of the python client now have
    the correct departDelay value. #2540
  - Vehicle type value retrieval via the [C++ API](../TraCI/C++TraCIAPI.md) is now working.
  - Setting and retrieving edge travel times and efforts via the
    [C++ API](../TraCI/C++TraCIAPI.md) is now working.
  - It is now possible to specify routes that contain
    [TAZ-edges](../Demand/Importing_O/D_Matrices.md#describing_the_taz). #2586


### Enhancements

- Simulation
  - [Ballistic integration method](../Simulation/Basic_Definition.md#defining_the_time_step_length)
    can be used by giving the option **--step-method.ballistic**.
  - [Areal Detectors](../Simulation/Output/Lanearea_Detectors_(E2).md)
    now output vehicle counts. #2523
  - [Rerouters can now be used to close individual lanes](../Simulation/Rerouter.md#closing_a_lane) to
    simulate dynamic work zones and accidents. #2229
  - Electricity consumption is now included in emission outputs. #2211
  - Added option **--save-state.period** {{DT_TIME}} for periodically [saving simulation state](../Simulation/SaveAndLoad.md#saving). The new option
    **--save-state.suffix** {{DT_STR}} can be used to determine whether state should be saved in
    binary (*.sbx*) or XML (*.xml*). #2441
  - Tripinfo-output now includes the values `departPosLat,arrivalPosLat` when running a
    [sublane-simulation](../Simulation/SublaneModel.md).
  - Sublane simulation now supports the [vehicle parameters](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#vehicles_and_routes) `departPosLat,arrivalPosLat`. #2003
  - Added option **--tls.all-off** {{DT_BOOL}} for switching off all traffic lights in the
    simulation.

- netedit
  - Added new option **--new** {{DT_BOOL}} for starting with an empty network.
  - Added new option **--window-size** {{DT_INT}},{{DT_INT}} for specifying the initial window size. #2514
  - Junction attribute *tltype* is now supported.
  - Joined TLS can now be defined by setting the junction attribute
    *tl* following the same rules as in [*.nod.xml* files](../Networks/PlainXML.md#node_descriptions). #2389
  - Added support for restricted lanes (Sidewalks, buslanes and
    bikelanes). #1568
  - Now junction visualization can be toggled between bubbles and
    their real shape when in *Move*-mode.
  - Connections can now be shown in inspect mode (by activating a
    check-box) and their attributes can be edited. They can also be
    selected in select-mode and deleted in delete mode. When
    inspecting edges or lanes, controls for accessing their
    connections are now available. #2067
  - Now delete mode has a frame with information and options for
    deleting. #2432
  - Intervals of Rerouters can be edited using a dialog #2480

- sumo-gui
  - Added new option **--window-size** {{DT_INT}},{{DT_INT}} for specifying the initial window size. #2514
  - Added new visualization setting for drawing right-of-way rules
    (colored bars).
  - [Background images (decals)](../sumo-gui.md#using_decals_within_sumo-gui) now
    support the new Boolean attribute *screenRelative*. When set to
    *true*, position and size are relative to the drawing window
    instead of being relative to the network. #2438

- netconvert
  - Ferry routes are now imported from [OSM when using the *ships* typemap](../Networks/Import/OpenStreetMap.md#recommended_typemaps).
  - Railway crossings are now imported from
    [OSM](../Networks/Import/OpenStreetMap.md). #2059
  - Added new option **--junctions.scurve-stretch** {{DT_FLOAT}} which generates longer intersection shapes
    whenever the number of lanes changes. This allows for smoother
    trajectories and is recommended when [writing OpenDRIVE networks](../Networks/Further_Outputs.md#opendrive_road_networks). #2522
  - Added new option **--rectangular-lane-cut** {{DT_BOOL}} which prevents oblique angles between edges
    and intersections. This option is automatically enabled when
    exporting OpenDRIVE networks. #2562
  - The distance below which all approaching vehicles on foe lanes
    to a specific connection are visible can now be [customized with the new connection attribute `visibility`](../Networks/PlainXML.md#explicitly_setting_which_edge_lane_is_connected_to_which).
    #2123 , #892
  - Elevation data is now interpolated when importing GeoTiff
    heightmaps. #1962
  - The geometry of [OpenDRIVE networks](../Networks/Further_Outputs.md#opendrive_road_networks)
    is now exported with parameterized curves according to
    specification version 1.4. #2041
  - Elevation data is now [imported from OpenDRIVE](../Networks/Import/OpenDRIVE.md) and [exported to OpenDRIVE](../Networks/Further_Outputs.md#opendrive_road_networks).
    #2239 , #2240
  - Added new option **--default.disallow** {{DT_STR}}**,...** for simplifying specification of [vehicle permissions](../Simulation/VehiclePermissions.md#network_definition) #2557
  - When converting an OSM network to [writingDlrNavteq or Amitran format](../Networks/Export.md), functional road class is
    now based on OSM highway types. #2602

- dfrouter
  - Vehicle types are now included in the
    [emitters-output](../Demand/Routes_from_Observation_Points.md#saving_flows_and_other_values)
    by default. The new option **--vtype-output** {{DT_FILE}} allows redirection the vTypes into a
    separate file.
  - If the average measured speeds are systematically above the
    speed limit of the respective roads, the written vTypes use
    attribute
    [speedFactor](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#speed_distributions)
    to reflect systematic speeding.
  - Added new option **--speeddev** {{DT_FLOAT}} to set a [speed deviation](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#speed_distributions)
    for the generate vTypes

- TraCI
  - Added new convenience function *traci.start* that automatically
    selects a port, starts sumo on that port and connects. #2236
  - Added new get/set command *height (0xbc)* to retrieve and modify
    vehicle height (applicable to vehicles and vehicle types). #2573
  - When adapting edge travel times and efforts, the (optional)
    parameters *begin* and *end* are now supported by the [python client](../TraCI/Interfacing_TraCI_from_Python.md) and the
    [C++ client](../TraCI/C++TraCIAPI.md). #2584

- Tools
  - The tool
    [route_departOffset.py](../Tools/Routes.md#route_departoffset)
    (previously undocumented) now supports additional options for
    departure time modification based on departure edge and for
    shifting departures within one time interval to another
    interval. #2568
  - [randomTrips.py](../Tools/Trip.md) supports the new alias **--edge-permission** for former option **--vclass**.

### Other

- Documentation
  - Added Tutorial for using [the OSM-Web-Wizard](../Tutorials/OSMWebWizard.md).
  - More details on [loading and inserting vehicles](../Simulation/VehicleInsertion.md)
  - Added new overview page on [vehicle permissions](../Simulation/VehiclePermissions.md)
  - The [netedit](../Netedit/index.md) documentation on old and novel
    features has been expanded.
  - The tool
    [showDepartsAndArrivalsPerEdge.py](../Tools/Routes.md#showdepartsandarrivalsperedge)
    is now documented. It allows edge-coloring in
    [sumo-gui](../sumo-gui.md) according to traffic statistics #2263

- TraCI
  - TraCI version is now 13

- Miscellaneous
  - The full 24h-dataset for the
    [TAPASCologne](../Data/Scenarios/TAPASCologne.md) scenario
    is now available
  - The deprecated option **--lanechange.allow-swap** is no longer supported. #2116
  - The `laneChangeModel="JE2013"` has been removed due to strong similarity with *LC2013*.
    Using the model is deprecated and now defaults to *LC2013*
  - [SUMOPy](../Contributed/SUMOPy.md) has been added to the
    SUMO repository and to the relase. Many thanks to Joerg
    Schweizer.
  - The libraries for the Windows build and release were updated to
    Fox 1.6.52 and Xerces-C 3.1.4 (with an additional fix for
    [XERCESC-2052](https://issues.apache.org/jira/browse/XERCESC-2052))


## Version 0.27.1 (27.07.2016)

### Bugfixes

- netconvert
  - Original IDs are no longer lost when splitting edges. #2497
  - Elevation data is now correctly imported from OpenDRIVE
    networks. #2239

- netedit
  - Fixed crash when increasing the number of lanes by setting
    numLanes. #2505
  - Fixed slow loading of large networks.
  - Custom colors for selected junctions and edges are now working. #2386

- duarouter
  - Fixed crash when using option **--ignore-error** with invalid `via`-edges. #2494

### Enhancements

- Simulation
  - Added new option **save-state.period** {{DT_TIME}} to save simulation state periodically. #2441
  - Added new option **save-state.suffix** {{DT_STR}} to determine the suffix of saved state files.
    The default is *.sbx* which saves in a binary format.
    Alternatively, *.xml* may be used which makes the state files
    human-readable.

- netconvert
  - Networks exported to OpenDRIVE now use parametric curves to
    represent smooth geometry in place of straight-line segments. #2041
  - Networks exported to OpenDRIVE now contain elevation data. #2240
  - Parametric curves as specified in OpenDRIVE version 1.4 can now
    be imported. #2478
  - Revised default OpenDRIVE typemap. Now imports additional lane
    types such as tram and rail. #2492
  - Added new option **osm.layer-elevation** {{DT_FLOAT}} to import implicit elevation data from
    [OSM-layering
    information](../Networks/Import/OpenStreetMap.md#layer_information).
  - Networks imported from OpenDRIVE now contain edge type
    information based on the OpenDRIVE lane types. #2509

- sumo-gui
  - Added Visualization options for drawing intersections with
    exaggerated size and disable edge drawing based on custom
    widths. #2140
  - Added Visualization option for indicating the driving direction
    of lanes. #2180
  - Added Visualization option for drawing [sublane
    boundaries](../Simulation/SublaneModel.md)
  - Lanes can now be colored according to the number of vehicles
    that are delayed from entering the network. #2489

- netedit
  - Added Visualization options for drawing intersections with
    exaggerated size and disable edge drawing based on custom
    widths. #2140
  - Added Visualization option for indicating the driving direction
    of lanes. #2180


## Version 0.27.0 (12.07.2016)

### Bugfixes

- Simulation
  - Attribute `via` is now used when routing trips and flows within the
    simulation. #1272
  - Vehicles stopping at a `<stop>` now reach the exact location when using
    sub-second step-lengths. #1224
  - Vehicles are no longer considered stopped at a `<stop>` while still
    driving with high speed. #1846
  - Scheduled
    [stops](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#stops)
    no longer count towards *waitSteps* and *timeLoss* in
    [tripinfo-output](../Simulation/Output/TripInfo.md#generated_output). #2311
  - Fixed bug where vehicles would not depart from a triggered stop #2339
  - Fixed deadlock when vehicles with triggered stops could not load
    passengers or containers due to capacity constraints.
  - Fixed invalid edge travel times used for dynamic routing in case
    flow differs among the lanes. #2362
  - Fixed invalid edge travel times used for dynamic routing due to
    invalid averaging #2351
  - Fixed invalid time stamps for leave times and an off by one for
    instant induction loops #1841

- MESO
  - The jam-front back-propagation speed now reaches realistic
    values (it was illogically low before). Note that default value
    of option **--meso-taujj** changes as well as it's semantics. #2244
  - Fixed bug where the simulation would not terminate when using
    calibrators. #2346
  - The options **--meso-tauff** and **--meso-taufj** now define net time gaps (default values were
    changed accordingly). The gross time gaps are computed based on
    vehicle lengths and edge speed to allow for more realistic flow
    in networks with widely varying speed limits. This also affects
    the threshold that defines jamming when using default options
    (thresholds based on allowed speeds). #2364

- netconvert
  - Fixed connection-guessing heuristic. #1992, #1219, #1633, #2398
  - Option **--remove-edges.by-vclass** is now working when loading a *.net.xml* file. #2280
  - Fixed bugs when importing cycleways from OSM. #2324
  - Option **--output.original-names** now records original edge ids even if input edges were
    joined. #2369
  - Fixed invalid road types when exporting OpenDRIVE networks. #2487
  - Fixed invalid lane permissions due to invalid removal of
    geometry-like nodes. #2488

- sumo-gui
  - Fixed crash when loading a large number of background images.
  - Fixed persons showing up too early in the locator. #1673

- netedit
  - Fixed crash when trying to set an empty string as edge length or
    edge width. #2322
  - Fixed crash when deleting the last lane of an edge.

- duarouter
  - Trips and flows that use attribute `via` to loop over the destination
    edge more than once are now working. #2271

- marouter
  - The output is now correctly sorted when using trips as input. #2361

- polyconvert
  - polyconvert output files can now be imported again by
    polyconvert (i.e. for further transformations). #1715

- TraCI
  - Fixed *route.add*, *gui.screenshot* and *gui.trackVehicle* and
    various *lane* commands for the [C++ TraCI API
    client](../TraCI/C++TraCIAPI.md)
  - Fixed crash when trying to set invalid routes. #2285
  - Fixed invalid values when retrieving edge mean speed and edge
    travel time in case flow differs among the lanes. #2362
  - Fixed retrieval of exit times for vehicles that spend multiple
    steps on an inductionloop when retrieving *last step vehicle
    data (0x17)*. #2326

### Enhancements

- Simulation
  - Added [model for
    sublane-simulation](../Simulation/SublaneModel.md). This is
    activated by setting the option **--lateral-resolution** {{DT_FLOAT}}. When using this option,
    vehicles may move laterally within their lanes. This is
    influenced by the new [vType-attributes *latAlignment*,
    *maxSpeedLat*,
    *minGapLat*](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#vehicle_types).
    Lane changing is performed according to the new lane changing
    model
    [SL2005](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#lane-changing_models).
  - Lane-changing models can now be configured with additional `vType`
    parameters. There exists [one parameter for each of the
    lane-changing
    motivations](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#lane-changing_models)
    *strategic,cooperative,speedGain* and *keepRight* which can be
    used to increase or reduce that type of lane changing. #1136
  - Added capabilities for [overtaking through the
    opposite-direction
    lane](../Simulation/OppositeDirectionDriving.md).
  - Added new option **--collision.action** {{DT_STR}} for configuring the action to take on vehicle
    collision. Allowed actions are *none,warn,teleport* and
    *remove*. The default is *teleport* which moves the rear vehicle
    involved in a collision onto a subsequent lane (as before).
  - Added new option **--collision.check-junctions** {{DT_BOOL}} to enable geometrical collision checking on
    junctions. By default this option is set to *false* and
    collisions between non-consecutive lanes are ignored as before.
    This option may slow down the simulation. #984
  - ChargingStations [can now be used to declare vehicle
    stops](../Models/Electric.md#stopping_at_a_charging_station). #2248
  - the [vehicle route
    output](../Simulation/Output/VehRoutes.md) now includes
    optional vehicle parameters as `param` entries
  - Added new option **--device.rerouting.adaptation-steps** {{DT_INT}} to switch the default algorithm for averaging
    edge travel times from exponential averaging to a moving average
    over the given number of steps. #2374
  - Added new option **--tls.all-off** {{DT_BOOL}} for switching off all traffic lights (the
    traffic lights can still be switched on via GUI or TraCI). #2442
  - Added new [output for tracking lane change
    events](../Simulation/Output/Lanechange.md). This is
    enabled using the new option **--lanechange-output** {{DT_FILE}}. #2461

- MESO
  - The option **--meso-jam-threshold** {{DT_FLOAT}} now gives additional freedom when configuring a
    speed dependent jam-threshold. When an value below 0 is given
    the absolute value is taking as a factor for the minimum
    unjammed speed. Thus, negative values closer to 0 result in less
    jamming. The default value remains at *-1* and results in the
    original behaviour (values above 0 set the occupancy fraction
    threshold independent of edge speed as before).

- sumo-gui
  - The number of running vehicles and persons is now shown in the
    status bar. This display also acts as a button for opening the
    network parameter dialog. #1943
  - [Charging
    stations](../Models/Electric.md#charging_stations) are now
    shown in a different color when active.
  - Persons are now more visible when selecting *Draw with constant
    size when zoomed out*.
  - Added the averaged speeds that are used for [simulation
    routing](../Demand/Automatic_Routing.md) to the lane
    parameter dialog.
  - Added new option **--demo** {{DT_BOOL}} which automatically reloads and starts the
    simulation every time it ends. #1645

- MESO-GUI
  - Can now color edges by the averaged speeds that are used for
    [simulation routing](../Demand/Automatic_Routing.md).
  - Can now color edge segments (mesoscopic vehicle queues)
    individually by various traffic measures. #2243

- netconvert
  - Added option **default.lanewidth** {{DT_FLOAT}} for setting the default width of lanes (also
    applies to [netgenerate](../netgenerate.md)).
  - Added option **numerical-ids** {{DT_BOOL}} for forcing all node and edge IDs to be integers
    (also applies to [netgenerate](../netgenerate.md)). #1742
  - Added Option **speed.minimum** to avoid negative speeds when using Option **--speed.offset**. #2363

- netedit
  - Many additional network structures such as busStops, detectors
    and variable speed signs can now be defined and manipulated. #1916

- netgenerate
  - Added option **grid.alphanumerical-ids** {{DT_BOOL}} for using a chess-like intersection naming scheme
    (A1, B3, etc).

- TraCI
  - Added [vehicle
    command](../TraCI/Vehicle_Value_Retrieval.md) *next TLS* to
    retrieve upcoming traffic lights along a vehicles route. #1760
  - The [vehicle
    command](../TraCI/Change_Vehicle_State.md#move_to_xy_0xb4)
    *move to XY* (formerly *move to VTD*) now supports an additional
    flag which selects whether the original route shall be kept or
    the route may change and whether the vehicle may leave the road
    network. #2033, #2258
  - The [vehicle
    command](../TraCI/Change_Vehicle_State.md#move_to_xy_0xb4)
    *move to XY* now allows moving vehicles that are still in the
    insertion buffer.
  - Added functions *vehicle.add, vehicle.remove* and
    *vehicle.moveToXY* to the [C++ TraCI API
    client](../TraCI/C++TraCIAPI.md)
  - Added object variable subscriptions and context subscriptions to
    the C++ TraCI-library (*subscribe, getSubscriptionResults,
    subscribeContext, getContextSubscriptionResults*). Thanks to
    Erik Newton for the patch.
  - Added person value retrieval functions to the [C++ TraCI API
    client](../TraCI/C++TraCIAPI.md). Thanks to Caner Ipek
    for the patch.
  - Added [vehicle
    command](../TraCI/Vehicle_Value_Retrieval.md) *get
    speedmode 0xb3* to retrieve the [speed
    mode](../TraCI/Change_Vehicle_State.md#speed_mode_0xb3)
    of vehicles. #2455
  - Added [vehicle
    command](../TraCI/Vehicle_Value_Retrieval.md) *get slope
    0x36* to retrieve the slope at its current position #2071
  - Added [vehicle](../TraCI/Vehicle_Value_Retrieval.md),
    [lane](../TraCI/Lane_Value_Retrieval.md) and
    [edge](../TraCI/Edge_Value_Retrieval.md) command *get
    electricity consumption 0x71* to retrieve the electricity
    consumption if the emission model supports it. #2211
  - Multiple subscriptions for the same object are now merged. #2318

- Tools
  - Added new tool
    [createVehTypeDistributions.py](../Tools/Misc.md#createvehtypedistributionspy)
    to simplify definition of heterogeneous vehicle fleets by
    sampling numerical attributes from configurable distributions.
    Thanks to Mirko Barthauer for the contribution.
  - parsing xml files with
    [sumolib.output.parse()](../Tools/Sumolib.md) is now much
    faster. #1879

### Other

- Documentation
  - Test coverage analysis can now be found at
    [\[1\]](http://sumo.dlr.de/daily/lcov/html/).
  - Documented [Wireless device detection
    model](../Simulation/Bluetooth.md) which has been available
    since version 0.18.0

- netconvert
  - Network version is now 0.27.0

- TraCI
  - TraCI version is now 12.
  - The [vehicle command](../TraCI/Change_Vehicle_State.md)
    *move to VTD* is now referred to as *move to XY* in client code. #2032

## Version 0.26.0 (19.04.2016)

### Bugfixes

- Simulation
  - Fixed crash when rerouting a large number of vehicles in
    parallel. #2169, #2180
  - Fixes related to [*zipper* nodes](../Networks/PlainXML.md#node_types).
    - Fixed deadlock #2075
    - Fixed collision #2165
    - Fixed undesired non-determinism.
  - Fixed collision of a vehicle with itself when departing at an
    edge that forms a tight circle. #2087
  - Fixed bug that was causing wrong vehicle counts in [summary
    output](../Simulation/Output/Summary.md) and was preventing
    the simulation from terminating automatically when [continuing
    from a loaded state](../Simulation/SaveAndLoad.md). #1494
  - Fixed route errors and crashing when [continuing from a loaded
    state](../Simulation/SaveAndLoad.md) and using [routing
    devices](../Demand/Automatic_Routing.md). #2102
  - Fixed bug that was causing false positives when calling
    *traci.inductionloop.getLastStepVehicleIds* and
    *traci.inductionloop.getLastStepVehicleNumber*. #2104
  - [Induction loop
    detectors](../Simulation/Output/Induction_Loops_Detectors_(E1).md)
    now count vehicles which occupy the detector position during
    insertion.
  - Fixed collisions when using using continuous lane-changing.
  - Fixed bug that was causing erratic emission behavior for stopped
    vehicles when using the [PHEMlight emission
    model](../Models/Emissions/PHEMlight.md). #2109
  - Fixed unsafe traffic light plans when building networks without
    exclusive left-green phase. #2113
  - When using the [*striping*
    model](../Simulation/Pedestrians.md#model_striping),
    pedestrians now avoid moving with near-zero speed. #2143
  - Fixed pedestrian collisions. #2145
  - Intersections with more than 64 connections can now be loaded.
  - When approaching a
    [double-connection](../Networks/PlainXML.md#multiple_connections_from_the_same_edge_to_the_same_target_lane)
    vehicles now prefer the lane with the prioritized connection. #2160
  - Fixed collision at
    [double-connection](../Networks/PlainXML.md#multiple_connections_from_the_same_edge_to_the_same_target_lane). #2170
  - The default vehicle class is now `passenger` when using the default vehicle
    type *DEFAULT_VEHTYPE*. #2181
  - Fixed stuck vehicles when teleporting past an closed edge with a
    [rerouter](../Simulation/Rerouter.md). #2194
  - Fixed invalid route lengths in
    [tripinfo-output](../Simulation/Output/TripInfo.md). #2201
  - [Rerouters](../Simulation/Rerouter.md) rerouters that
    combine `closingReroute` and `destProbReroute` now only apply to vehicles that are affected by
    the closed edges. #2208
  - [Rerouting devices](../Demand/Automatic_Routing.md) can now
    be specified with [generic
    parameters](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#devices). #2209
  - Fixed invalid waiting position of pedestrians after walking. #2220
  - Fixed detector data for teleporting vehicles. #1452
  - Fixed meandata for circular networks. #1032

- sumo-gui
  - Fixed bug that was causing slightly exaggerated exit times to be
    shown when activating *Show Link Items* from the vehicle context
    menu. #2022
  - Fixed flickering brake lights due to small random decelerations. #2177
  - [Areal
    detectors](../Simulation/Output/Lanearea_Detectors_(E2).md)
    can now be hidden by setting their size exaggeration to 0.
  - Fixed invalid occupancy value for [lane area
    detectors](../Simulation/Output/Lanearea_Detectors_(E2).md)
    (was exaggerated by a factor of 100). #2217
  - Fixed crashing related to showing and tracking parameters of
    arrived vehicles. #2226, #2227
  - Fixed glitch when drawing rail carriages on edges with
    customized length. #2192
  - Fixed coordinate update without mouse movement. #2076
  - Fixed time display switch in initial view. #2069
  - Vehicle shape and size are now correctly updated when set via
    TraCI. #2791

- MESO
  - Fixed bug that broke **--meso-multi-queue** behavior (regression in 0.25.0). #2150
  - vClass-specific speed limits are now used. #2205
  - tripinfo-output now contains valid *timeLoss* values. #2204
  - Fixed invalid travel time computation during simulation routing
    (was averaging segments instead of vehicles).

- MESO-GUI
  - Fixed crash. #2187
  - Coloring vehicles *by selection* is now working. #2149

- netedit
  - Fixed bug that made it impossible to modify numerical attributes
    (lane numbers, phase duration etc.) on some computers. #1966
  - Fixed error when modifying signal plans for joined traffic
    lights. #2185
  - Fixed invalid edge length attribute when inspecting networks
    without internal links. #2210
  - Fixed bug where junctions with uncommon shapes could not be
    selected.

- netconvert
  - Fixed bug that was causing unsafe [internal
    junctions](../Simulation/Intersections.md#waiting_within_the_intersection)
    to be built. #2086, #2097
  - Fixed bug that was causing z-information to become corrupted.
    Thanks to Mirco Sturari for the patch.
  - Fixed bug where pedestrians never got the green light when
    loading a *.net.xml* file and adding pedestrian crossings.
  - Fixed bug where pedestrian *walkingarea* edges were missing.
    (This could cause invalid routes to be generated). #2060
  - Multiple connections from the same edge to the same target lane
    can now be set in post-processing (i.e. after removal of
    geometry-like nodes). #2066
  - Option **--tls.guess-signals** now respects option **--tls.unset**. #2093
  - Fixed invalid traffic light plans for networks with pedestrian
    crossings. #2095
  - Loading [custom traffic light
    plans](../Networks/PlainXML.md#traffic_light_program_definition)
    now correctly affects the [building of internal
    junctions](../Simulation/Intersections.md#waiting_within_the_intersection). #2098
  - Several fixes in regard to OpenDrive networks:
    - Added missing `contactPoint` attribute when writing
      [OpenDrive](../Networks/Export.md#opendrive) networks.
    - Fixed geometry of lanes within intersections when writing
      [OpenDrive](../Networks/Export.md#opendrive) networks.
    - Fixed geometry of lanes when importing imprecise
      [OpenDrive](../Networks/Export.md#opendrive) networks #2085.
  - Option **--street-sign-output** now writes [xsd-conforming](../XMLValidation.md)
    output.
  - Fixed bugs that were causing invalid TLS plans to be generated
    for joined traffic lights. #2106, #2107
  - Fixed crash when importing OSM networks related to self-looping
    edges. #2135
  - Fixed bug that was causing invalid junction shapes and extremely
    large network boundaries. #2141
  - Fixed crashing (on Windows) and invalid traffic lights (Linux)
    when loading a *.net.xml* file and adding splits. #2147
  - Fixed invalid connections at edges with vClass-exclusive lanes. #2157, #2158
  - Fixed invalid traffic light plans for node type `traffic_light_right_on_red`. #2162
  - Fixed unsafe junction logic when using custom tls plans with
    node type `traffic_light_right_on_red`. #2163
  - Connections from lanes that are added during ramp guessing can
    now be specified. #2175
  - User-defined connections are no longer discarded at guessed
    ramps. #2196
  - Fixed error when guessing overlapping off-ramps. #2213
  - Fixed error when computing edge shapes with unusual input
    geometries. #2218

- TraCI
  - Multiple fixes to the [C++ TraCI
    library](../TraCI/C++TraCIAPI.md)
    - commands *gui.setScheme, gui.getScheme*,
      *inductionloop.getVehicleData*
    - various *set*-commands were not consuming all result bytes
      and thus corrupted the message stream. Thanks to Alexander
      Weidinger for the patch. #2007
  - Fixes to TraaS functions *Simulation_getDistance2D* and
    *Simulation_getDistanceRoad*. #2127
  - Fixed crash when using vehicle command *move to VTD*. #2129
  - vehicle command *move to VTD* can now position vehicles on
    internal lanes. #2130
  - Commands that return the road distance no longer return an
    exaggerated value in networks without internal links (distances
    across intersections were counted twice).
  - vehicle command *distance* now takes the depart position into
    account (was using 0 before). #2203

- Tools
  - Fixed error that prevented
    [Tools/Visualization\#plot_net_dump.py](../Tools/Visualization.md#plot_net_dumppy)
    from running.
  - Fixed import errors in
    [vehrouteDiff.py](../Tools/Output.md#vehroutediffpy)
    (formerly undocumented tool).
  - [netdiff.py](../Tools/Net.md#netdiffpy) now correctly
    handles non-ascii characters (i.e. international street names). #2112
  - [netdiff.py](../Tools/Net.md#netdiffpy) now handles
    pedestrian crossings. #1893

### Enhancements

- Simulation
  - [MESO](../Simulation/Meso.md) and
    [MESO\#MESO-GUI](../Simulation/Meso.md#meso-gui) are now open.
  - Can now simulated rail road crossings (see netconvert
    enhancement below). Vehicles will get a red light whenever a
    train is approaching. #1744
  - Added option **--emission-output.precision** {{DT_INT}} for configuring the numerical precision of vehicle
    emissions. #2108
  - Added new
    [departPos](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#departpos)
    value `last` which can be used to maximize flow and still maintain
    vehicle ordering. #2025
  - [Rerouters](../Simulation/Rerouter.md) now apply to
    vehicles that are already on the rerouter edge at the start of
    the active interval. #2207
  - Vehicles that are equipped with a [rerouting
    device](../Demand/Automatic_Routing.md) now incorporate
    knowledge about the current traffic state when computing a new
    route due to encountering a [rerouter
    object](../Simulation/Rerouter.md#assigning_a_new_destination). #2197
  - [Parking](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#stops)
    vehicles are no included in the
    [FCD-Output](../Simulation/Output/FCDOutput.md).
  - Riding [persons](../Specification/Persons.md#rides) and
    [containers](../Specification/Containers.md#transports) are
    now included in
    [FCD-Output](../Simulation/Output/FCDOutput.md) and
    [Netstate-Output](../Simulation/Output/RawDump.md). #1726, #1727
  - The [car following
    model](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#car-following_models)
    can now be defined using the new `vType`-attribute
    [carFollowModel](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#vehicle_types)
    (as alternative to using a child XML-element). #1968
  - Added new [PHEMlight](../Models/Emissions/PHEMlight.md)
    version. #2206

- sumo-gui
  - Adjusted zooming distance when centering on simulation objects
    to make objects easier to find. #2146
  - Added option **--waiting-time-memory** {{DT_TIME}} for tracking accumulated waiting time of vehicles.
    The accumulated waiting time (seconds of waiting time within the
    configured interval, default 100s) can be inspected in the
    vehicle's parameter window and the vehicles can be colored
    according to this value. #999
  - Lanes can now be colored *by routing device assumed speed*. This
    is an exponential moving average of mean travel speeds used for
    [dynamic rerouting](../Demand/Automatic_Routing.md). #2189

- MESO
  - Added option **--meso-tls-penalty** {{DT_FLOAT}} as an alternative way to model the delay effects
    of traffic lights. When this option is set to a positive value,
    the expected delay time for each controlled link (based on red
    duration and cycle duration) is added to the travel time,
    multiplied with the {{DT_FLOAT}} argument. By calibrating the parameter, the
    quality of TLS coordination can be modeled. #2199

- MESO-GUI
  - Coloring vehicles *by depart delay* is now working. Added
    *depart delay* to the vehicle parameter dialog.
  - Added *event time*, *entry time* and *block time* to the vehicle
    parameter dialog. These values record when a vehicle leaves,
    entered and was blocked on an edge segment.

- netconvert
  - Added new [node type
    *rail_crossing*](../Networks/PlainXML.md#node_types)
    to model behavior at a rail road crossings. #1744

- polyconvert
  - Added default typemaps similar to netconvert. #1853

- duarouter
  - Added person trips and
    [IntermodalRouting](../IntermodalRouting.md)
  - When a flow has a stop definition with attribute `until`, the time is
    shifted for each successive vehicle in the flow. #1514

- marouter
  - Added bulk routing and better OD cell handling for speed
    improvements. #2167

- TraCI
  - The [python
    client](../TraCI/Interfacing_TraCI_from_Python.md) is now
    thread safe when using multiple connections in parallel. Each
    opened connection returns an independent TraCI instance. #2091
  - Added support for vehicle commands to the [C++ TraCI
    library](../TraCI/C++TraCIAPI.md) Thanks to Alexander
    Weidinger for the patch. #2008
  - Added new [TraaS
    commands](../TraCI.md#interfaces_by_programming_language)
    *Edge.getLastStepPersonIDs, Person.getNextEdge,
    Vehicle.getRouteIndex, Vehicle.getStopState, Vehicle.isStopped*
    and some more stop-related vehicle commands.
  - The angle argument of vehicle command [*move to
    VTD*](../TraCI/Change_Vehicle_State.md#command_0xc4_change_vehicle_state)
    now overrides the vehicle angle for drawing and
    [fcd-output](../Simulation/Output/FCDOutput.md). #2131
  - Added new simulation command [*save state
    0x95*](../TraCI/Change_Simulation_State.md#command_0xcc_change_simulation_state)
    for saving the current simulation state. #2191

- Tools
  - [tls_csv2SUMO.py](../Tools/tls.md#tls_csv2sumopy) now
    handles controlled edges within a joined traffic light
    definition automatically.
  - Added option **--ignore-connections** to
    [netcheck.py](../Tools/Net.md#netcheckpy). This can be
    used to compute all components in the node graph without
    considering lane-to-lane connections.
  - Added option **--symmetrical** to
    [generateBidiDistricts.py](../Tools/District.md#generatebididistrictspy)
    (previously undocumented tool). This can improve
    opposite-finding in conjunction with option **--radius**.
  - Added option **--geo** to
    [route2poly.py](../Tools/Routes.md#route2polypy). This can
    be used to visualize routes from one network within another
    network.

### Other

- Miscellaneous
  - Visual Studio project files have been updated MSVC12. While it
    is still possible to build SUMO with MSVC10, this support may be
    dropped in the future in favor of C++11.

- Simulation
  - When [saving simulation
    state](../Simulation/SaveAndLoad.md) as *XML*, lane
    elements now contain their id for easier inspection.
  - The
    [departPos](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#departpos)
    values `maxSpeedGap`, `pwagSimple` and `pwagGeneric` were removed since they never worked as intended.
  - Option **lanechange.allow-swap** is now deprecated.

- sumo-gui
  - Vehicle type parameters were moved to a separate dialog box
    (*Show Type Parameter*). #2133

- TraCI
  - TraCI version is now 11.

- Documentation
  - Added page on [saving and loading simulation
    state](../Simulation/SaveAndLoad.md)
  - [Arrival parameters are now
    documented](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#a_vehicles_depart_and_arrival_parameter)
  - Extended [netedit](../Netedit/index.md) documentation.
  - Described [netdiff.py](../Tools/Net.md#netdiffpy), a tool
    for comparing networks which was undocumented for a long time.
  - Added page on [modelling networks for motorway simulation. In
    particular
    on-off-ramps](../Simulation/Motorways.md#building_a_network_for_motorway_simulation)
  - Added new [overview page for usage of elevation
    data](../Networks/Elevation.md)
  - Added documentation [on influencing the simulation via
    sumo-gui](../sumo-gui.md#influencing_the_simulation)
  - Added detailed [License](../Libraries_Licenses.md) information.
  - All applications report some build configuration when called
    without options. #2118