---
title: Changes in the 2013 releases (versions 0.17.0, 0.17.1, 0.18.0 and 0.19.0)
---

## Version 0.19.0 (27.11.2013)

### Bugfixes

- Simulation
  - Fixed broken xml in vtk-output
  - Fixed crash when using calibrators with discontinuous intervals
  - instantInductionLoops no longer miscount vehicles which change
    lanes on the detector edge.
  - instantInductionLoops now write events in the correct order
  - instantInductionLoops now properly register vehicles staying on
    the detector
  - fixed invalid speeds when running with step-length < 1 (see #1024)
  - the departSpeed value of a vehicle can now be as high as
    laneSpeed \* speedFactor
  - fixed error in a car-following related formula which was causing
    collisions (see #1026)
  - queue-output now correctly measures the length from the start of
    the queue to the rear of the last standing vehicle and no longer
    includes one additional minGap
  - when using `speedDev` and
    `departSpeed`, the distribution will
    be adapted so that a vehicle can use the specified speed.
  - The attribute `emitted` of the
    [summary](../Simulation/Output/Summary.md) output was
    renamed to `inserted`
  - fixed bug which caused vehicles to brake without good reason
    (see #1052)
  - flow on a highway will no longer come to standstill just to let
    vehicles from an on-ramp merge.
  - vehicles now manage to change to the necessary lanes much more
    often and thus avoid having to stop and block upstream traffic.
  - vehicles now use all lanes of a multi-lane roundabout instead of
    only the outer lane.
  - fixed tripinfo output for teleporting vehicles
    (#990)
  - fixed handling of stops before starting position
    (#1056)
  - fixed state loading with vehicle type distributions
    (#1080)
  - fixed memory leak of route distributions together with route
    probes
    (#1077)
  - fixed reference counts for routes in state save

- sumo-gui
  - Right-click now always resolves to the correct object again. The
    problem was introduced in 0.18.0 and was triggered by drawing
    railways
  - The object locator no longer jumps to an arbitrary location if
    the requested object is not found

- TraCI
  - TraCI now returns the "netto" occupancy (without minGap) as
    other outputs do (was #932)
  - Traffic light program changes via TraCI now takes effect
    immediately instead of one step later. (Thanks to Christoph
    Sommer for reporting and fixing this problem)
  - TraCI respects now the begin time option of a simulation
    (#1049)

- duarouter
  - fixed crash when repairing routes with intermediate dead-end
    edges
  - fixed generation of route distributions with 0 probability for
    trips where the starting and ending edge were connected

- netconvert
  - fixed bug where roundabouts sometimes had incorrect right-of-way
    rules
  - option **--keep-edges.in-geo-boundary** now works when giving a .net.xml file as input
  - OSM import no longer fails for non-standard input data (i.e.
    empty *key* attribute or non-numerical value for *tracks*
    attribute)
  - option **--junctions.join** no longer causes edges with a length above **--junctions.join-dist** {{DT_FLOAT}} to be
    removed.

- dfrouter
  - handling end times correctly
    (#428)

### Enhancements

- Simulation
  - Increased Simulation performance when handling lots of traffic
    lights (rewrote code which was updating traffic lights more
    often than necessary)
  - added `vType`-attribute
    `laneChangeModel` for customizing the
    lane changing behavior. Allowed values are *DK2008* (the model
    used until version 0.18.0), *JE2013* and *LC2013* (the new
    default model).
  - Vehicles moving across junctions now consider the exact crossing
    points at which their paths intersect with other vehicles when
    deciding on their speed. This leads to smoother traffic flow
    across junctions.
  - Vehicles may now change lanes while driving across junctions if
    the edge they were coming from has priority (only on networks
    with merged internal lanes, see netconvert changes). This
    improves simulation performance, particularly on multi-lane
    roundabouts.
  - Teleport warnings now always include a reason. For a vehicle
    that is teleported because its waitingTime exceeds
    time-to-teleport the reason can be:
    - **wrong lane** when the vehicle was stuck on a lane which
      did not allow it to continue its route,
    - **yield** when the vehicle was stuck on an unprioritized
      road and did not find a gap in traffic or
    - **jam** when the vehicle could not continue because there
      was no space on the next lane
  - conflicts between speed deviation and departure speed are
    handled more gracefully (#1035)

- sumo-gui
  - now drawing blinkers to visualize a vehicles desire to change
    lanes (this is only noticeable if a vehicle cannot change lanes
    immediately).
  - added vehicle coloring modes *by offset from best lane* and *by
    acceleration*
  - when drawing a vehicles route, the lanes which the vehicle
    intends to use are highlighted where this is known (previously
    the rightmost lane was always used).
  - Added hotkeys <kbd>Ctrl</kbd> + <kbd>A</kbd>, <kbd>Ctrl</kbd> + <kbd>S</kbd>, <kbd>Ctrl</kbd> + <kbd>D</kbd> for running, stopping and
    stepping the simulation
  - Added a menu for opening the object locator dialogs. Using the
    menu hotkeys allows searching for simulation objects without
    using the mouse.
  - The vehicle parameter dialog now also contains information
    related to the vehicle type.

- TraCI
  - lane change requests now have a higher priority by default and
    will thus succeed much more frequently.
  - added the possibility to get the number/IDs of vehicles that
    begin or end to park or stop, see
    [TraCI/Simulation_Value_Retrieval](../TraCI/Simulation_Value_Retrieval.md)
    (was #353)
  - added [new command *lane change mode (0xb6)*](../TraCI/Change_Vehicle_State.md#lane_change_mode_0xb6) to control
    conflict resolution between TraCI-lane-changing request and
    lane-changing decisions by the laneChangeModel. It also allows
    to override safety constraints and to trigger cooperative speed
    adjustments to better fulfill change requests.
  - The python API throws now a TraCIException on recoverable errors
    (such as an unknown vehicle id) and brings the system in a
    consistent state
    (#1043)
  - added possibility to remove waiting cars inspired by Bob Holcomb (#942)
  - improved execution speed of context subscriptions

- netedit
  - added option for reversing the direction of edges
  - lanes can now be selected based on their index

- netconvert
  - when specifying connections it is now possible to refer to edges
    which got split (#492)
  - added new projection option to convert Gauss-Krueger to UTM

- duarouter
  - major refactoring of input parsing, it is now possible to mix
    trips, flows and vehicles in one file
  - stops are respected on routing (#988)

### Other

- netconvert
  - reworked type-related warnings when importing OSM data to
    increase readability
  - modified naming conventions for internal lanes: previously every
    internal lane had its own internal edge. The id of this internal
    edge was ":<junction_id\>_<link_index\>" and the id of the
    internal lane was ":<junction_id\>_<link_index\>_0".
    Now Internal lanes with the same start and end edge have a common
    internal edge (except for lanes following an internal junction).
    The id of this internal edge is
    ":<junction_id\>_<link_index_of_first_lane\>" and the id of the
    internal lanes is ":<junction_id\>_<link_index\>_<lane_index\>".
    The following relation still holds between link indices of
    internal lanes and their ids: <interal_edge_id\> +
    <internal_lane_id\> == <link_index\>. The change was done to allow
    lane changing on internal lanes. As a side effect of merging
    internal lanes into a single edge, lane lengths may differ
    compared to the previous release. This is because the simulation
    length of a lane is always the average of the geometrical length
    of all lanes of an edge.

- TraCI
  - The documentation for vehicle value
    [retrieval](../TraCI/Vehicle_Value_Retrieval.md)/[modification](../TraCI/Change_Vehicle_State.md)
    now includes information on setting/retrieving vehicle-type
    related values for individual vehicles (i.e. length, maxSpeed).
  - removed obsolete vehicle add command
  - TraCI version is now 7

- reworked checks for gdal and fox in configure
- documentation now contains links to the wiki pages it was generated from


## Version 0.18.0 (28.08.2013)

### Bugfixes

- Simulation
  - fixed crash/hangup if a step-length below the minimum of 0.001
    is given
  - fixed bug that caused visual collisions (vehicles on different
    lanes occupying the same space while on a junction)
  - fixed bug where vehicles were inserted with unsafe speeds on
    minor roads causing collisions
  - removed arrival attribute from vehicle routes output for non
    arrived vehicles
  - now saving all routes if a vehicle is rerouted multiple times
  - fixed crash when using calibrators
- TraCI
  - the return values for link direction and link state are now
    working for lane variable: link (0x33)
  - routes are now preloaded before the first simulation step such
    that getMinExpectedNumber gives good results from the very start
  - Fixed documentation bug that stated geodetic positions have the
    format LAT,LON whereas they actually use LON,LAT. TraCI behavior
    remains unchanged.
  - Fixed rerouting for not departed vehicles (change target, change
    route and change route id)
- netconvert
  - street-names will no longer be missing from the output when
    importing a net.xml or edg.xml file which has them
  - using option **--ramps.no-split** {{DT_BOOL}} no longer creates networks with faulty
    connections.
  - fixed bug that caused invalid connections to be generated where
    roads split or merge
  - fixed invalid traffic light logics and occasional crashes when
    importing a .net.xml file along with split elements
  - when importing sumo networks, dangerous connections (multiple
    connections from the same edge to the same lane) are no longer
    silently discarded
  - fixed bug that caused invalid node geometries when using **--junctions.join** together with ramp building options **--ramps.\***.
- duarouter
  - option **--ignore-errors** {{DT_BOOL}} now works correctly when a vehicle is not allowed to
    drive on its depart edge
- sumo-gui
  - vehicles with guiShape="pedestrian" now have the same size in
    all levels of detail
  - traffic lights and lane-to-lane connections are now drawn for
    railways

### Enhancements

- Simulation
  - improved simulation speed
  - added option **--link-output** {{DT_FILE}} for recording the approach of vehicles at
    junctions. This option addresses the need of developers when
    investigating junction control.
  - added new abstract vehicle classes "custom1" and "custom2" for
    user defined purposes.
  - drivers on a minor road now grow impatient while waiting for a
    gap in the traffic. After a while they will drive even if it
    means that another car has to slow down to accommodate them (as
    before, they will only drive if it is safe to do so).
  - added vType attribute **impatience** which affects the
    willingness of drivers to cut into traffic even if it forces
    other drives to brake.
  - added support for stop-signs at minor roads
  - added support for the intersection type *allway_stop*. [This corresponds to an intersection type found in north america](https://en.wikipedia.org/wiki/All-way_stop)
  - added option **--fcd-output.signals** {{DT_BOOL}} for including a vehicles signal state in the
    output (brake lights etc.)
  - implemented "expected" attribute for stops, which expects IDs of
    people that have to enter the vehicle to allow the vehicle to
    leave the stop. Should be used with "triggered='true'" (would
    not work otherwise). Thanks to Shubham Jain for pointing out the
    wrong description of the possibilities in
    [\[1\]](https://sumo.dlr.de/cite.bib).
  - added options for saving and loading simulation states.
    Simulation states can be saved at predetermined times and then loaded to initialize a new
    simulation (this also requires loading the network and routes of the
    original simulation). When the filename for saving ends with **.sbx**
    the file will be saved in a binary format instead of xml.

-   - **--save-state.times** {{DT_TIME}} \[,{{DT_TIME}}...\]
  - **--save-state.files** {{DT_FILE}} \[,{{DT_FILE}}\]\*
  - **--save-state.prefix** {{DT_FILE}}
  - **--load-state** {{DT_FILE}}
  - **--load-state.offset** {{DT_TIME}}
   - option **--sloppy-insert** {{DT_BOOL}} is marked deprecated because the behavior is now the
    default, use **--eager-insert** {{DT_BOOL}} to get the old behavior
  - vehicle route output for persons now includes complete plans

- sumo-gui
  - it is now possible to toggle the simulation time display between
    'seconds' and 'hour-minute-seconds' by clicking on the
    'Time'-button next to the time display. The state of this
    setting persists across GUI sessions.
  - it is now possible to toggle between alternate delay values by
    clicking on the 'Delay'-button next to the delay dial. Clicking
    causes the current delay value to be saved as the alternative
    value. The alternative delay value persists across GUI sessions.
  - parking vehicles are now being drawn (at the side of the road)
  - the vehicle parameter table now contains information about the
    status of stopped vehicles
  - vehicles can now be colored by their selection state
  - added option for selecting a vehicles foes at the next junction
    to the vehicle popup menu (useful when coloring by selecting)
  - brake lights will no longer be triggered when the route ends
    unless the vehicle actually has to slow down.
  - now providing separate options for customizing the visualization
    of persons

- netconvert
  - when using the option **--no-internal-links** the edge lengths are now increased to
    compensate for the missing internal links. Thus, route lengths
    will correspond better to physical distances and roundabouts are
    less prone to congestions.
  - added option **--keep-edges.in-geo-boundary** which works similar to option **--keep-edges.in-boundary** but uses geodetic
    coordinates instead of cartesian.
  - added new node type *priority_stop*. This works similar to type
    *priority* but minor links now require a stop
  - added new node type *allway_stop*. [This corresponds to an intersection type found in north America](https://en.wikipedia.org/wiki/All-way_stop)
  - added option **--ignore-errors.edge-type** for ignoring unknown edge types during loading.
    This happens frequently when dealing with plain xml files based
    on OSM data.
  - now parsing the *OSM*-Attribute "tracks" to determine whether a
    railway element is unidirectional or bidirectional.
  - added option **--osm.railway.oneway-default** {{DT_BOOL}} for setting the default behavior when importing
    railways without "tracks" attribute. The appropriate value
    depends on the mapping style of the regional *OSM* community.
  - added option **--geometry.max-angle** {{DT_FLOAT}} which warns about sharp turns on an edge. This is
    enabled by default for turns above 99 Degrees.
  - added option **--geometry.min-radius** {{DT_FLOAT}} which warns about sharp turns at the start and end
    of an edge. This is enabled by default for radii below 9 meters
  - added option **--geometry.min-radius.fix** {{DT_BOOL}} which prunes geometry to avoid sharp turns at the
    start and end of an edge. This is disabled by default.
  - added option **--check-lane-foes.all** {{DT_BOOL}} (default=false) for lifting junction restrictions
    for vehicles which exit on different lanes of the same edge
    without crossing paths. This improves junction throughput in
    many cases but is a more aggressive interpretation of german
    traffic laws.
  - added option **--check-lane-foes.roundabout** {{DT_BOOL}} (default=true) for lifting some junction
    restrictions at roundabouts. This conforms to the typical layout
    of roundabouts where restrictions on lane-changing allow such
    movements.

- TraCI
  - added optional flags for cmd stop (0x12) to set the parking and
    triggered status
  - added command stop state (id 0xb5) for retrieving the
    stopped,parking and triggered status
  - added command resume (0x19) for resuming from the current stop
  - reduced verbosity on connection close (no more good byes)

### Other

- Simulation
  - The elements **<vehicle\>**, **<flow\>** and **<vType\>** now
    support child elements of the form **<param key="myKey"
    value="myValue/\>**. These can be used when implementing custom
    functionality (see
    [Developer/How_To/Device](../Developer/How_To/Device.md)).
  - code for messaging functionality removed (was disabled by
    default anyway)
- [Contributed/SmallMaps](../Contributed/SmallMaps.md) tools was
removed because it was outdated
- switched all references in code and documentation to sumo-sim.org
- TraCI version is now 6


## Version 0.17.1 (08.05.2013)

### Bugfixes

- sumo-gui
  - fixed crash when opening the viewport-dialog
- netconvert
  - fixed wrong permissions when writing output in the dlr-navteq format


## Version 0.17.0 (03.05.2013)

### Bugfixes

- Simulation
  - tripinfos now contain the correct arrivalPos and routeLength
    values
    ([\#814](https://sourceforge.net/apps/trac/sumo/ticket/814))
  - fixed collision bug for simulations that were running with
    option **--no-internal-links**
  - fixed bug that caused vehicles to sometimes use the wrong lane
    and occasionally lead to collisions
    ([\#586](https://sourceforge.net/apps/trac/sumo/ticket/586),
    [\#836](https://sourceforge.net/apps/trac/sumo/ticket/836))
  - fixed bug in the default car-following model which lead to
    collisions when using vehicle types with different deceleration
    values
  - fixed crash when specifying an invalid car-following model in
    the input files
  - fixed invalid vehicle insertions under special conditions
  - increased junction throughput (fixed bugs related to
    right-of-way timing computation)
  - fixed bug where vehicles drove faster than allowed at arrival
    and when changing from fast to slow lanes
  - simulation routing with taz now works in the presence of vClass
    restrictions.
  - fixed bug where vehicles sometimes did not follow the
    **arrivalLane** attribute.
  - fixed bug where entryExitDetectors issued invalid warnings about
    vehicles leaving without entering
  - fixed bug where inductionLoops and instantInductionLoops
    sometimes failed to detect vehicles
- sumo-gui
  - fixed occasional crash when additional vehicle visualizations
    are active
  - fixed bug where the simulation control buttons (start,step)
    sometimes remained gray when tracking a vehicle
- netconvert
  - fixed bug that caused right-of-way computations to fail
    (resulting in collision) at left-turning connections
  - fixed bug where networks imported from
    [OpenStreetMap](../Networks/Import/OpenStreetMap.md) had
    incorrect lane numbers when forward and backward directions
    differed in lane count.
  - fixed bug where some sets of junctions where erroneously
    classified as roundabouts when using option **--roundabouts.guess** resulting in
    invalid junction logics and causing collisions during simulation
  - fixed bug where inconsistent (unloadable) networks where created
    using option **--ramps.guess**
  - deleted nodes are no longer included in the final network
    boundary
- polyconvert
  - Removed dependency on key/value ordering when importing from
    OSM. Instead, now elements which match multiple typemap entries
    are exported multiple times.
  - fixed projection errors when importing polygons for very large
    networks.
- ROUTER
  - Fixed bug where output files contained wrong values for the
    emissionClass attribute (i.e routers did not properly copy the
    the value from input to output files). This sometimes caused
    duaIterate.py to fail.
  - major refactoring, now handling route, trip and flow inputs with
    one handler (can be mixed in one file)
  - persons and stops in route input files no longer disappear
  - flow handling is similar to simulation (ids generated with "."
    separator)
- TraCI
  - Fixed handling of the parameter to the simulationStep command
    (CMD_SIMSTEP2). Formerly a single simulation step was performed
    if the given target time was lower than the current simulation
    step. Now the simulationStep command is ignored in this case.
    The proper use case for this command is to run the simulation
    up to the given target time. Some scripts (including tutorial
    scripts) wrongly passed the step duration as argument and only
    worked because of this bug. To make these scripts work it is
    sufficient to omit the parameter altogether.

### Enhancements

- Simulation
  - added support for traffic-actuated traffic lights (see
    [Simulation/Traffic_Lights\#loading_a_new_program](../Simulation/Traffic_Lights.md#loading_a_new_program),
    [Networks/PlainXML\#node_descriptions](../Networks/PlainXML.md#node_descriptions))
  - added support for microscopic calibrators. These trigger-type
    objects can be used to increase or reduce traffic volume and to
    regulate speeds at selected edges. Using calibrators it becomes
    possible to calibrate a simulation according to known traffic
    measurements (i.e. from real-life induction loops) without using
    TraCI. For usage examples see the test suite at
    tests/sumo/extended/calibrator.
  - renamed XML root elements of the netstate (raw dump) and the
    mean data outputs.
  - added a car following model which respects slope (kraussPS),
    your network needs z-coordinates then
- sumo-gui
  - added optional POI attributes 'lon' and 'lat' for specifying
    position in geo-referenced networks.
  - added options for showing polygon names and changing their line
    width
  - gui-setting files now support the element `<breakpoints-file value="mybreakpoints.txt"/>` for loading
    breakpoints at the start of the simulation
  - new color representation as four unsigned bytes including an
    alpha channel (transparency) all in the range of 0-255
  - human readable color representations for red, green, blue, cyan,
    magenta, yellow, black, white, and grey
  - junction coloring
- netconvert
  - added support for traffic-actuated traffic lights (see
    [Networks/PlainXML\#Node_Descriptions](../Networks/PlainXML.md#node_descriptions))
  - in plain connection files the element **delete** now also works
    when the connection doesn't exist yet (i.e. when building a
    network from plain nodes and edges and the connection file only
    contains delete elements).
  - now importing most turning restrictions from
    [OpenStreetMap](../Networks/Import/OpenStreetMap.md)
  - when importing SUMO networks without internal link the output
    network will be built without internal links by default.
  - now warning about networks with large absolute coordinates since
    these will cause visual errors in guisim
  - added option **--tls.discard-simple** which discards traffic lights at geometry-like
    junctions (no intersecting streams) loaded from formats other
    than plain-XML. This is particularly useful when importing
    networks from OpenStreetMap which contain many pedestrian
    actuated traffic-lights.
- Tools
  - added tool for creating smaller scenarios from a big scenario
    [Tools/Routes\#cutRoutes.py](../Tools/Routes.md#cutroutespy)
  - replaced the old trace exporter by a new
    [Tools/TraceExporter](../Tools/TraceExporter.md) tool
  - experimental Python 3 support for TraCI python API and
    duaIterate.py
- Tutorials
  - added a [tutorial on trace generation](../Tutorials/Trace_File_Generation.md)
- Documentation
  - added and improved several XML schemata (thanks to Matthew
    Fullerton)
