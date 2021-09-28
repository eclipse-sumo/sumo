---
title: Changes in the 2018 releases (versions 1.0.0, 1.0.1 and 1.1.0)
---

## Version 1.1.0 (18.12.2018)

### Bugfixes

- Simulation
  - Fixed Bug where vehicles would stop moving after changing to the opposite direction lane. Issue #4610
  - Fixed [parkingAreaReroute](../Simulation/Rerouter.md#rerouting_to_an_alternative_parking_area) to a destination which is too close for stopping. Issue #4612
  - Fixed crash when using multiple vehicles with device.bluelight. Issue #4629
  - Fixed crash when using carFollowModel ACC on multi-lane roads. (regression in 1.0.1)
  - Fixed crash when loading signal plan with invalid characters. Issue #4673
  - Fixed invalid halting count in E3 detector output Issue #4756
  - Fixed crashes related to vehicles that lane-change multiple times on the E3-detector entry edge. Issue #4803, Issue #4845
  - Fixed bugs that were causing unnecessarily strong deceleration. Issue #2310, Issue #4806, Issue #4809, Issue #4851, Issue #4855, Issue #4462, Issue #4900, Issue #4928, Issue #4930
  - Fixed deadlock caused by long vehicles being unable to re-enter traffic after parking in a short parkingArea. Issue #4832
  - Fixed invalid error when defining a stop on an edge and its consecutive internal edge. Issue #4837
  - Fixed bug where vehicles from minor roads would drive onto the intersection to aggressively. Issue #4854
  - Fixed bug where pedestrians would ignore prioritized vehicles when walking onto a crossing. Issue #4865
  - Fixed invalid upper bound on density in [edgeData-output](../Simulation/Output/Lane-_or_Edge-based_Traffic_Measures.md#generated_output) for multi-lane edges. Issue #4904
  - Fixed bug where trains would fail to reverse on bidirectional rail edges. Issue #4921

- sumo-gui
  - Closing and re-opening an edge or lane no longer causes invalid lane choice. Issue #4514
  - Errors due to invalid `<param>` values are now shown in the message window. Issue #4753
  - Fixed invalid vehicle shapes when drawing trains as *simple shapes* in mesoscopic mode. Issue #4773

- netconvert
  - Fixed bug that was causing invalid road geometry when using high-resolution input data at dense junction clusters. Issue #4648
  - Fixed bug when building networks for opposite-direction overtaking that was causing collisions in the simulation. Issue #4628
  - Fixed invalid right-of-way rules in left-hand networks. Issue #4256
  - Duplicate edge ids in shapefile import are no longer silently ignored. Instead, a warning is issued and ids are made unique with a numerical suffix.
  - Fixed crash when loading height data from multiple geotif files. Issue #4786
  - Option **--tls.guess** no longer builds traffic lights at almost every intersection. The default lane-speed-sum threshold (Option **--tls.guess.threshold**) was changed from 150km/h to 250km/h. Issue #1688
  - Options **--check-lane-foes.all**, **--check-lane-foes.roundabout** are no longer lost when re-importing a .net.xml file. Issue #4813
  - Fixed unnecessary right-of-way restrictions in some cases where incoming edges target distinct lanes on the target edge. Issue #4815
  - Fixed bug where custom walkingarea shapes where sometimes ignored. Issue #4847
  - Fixed bugs that could cause loss of elevation information. Issue #4877, Issue #4878
  - Fixed permissions on internal lanes after internal junctions. Issue #4912

- netedit
  - Loading joined traffic light programs via menu is now working. Issue #4622
  - Menu option *Load Foreign* can now be used to import OSM files directly. Issue #4634
  - Fixed bug where *right-of-way* mode would show invalid properties. Issue #4637
  - TLS locator now lists traffic lights even if their id differs from the junction id. Issue #4661
  - Fixed crash when clicking buttons that should be disabled. Issue #4662
  - Fixed creation of unyielding connections via shift-click. Issue #4785 (regression in 1.0.1)
  - Fixed modifying elevation in Move-mode (regression in 1.0.0). Issue #4841

- TraCI
  - Fixed bug that was causing exaggerated output for *traci.vehicle.getDrivingDistance* when vehicles were driving on a looped route. Issue #4642
  - *traci.vehicle.getLaneChangeState* now returns correct information while controlling the vehicle with *moveToXY*. Issue #4545
  - Fixed retrieval of shape data with more than 255 points (*polygon.getShape, lane.getShape, junction.getShape* Issue #4707
  - Fixed bug when setting polygon shape with the C++ client.
  - Vehicle.isStopped is now working in TraaS. Issue #4883
  - Fixed adding vehicles after a simulation time of 24.8 days (python client). Issue #4920
  - Fixed bug where lateral collisions from unsafe calls to vehicle.changeSublane are prevented despite disabling safeguards via setLaneChangeMode. Issue #4864

### Enhancements

- Simulation
  - The eagerness for opposite-direction overtaking can now be
    configured with the new vType attribute *lcOpposite* (range
    \[0,inf\[). Issue #4608
  - `<personinfo>` elements now include attribute *type*. Issue #4695
  - The [ToC Device](../ToC_Device.md) now provides an option
    for generating output. Issue #4750
  - Added warning messages for vehicles that perform emergency
    braking. The warning threshold can be configured with the new
    option **--emergencydecel.warning-threshold** {{DT_FLOAT}}. The given threshold is compared against the severity of
    the event (braking with emergencyDecel corresponds to 1 and
    braking with decel corresponds to 0). By default the threshold
    is set to 1. Issue #4792
  - Trains can now [reverse
    direction](../Simulation/Railways.md#reversing_direction)
    anywhere on a bidirectional track.
  - TAZ can now be used as *via* destinations by using
    *<TAZID\>-sink* as edge id. Issue #4882
  - ParkingAreas now support the attribute *onRoad*. If set to
    *true*, vehicles will park on the road lane, thereby reducing
    road capacity. Issue #4895

- sumo-gui
  - Added `<poly>` attribute *lineWidth* to determine the drawing width of
    unfilled polygons. Issue #4715
  - TAZ now support attribute *color* for rendering the associated
    shape. This color can also be used in the new edge coloring mode
    *color by TAZ*. Issue #3979
  - Can now color streets according to custom lane or edge
    [parameters](../Simulation/GenericParameters.md). Issue #4276
  - The numerical value for coloring edges/lanes, vehicles and
    persons (e.g. speed limit, acceleration, waitingTime) can now
    optionally be shown. When this option is set, the value will
    also be printed in the object tool tip. Likewise, the numerical
    value for coloring vehicles (e.g. acceleration) can optionally
    be shown. Issue #4840
  - Breakpoints can now be set via option **--breakpoints** {{DT_TIME}}**,**{{DT_TIME}}**...**. Issue #1957
  - Added visualization option to apply size exaggeration only to
    selected objects. Issue #3422
  - The text background color for IDs and object descriptions can
    now be configured. Issue #4894

- netedit
  - New editing mode for TAZ (Traffic Analysis Zones). Issue #3425
  - Overlapping objects can now be inspected via consecutive clicks
    on the same location. Issue #2385
  - Shift-click can now be used to inspect, select and delete lanes.
  - LaneAreaDetectors (E2) that span multiple consecutive lanes can
    now be created within netedit. Issue #2909
  - POIs can now be created from geo-coordinates in the clipboard
    (e.g. from online map sites) with a single click. Issue #4496
  - Selection colors can now be customized. Issue #4736
  - Can now color streets according to custom lane or edge
    [parameters](../Simulation/GenericParameters.md). Issue #4276

- netgenerate
  - Added options **--perturb-x, --perturb-y, --perturb-z** to add random perturbations to all node
    coordinates. Perturbations can be specified as *mean* or capped
    normal distribution *normc(a,b,c,d)*. Issue #4776

- netconvert
  - Added option **--tls.layout** {{DT_STR}} for selecting between layouts that group opposite
    directions ('opposites') and layouts that have one phase per
    incoming edge ('incoming'). Issue #4033
  - Added option **--tls.guess.threshold** {{DT_FLOAT}} to control the heuristic for guessing traffic
    lights. A traffic light is guessed if the sum of speeds of all
    incoming lanes exceeds the threshold value. Issue #4681
  - Added new node attribute `rightOfWay` to configure the algorithm for
    computing right-of-way rules (allowed values are *default*,
    *edgePriority*). Also added new option **--default.right-of-way** to set this value for
    all nodes in a network. Issue #4843
  - Importing internal lane shapes from OpenDRIVE is now supported
    using option **--opendrive.internal-shapes**. Issue #4331

- Tools
  - [netcheck.py](../Tools/Net.md#netcheckpy) checks
    lane-based reachability and handles pedestrian infrastructure. Issue #4619
  - [route2poly.py](../Tools/Visualization.md#intersection_flow_diagram)
    can now be used to generate intersection flow diagrams. Issue #4657
  - [tripinfoDiff.py](../Tools/Output.md#tripinfodiffpy) now
    supports comparing `<personinfo>`s by setting option **--persons**.

- TraCI
  - [Context Subscriptions can now be equipped with
    filters](../TraCI/Interfacing_TraCI_from_Python.md#context_subscription_filters)
    acting on the server side. Issue #4343
  - TraCI now offers the method
    [openGap()](../TraCI/Change_Vehicle_State.md#open_gap_0x16)
    for temporary changes of the vehicle's desired headway. Issue #4172
  - Added functions *person.getPosition3D, person.getAngle,
    person.getLanePosition, person.getColor, person.getLength* to
    the C++ client.

### Other

- Simulation
  - Simulation now runs about 17% faster compared to the previous
    release due to improvements in data structures and less
    (redundant) collision checking Issue #4917.
  - The default speedDeviation of vehicle classes
    *private,vip,hov,custom1* and *custom2* was changed from 0 to
    0.1. The default speed deviation for vehicle class *taxi* was
    changed from 0 to 0.05.
- Documentation
  - Updated [documentation for railway
    simulation](../Simulation/Railways.md)
  - Updated [marouter](../marouter.md#usage_description)
    documentation
- Miscellaneous
  - Network version is now 1.1
  - TraCI Version is now 19

- [sumolib](../Tools/Sumolib.md) now uses an object with members
  *state, duration, minDur, maxDur, next* instead of tuple *(state,
  duration)* to represent traffic light phases.

## Version 1.0.1 (18.09.2018)

### Bugfixes

- Simulation
  - Fixed deadlock on parallel routing (regression in 1.0.0). Issue #4518
  - default emergency deceleration did not depend on the vClass but
    was always 9
  - Fixed bugs related to opposite-direction overtaking that were
    causing collisions and other invalid behavior. Issue #4550, Issue #3718, Issue #4564, Issue #4570, Issue #4588, Issue #4589, Issue #4591, Issue #4592
  - Fixed crash when using sublane model with varying lane widths. Issue #4547
  - Fixed hard braking after lane-changing when using carFollowModel
    *IDM* Issue #4517
  - Fixed exaggerated gap between standing vehicles when using
    carFollowModel *IDM*. Issue #4527
  - Fixed crash when using *device.ssm*. Issue #4556
  - Fixed invalid cooperative lane-changing attempts. Issue #4566
  - Fixed invalid speed adaptation for lane changing. Issue #4563
  - Public transport routing now properly uses stops that are
    defined as child elements of a vehicle. Issue #4575

- sumo-gui
  - Fixed freeze when activating 'Show lane direction' for lanes
    with width=0. Issue #4533

- netedit
  - Fixed freeze when activating 'Show lane direction' for lanes
    with width=0. Issue #4533
  - Custom endpoints are now honored when reversing an edge or
    adding a reverse direction edge via context menu. Issue #4501

- netconvert
  - Networks for opposite direction driving now allow overtaking
    past junctions on straight connections that are slightly curved. Issue #4585

- od2trips
  - Flows with non-positive probability are no longer written. Issue #4600

- MESO
  - Fixed deadlock in public transport simulation. Issue #4560

- TraCI
  - Fixed bug when calling TraaS method
    *Simulation.getDistanceRoad*. Issue #4603
  - Closing and re-opening a lane no longer causes invalid lane
    choice. Issue #4514

### Enhancements

- Simulation
  - The collision-detection threshold configured via option **--collision.mingap-factor** can now
    be customized separately for each vType using attribute
    *collisionMinGapFactor*. Issue #4529
  - Increased running speed of simulations with *device.rerouting*
    using few vehicles in a large network. Issue #4598

- netedit
  - Added option *reset custom shape* to the right-click menu of
    junction- and lane-selections Issue #4481, Issue #4490
  - When editing junction shapes, vertices can now be removed by
    shift-click. Issue #4494

- TraCI
  - added method *traci.edge.getStreetName* Issue #4557
  - method *simulation.getParameter* can now be used to [retrieve
    stopping place
    names](../TraCI/Simulation_Value_Retrieval.md#generic_parameter_retrieval_0x7e). Issue #4558

- Tools
  - [cutRoutes.py](../Tools/Routes.md#cutroutespy) now
    supports route references and stop access. Issue #4554, Issue #4595

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
    *emergencyDecel* as an absolute bound on deceleration. Issue #3556
  - Fixed some issues when using [continous lane
    change](../Simulation/SublaneModel.md#simple_continous_lane-change_model). Issue #3767, Issue #3769, Issue #3770, Issue #3771, Issue #4364
  - Fixed back-and-forth changing when using [continous lane
    change](../Simulation/SublaneModel.md#simple_continous_lane-change_model). Issue #4010
  - Fixed loading of teleporting vehicles from simulation state in
    *.sbx* format. Issue #3787
  - Fixed invalid vehicle counts by E2-detector related to
    lane-changing. Issue #3791
  - Fixed invalid vehicle counts by E3-detector related to re-using
    vehicle pointers Issue #3108, Issue #4079
  - Fixed bug that was causing invalid slowdown while passing an
    intersection. Issue #3861
  - Fixed bug that was causing pedestrians on looped routes to block
    themselves. Issue #3888
  - Vehicle speedFactor is now included in saved state. Issue #3881
  - Fixed invalid collision events when lanes are too narrow for the
    vehicles. Issue #3056
  - Fixed collision between pedestrians and vehicles. Issue #3964
  - Fixed bug where option **--random-depart-offset** would trigger invalid warnings regarding
    unsorted route file. Issue #4076
  - Fixed invalid stopping position after collision when using
    option **--collision.stoptime** Issue #4106
  - Fixed right-of-way rules for vehicles standing next to each
    other on the same lane and driving towards different edges. Issue #4071
  - Fixed crash within intersection between vehicles coming from the
    same lane. Issue #4100
  - Fixed invalid *departDelay* for triggered vehicles. Issue #4199
  - Fixed hang-up when scheduling a stop on internal edges after
    internal junctions Issue #4254
  - ParkingAreaReroute no longer triggers if the destination is not
    among the set of alternatives. Issue #4243
  - Fixed (near) infinite loop when specifying invalid speed
    distribution. Issue #4282
  - Fixed invalid car-following behavior at changing lane widths
    when using the sublane model. Issue #4223
  - Fixed crash when simulating invalid pedestrian routes with
    option **--ignore-route-errors**. Issue #4306
  - Option **--carfollow.model** is now working.
  - Car-follow models *IDM* and *IDMM* no longer fail to reach an
    exact stop position. Issue #658
  - Strategic look-ahead is now working as expected in networks with
    many short edges. Issue #4349
  - Switching a traffic light to the "off" definition now sets the
    correct right-of-way rules. Issue #1484
  - Fixed invalid deceleration at yellow traffic light. Issue #4450

- sumo-gui
  - width of railway edges is now taken into account when drawing
    (interpreted as gauge). Issue #3748
  - window-size and position now remain unchanged when reloading the
    simulation. Issue #3641
  - Random vehicle and person coloring is now more random on
    windows. Issue #3740
  - Vehicles that were not inserted (i.e. due to option **--scale** or **--max-depart-delay**) no
    longer count as *arrived* in the simulation parameter dialog.
    Instead the are counted under the new item *discarded vehicles*. Issue #3724
  - Fixed crashing due to concurrent access to vehicle numbers. Issue #3804
  - Fixed issues related to drawing smooth corners at curving roads. Issue #3840
  - Fixed vehicle positions when using the sublane model in lefthand
    networks. Issue #3923
  - Fixed crashing and visualization problems when using the
    3D-view. Issue #4037, Issue #4039
  - Fixed glitch where persons would appear to jump around while
    riding in a vehicle across an intersection. Issue #3673
  - Tracking of riding persons now centers on the person rather than
    the front its vehicle. Issue #4209
  - ChargingStation are visible again (regression in 0.32.0) Issue #4183
  - Fixed invalid vehicle blinkers in lefthand simulation. Issue #4258
  - Traffic lights now remain accessible (right-click, tooltip)
    after calling *traci.trafficlight.setRedYellowGreenStaate*. Issue #4426
  - Fixed invalid vehicle angle on lane with zero-length geometry.

- MESO
  - Fixed deadlock at roundabouts when running with option **--meso-junction-control.limited** or **--meso-junction-control false**. Issue #4074

- netedit
  - Splitting edges, deleting individual edges and lanes or adding
    lanes via the *duplicate* menu option no longer resets
    connections and traffic light plans. Issue #3742
  - The viewing area and zoom loaded via option **--gui-settings-file** is no longer
    ignored
  - Fixed bug where connections on large junctions did not register
    clicks or were not drawn. Issue #3726
  - Fixed crash when removing inspected object via *undo*. Issue #3781
  - Fixed various bugs that led to re-computation of signal plans
    when modifying connections or tls indices. Issue #3742, Issue #3832
  - Fixed bug that could lead to the creation of invalid networks
    when adding and removing connections. Issue #3824, Issue #3813
  - Custom connection shape is now longer lost after
    *delete*+*undo*. Issue #3822
  - Moving geometry no longer removes z-data. Issue #3723
  - Splitting edges no longer introduces unnecessary custom
    endpoints. Issue #3895
  - Fixed inverted interpretation of *lanePosLat* for POIs compared
    to sumo-gui. Issue #4002
  - Fixed crash when deleting one of multiple signal programs for
    the same junction. Issue #4132
  - Added support for BusStops Acces. Issue #4018
  - Crossings no longer become invalid when splitting an edge. Issue #4295

- netconvert
  - Option **--ramp.guess** no longer builds ramps at traffic light controlled
    nodes. Issue #3848
  - Fixed bug that was causing invalid link states at intermodal
    junctions. Issue #2944 Issue #3851
  - Fixed bugs that were causing invalid link directions. Issue #3852 Issue #3853
  - Fixed invalid junction logic in lefthand networks. Issue #3854
  - Generated signal plans will no longer have a protected left-turn
    phase if there is no dedicated left-turn lane. Issue #4087
  - Various fixes in regard to classifying nodes as type
    *rail_crossing* and joining clusters of rail crossing nodes.
  - Option **--dismiss-vclasses** is now working when loading a *.net.xml* file. Issue #4230
  - Fixed invalid junction shapes when using option **--junction.corner-detail**. Issue #4292
  - Fixed invalid intermodal junction logic that could cause
    collisions and deadlocks. Issue #4198, Issue #4252

- duarouter
  - Fixed crash on intermodal routing. Issue #3883

- polyconvert
  - Fixed bug when importing OSM data with objects that are marked
    as *deleted*. Issue #3786

- TraCI
  - Fixed bug where persons would "jump" when replacing the current
    walking stage with a new one. Issue #3744
  - Fixed crash when trying to access empty subscription result list
    using the C++ client.
  - Vehicle *emergencyDecel* is now at least as high as *decel*
    after calling *traci.vehicle.setDecel*. Issue #3755
  - Fixed python client bug in *traci.polygon.setShape*. Issue #3762
  - Adding a route with an empty list of edges now results in an
    error. Issue #3845
  - Vehicles that drive outside the road network under the control
    of *moveToXY* now properly updated their speed and brake lights. Issue #3837
  - Function *vehicle.getLaneChangeMode* now returns correct values
    for the original lane change state when the state is affected by
    *vehicle.setLaneChangeMode*. Issue #3810
  - Car-following related vehicle type parameters (e.g. *accel*)
    that are changed via traci are now correctly saved when saving
    simulation state. Issue #3522
  - Functions *simulation.findRoute* and
    *simulation.findIntermodalRoute* no longer crash sumo when
    trying to route from a forbidden edge. Issue #4121
  - Fixed invalid results when calling
    *simulation.findIntermodalRoute* Issue #4145, Issue #4147, Issue #4148
  - Multi-Lane E2-Detectors now return the correct length. Issue #4356
  - Fixed bug where *vehicle.couldChangeLane* returned *True*
    immediately after a lane change even though the requested target
    lane did not exist. Issue #4381
  - Fixed collisions when combining *vehicle.slowDown* with
    ballistic update. Issue #4418
  - Fixed python client bug in *traci.lane.getLinks* which returned
    always true for isOpen, hasFoe and hasPrio.
  - traci.person.setColor was fixed (was a noop before)

- Tools
  - [randomTrips.py](../Tools/Trip.md#randomtripspy) now uses
    vType attributes from option **--trip-attributes** when generating persons.
  - Removed singularities at the distribution boundaries generated
    by
    [createVehTypeDistributions.py](../Tools/Misc.md#createvehtypedistributionspy) Issue #4193


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
    records the time spent with intentional stopping. Issue #3693
  - A pedestrian crossing can now have different signal states for
    both directions. Issue #3820
  - FCD-output can now be switched on selectively for a subset of
    vehicles and the reporting period can be configured. Issue #1910
  - FCD-output can now be restricted to a subset of network edges
    loaded from a file with option **--fcd-output.filter-edges.input-file** {{DT_FILE}}. The file format is that of an
    edge selection as saved by [netedit](../Netedit/index.md).
  - Intended departure times (attribute *depart*) and intended
    vehicle id (attribute *intended*) are now added to
    vehroute-output of public transport rides. Issue #3948
  - Stopping place names are now added as XML-comments in
    vehroute-output of public transport rides. Issue #3952
  - Lane-Change-Model parameter *lcTurnAlignmentDistance* added for
    the control of dynamic alignment in simulations using the
    sublane model, see [Lane-Changing
    Models](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#lane-changing_models), Issue #4025
  - Implemented [SSM
    Device](../Simulation/Output/SSM_Device.md), which allows
    output of saftey related quantities. Issue #2669, Issue #4119
  - 'Smoothed' emergency braking Issue #4116
  - Added an initial version of a [driver state
    device](../Driver_State.md) for modelling imperfect
    driving.
  - Added an initial version of a [transition of control
    model](../ToC_Device.md).
  - Option **--ignore-route-errors** now also allows insertion of vehicles with unsafe
    headways. Issue #4118
  - Added a new car-following model "ACC" based on \[Milanés et al.
    "Cooperative Adaptive Cruise Control in Real Traffic
    Situations." IEEE Trans. Intelligent Transportation Systems 15.1
    (2014): 296-305.\]
  - [Statistic
    output](../Simulation/Output/index.md#aggregated_traffic_measures)
    now also includes total delay by vehicles which could not be
    inserted by the end of the simulation if options **--duration-log.statistics** and **--tripinfo-output.write-unfinished** are set. Issue #4174
  - The default lane-changing model *LC2013* now supports [parameter
    *lcAssertive*](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#lane-changing_models). Issue #4194
  - [Vehroute-output](../Simulation/Output/VehRoutes.md) now
    includes the reason for rerouting. Issue #4204
  - Added optional attribute *visible* to `<parkingAreaReroute` (default *false*). This
    controls whether occupancy is known before reaching the
    parkingArea edge and can be used to model incomplete knowledge
    in parking reroute choice. Issue #4244
  - Rerouters now support the attribute *vTypes* which restricts
    their effect to vehicles from the given list of vehicle type
    IDs. Issue #4031
  - Vehicles are forming a coridor for emergency vehicles Issue #1967
  - Added option **--default.speeddev** {{DT_FLOAT}} to control the default speed deviation of all
    vehicle types that do not define it. Issue #4421
  - Added lane/edge-attribute stopOffset for defining vClass
    specific stopping positions in front of traffic lights. Issue #3754

- sumo-gui
  - Added control for scaling traffic demand dynamically. Issue #1951
  - Added option to disable drawing of bicycle lane markings on
    intersections.
  - All laneChangeModel-related vType-parameters and all
    junction-model related vType-parameters are now shown in the
    vType-Parameter dialog.
  - The simulation view can now be rotated via the new gui-settings
    attribute *angle* in the `<viewport>` or via the viewport dialog. Issue #3841
  - When drawing junction shapes with exaggerated size and setting
    option *show lane-to-lane connections*, the connecting lines are
    now scaled up at traffic light junctions. Issue #3796
  - The simulation state can now be saved via the *Simulation* Menu. Issue #2513
  - Object tracking can now be aborted via double-click.
  - Person plans can now be inspected via the right-click menu. Issue #3886
  - Object name rendering size can now be toggled between constant
    pixel size (all visible when zoomed out) and constant network
    size (invisible when zoomed out). Issue #3931
  - The *Delay* value is now invariant with regard to the simulation
    step length and always denotes delay per simulated second. Issue #4176
  - Added GUI Shape for different types of emergency vehicles
    (ambulance, police and firebrigade) Issue #1967

- MESO
  - Simulation of persons and public transport is now supported. Issue #3816

- netedit
  - Added *Prohibition*-mode for checking right-of-way at junctions
    (hotkey 'w'). Thanks to Mirko Barthauer for the contribution. Issue #3850
  - Added virtual attribute to identify [bidirectional rail edge
    pairs](../Simulation/Railways.md#bidirectional_track_usage). Issue #3720
  - Added option to modify the visualisation of [bidirectional rail
    edge
    pairs](../Simulation/Railways.md#bidirectional_track_usage)
    (*spread superposed*) Issue #3720
  - Added button *add states* to *traffic light*-mode to complement
    the functionality of *cleanup states*. Issue #3846
  - netedit now flags connection targets with incompatible
    permissions as *conflict* and only creates them with
    *<ctrl\>+<click\>*. Issue #3823
  - Traffic light indices are now drawn for pedestrian crossings
    when enabled via gui settings. Issue #3814
  - Now ParkingAreas and ParkingSpaces can be created in netedit. Issue #3104
  - When adding a new phase to a traffic light, the new phase will
    now have a plausible successor state depending on the selected
    previous phase (rather the being a copy). Issue #4041
  - Added function 'split' to junction context menu. This can be
    used to disaggregated joined junctions. Issue #4046
  - When joining traffic lights (by editing junction attribute
    'tl'), link indices of the target traffic light are now
    preserved if signal groups are used (multiple connections with
    the same *linkIndex* value). Issue #4094

    !!! caution
        The the signal states for the edited junction must be checked by the user

  - Width, height and diagonal size of a rectangle selection are now
    shown in the status bar. This can be used to measure distances.
  - Now Generic Parameters can be loaded, saved and edited. Issue #3485
  - Re-organized options in the options-dialog (F10). Issue #4420
  - Option help is now shown in the status bar of the options
    dialog. Issue #2900
  - Custom edge geometry endpoints can now be manipulated in *Move
    mode*. Issue #3716

- netconvert
  - Geo-referenced networks (i.e. from OSM) can now be merged by loading them together (**-s net1.net.xml,net2.net.xml**). Issue #4085
  - Element `<crossing>` now supports attribute *linkIndex2* to specify a custom signal index for the reverse direction. Issue #3820
  - When defining [double connections](../Networks/PlainXML.md#multiple_connections_from_the_same_edge_to_the_same_target_lane), the right-of-way rules now take the road topology into account to differentiate between on-ramp and off-ramp situations. Issue #3834
  - Importing VISUM networks up to format version 10 is now support. Issue #3898
  - Improved heuristics for options **--junctions.join**. Issue #876
  - Improved computation of intermodal junctions imported from OSM. Issue #4003
  - Improved computation of junction shapes.
  - Added option **--proj.rotate** {{DT_FLOAT}} for rotating the network.
  - Added option **--prefix** {{DT_STR}} which can be used to add a prefix to the written junction and edge IDs.
  - Added options **--tls.scramble.time --tls.crossing-clearance.time --tls.crossing-min.time** to control the timing of pedestrian crossing phases. Issue #4078
  - Added option **--tls.minor-left.max-speed** {{DT_FLOAT}} to ensure that left turns through oncoming traffic are not build for high-speed roads. Issue #4091
  - Sidewalk information is now imported from OSM for road types that have a positive sidewalkWidth attribute (e.g. by using typemap [osmNetconvertPedestrians.typ.xml](../Networks/Import/OpenStreetMap.md#recommended_typemaps)). Issue #4096
  - Added option **--osm.stop-output.footway-access-factor** {{DT_FLOAT}} for increasing the length of stop access edges above the airline distance. Issue #4143.
  - Added option **--junctions.limit-turn-speed** {{DT_FLOAT}} which sets an upper bound on speed while passing an intersection based on the turning radius. To account for imperfect road geometry, the option **--junctions.limit-turn-speed.min-angle** can be used to avoid speed limits for small turns. Warnings are issued when the difference between road speed and connection speed rises above a configurable threshold (**--junctions.limit-turn-speed.warn.straight, --junctions.limit-turn-speed.warn.turn**). These warnings often indicate exaggerated road speeds on urban roads as well as faulty geometry. Issue #1141
  - Geo-reference information is now imported from OpenDRIVE input. Issue #4414

- netgenerate
  - Simplified node and edge names
    - The alphanumerical junction naming scheme now supports
      arbitrary grid sizes (using ids like *XY23*)
    - The alphanumerical junction naming scheme also extends to
      spider networks
    - The alphanumerical junction naming scheme is active by
      default (the option for enabling the old scheme was renamed
      from **grid.alphanumerical-ids** to **--alphanumerical-ids**).
    - When using alphanumerical junction ids, the intermediate
      string *to* is omitted from edge names because the edge ID
      already allows unambiguous determination of its junctions
      without it
  - Added option **--rand.random-lanenumber** to randomize lane numbers in random networks
    between 1 and *default.lanenumber*
  - Added option **--rand.random-priority** to edge priorities in random networks between 1
    and *default.priority*
  - Added option **--rand.grid** to place generated junctions on a regular grid
  - Added option **--prefix** {{DT_STR}} which can be used to add a prefix to the generated
    junction and edge IDs.
  - Corridor networks can now be generated by using options such as **-g --grid.x-number 3 --grid.y-number 1 --grid.attach-length 100**
  - Added options **--turn-lanes** {{DT_INT}} and **--turn-lanes.length** {{DT_FLOAT}} to add left-turn lanes to generated
    networks. Issue #3892

- duarouter
  - Intended departure times (attribute *depart*) and intended
    vehicle id (attribute *intended*) are now added to
    vehroute-output of public transport rides. Issue #3948
  - Stopping place names are now added as XML-comments in
    route-output of public transport rides. Issue #3952

- polyconvert
  - Option **--osm.use-name** now also applies to POIs. Issue #4246

- TraCI
  - [Libsumo](../Libsumo.md) can now be used in place of the
    TraCI client libraries to increase execution speed of
    TraCI-Simulations by directly linking against
    [sumo](../sumo.md) for all
    [SWIG](https://en.wikipedia.org/wiki/SWIG)-supported languages.
    Graphical simulation with libsumo is not yet supported.
  - function *vehicle.add* now supports using the empty string ("")
    as a route id to insert the vehicle on an arbitrary valid edge.
    This makes it easier to remote-control vehicles using moveToXY
    without defining an initial route. Issue #3722
  - added functions *simulation.getCollidingVehiclesNumber* and
    *simulation.getCollidingVehiclesIDList* to track collisions. Issue #3099
  - added functions *simulation.getEmergencyStoppingVehiclesNumber*
    and *simulation.getEmergencyStoppingVehiclesIDList* to track
    emergency stops. Issue #4312
  - added function *edge.getLaneNumber* to retrieve the number of
    lanes of an edge. Issue #3630
  - added function *vehicle.getAcceleration* to retrieve the
    acceleration in the previous step. Issue #4112
  - TraCI now allows to update a vehicle's best lanes
    (vehicle.updateBestLanes). Issue #4146
  - added function *gui.hasView* to determine whether graphical
    capabilities exist. Issue #4014
  - added function *lane.getFoes* to [to determine right-of-way and
    conflict relationships between incoming
    lanes](../TraCI/Lane_Value_Retrieval.md#extended_retrieval_messages).
  - function *simulation.getMinExpectedNumber()* now includes
    persons that are still active in the simulation. Issue #3707
  - added function *traci.getLabel* to the python client to help
    working with multiple connections.
  - When adding a vehicle with a disconnected 2-edge route, it will
    be treated like a `<trip>` and rerouted automatically. Issue #4307
  - added functions *vehicle.getRoutingMode* and
    *vehicle.setRoutingMode*. When setting routing mode to
    *traci.constants.ROUTING_MODE_AGGREGATED*, [smoothed travel
    times](../Demand/Automatic_Routing.md#edge_weights) are
    used instead of current travel times. Issue #3383

- Tools
  - added script
    [plot_trajectories.py](../Tools/Visualization.md#plot_trajectoriespy)
    which allows plotting various combinations of time, distance,
    speed and acceleration for all trajectories in a given **--fcd-output** file.
  - added script
    [averageRuns.py](../Tools/Misc.md#averagerunspy) for
    running a scenario multiple times with different random seeds
    and computing trip statistics
  - webWizard now correctly builds regions with left-hand traffic Issue #3928
  - Additional options for
    [generateTLSE3Detectors.py](../Tools/Output.md#generatetlse3detectorspy)
    that make it suitable for generating various kind of junction
    related output. Issue #1622
  - [createVehTypeDistributions.py](../Tools/Misc.md#createvehtypedistributionspy)
    now also allows to generate vehicle parameter distributions. Issue #4171
  - Added function simpla.createGap() Issue #4172
  - sumolib now supports lane.getWidth(). Issue #3842

### Other

- Miscellaneous
  - The space character ' ' is no longer allowed in xml option
    values when separating file names. Instead the comma ',' must be
    used. Files with space in their path are now supported. Issue #3817

- Documentation
  - Added [documentation on joined traffic lights and defining
    signal groups](../Simulation/Traffic_Lights.md).
  - Added [documentation on the simple continous lane-change
    model](../Simulation/SublaneModel.md#simple_continous_lane-change_model).
  - Documented all supported [routing
    algorithms](../Simulation/Routing.md#routing_algorithms).

- Simulation
  - `<vTypeProbe>` is now deprecated in favour of FCD-output filtering
  - `<vaporizer>` is now deprecated in favour of calibrators.
  - The default arrival position for person walks is now the middle
    of the destination edge. This reduces the assymetry from
    arriving in forward or backward direction. Issue #3843
  - tripinfo-output attribute *waitSteps* which counts the number of
    simulation steps in which the vehicle was below a threshold
    speed of 0.1m/s was replaced by attribute *waitingTime* which
    measures the same time in seconds. This gives results which are
    independent of the **--step-length** simulation parameter. Issue #3749
  - The default value for option **--device.rerouting.pre-period** was changed from *1* to *60* to
    speed up simulation. Issue #3865
  - The default speed deviation has been changed to *0.1*. When
    defining a new `<vType>` with attribute *vClass*, this also influences
    the default speed deviation:
    - *truck*, *trailer*, *coach*, *delivery*: 0.05
    - *tram*, *rail*, *rail_electric*, *rail_urban*,
      *emergency*: 0
    - everything else 0.1

!!! note
    The previous behavior can be restored by setting option **--default.speeddev 0**

- sumo-gui
  - Default color for persons is now 'blue' to better distinguish
    them from vehicles.

- netconvert
  - There is no longer an offset of 0.1m between lanes. This means
    the total visual width of an edge is now the sum of all lane
    widths. This also fixes an inconsistency between visualization
    and simulation as the vehicles always ignored this offset
    anyway. Road markings are now drawn on top of the lanes rather
    than between them. This causes small visual gaps in old networks
    (fixable by calling *netconvert -s old.net.xml -o new.net.xml*). Issue #3972
  - The default value for option **--default.junctions.radius** was increased from 1.5 to 4 in
    order to improve realism of turning angles.
  - The default value for option **--default.junctions.corner-detail** was increased from 0 to 5 to
    improve the visual realism of larger intersections.
  - Option **--ramps.guess** is now enabled by default
  - Vehicle speed while turning at intersections is now limited via
    the new option default **--limit-turn-speed 5.5**. To obtain the old behavior, this value
    can be set to -1.

- TraCI
  - TraCI Version is now 18
  - Embeded python is now deprecated in favour of
    [Libsumo](../Libsumo.md).
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