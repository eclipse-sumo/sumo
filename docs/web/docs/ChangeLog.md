---
title: ChangeLog
permalink: /ChangeLog/
---

## Git Master

### Bugfixes

- Simulation
  - Fixed invalid default parameters for the electrical vehicle
    model that were causing too high energy consumption.
  - Fixed deadlock on controlled intersection with pedestrian
    crossing.  (regression in 1.1.0)
  - Fixed invalid emergency braking in jammed scenario.
  - Fixed inappropriate lane choice when approaching a multi-lane
    roundabout in dense traffic. ,
  - Rail signals ahead of uncontrolled switches now properly take
    the vehicle route into account (this was causing invalid red
    states previously).
  - Options  now also apply to persons.
  - Fixed bug that could corrupt pedestrian routes when walking
    across very short lanes.
  - Fixed bug that could cause sumo to freeze when using
    opposite-direction driving.
  - Fixed crash when using option  with a  lower than 1.
  - Fixed emergency braking after lane changing when using .
  - Fixed exaggerated braking to avoid overtaking on the right.
  - Fixed invalid junction rules when using sublane simulation on
    lefthand-networks
  - Fixed invalid edgeData and tripinfo output in the [mesoscopic model](Simulation/Meso.md) when vehicles are completely
    jammed.
  - Fixed crash when using SSM-Device with opposite-direction-driving #5231 , #5839
  - [FullOutput](Simulation/Output/FullOutput.md) now
    returns all speeds as m/s.
  - Fixed invalid error when using calibrators with a fixed departLane. #5794
  - Fixed invalid jamming at occupied parkingAreas close to the start of a lane. #5864

- SUMO-GUI
  - Fixed crash when rendering short vehicles as *simple shapes*
    with guiShape *truck/trailer* and *truck/semitrailer*.
    (regression in 1.2.0)
  - Pedestrian crossings and walkingareas are no longer listed in
    the edge locator dialog by default (they can still be enabled by
    checking *Show internal structures*.
  - Fixed invalid image position and size when rendering vehicles as
    raster images.  (regression in 1.2.0)
  - Fixed invalid vehicle coloring in mesoscopic mode. (regression
    in 0.32.0).
  - Fixed invalid vehicle angle when using the sublane model in
    lefthand-networks.
  - Fixed 3D-View rotation when holding middle-mouse button.

- NETCONVERT
  - Fixed handling of custom shapes in lefthand networks (for connections, crossings, walkingareas).
  - Fixed missing connections when importing OpenDRIVE networks with
    short laneSections in connecting roads.
  - Fixed invalid link direction at intermodal intersection and at
    intersections with sharp angles. ,
  - Fixed invalid large traffic light clusters when using options .
  - Fixed invalid custom edge lengths when using option
  - Zipper junctions with multiple incoming edges are now supported.
  - Fixed error when guessing ramps which are close to each other.
  - Option  now works correctly at junctions with 2 incoming edges.
  - Fixed invalid junction shape at geometry-like nodes where only
    the lane-width changes.

- NETEDIT
  - Crossing tls indices are now properly reset when deleting
    traffic light.
  - Controlled pedestrian crossings at a *rail_crossing* nodes are
    no longer lost when saving the network with netedit.
    (regression in 1.0.0)
  - Demand mode issues
    - Creating routes from non-consecutive edges is now working
    - Flow attribute route is now correctly saved
    - Fixed bug where some demand attributes could not be edited

- MESO
  - Fixed invalid *departPos* in tripinfo-output when loading saved
        state.

- DUAROUTER
  - Option  is now working for persons.

- JTRROUTER
  - Vehicle types with PHEMlight emission class can now be handled.

- TraCI
  - Fixed TraaS method *Simulation.convertRoad*.
  - Fixed bug where vehicle is frozen after removing stop at
    stopping place via TraCI.
  - Fixed invalid distance to far-away TLS returned by
    *vehicle.getNextTLS*
  - Fixed crash due to invalid person list at busStop.
  - Persons are no longer listed before their departure in
    *traci.person.getIDList()*  (regression in 1.1.0)
  - Fixed crash when using parallel rerouting with TraCI.
  - Function *traci.vehicle.setStop* now correctly updates an existing stop when setting a new *until*-value. #5712
  - TraaS function *SumoTraciConection.close()* now cleanly disconnects from the SUMO server. #4962

### Enhancements

- Simulation
  - Added junction model parameter *jmDriveAfterYellowTime* to
    configure driving at yellow behavior.
  - calibrators now accept attribute *vTypes* to restrict their
    application (insertion/removal) to selected vehicle types.
  - Vehicle
    [<stops>s](Definition_of_Vehicles,_Vehicle_Types,_and_Routes#Stops.md)
    now support the optional attributes *tripId* and *line* to track
    the current state when serving a cyclical public transport
    route. ,
  - Added vehicle class *rail_fast* to model
    [High-Speed-Rail](https://en.wikipedia.org/wiki/High-speed_rail)
  - Netstate-output now includes attribute *speedLat* when using [a
    model for lateral dynamics](Simulation/SublaneModel.md).
  - Pedestrians now switch to *jam resolving* behavior earlier when
    jammed on a pedestrian crossing. The time threshold can be
    configured with the new option  (default 10s)
  - Extended [ToC Model](ToC_Device#Configuration.md) by
    various functionalities (4 new parameters: dynamicToCThreshold,
    dynamicMRMProbability, mrmKeepRight, maxPreparationAccel).
  - Detectors for actuated traffic lights can now be placed on
    upstream lane to achieve the desired distance to the stop line.
  - Added lane-change model parameter *lcOvertakeRight* to model
    violation of rules against overtaking on the right side.
  - Lanechange-output now includes the speeds of surrounding
    vehicles at the time of lane-change.
  - Added new departSpeed values *desired* (departure with
    speedLimit \* speedFactor) and *speedLimit* (departure with
    speedLimit).
  - Added new carFollowModel *W99* which is a 10-Parameter version
    of the Wiedemann model.



- NETCONVERT
  - Now importing High-Speed-Rail tracks from OSM
  - Now importing attribute *railway:bidirectional* from OSM
  - Now importing track number (track_ref) parameter from OSM
  - Railway usage information (main, branch, industrial, ...) can
    now be imported from OSM by loading the new typemap
    [osmNetconvertRailUsage.typ.xml](Networks/Import/OpenStreetMap#Recommended_Typemaps.md).

  - Vehicle class *rail_electric* is now correctly assigned
    depending on railway electrification.
  - Now distinguishing more junction types in verbose summary.
  - Added experimental multi-language support for VISUM import using
    the new option  and language mapping files in .
  - Added option  to influence the heuristic for guessing junction
    type (priority/right-before-left) from edge speeds.



- NETEDIT
  - Inverting selection now takes selection locks into account.
  - When setting tltype to *actuated*, suitable values for *minDur*
    and *maxDur* will be added to the phases.



- SUMO-GUI
  - Added new visualization presets *selection* (coloring everything
    according to selection status) and *rail* (analyze rail networks
    by showing allowed driving directions for all tracks and
    highlighting rail signals when zoomed out).
  - Added context menu option to show/hide detectors for actuated
    traffic lights
  - Added visualization option for drawing an edge color legend.
  - Vehicles and persons can now be removed via right-click menu
    (*remove*).
  - All Vehicle and person parameters that change during the
    simulation are now updated in any open parameter window.
    (previously string parameters were always static). #4210
  - The parkingAreas parameter dialog now includes the number of alternative parkingAreas due to loaded *parkingAreaReroute* definitions.



- POLYCONVERT
  - Importing railway entities from OSM is now supported and [a new
    typemap](Networks/Import/OpenStreetMap#Railway-specific_Objects.md)
    was added for this.



- TraCI
  - Added function *simulation.getBusStopWaitingIDList* to retrieve
    waiting persons.
  - Added function *getPersonCapacity* to vehicle and vehicletype
    domains.
  - Added function *addDynamics* to polygon domain.
  - Added function *highlight* to poi and vehicle domain.
  - TraaS function *Vehicle.setStop* now supports the arguments
    *startPos* and *until* in line with the other clients.
  - Added subscription filtering to C++ client.
  - Added function *person.appendStage* which allows adding a stage object directly to a person plan. #5498
  - Added function *person.replaceStage* which allows replacing an upcoming stage with a stage object. #5797
  - Added function *vehicle.getLateralSpeed* to the python and C++ client. #5010

- Tools
  - [Public transport import from
    OSM](Tutorials/PT_from_OpenStreetMap.md) (also used by
    [osmWebWizard](Tools/Import/OSM#osmWebWizard.py.md) now
    filters out lines that only run at night. If the option  is set,
    only night-service lines are exported.
  - Improving UTF8 support for xml2csv.py and csv2xml.py. ,
  - Added a new tool
    [generateTurnRatios.py](Tools/Misc#generateTurnRatios.py.md)
    for generating turning ratios according to a given route file.
  - The option cost modifier in
    [duaIterate.py](Tools/Assign#dua-iterate.py.md) is
    removed, since it is used for specific projects. Ticket  is open
    to check the respective content for publication and to extend
    the cost modifier function.
  - Added [randomTrips.py](Tools/Trip#Edge_Probabilities.md)
    options  and  to modify trip probabilities by direction of
    travel.

### Other

- TraCI
  - Function *person.getStage* now returns a TraCIStage object instead of an integer denoting the stage type. #5495
  - Functions *person.appendWalkingStage*,
    *person.appendWaitingStage* now send duration value as a double
    in line with all other time values. #5708
  - Fixed inconsistency in client data model for routing results
    (TraCIStage). In the python client, the stage member which
    describes the stage type is now called *type* instead of
    *stageType* in line with the other clients. #5502
  - Function *vehicle.getNextStops* now includes the current stop if
    the vehicle is stopped. This case can be distinguished by the
    first bit ('reached').

- NETCONVERT
  - Network version is now 1.3
  - In the previous version, setting options  activated a heuristic
    that could create joint controllers for clusters of uncontrolled
    nodes (which would not be guessed as controled individually).
    Now this heuristic must be activated explicitly using option .

- NETEDIT
  - Changed button in allow/disallow-dialog from *allow only
    non-road vehicles* to *allow only road vehicles* vClasses.

- NETGENERATE
  - Option  now expects and argument in degrees instead of radians.

## Version 1.2.0 (16.04.2019)

### Bugfixes

- Simulation
  - Fixed too high insertion speeds for the ACC car following model
    leading to emergency braking events.
  - Fixed emergency deceleration after insertion when using IDM
    model.
  - Reduced emergency braking for the Wiedemann carFollowModel.
  - Vehicles approaching an occupied parkingArea where other
    vehicles wish to exit, now cooperate with the exiting vehicle
    instead of rerouting to another parkingArea.
  - Fixed error when loading a  from saved simulation state.
  - Fixed crash when loading a  from saved simulation state and
    vehroute-output is active
  - ParkingAreaReroute now takes subsequent stops into account when
    computing a new route.
  - ParkingAreaReroute now adapts subsequent person stages if the
    person returns to the same vehicle later.
  - Fixed a program crash potentially occurring when different
    car-following models were used in the same situation.
  - Fixed invalid route when adding trip with cyclical stops on the
    same edge.
  - Fixed invalid route and crashing when re-routing trip with
    cyclical stops.
  - Fixed freeze when setting -attribute *lcOpposite="0"* to disable
    opposite direction driving.
  - Person plans that start with a  and continue with a  are now
    working.
  - Fixed invalid public transport routing when a  is not the first
    item in the plan.
  - Simulation now termines if triggered vehicles are not inserted
    due to
  - Intersection behavior
    - Fixed routing bug where the cost of some left-turns was
      underestimated.
    - Fixed deadlock on intersection related to symmetrical left
      turns on multi-modal intersections.
    - Fixed invalid right-of-way behavior on multi-modal
      intersections
    - Fixed bug where vehicles would sometimes drive onto the
      intersection despite downstream jamming.
    - Fixed bugs where actuated traffic light would switch too
      early or too late. ,
    - Fixed deadlock at multi-lane roundabout
    - Fixed emergency deceleration when approaching a zipper node
    - Fixed collisions between vehicles and persons on
      walkingareas. ,
  - Railway simulation
    - Update of averaged edge speeds within *device.rerouting* is
      now working correctly for bidirectional tracks.
    - Fixed behavior at *railSignal* when using *endOffset* for
      signal placement.
    - Fixed invalid behavior at *railSignal* when using
      bidirectional tracks (deadlocks / collisions). , ,
    - Fixed detection of railway collisions. ,
    - Railway insertion on bidirectional tracks now checks for
      presence of oncoming vehicles.

- SUMO-GUI
  - Fixed crash when using the 3D-View.
  - Pedestrian crossing traffic-light indices can be drawn again
    (regression in 1.1.0).
  - Loading breakpoints from a gui settings file using the settings
    dialog is now working.
  - Fix visual glitch when drawing rail carriages with exaggerated
    length.
  - Fixed invalid *pos* value in lane popup for lanes with a strong
    slope.
  - Stopped vehicles no longer have active brake signals.

- NETEDIT
  - *split junction* is now working reliably in intermodal networks.
  - Fixed crash when [copying edge
    template](NETEDIT#Edge_template.md) with lane-specific
    attributes.
  - Fixed index of created lanes when adding restricted lanes with
    context menu. This is partly a regression fix and partly an
    improvement over the earlier behavior.
  - Inspection contour now works correctly for spread bidirectional
    rail edges.
  - Now showing correct edge length when using *endOffset*.
  - Defining e1Detector with negative position (counting backwards
    from the lane end) is now working
  - Setting connection attribute *uncontrolled* now takes effect.
  - Fixed crossing geometries after editing left-hand network.

- NETCONVERT
  - Custom node and crossing shapes are now correctly shifted when
    using options .
  - Fixed invalid right of way rules at traffic light intersection
    with uncontrolled connections. ,
  - Connection attribute *uncontrolled* is no longer lost when
    saving as *plain-xml*.
  - Normal right-of-way rules are never used for unsignalized
    railway switches where all edges have the same *priority* value.
    (all links will have linkstate *M*).
  - Fixed bug where connections were not imported from OpenDRIVE
    networks if junction internal edges have lane sections with
    different lane numbers.
  - Fixed bugs where superfluous traffic light phase were generated. ,
  - Fixed bug where generated traffic light phases had unnecessary
    red lights.
  - Fixed missing connection at roundabout when importing
    *dlr-navteq* networks.
  - Fixed bugs in connection to option  ,
  - Nodes that were joined due to a  declaration are no longer
    joined with further nodes when option  is set.
  - Fixed invalid street names in dlr-navteq output.
  - Fixed invalid edge geometry when importing VISSIM networks
  - Fixed bug where joined traffic lights could get lost when
    importing a .net.xml file with option .
  - Fixed bug where traffic lights could get lost when importing a
    .net.xml file containing joined traffic lights with option .
  - Fixed low-radius connection shapes for left turns at large
    intersections
  - Joining junctions now preserves the prior connection topology. ,
  - Loading patch files with  elements is now working.
  - Fixed invalid turnaround-lane in multimodal networks
  - Patching attributes of existing connections is now working.
  - The default edge priorities for link-roads imported from OSM
    (e.g. on- and offRamps) have been changed to avoid invalid
    right-of-way rules at intersections.

- DUAROUTER
  - Fixed routing bug where the cost of some left-turns was
    underestimated.
  - vType attribute  is no longer lost in the output.
  - Person plans that start with a  and continue with a  are now
    working.
  - Fixed invalid public transport routing when a  is not the first
    item in the plan.

- DFROUTER
  - Fixed duplicate vehicle ids in generated output.

- TraCI
  - Fixed crash when calling moveToXY for a vehicle that is driving
    on the opposite direction lane.
  - Fixed *getParameter* and added *setParameter* calls to TraaS
    client library.
  - Fixed *vehicle.setStop* in TraaS client library.
  - Fixed crash when calling *simulation.getDistance* to compute the
    driving distance between unconnected parts of the network
  - Fixed invalid driving distance result when calling
    *simulation.getDistanceRoad* and one of the edges is an internal
    edges after an internal junction.
  - TraaS functions *Trafficlight.setPhaseDuraton*,
    *Trafficlight.getPhaseDuration*, *Trafficlight.getNextSwitch*,
    *Trafficlight.setCompleteRedYellowGreenDefinition*,
    *Vehicle.setAdaptedTraveltime*, *Vehicle.getAdaptedTraveltime*,
    *Lanearea.getLastStepHaltingNumber* and
    *Lanearea.getLastStepVehicleIDs* are now working (regression due
    to protocol change in 1.0.0).
  - Fixed C++ client function *vehicle.getStopState* (regression due
    to protocol change in 1.0.0)
  - Fixed estimation of speed and acceleration after using moveToXY.
  - *traci.vehicle.getLaneChangeState* now returns correct
    information while controlling the vehicle with moveToXY in
    sublane simulation.
  - Fixed invalid vehicle position after mapping with
    *vehicle.moveToXY* onto a lane with strong slope.
  - Fixed invalid lane change when moving to a new edge with
    different lane number after calling *traci.vehicle.changeLane*.

- Tools
  - restored python3 support for
    [osmWebWizard.py](Tools/Import/OSM#osmWebWizard.py.md)
    (regression in 1.1.0)
  - Fixed error when using sumolib functions
    *net.getNeighboringEdges* and *net.getNeighboringLanes* with the
    same net object.
  - [netdiff.py](Tools/Net#netdiff.py.md) now handles  and
    -elements.,

### Enhancements

- Simulation
  - added new input element
  - lanechange-output now includes *maneuverDistance* when using the
    sublane model.
  - Traffic light phases now support the optional attribute *name*.
    This attribute can be set and retrieved via TraCI and makes it
    easier to establish the correspondence between phase indexing in
    SUMO and phase descriptions used by other traffic engineering
    methods
  - Time losses due to passing intersections from a non-priority
    road are now anticipated when routing even when no vehicle has
    driven there previously. The anticipated loss can be configured
    using the new option  (new default 1.5, previous behavior when
    set to 0). .
  - Vehicles approaching a stop with *parking=true* now signal their
    intention by activating the blinker. When exiting from such a
    stop in dense traffic they signal their intention by activating
    the blinker.
  - Vehicles approaching a signalized junction now either stop at
    the given stop position (stopOffset) at red or they drive on to
    cross the junction if they already passed the stop position.
  - Person [rides](Specification/Persons#Rides.md) now allow
    the special value *ANY* for the *lines* attribute. When this is
    set, persons may enter any vehicle that will stop at the
    destination *busStop* of the ride.
  - Option  now also applies when routing persons.
  - Added output of harmonic mean speed to induction loops, which
    approximates the space mean speed.
  - Added controlability of reaction time (actionStepLength) to
    DriverState Model.
  - \-output now supports attribute  which can be used to record
    pedestrian traffic instead of vehicular traffic.
  - Railway simulation improvements:
    - When a railSignal is placed on a short edge at the incoming
      network fringe, the signal will regulate train insertion for
      all trains longer than the insertion edge.
    - Trains can now reverse on bidirectional edges without the
      definition of a .
    - RailSignal logic no longer requires all railway switch nodes
      to be of type *railSignal*. Instead they can be guarded by
      other railSignal nodes along the incoming tracks.
    - Added new output option  to support analysis of the
      [generated signal block
      structure](Simulation/Output.md#additional_debugging_outputs).

- SUMO-GUI
  - Major improvement in rendering speed
  - Can [now load edge-data](SUMO-GUI#Newer_versions.md)
    ([edgeData-output](Simulation/Output/Lane-_or_Edge-based_Traffic_Measures.md),
    [MAROUTER-output](MAROUTER#Macroscopic_Outputs.md),
    [randomTrips-weights](Tools/Trip#Customized_Weights.md))
    for visualization (time-varying edge colors).
  - The current phase index can now optionally be shown for
    traffic-light controlled junctions. If a name was set for the
    current phase it is shown as well.
  - The current lane-changing state is now included in the vehicle
    parameter dialog.
  - Reloading is now disabled while running as TraCI-server.
  - When *show-detectors* is set for actuated traffic lights, the
    detector outline will switch to green for detectors that are
    used to control the active phase.
  - Added option  to set the default for actuated detector
    visibility.
  - Vehicles with guiShape *truck/trailer* and *truck/semitrailer*
    now bend when cornering.
  - Added vehicle route visualisation *Show Future Route* to the
    vehicle context menu. This only shows the remaining portion of
    the route.
  - Persons riding in a vehicle now have distinct seat position.
  - Persons waiting at a busStop can now wait in multiple rows
    according the specified stop length and personCapacity.
  - Added person drawing style *circles*
  - Added openGL gui settings option *FPS* to enable a
    frames-per-second display.
  - Can now locate objects by their name (streetname or generic
    parameter key="name").
  - Railway simulation improvements:
    - Improved visibility of railSignal-state when zoomed out and
      junction-exaggeration is active.
    - railSignal state indicator is now drawn with an offset to
      indicate the applicable track direction.
    - Junction shapes are now longer drawn for railway switches at
      default GUI settings.
    - Drawing bidirectional railways in *spread* style is now
      supported. Edge IDs are also drawn at an offset to improve
      readability.
    - The visual length of railway carriages and locomotive [can
      now be
      configured](Simulation/Railways#Visualisation.md).

- NETCONVERT
  - Improved junction joining heuristic to prevent superfluous
    joins.
  - [OpenDrive road
    objects](Networks/Import/OpenDRIVE#Road_Objects.md) can
    now be imported
  - Road objects can now be embedded when [generating OpenDRIVE
    output](Networks/Further_Outputs#Embedding_Road_Objects.md).
  - Attribute *endOffset* can now be used to move the signal
    position for bidirectional rail edges.
  - Minimum phase duration for actuated traffic lights now takes
    road speed into account to improve traffic light efficiency.
  - all -attributes are now also supported within a  element to
    affect the joined node.
  - Various improvements to the generation of traffic light plans. ,
    , ,
  - If a custom ID was assigned to a traffic light (different from
    the junction ID), it will no longer be joined with other traffic
    lights when option  is set.
  - Lane widths are now imported from VISSIM networks.
  - Changed option default for  to *0* to improve generated
    geometries in most cases.
  - Roundabouts can now be disabled and removed by setting the node
    type to *right_before_left*

- NETEDIT
  - Major improvement in rendering speed
  - Junction context menu function *split junctions* now restores
    original node ids
  - The new function *split junctions and reconnect* now recreates
    edges heuristically.
  - can now edit tls phase attributes *next* and *name*.
  - can now load additionals and shapes using -a "file.xml" or
    --additionals "file.xml".
  - Connection attributes *dir* and *state* can now be inspected and
    used as selection filter.
  - Can now locate objects by their name (streetname or generic
    parameter key="name").
  - Roundabouts can now be removed by setting the node type to
    *right_before_left*

- DUAROUTER
  - added new input element
  - Added option  to output trips instead of routes. This is useful
    for validating trip input that shall be routed during
    simulation.
  - Added option  to write trips with attributes *fromLonLat,
    toLonLat* instead of *from* and *to*
  - Reading trips with attributes *fromLonLat, toLonLat, fromXY,
    toXY, viaLonLat, viaXY* is now supported.
  - Time losses due to passing intersections from a non-priority
    road are now anticipated. The anticipated loss can be configured
    using the new option  (new default 1.5, previous behavior when
    set to 0). .
  - Added option  to randomize routing results with bounded
    deviation from optimality.



- TraCI
  - Added multi-client support to [TraaS](TraCI/TraaS.md)
    (*setOrder*)
  - *traci.vehicle.getNextTLS* now returns upcoming traffic lights
    for the whole route of the vehicle (before, only traffic lights
    until the first required lane change were returned).
  - Added functions *trafficlight.getPhaseName* and
    *trafficlight.getPhaseName* to all clients.
  - Extended the function *traci.vehicle.openGap()* to include an
    optional parameter specifying a reference vehicle.
  - Added function *traci.vehicle.getNeighbors()* and convenience
    wrappers (getLeftFollowers(), etc) to retrieve neighboring,
    lanechange relevant vehicles.
  - Added function *person.getSlope* to all clients.
  - Function *vehicle.changeLaneRelative(vehID, 0)* can now be used
    to a pin a vehicle to the current lane.

- Tools
  - [Generic parameters](Simulation/GenericParameters.md) of
    edges, lanes, junctions and traffic lights are now supported in
    [sumolib](Tools/Sumolib.md).
  - [randomTrips edge
    probabilities](Tools/Trip#Edge_Probabilities.md) can now
    be scaled by [Generic edge
    parameters](Simulation/GenericParameters.md).
  - Added new tool
    [tripinfoByTAZ.py](Tools/Output#tripinfoByTAZ.py.md) for
    aggregating tripinfo attributes by origin/destination pair.
  - [netdiff.py](Tools/Net#netdiff.py.md) now supports the
    new option  to generate patch files which can be applied during
    initial network import.

- All Applications
  - The option  now supports filtering by topic (e.g. .

### Other

- Simulation
  - The default [algorithm for averaging road speed to estimate
    rerouting travel
    times](Demand/Automatic_Routing#Edge_weights.md) was
    changed from *exponential average (0.5)* to *moving average
    (180s)*. This is better suited for averaging traffic light
    cycles in urban road networks. To restore the old behavior, the
    option  can be used.

- NETCONVERT
  - Turn-arounds are no longer built at geometry-like nodes by
    default (when there is a node but no intersection so there would
    only be straight connections and turn-arounds). The old behavior
    can be restored by setting option .
  - The default priorities for different railway edge types imported
    from OSM haven been changed so that all types use a different
    priority value.
  - The assigned edge speed for unrestricted motorways was lowered
    to 142km/h (from 300km/h) to better reflect real world speed
    distributions (together with the default speed factor
    distribution). Also, the default motorway speed was lowered from
    160km/h to 142km/h for the same reason.

- TraCI
  - TraCI Version is now 20
  - Function *trafficlight.setCompleteRedYellowGreeenDefinition* now
    expectes the 5th component *next* to be a list of integers when
    defining phases. Function
    '*'trafficlight.getCompleteRedYellowGreeenDefinition* now
    returns a list of integers as the 5th component (next) when
    defining phases. Custom clients must be patched to handle this.
  - Function *trafficlight.setCompleteRedYellowGreeenDefinition* now
    accepts *name* as an optional 6th component when defining
    phases. Function
    '*'trafficlight.getCompleteRedYellowGreeenDefinition* now
    returns *name* as the 6th component when defining phases. Custom
    clients must be patched to handle this.
  - TraaS function *Simulation.findRoute* now returns SumoStage
    object (containing all data returned by the TraCI API) instead
    of SumoStringList with just the edge IDs
  - TraaS function *Simulation.findRoute* now returns
    LinkedList<SumoStage\> (containing all data returned by the TraCI
    API) instead of LinkedList<SumoStringList>

- Documentation
  - Added Tutorial for [Manhattan
    Mobility](Tutorials/Manhattan.md).
  - The [TraaS](TraCI/TraaS.md) library is now documented
    within the wiki.

## Version 1.1.0 (18.12.2018)

### Bugfixes

- Simulation
  - Fixed Bug where vehicles would stop moving after changing to the
    opposite direction lane.
  - Fixed
    [parkingAreaReroute](Simulation/Rerouter#Rerouting_to_an_alternative_Parking_Area.md)
    to a destination which is too close for stopping.
  - Fixed crash when using multiple vehicles with
    *device.bluelight*.
  - Fixed crash when using carFollowModel *ACC* on multi-lane roads.
    (regression in 1.0.1)
  - Fixed crash when loading signal plan with invalid characters.
  - Fixed invalid halting count in E3 detector output
  - Fixed crashes related to vehicles that lane-change multiple
    times on the E3-detector entry edge. ,
  - Fixed bugs that were causing unnecessarily strong deceleration.
    , , , , , , , ,
  - Fixed deadlock caused by long vehicles being unable to re-enter
    traffic after parking in a short parkingArea.
  - Fixed invalid error when defining a stop on an edge and its
    consecutive internal edge.
  - Fixed bug where vehicles from minor roads would drive onto the
    intersection to aggressively.
  - Fixed bug where pedestrians would ignore prioritized vehicles
    when walking onto a crossing.
  - Fixed invalid upper bound on density in
    [edgeData-output](Simulation/Output/Lane-_or_Edge-based_Traffic_Measures#Generated_Output.md)
    for multi-lane edges.
  - Fixed bug where trains would fail to reverse on bidirectional
    rail edges.

- SUMO-GUI
  - Closing and re-opening an edge or lane no longer causes invalid
    lane choice.
  - Errors due to invalid  values are now shown in the message
    window.
  - Fixed invalid vehicle shapes when drawing trains as *simple
    shapes* in mesoscopic mode.

- NETCONVERT
  - Fixed bug that was causing invalid road geometry when using
    high-resolution input data at dense junction clusters.
  - Fixed bug when building networks for opposite-direction
    overtaking that was causing collisions in the simulation.
  - Fixed invalid right-of-way rules in left-hand networks.
  - Duplicate edge ids in shapefile import are no longer silently
    ignored. Instead, a warning is issued and ids are made unique
    with a numerical suffix.
  - Fixed crash when loading height data from multiple geotif files.
  - Option  no longer builds traffic lights at almost every
    intersection. The default lane-speed-sum threshold (Option ) was
    changed from 150km/h to 250km/h.
  - Options  are no longer lost when re-importing a .net.xml file.
  - Fixed unnecessary right-of-way restrictions in some cases where
    incoming edges target distinct lanes on the target edge.
  - Fixed bug where custom walkingarea shapes where sometimes
    ignored.
  - Fixed bugs that could cause loss of elevation information. ,
  - Fixed permissions on internal lanes after internal junctions.

- NETEDIT
  - Loading joined traffic light programs via menu is now working.
  - Menu option *Load Foreign* can now be used to import OSM files
    directly.
  - Fixed bug where *right-of-way* mode would show invalid
    properties.
  - TLS locator now lists traffic lights even if their id differs
    from the junction id.
  - Fixed crash when clicking buttons that should be disabled.
  - Fixed creation of unyielding connections via shift-click.
    (regression in 1.0.1)
  - Fixed modifying elevation in Move-mode (regression in 1.0.0).

- TraCI
  - Fixed bug that was causing exaggerated output for
    *traci.vehicle.getDrivingDistance* when vehicles were driving on
    a looped route.
  - *traci.vehicle.getLaneChangeState* now returns correct
    information while controlling the vehicle with *moveToXY*.
  - Fixed retrieval of shape data with more than 255 points
    (*polygon.getShape, lane.getShape, junction.getShape*
  - Fixed bug when setting polygon shape with the C++ client.
  - Vehicle.isStopped is now working in TraaS.
  - Fixed adding vehicles after a simulation time of 24.8 days
    (python client).
  - Fixed bug where lateral collisions from unsafe calls to
    vehicle.changeSublane are prevented despite disabling safeguards
    via setLaneChangeMode.

### Enhancements

- Simulation
  - The eagerness for opposite-direction overtaking can now be
    configured with the new vType attribute *lcOpposite* (range
    \[0,inf\[).
  - elements now include attribute *type*.
  - The [ToC Device](ToC_Device.md) now provides an option
    for generating output.
  - Added warning messages for vehicles that perform emergency
    braking. The warning threshold can be configured with the new
    option . The given threshold is compared against the severity of
    the event (braking with emergencyDecel corresponds to 1 and
    braking with decel corresponds to 0). By default the threshold
    is set to 1.
  - Trains can now [reverse
    direction](Simulation/Railways#Reversing_Direction.md)
    anywhere on a bidirectional track.
  - TAZ can now be used as *via* destinations by using
    *<TAZID\>-sink* as edge id.
  - ParkingAreas now support the attribute *onRoad*. If set to
    *true*, vehicles will park on the road lane, thereby reducing
    road capacity.

- SUMO-GUI
  - Added  attribute *lineWidth* to determine the drawing width of
    unfilled polygons.
  - TAZ now support attribute *color* for rendering the associated
    shape. This color can also be used in the new edge coloring mode
    *color by TAZ*.
  - Can now color streets according to custom lane or edge
    [parameters](Simulation/GenericParameters.md).
  - The numerical value for coloring edges/lanes, vehicles and
    persons (e.g. speed limit, acceleration, waitingTime) can now
    optionally be shown. When this option is set, the value will
    also be printed in the object tool tip. Likewise, the numerical
    value for coloring vehicles (e.g. acceleration) can optionally
    be shown.
  - Breakpoints can now be set via option .
  - Added visualization option to apply size exaggeration only to
    selected objects.
  - The text background color for IDs and object descriptions can
    now be configured.

- NETEDIT
  - New editing mode for TAZ (Traffic Analysis Zones).
  - Overlapping objects can now be inspected via consecutive clicks
    on the same location.
  - Shift-click can now be used to inspect, select and delete lanes.
  - LaneAreaDetectors (E2) that span multiple consecutive lanes can
    now be created within NETEDIT.
  - POIs can now be created from geo-coordinates in the clipboard
    (e.g. from online map sites) with a single click.
  - Selection colors can now be customized.
  - Can now color streets according to custom lane or edge
    [parameters](Simulation/GenericParameters.md).

- NETGENERATE
  - Added options  to add random perturbations to all node
    coordinates. Perturbations can be specified as *mean* or capped
    normal distribution *normc(a,b,c,d)*.

- NETCONVERT
  - Added option  for selecting between layouts that group opposite
    directions ('opposites') and layouts that have one phase per
    incoming edge ('incoming').
  - Added option  to control the heuristic for guessing traffic
    lights. A traffic light is guessed if the sum of speeds of all
    incoming lanes exceeds the threshold value.
  - Added new node attribute  to configure the algorithm for
    computing right-of-way rules (allowed values are *default*,
    *edgePriority*). Also added new option  to set this value for
    all nodes in a network.
  - Importing internal lane shapes from OpenDRIVE is now supported
    using option .

- Tools
  - [netcheck.py](Tools/Net#netcheck.py.md) checks
    lane-based reachability and handles pedestrian infrastructure.
  - [route2poly.py](Tools/Visualization#Intersection_Flow_Diagram.md)
    can now be used to generate intersection flow diagrams.
  - [tripinfoDiff.py](Tools/Output#tripinfoDiff.py.md) now
    supports comparing s by setting option .

- TraCI
  - [Context Subscriptions can now be equipped with
    filters](TraCI/Interfacing_TraCI_from_Python#Context_Subscription_Filters.md)
    acting on the server side.
  - TraCI now offers the method
    [openGap()](TraCI/Change_Vehicle_State#open_gap_.280x16.29.md)
    for temporary changes of the vehicle's desired headway.
  - Added functions *person.getPosition3D, person.getAngle,
    person.getLanePosition, persion.getColor, person.getLength* to
    the C++ client.

### Other

- Simulation
  - Simulation now runs about 17% faster compared to the previous
    release due to improvements in data structures and less
    (redundant) collision checking .
  - The default speedDeviation of vehicle classes
    *private,vip,hov,custom1* and *custom2* was changed from 0 to
    0.1. The default speed deviation for vehicle class *taxi* was
    changed from 0 to 0.05.
- Documentation
  - Updated [documentation for railway
    simulation](Simulation/Railways.md)
  - Updated [MAROUTER](MAROUTER#Usage_Description.md)
    documentation
- Miscellaneous
  - Network version is now 1.1
  - TraCI Version is now 19

- [sumolib](Tools/Sumolib.md) now uses an object with members
  *state, duration, minDur, maxDur, next* instead of tuple *(state,
  duration)* to represent traffic light phases.

## Version 1.0.1 (18.09.2018)

### Bugfixes

- Simulation
  - Fixed deadlock on parallel routing (regression in 1.0.0).
  - default emergency deceleration did not depend on the vClass but
    was always 9
  - Fixed bugs related to opposite-direction overtaking that were
    causing collisions and other invalid behavior. , , , , , , ,
  - Fixed crash when using sublane model with varying lane widths.
  - Fixed hard braking after lane-changing when using carFollowModel
    *IDM*
  - Fixed exaggerated gap between standing vehicles when using
    carFollowModel *IDM*.
  - Fixed crash when using *device.ssm*.
  - Fixed invalid cooperative lane-changing attempts.
  - Fixed invald speed adaptation for lane changing.
  - Public transport routing now properly uses stops that are
    defined as child elements of a vehicle.

- SUMO-GUI
  - Fixed freeze when activating 'Show lane direction' for lanes
    with width=0.

- NETEDIT
  - Fixed freeze when activating 'Show lane direction' for lanes
    with width=0.
  - Custom endpoints are now honored when reversing an edge or
    adding a reverse direction edge via context menu.

- NETCONVERT
  - Networks for opposite direction driving now allow overtaking
    past junctions on straight connections that are slightly curved.

- OD2Trips
  - Flows with non-positive probability are no longer written.

- MESO
  - Fixed deadlock in public transport simulation.

- TraCI
  - Fixed bug when calling TraaS method
    *Simulation.getDistanceRoad*.
  - Closing and re-opening a lane no longer causes invalid lane
    choice.

### Enhancements

- Simulation
  - The collision-detection threshold configured via option  can now
    be customized separately for each vType using attribute
    *collisionMinGapFactor*.
  - Increased running speed of simulations with *device.rerouting*
    using few vehicles in a large network.

- NETEDIT
  - Added option *reset custom shape* to the right-click menu of
    junction- and lane-selections ,
  - When editing junction shapes, vertices can now be removed by
    shift-click.

- TraCI
  - added method 'traci.edge.getStreetName''
  - method *simulation.getParameter* can now be used to [retrieve
    stopping place
    names](TraCI/Simulation_Value_Retrieval#Generic_Parameter_Retrieval_0x7e.md).

- Tools
  - [cutRoutes.py](Tools/Routes#cutRoutes.py.md) now
    supports route references and stop access. ,

### Other

- Simulation
  - Collision for vehicles controlled by carFollowModel *IDM* are
    only registered when less then half of the minGap distance
    remains between vehicles. This is done to compensate for the
    fact that the model does not guarantee the minGap distance (most
    of the time at least \~90% of the minGap are kept). The
    threshold can be customized via global option and vType
    attributes.

## Version 1.0.0 (04.09.2018)

### Bugfixes

- Simulation
  - All car-following models now respect the vType-attribute
    *emergencyDecel* as an absolute bound on deceleration.
  - Fixed some issues when using [continous lane
    change](Simulation/SublaneModel#Simple_Continous_lane-change_model.md).
    , , , ,
  - Fixed back-and-forth changing when using [continous lane
    change](Simulation/SublaneModel#Simple_Continous_lane-change_model.md).
  - Fixed loading of teleporting vehicles from simulation state in
    *.sbx* format.
  - Fixed invalid vehicle counts by E2-detector related to
    lane-changing.
  - Fixed invalid vehicle counts by E3-detector related to re-using
    vehicle pointers ,
  - Fixed bug that was causing invalid slowdown while passing an
    intersection.
  - Fixed bug that was causing pedestrians on looped routes to block
    themselves.
  - Vehicle speedFactor is now included in saved state.
  - Fixed invalid collision events when lanes are to narrow for the
    vehicles.
  - Fixed collision between pedestrians and vehicles.
  - Fixed bug where option  would trigger invalid warnings regarding
    unsorted route file.
  - Fixed invalid stopping position after collision when using
    option
  - Fixed right-of-way rules for vehicles standing next to each
    other on the same lane and driving towards different edges.
  - Fixed crash within intersection between vehicles coming from the
    same lane.
  - Fixed invalid *departDelay* for triggered vehicles.
  - Fixed hang-up when scheduling a stop on internal edges after
    internal junctions
  - ParkingAreaReroute no longer triggers if the destination is not
    among the set of alternatives.
  - Fixed (near) infinite loop when specifying invalid speed
    distribution.
  - Fixed invalid car-following behavior at changing lane widths
    when using the sublane model.
  - Fixed crash when simulating invalid pedestrian routes with
    option .
  - Option  is now working.
  - Car-follow models *IDM* and *IDMM* no longer fail to reach an
    exact stop position.
  - Strategic look-ahead is now working as expected in networks with
    many short edges.
  - Switching a traffic light to the "off" definition now sets the
    correct right-of-way rules.
  - Fixed invalid deceleration at yellow traffic light.

- SUMO-GUI
  - width of railway edges is now taken into account when drawing
    (interpreted as gauge).
  - window-size and position now remain unchanged when reloading the
    simulation.
  - Random vehicle and person coloring is now more random on
    windows.
  - Vehicles that were not inserted (i.e. due to option  or ) no
    longer count as *arrived* in the simulation parameter dialog.
    Instead the are counted under the new item *discarded vehicles*.
  - Fixed crashing due to concurrent access to vehicle numbers.
  - Fixed issues related to drawing smooth corners at curving roads.
  - Fixed vehicle positions when using the sublane model in lefthand
    networks.
  - Fixed crashing and visualization problems when using the
    3D-view. ,
  - Fixed glitch where persons would appear to jump around while
    riding in a vehicle across an intersection.
  - Tracking of riding persons now centers on the person rather than
    the front its vehicle.
  - ChargingStation are visible again (regression in 0.32.0)
  - Fixed invalid vehicle blinkers in lefthand simulation.
  - Traffic lights now remain accessible (right-click, tooltip)
    after calling *traci.trafficlight.setRedYellowGreenStaate*.
  - Fixed invalid vehicle angle on lane with zero-length geometry.

- MESO
  - Fixed deadlock at roundabouts when running with option  or .

- NETEDIT
  - Splitting edges, deleting individual edges and lanes or adding
    lanes via the *duplicate* menu option no longer resets
    connections and traffic light plans.
  - The viewing area and zoom loaded via option  is no longer
    ignored
  - Fixed bug where connections on large junctions did not register
    clicks or were not drawn.
  - Fixed crash when removing inspected object via *undo*.
  - Fixed various bugs that led to re-computation of signal plans
    when modifying connections or tls indices. ,
  - Fixed bug that could lead to the creation of invalid networks
    when adding and removing connections. ,
  - Custom connection shape is now longer lost after
    *delete*+*undo*.
  - Moving geometry no longer removes z-data.
  - Splitting edges no longer introduces unnecessary custom
    endpoints.
  - Fixed inverted interpretation of *lanePosLat* for POIs compared
    to SUMO-GUI.
  - Fixed crash when deleting one of multiple signal programs for
    the same junction.
  - Added support for BusStops Acces.
  - Crossings no longer become invalid when splitting an edge.

- NETCONVERT
  - Option  no longer builds ramps at traffic light controlled
    nodes.
  - Fixed bug that was causing invalid link states at intermodal
    junctions.
  - Fixed bugs that were causing invalid link directions.
  - Fixed invalid junction logic in lefthand networks.
  - Generated signal plans will no longer have a protected left-turn
    phase if there is no dedicated left-turn lane.
  - Various fixes in regard to classifying nodes as type
    *rail_crossing* and joining clusters of rail crossing nodes.
  - Option  is now working when loading a *.net.xml* file.
  - Fixed invalid junction shapes when using option .
  - Fixed invalid intermodal junction logic that could cause
    collisions and deadlocks. ,

- DUAROUTER
  - Fixed crash on intermodal routing.

- POLYCONVERT
  - Fixed bug when importing OSM data with objects that are marked
    as *deleted*.

- TraCI
  - Fixed bug where persons would "jump" when replacing the current
    walking stage with a new one.
  - Fixed crash when trying to access empty subscription result list
    using the C++ client.
  - Vehicle *emergencyDecel* is now at least as high as *decel*
    after calling *traci.vehicle.setDecel*.
  - Fixed python client bug in *traci.polygon.setShape*.
  - Adding a route with an empty list of edges now results in an
    error.
  - Vehicles that drive outside the road network under the control
    of *moveToXY* now properly updated their speed and brake lights.
  - Function *vehicle.getLaneChangeMode* now returns correct values
    for the original lane change state when the state is affected by
    *vehicle.setLaneChangeMode*.
  - Car-following related vehicle type parameters (e.g. *accel*)
    that are changed via traci are now correctly saved when saving
    simulation state.
  - Functions *simulation.findRoute* and
    *simulation.findIntermodalRoute* no longer crash sumo when
    trying to route from a forbidden edge.
  - Fixed invalid results when calling
    *simulation.findIntermodalRoute* ,,
  - Multi-Lane E2-Detectors now return the correct length.
  - Fixed bug where *vehicle.couldChangeLane* returned *True*
    immediately after a lane change even though the requested target
    lane did not exist.
  - Fixed collisions when combining *vehicle.slowDown* with
    ballistic update.
  - Fixed python client bug in *traci.lane.getLinks* which returned
    always true for isOpen, hasFoe and hasPrio.
  - traci.person.setColor was fixed (was a noop before)

- Tools
  - [randomTrips.py](Tools/Trip#randomTrips.py.md) now uses
    vType attributes from option  when generating persons.
  - Removed singularities at the distribution boundaries generated
    by
    [createVehTypeDistributions.py](Tools/Misc#createVehTypeDistributions.py.md)


### Enhancements

- All applications
  - All time values in options and xml inputs can now be specified
    in the format *h:m:s* and *d:h:m:s* (where the values for days,
    hours and minutes are all positive integers and seconds may also
    be a positive floating point number).

    !!! note
        This does not apply to the python tools.
    
  - Added option **--human-readable-time** (short **-H**) that causes all time values to be written
    in h:m:s (or d:h:m:s) format.

- Simulation
  - Tripinfo-output now includes the attribute *stopTime* which
    records the time spent with intentional stopping.
  - A pedestrian crossings can now have different signal states for
    both directions.
  - FCD-output can now be switched on selectively for a subset of
    vehicles and the reporting period can be configured.
  - FCD-output can now be restricted to a subset of network edges
    loaded from a file with option . The file format is that of an
    edge selection as saved by [NETEDIT](NETEDIT.md).
  - Intended departure times (attribute *depart*) and intended
    vehicle id (attribute *intended*) are now added to
    vehroute-output of public transport rides.
  - Stopping place names are now added as XML-comments in
    vehroute-output of public transport rides.
  - Lane-Change-Model parameter *lcTurnAlignmentDistance* added for
    the control of dynamic alignment in simulations using the
    sublane model, see [Lane-Changing
    Models](Definition_of_Vehicles,_Vehicle_Types,_and_Routes#Lane-Changing_Models.md),
    .
  - Implemented [SSM
    Device](Simulation/Output/SSM_Device.md), which allows
    output of saftey related quantities. ,
  - 'Smoothed' emergency braking
  - Added an initial version of a [driver state
    device](Driver_State.md) for modelling imperfect
    driving.
  - Added an initial version of a [transition of control
    model](ToC_Device.md).
  - Option  now also allows insertion of vehicles with unsafe
    headways.
  - Added a new car-following model "ACC" based on \[Milanés et al.
    "Cooperative Adaptive Cruise Control in Real Traffic
    Situations." IEEE Trans. Intelligent Transportation Systems 15.1
    (2014): 296-305.\]
  - [Statistic
    output](Simulation/Output.md#aggregated_traffic_measures)
    now also includes total delay by vehicles which could not be
    inserted by the end of the simulation if options  and  are set.
  - The default lane-changing model *LC2013* now supports [parameter
    *lcAssertive*](Definition_of_Vehicles,_Vehicle_Types,_and_Routes#Lane-Changing_Models.md).
  - [Vehroute-output](Simulation/Output/VehRoutes.md) now
    includes the reason for rerouting.
  - Added optional attribute *visible* to  (default *false*). This
    controls whether occupancy is known before reaching the
    parkingArea edge and can be used to model incomplete knowledge
    in parking reroute choice.
  - Rerouters now support the attribute *vTypes* which restricts
    their effect to vehicles from the given list of vehicle type
    IDs.
  - Vehicles are forming a coridor for emergency vehicles
  - Added option  to control the default speed deviation of all
    vehicle types that do not define it.
  - Added lane/edge-attribute stopOffset for defining vClass
    specific stopping positions in front of traffic lights.

- SUMO-GUI
  - Added control for scaling traffic demand dynamically.
  - Added option to disable drawing of bicycle lane markings on
    intersections.
  - All laneChangeModel-related vType-parameters and all
    junction-model related vType-parameters are now shown in the
    vType-Parameter dialog.
  - The simulation view can now be rotated via the new gui-settings
    attribute *angle* in the  or via the viewport dialog.
  - When drawing junction shapes with exaggerated size and setting
    option *show lane-to-lane connections*, the connecting lines are
    now scaled up at traffic light junctions.
  - The simulation state can now be saved via the *Simulation* Menu.
  - Object tracking can now be aborted via double-click.
  - Person plans can now be inspected via the right-click menu.
  - Object name rendering size can now be toggled between constant
    pixel size (all visible when zoomed out) and constant network
    size (invisible when zoomed out).
  - The *Delay* value is now invariant with regard to the simulation
    step length and always denotes delay per simulated second.
  - Added GUI Shape for different types of emergency vehicles
    (ambulance, police and firebrigade)

- MESO
  - Simulation of persons and public transport is now supported.

- NETEDIT
  - Added *Prohibition*-mode for checking right-of-way at junctions
    (hotkey 'w'). Thanks to Mirko Barthauer for the contribution.
  - Added virtual attribute to identify [bidirectional rail edge
    pairs](Simulation/Railways#Bidirectional_track_usage.md).

  - Added option to modify the visualisation of [bidirectional rail
    edge
    pairs](Simulation/Railways#Bidirectional_track_usage.md)
    (*spread superposed*)
  - Added button *add states* to *traffic light*-mode to complement
    the functionality of *cleanup states*.
  - Netedit now flags connection targets with incompatible
    permissions as *conflict* and only creates them with
    *<ctrl\>+<click\>*.
  - Traffic light indices are now drawn for pedestrian crossings
    when enabled via gui settings.
  - Now ParkingAreas and ParkingSpaces can be created in netedit.
  - When adding a new phase to a traffic light, the new phase will
    now have a plausible successor state depending on the selected
    previous phase (rather the being a copy).
  - Added function 'split' to junction context menu. This can be
    used to disaggregated joined junctions.
  - When joining traffic lights (by editing junction attribute
    'tl'), link indices of the target traffic light are now
    preserved if signal groups are used (multiple connections with
    the same *linkIndex* value).  .

    !!! caution
        The the signal states for the edited junction must be checked by the user

  - Width, height and diagonal size of a rectangle selection are now
    shown in the status bar. This can be used to measure distances.
  - Now Generic Parameters can be loaded, saved and edited.
  - Re-organized options in the options-dialog (F10).
  - Option help is now shown in the status bar of the options
    dialog.
  - Custom edge geometry endpoints can now be manipulated in *Move
    mode*.

- NETCONVERT
  - Geo-referenced networks (i.e. from OSM) can now be merged by
    loading them together ().
  - Element  now supports attribute *linkIndex2* to specify a custom
    signal index for the reverse direction.
  - When defining [double
    connections](Networks/Building_Networks_from_own_XML-descriptions#Multiple_connections_from_the_same_edge_to_the_same_target_lane.md),
    the right-of-way rules now take the road topology into account
    to differentiate between on-ramp and off-ramp situations.
  - Importing VISUM networks up to format version 10 is now support.
  - Improved heuristics for options .
  - Improved computation of intermodal junctions imported from OSM.
  - Improved computation of junction shapes.
  - Added option  for rotating the network.
  - Added option  which can be used to add a prefix to the written
    junction and edge IDs.
  - Added options  to control the timing of pedestrian crossing
    phases.
  - Added option  to ensure that left turns through oncoming traffic
    are not build for high-speed roads.
  - Sidewalk information is now imported from OSM for road types
    that have a positive sidewalkWidth attribute (e.g. by using
    typemap
    [osmNetconvertPedestrians.typ.xml](Networks/Import/OpenStreetMap#Recommended_Typemaps.md)).
  - Added option  for increasing the length of stop access edges
    above the airline distance. .
  - Added option  which sets an upper bound on speed while passing
    an intersection based on the turning radius. To account for
    imperfect road geometry, the option  can be used to avoid speed
    limits for small turns. Warnings are issued when the difference
    between road speed and connection speed rises above a
    configurable threshold (). These warnings often indicate
    exaggerated road speeds on urban roads as well as faulty
    geometry.
  - Geo-reference information is now imported from OpenDRIVE input.

- NETGENERATE
  - Simplified node and edge names
    - The alphanumerical junction naming scheme now supports
      arbitrary grid sizes (using ids like *XY23*)
    - The alphanumerical junction naming scheme also extends to
      spider networks
    - The alphanumerical junction naming scheme is active by
      default (the option for enabling the old scheme was renamed
      from  to ).
    - When using alphanumerical junction ids, the intermediate
      string *to* is omitted from edge names because the edge ID
      already allows unambiguous determination of its junctions
      without it
  - Added option  to randomize lane numbers in random networks
    between 1 and *default.lanenumber*
  - Added option  to edge priorities in random networks between 1
    and *default.priority*
  - Added option  to place generated junctions on a regular grid
  - Added option  which can be used to add a prefix to the generated
    junction and edge IDs.
  - Corridor networks can now be generated by using options such as
  - Added options  and  to add left-turn lanes to generated
    networks.

- DUAROUTER
  - Intended departure times (attribute *depart*) and intended
    vehicle id (attribute *intended*) are now added to
    vehroute-output of public transport rides.
  - Stopping place names are now added as XML-comments in
    route-output of public transport rides.

- POLYCONVERT
  - Option  now also applies to POIs.

- TraCI
  - [Libsumo](Libsumo.md) can now be used in place of the
    TraCI client libraries to increase execution speed of
    TraCI-Simulations by directly linking against
    [SUMO](SUMO.md) for all
    [SWIG](https://en.wikipedia.org/wiki/SWIG)-supported languages.
    Graphical simulation with libsumo is not yet supported.
  - function *vehicle.add* now supports using the empty string ("")
    as a route id to insert the vehicle on an arbitrary valid edge.
    This makes it easier to remote-control vehicles using moveToXY
    without defining an initial route.
  - added functions *simulation.getCollidingVehiclesNumber* and
    *simulation.getCollidingVehiclesIDList* to track collisions.
  - added functions *simulation.getEmergencyStoppingVehiclesNumber*
    and *simulation.getEmergencyStoppingVehiclesIDList* to track
    emergency stops.
  - added function *edge.getLaneNumber* to retrieve the number of
    lanes of an edge.
  - added function *vehicle.getAcceleration* to retrieve the
    acceleration in the previous step.
  - TraCI now allows to update a vehicle's best lanes
    (vehicle.updateBestLanes).
  - added function *gui.hasView* to determine whether graphical
    capabilities exist.
  - added function *lane.getFoes* to [to determine right-of-way and
    conflict relationships between incoming
    lanes](TraCI/Lane_Value_Retrieval#Extended_retrieval_messages.md).
  - function *simulation.getMinExpectedNumber()* now includes
    persons that are still active in the simulation.
  - added function *traci.getLabel* to the python client to help
    working with multiple connections.
  - When adding a vehicle with a disconnected 2-edge route, it will
    be treated like a  and rerouted automatically.
  - added functions *vehicle.getRoutingMode* and
    *vehicle.setRoutingMode*. When setting routing mode to
    *traci.constants.ROUTING_MODE_AGGREGATED*, [smoothed travel
    times](Demand/Automatic_Routing#Edge_weights.md) are
    used instead of current travel times.

- Tools
  - added script
    [plot_trajectories.py](Tools/Visualization#plot_trajectories.py.md)
    which allows plotting various combinations of time, distance,
    speed and acceleration for all trajectories in a given  file.
  - added script
    [averageRuns.py](Tools/Misc.md#averagerunspy) for
    running a scenario multiple times with different random seeds
    and computing trip statistics
  - webWizard now correctly builds regions with left-hand traffic
  - Additional options for
    [generateTLSE3Detectors.py](Tools/Output.md#generatetlse3detectorspy)
    that make it suitable for generating various kind of junction
    related output.
  - [createVehTypeDistributions.py](Tools/Misc.md#createvehtypedistributionspy)
    now also allows to generate vehicle parameter distributions.
  - Added function simpla.createGap()
  - sumolib now supports lane.getWidth().

### Other

- Miscellaneous
  - The space character ' ' is no longer allowed in xml option
    values when separating file names. Instead the comma ',' must be
    used. Files with space in their path are now supported.

- Documentation
  - Added [documentation on joined traffic lights and defining
    signal groups](Simulation/Traffic_Lights.md).
  - Added [documentation on the simple continous lane-change
    model](Simulation/SublaneModel#Simple_Continous_lane-change_model.md).
  - Documented all supported [routing
    algorithms](Simulation/Routing#Routing_Algorithms.md).

- Simulation
  - is now deprecated in favour of FCD-output filtering
  - is now deprecated in favour of calibrators.
  - The default arrival position for person walks is now the middle
    of the destination edge. This reduces the assymetry from
    arriving in forward or backward direction.
  - tripinfo-output attribute *waitSteps* which counts the number of
    simulation steps in which the vehicle was below a threshold
    speed of 0.1m/s was replaced by attribute *waitingTime* which
    measures the same time in seconds. This gives results which are
    independent of the  simulation parameter.
  - The default value for option  was changed from *1* to *60* to
    speed up simulation.
  - The default speed deviation has been changed to *0.1*. When
    defining a new  with attribute *vClass*, this also influences
    the default speed deviation:
    - *truck*, *trailer*, *coach*, *delivery*: 0.05
    - *tram*, *rail*, *rail_electric*, *rail_urban*,
      *emergency*: 0
    - everything else 0.1

!!! note
    The previous behavior can be restored by setting option **--default.speeddev 0**

- SUMO-GUI
  - Default color for persons is now 'blue' to better distinguish
    them from vehicles.

- NETCONVERT
  - There is no longer an offset of 0.1m between lanes. This means
    the total visual width of an edge is now the sum of all lane
    widths. This also fixes an inconsistency between visualization
    and simulation as the vehicles always ignored this offset
    anyway. Road markings are now drawn on top of the lanes rather
    than between them. This causes small visual gaps in old networks
    (fixable by calling *netconvert -s old.net.xml -o new.net.xml*).
  - The default value for option  was increased from 1.5 to 4 in
    order to improve realism of turning angles.
  - The default value for option  was increased from 0 to 5 to
    improve the visual realism of larger intersections.
  - Option  is now enabled by default
  - Vehicle speed while turning at intersections is now limited via
    the new option default . To obtain the old behavior, this value
    can be set to -1.

- TraCI
  - TraCI Version is now 18
  - Embeded python is now deprecated in favour of
    [Libsumo](Libsumo.md).
  - TraCI now consistently represents time in seconds as a double
    precision floating point. This affects all client
    implementations. For convenience the affected python calls are
    listed below. Modifying your scripts should be necessary only
    for the methods printed in **bold**:
    - **traci.simulationStep**
    - traci.edge.getTravelTime
    - traci.edge.getEffort
    - traci.edge.adaptTravelTime
    - traci.edge.setEffort
    - traci.person.add
    - traci.vehicle.getAdaptedTravelTime
    - traci.vehicle.getEffort
    - traci.vehicle.adaptTravelTime
    - traci.vehicle.setEffort
    - **traci.vehicle.slowDown**
    - **traci.vehicle.changeLane**
    - **traci.vehicle.setStop**
    - traci.simulation.getCurrentTime still returns ms as int but
      has been deprecated in favor of traci.simulation.getTime
    - traci.simulation.findRoute
    - traci.simulation.findIntermodalRoute
    - **traci.simulation.getDeltaT**
    - **traci.trafficlight.getNextSwitch**
    - **traci.trafficlight.getPhaseDuration**
    - traci.trafficlight.getCompleteRedYellowGreenDefinition
    - **traci.trafficlight.setPhaseDuration**
    - traci.trafficlight.setCompleteRedYellowGreenDefinition
  - The TraCI boundary type has been replaced by a position list of
    2 positions
  - The TraCI float type has been removed
  - All protocol functions returning single bytes now return
    integers (lane.getLinkNumber, polygon.getFilled,
    vehicle.isRouteValid, vehicle.getStopState)
  - In the C++ client the function vehicle.getEdges was renamed to
    getRoute to be consistent with other clients
  - The order of error checks in the TraCI server changed so that
    unknown object errors show up before unknown variable / command
    errors
  - The python client
    - the function vehicle.add was replaced by the function
      addFull. The old "add" is still present and can be
      reactivated by monkey patching the module
      (traci.vehicle.add=traci.vehicle.addLegacy)
    - constants which were previously available in the single
      domains have moved to traci.constants
    - now returns tuples instead of lists for all getIDList and
      several other calls
    - the function *getSubscriptionResults* now requires the ID of
      the subscribed object as argument. To retrieve all results
      in a map, the function *getAllSubscriptionResults* can be
      used

## Older Versions

- [Z/Changes from Version 0.31.0 to Version
  0.32.0](Z/Changes_from_Version_0.31.0_to_Version_0.32.0.md)
- [Z/Changes from Version 0.30.0 to Version
  0.31.0](Z/Changes_from_Version_0.30.0_to_Version_0.31.0.md)
- [Z/Changes from Version 0.29.0 to Version
  0.30.0](Z/Changes_from_Version_0.29.0_to_Version_0.30.0.md)
- [Z/Changes from Version 0.28.0 to Version
  0.29.0](Z/Changes_from_Version_0.28.0_to_Version_0.29.0.md)
- [Z/Changes from Version 0.27.1 to Version
  0.28.0](Z/Changes_from_Version_0.27.1_to_Version_0.28.0.md)
- [Z/Changes from Version 0.27.0 to Version
  0.27.1](Z/Changes_from_Version_0.27.0_to_Version_0.27.1.md)
- [Z/Changes from Version 0.26.0 to Version
  0.27.0](Z/Changes_from_Version_0.26.0_to_Version_0.27.0.md)
- [Z/Changes from Version 0.25.0 to Version
  0.26.0](Z/Changes_from_Version_0.25.0_to_Version_0.26.0.md)
- [Z/Changes from Version 0.24.0 to Version
  0.25.0](Z/Changes_from_Version_0.24.0_to_Version_0.25.0.md)
- [Z/Changes from Version 0.23.0 to Version
  0.24.0](Z/Changes_from_Version_0.23.0_to_Version_0.24.0.md)
- [Z/Changes from Version 0.22.0 to Version
  0.23.0](Z/Changes_from_Version_0.22.0_to_Version_0.23.0.md)
- [Z/Changes from Version 0.21.0 to Version
  0.22.0](Z/Changes_from_Version_0.21.0_to_Version_0.22.0.md)
- [Z/Changes from Version 0.20.0 to Version
  0.21.0](Z/Changes_from_Version_0.20.0_to_Version_0.21.0.md)
- [Z/Changes from Version 0.19.0 to Version
  0.20.0](Z/Changes_from_Version_0.19.0_to_Version_0.20.0.md)
- [Z/Changes from Version 0.18.0 to Version
  0.19.0](Z/Changes_from_Version_0.18.0_to_Version_0.19.0.md)
- [Z/Changes from Version 0.17.1 to Version
  0.18.0](Z/Changes_from_Version_0.17.1_to_Version_0.18.0.md)
- [Z/Changes from Version 0.17.0 to Version
  0.17.1](Z/Changes_from_Version_0.17.0_to_Version_0.17.1.md)
- [Z/Changes from Version 0.16.0 to Version
  0.17.0](Z/Changes_from_Version_0.16.0_to_Version_0.17.0.md)
- [Z/Changes from Version 0.15.0 to Version
  0.16.0](Z/Changes_from_Version_0.15.0_to_Version_0.16.0.md)
- [Z/Changes from Version 0.14.0 to Version
  0.15.0](Z/Changes_from_Version_0.14.0_to_Version_0.15.0.md)
- [Z/Changes from Version 0.13.1 to Version
  0.14.0](Z/Changes_from_Version_0.13.1_to_Version_0.14.0.md)
- [Z/Changes from Version 0.13.0 to Version
  0.13.1](Z/Changes_from_Version_0.13.0_to_Version_0.13.1.md)
- [Z/Changes from Version 0.12.3 to Version
  0.13.0](Z/Changes_from_Version_0.12.3_to_Version_0.13.0.md)
- [Z/Changes from Version 0.12.2 to Version
  0.12.3](Z/Changes_from_Version_0.12.2_to_Version_0.12.3.md)
- [Z/Changes from Version 0.12.1 to Version
  0.12.2](Z/Changes_from_Version_0.12.1_to_Version_0.12.2.md)
- [Z/Changes from Version 0.12.0 to Version
  0.12.1](Z/Changes_from_Version_0.12.0_to_Version_0.12.1.md)
- [Z/Changes from Version 0.11.1 to Version
  0.12.0](Z/Changes_from_Version_0.11.1_to_Version_0.12.0.md)
- [Z/Changes from Version 0.11.0 to Version
  0.11.1](Z/Changes_from_Version_0.11.0_to_Version_0.11.1.md)
- [Z/Changes from Version 0.10.3 to Version
  0.11.0](Z/Changes_from_Version_0.10.3_to_Version_0.11.0.md)
- [Z/Changes from Version 0.10.2 to Version
  0.10.3](Z/Changes_from_Version_0.10.2_to_Version_0.10.3.md)
- [Z/Changes from Version 0.10.1 to Version
  0.10.2](Z/Changes_from_Version_0.10.1_to_Version_0.10.2.md)
- [Z/Changes from Version 0.10.0 to Version
  0.10.1](Z/Changes_from_Version_0.10.0_to_Version_0.10.1.md)
- [Z/Changes from Version 0.9.10 to Version
  0.10.0](Z/Changes_from_Version_0.9.10_to_Version_0.10.0.md)
- [Z/Changes from version 0.9.9 to version
  0.9.10](Z/Changes_from_version_0.9.9_to_version_0.9.10.md)
- [Z/Changes from version 0.9.8 to version
  0.9.9](Z/Changes_from_version_0.9.8_to_version_0.9.9.md)
- [Z/Changes from version 0.9.7 to version
  0.9.8](Z/Changes_from_version_0.9.7_to_version_0.9.8.md)
- [Z/Changes from version 0.9.6 to version
  0.9.7](Z/Changes_from_version_0.9.6_to_version_0.9.7.md)
- [Z/Changes from version 0.9.5 to version
  0.9.6](Z/Changes_from_version_0.9.5_to_version_0.9.6.md)
- [Z/Changes from version 0.9.3 to version
  0.9.4](Z/Changes_from_version_0.9.3_to_version_0.9.4.md)
- [Z/Changes from version 0.9.2 to version
  0.9.3](Z/Changes_from_version_0.9.2_to_version_0.9.3.md)
- [Z/Changes from version 0.9.1 to version
  0.9.2](Z/Changes_from_version_0.9.1_to_version_0.9.2.md)
- [Z/Changes from version 0.9.0 to version
  0.9.1](Z/Changes_from_version_0.9.0_to_version_0.9.1.md)
- [Z/Changes from version 0.8.x to version
  0.9.0](Z/Changes_from_version_0.8.x_to_version_0.9.0.md)
- [Z/Changes from version 0.8.2.2 to version
  0.8.3](Z/Changes_from_version_0.8.2.2_to_version_0.8.3.md)
- [Z/Changes from version 0.8.2.1 to version
  0.8.2.2](Z/Changes_from_version_0.8.2.1_to_version_0.8.2.2.md)
- [Z/Changes from version 0.8.0.2 to version
  0.8.2.1](Z/Changes_from_version_0.8.0.2_to_version_0.8.2.1.md)
- [Z/Changes from version 0.8.0.1 to version
  0.8.0.2](Z/Changes_from_version_0.8.0.1_to_version_0.8.0.2.md)
- [Z/Changes from version 0.8.0 to version
  0.8.0.1](Z/Changes_from_version_0.8.0_to_version_0.8.0.1.md)
- [Z/Changes from version 0.7.0 to version
  pre0.8](Z/Changes_from_version_0.7.0_to_version_pre0.8.md)