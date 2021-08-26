---
title: ChangeLog
---

## Git Main

### Bugfixes

- simulation
  - Fixed invalid approach information with step-method.ballistic that could cause collision. Issue #8955
  - Fixed rare collision on junction when the outbound lane is jammed. Issue #6415
  - Fixed invalid error after emergency braking at red light with ballistic update. Issue #8978
  - Fixed bug where ehicle never starts with startPos="stop" departSpeed="max" and ballistic update. Issue #8988
  - Fixed crash when using flow with invalid departEdge. Issue #8993
  - Fixed invalid use of busStop instead of containerStop in vehroute-output. Issue #8989
  - Fixed invalid behavior after using stop posLat in non-sublane simulation. Issue #9013
  
- netedit
  - Inverting selection of shapes now works even when no edges are loaded. Issue #8951 (regresssion in 1.9.2)
  - Fixed disappearance of connecting lines between E3 detector and its entry/exit elements. Issue #8916
  - Multi-parameter speedFactor value can now be set. Issue #8263
  - trainStops with access can now be loaded. Issue #8969

- sumo-gui
  - Fixed invalid person angle in output. Issue #9014
  
- netconvert
  - Connection attribute visibility does is now working if the connection has an internal junction. Issue #8953
  - Fixed crash when importing OpenDrive with internal lane shapes when the input defines no width. Issue #9009

- duarouter
  - Fixed bug where some input flows where ignored when preceded by non-flow elements. Issue #8995
  - tranship with attributes from,to is no longer ignored. Issue #8976
  - Fixed crash when loading containerFlow. Issue #8975
  - Fixed endless loop related to triggered personFlow. Issue #8977
  - Persons and containers with depart=triggered are now written in the correct order: directly after their intended vehicle. Issue #9000
  - Fixed crash when loading transport outside a container #9008

- polyconvert
  - Shapefiles will now be interpreted correctly when no projection is defined #8948
  
- traci
  - trafficLights.getControlledLinks passes the correct response size (this has no effect for the end user and is only relevent for client developers). Issue #8891
  - The debug-build of libsumo is now working. Issue #8946
  - Removing and reinserting a vehicle in the same step is now working. Issue #5647
  - traci.vehicle.getLeader can no longer return -inf when the leader is on an intersection. Issue #9001

- tools
  - cutRoutes.py: Fixed mixed usage of trainStop and busStop. Issue #8982
  - cutRoutes.py: Handles flows without end time
  - [generateRailSignalConstraints.py](Simulation/Railways.md#generaterailsignalconstraintspy)
    - Initial tripId set via vehicle param is now used. Issue #8959
    - Now using correct tripId when generating constraints for intermediate stop. Issue #8960
    - Fixed crash when there are two stops on the same edge. Issue #8958 (regression in 1.10)
    - 

### Enhancements

- simualation
  - When option **--vehroute-output.exit-times** is set, The output for walk,ride, transport and tranship now includes the values 'started' and 'ended.' Issue #9005

- sumo-gui
  - An index value is now drawn for each train reversal in 'show route' mode. Issue #8967
  - All stopping places (busStop, parkingArea, ...) now support custom color. Issue #8280
  
- netedit
  - Added context menu function to reset opposite-lane information for a selection of lanes. Issue #8888
  - Added now 'Lock' menu to protect differnt types of objects from inspect,move,select and delete operations. Issue #8342
  - Vehicle stop attribute posLat is now supported. Issue #8808
  - Saved busStop attributes now have the same order as netconvert. Issue #7624

- netgenerate
  - Added options **--grid.x-attach-length --grid.y-attach-length** to configure attachments separately by direction. Issue #8991
  - The option **--bidi-probability** can now be used to control the generation of reverse edges for grid and spider networks. When set to 0, pure oneway networks are generated. Issue #9006

- traci
  - Added function 'traci.simulation.getEndTime' to retrieve the **--end** value that was set when starting sumo. Issue #2764  

- tools
  - cutRoutes.py: Can now handle multiple additional and public transport files in input. Issue #8997
  - [generateRailSignalConstraints.py](Simulation/Railways.md#generaterailsignalconstraintspy) now supports options **--comment.stop --comment.time --comment.all** for extended annotation of generated constraints. Issue #8963

### Other

- Miscellaneous
  - Renamed the "master" branch in git to "main". Issue #8591

- polyconvert
  - When no network is loaded, output will now be in lon,lat by default (if the input is geo-referenced) in order to be useful with any network. The old behavior of writing raw utm values in this case can be restored by setting option **--proj.plain-geo false**.

## Version 1.10.0 (17.08.2021)

### Bugfixes

- simulation
  - Fixed invalid state file when using option **--vehroute-output.exit-times** and saved vehicles are still on their first edge. Issue #8536 (regression in 1.9.1)
  - Saved simulation state now restores traffic light phase and phase duration. Issue #7020
  - Fixed deadlock after loading rail signal constraints from state. Issue #8732
  - Fixed emergency braking with continuous lane changing. Issue #8466
  - Fixed crash when using routing algorithm 'CH' with device.rerouting.threads > 1. Issue #8767
  - Fixed train colliding with itself after early reversal. Issue #8768
  - Fixed collision with indirect left turn at priority junction (requires network with new 'indirect' attribute). Issue #8775
  - Fixed unnecessary deceleration of vehicle with low deceleration (i.e. freight train). Issue #8798
  - Fixed emergency braking with continuous lane changing and opposite direction driving. Issue #8760
  - Fixed invalid speed when train approaches reversal position. Issue #8867
  - Fixed crash on opposite direction driving with sublane model. Issue #8889, #8890
  - Lane closing (via rerouter) now triggers strategic changing for all vehicles. Issue #8895
  - Fixed invalid strategic lane choice in multimodal network. Issue #8900
  - Fixed emergency braking in sublane simulation. Issue #8908
  - Fixed schema validation when loading simulation state with time values larger than 24 days. Issue #8926
  - Fixed crash when loading simulation state with vehicle attribute 'departEdge', Issue #8929
  - Fixed crash when loading simulation state together with vehicles from an additional file. Issue #8927
  - Simulation with persons now terminates when participants have arrived after calling traci.simulation.loadState. Issue #8947
  - Fixed collision on junction due to premature acceleration. Issue #8817

- sumo-gui
  - Fixed briefly invisible vehicle while passing short internal edge. Issue #8749 (regression in 1.9.0)
  - Drawing parkingAreas with large roadsideCapacity no longer causes the gui to slow down. Issue #8400
  - Fixed invalid window size after using "restore" button. Issue #8826
  - Fixed flickering whiile vehicles drive on the opposite side. Issue #8887
  - Invisible vehicles (alpha=0) no longer register clicks. Issue #8913
  - Fixed invalid position of size-exaggerated persons at a size-exaggerated busStop. Issue #8883
  - Fixed hidden context menu near the screen border. Issue #8860
  - Asian street names are now shown correctly in lane parameter dialog on Linux if the font 'Noto CJK' is installed. Issue #8907

- netedit
  - Fixed unwanted modification of lane/edge permissions after leaving the dialog with 'Cancel'. Issue #8657
  - Fixed invalid error when trying to place additional objects on edges with length/geometry mismatch. Issue #8692
  - Fixed invalid restriction when trying to move additional objects on edges with length/geometry mismatch. Issue #8694
  - Fixed invalid rendering of detEntry and detExit direction. Issue #8693
  - Default edge permission in create edge mode are now working. Issue #8562
  - Fixed crash in tls mode after deleting lanes. Issue #8743
  - Fixed superfluous save-dialog after loading additional file. Issue #8572
  - Fixed crash when setting invalid edge in rerouter dialog. Issue #8859
  - Fixed hidden context menu near the screen border. Issue #8860

- netconvert
  - Fixed missing bus permissions in OSM import. Issue #8587
  - Fixed invalid junction shape at geometry-like junction with crossing (was causing invalid simulation behavior). Issue #8779
  - Fixed duplicate busStops when importing public transport lines from OSM. Issue #8791
  - Fixed missing turnaround when specifying edge-level connectivity in .con.xml. Issue #8796
  - Fixed missing pedestrian permissions when importing OSM. Issue #8782

- duarouter
  - Access cost is no longer ignored when using persontrip.transfer.car-walk=ptStops. Issue #8515
  - Fixed invalid error when using routing algorithm 'CH' in multimodal network. Issue #8756
  - Fixed invalid acceptance of broken input when using routing algorithm CHWrapper. Issue #8766
  - Fixed crash when loading stops on internal edge. Issue #8885

- netgenerate
  - Fixed bug when using option --remove-edges.explicit. Issue #8841

- traci
  - Call tracing is now reset properly on restart in the python client for libsumo. Issue #8671
  - Fixed sumo-gui crash when calling loadState. Issue #8698
  - Fixed crash when calling vehicle.moveTo. Issue #8714
  - Fixed missing follower information when calling 'traci.vehicle.getFollower' while on an internal edge. Issue #8701
  - Fixed crash when calling simulation.loadState while retrieving insertion backlog in sumo-gui. Issue #8730
  - Fixed deadlock after calling trafficlight.swapConstraints. Issue #8455
  - Libsumo now works with python3.8 on windows. Issue #5700
  - Libtraci now supports multiple traci client. Issue #8773
  - Function 'vehicle.replaceStop' now raises an error when trying to replace a stop that has already started. Issue #8878
  - Function 'vehicle.replaceStop' no longer ignores teleport flag if replacement stop is at the same location. Issue #8879
  - libsumo python wheel for python v3.7 is now compiled against the correct python version. Issue #8877
  - Fixed crash when calling libsumo.start again after after libsumo.close. Issue #8945
  - Fixed invalid return values from 'traci.lanearea' before the first simulation step. Issue #8949

- tools
  - sumolib function 'parse_fast_nested' can now (again) ignore intermediate child elements (i.e. to parse vehicle and route if the route is inside a routeDistribution element). Issue #8508 (regression in 1.9.2)
  - routeSampler.py: Fixed crashing when using **--geh-ok** or when setting more threads than intervals. Issue #8755
  - [generateRailSignalConstraints.py](Simulation/Railways.md#generaterailsignalconstraintspy)
     - Fixed crash when loading stops define by edge instead of lane. Issue #8937
     - No longer generates constraint where a train waits for itself. Issue #8935
     - Fixed invalid constraint signal id if a train reverses directly after stopping. Issue #8936
     - Fixed missing constraints when a train skips a stop. Issue #8943

   
### Enhancements

- simulation
  - CarFollowModel 'EIDM' (extended IDM) is now supported. Issue #8909 (Thanks to Dominik Salles)
  - ParkingAreas now support attribute 'departPos' to set a custom position for vehicles when exiting the parkingArea. Issue #8634
  - Added option **--save-state.period.keep INT** which allows saving state with constant space requirements (combined with option **--save-state.period**).
  - Added option **--persontrip.walk-opposite-factor FLOAT** which can be used to discourage walking against traffic flow (for FLOAT < 1). Issue #7730
  - Persons that walk against the flow of traffic now walk on the left side of the road rather than in the middle (left side of the lane instead of right side). Issue #7744
  - Vehicle stops now support attribute 'posLat' to stop with a lateral offset. Issue #8752.
  - Rail signals can now be switched into "moving block" mode where they only guard against flanking and oncoming trains. (option **--railsignal-moving-block** or `<param key="moving-block" value="true"/>`. Issue #8518
  - Vehroute-outupt now includdes attribute "priorEdgesLength" if option **--vehroute-output.stop-edges** is set. Issue #8815
  - Added option **--emission-output.step-scaled** to write emission output scaled by step length rather than per second. Issue #8866
  - Each distinct collision now creates exactly one warning message (even if the situation persists due to **--collision.action warn**). Issue #8918
  - Persons with different vClasses are now supported (i.e. 'ignoring' to walk on a forbidden road). Issue #8875
  - Element `<walk>` now supports attriubte 'departLane' (i.e. to place a person on the road lane instead of the sidewalk). Issue #8874
  - A warning is now issued if a person is configured to use a vType with the default vehicular vClass 'passenger'. Issue #8922
  - Attribute 'latAlignment' now supports numerical values to configure a fixed offset from the center line. Issue #8873
  
- netedit
  - Connection mode button 'Reset connections' now immediately recomputes connections at the affected junctions. Issue #8658
  - Add demand mode toggle button to show the shortest route for all trips. Issue #8638
  - Vehicle arrival position can now be modified in move mode. Issue #8543
  - When adding green verges via lane context menu, the target side can now be selected. Issue #8781
  - Pois along lanes now support attribute 'friendlyPos'. Issue #4161
  - Selection frame now has a 'delete objects' button. Issue #8911

- sumo-gui
  - Active insertionPredecessor constraints are now indicated via lane parameters. Issue #8737

- netconvert
  - Indirect (bicycled) turns can now be created by setting connection attribute 'indirect'. Issue #4252
  - Option **--default.spreadtype roadCenter** can now be used to improve the geometry of edges with different lane numbers per direction when importing OSM. Issue #8713
  - Option **--osm.bike-access** now enables the import of additional bike path tags such as bicycle=yes/no and oneway:bicycle=yes/no. Issue #8788
  - Tag foot=yes/no is now imported from OSM to adapt permissions for pedestrians. Issue #8788
  - Improved interpretation of OSM Input with missing 'lanes' attribute. Issue #8942

- duarouter
  - Attributes fromLonLat and toLonLat are now supported for personTrip. Issue #8665
  - Attributes 'x', 'y' and 'lon', 'lat' can now be used in place of stop attribute 'edge' and 'endPos'. Issue #8666
  - Added option **--persontrip.walk-opposite-factor FLOAT** which can be used to discourage walking against traffic flow (for FLOAT < 1). Issue #7730

- traci
  - Added function 'traci.vehicle.getTimeLoss' to retrieve the timeLoss since departure. Issue #8679
  - Added function 'traci.vehicle.setPreviousSpeed' to modify the speed assumed by Sumo during the prior step (i.e. for computation of possible acceleration). This can be combined with 'traci.vehicle.moveTo' to override the behavior in the previous step. Issue #7190  
  - Added new speed mode bit to control right-of-way compliance w.r.t. foe vehicles within an intersection. Issue #8675
  - 'traci.vehicle.moveToXY' and 'traci.person.moveToXY' now support optional parameter *matchThreshold* to configure the maximum distance between position and matched road (default 100m). #8668

- tools
  - [cutRoutes.py](Tools/Routes.md#cutroutespy) now handles vehicle attributes 'arrivalEdge' and 'departEdge'. Issue #8644  
  - Added new tool [stateReplay.py](Tools/Misc.md#statereplaypy) to visually observe a simulation that is running without gui (i.e. on a remote server).
  - [generateRailSignalConstraints.py](Simulation/Railways.md#generaterailsignalconstraintspy) can now make use of post-facto stop timing data (attribute 'started', 'ended'). Issue #8610
  - [generateRailSignalConstraints.py](Simulation/Railways.md#generaterailsignalconstraintspy) now generates insertionPredecessor constraint to fix the insertion order after a parking-stop. Issue #8736
  - Added netdiff.py option **--remove-plain** to automatically clean up temporary files. Issue #8712
  - [gtfs2pt.py](Tools/Import/GTFS.md) vTypes are now written as a separated output file. The name of the file can be defined with **--vtype-output**. Issue #8646
  - Added option **--dpi** to plot_summary.py and other plotting tools. Issue #8761
  - [plot_trajectories.py](Tools/Visualization.md#plot_trajectoriespy) now supports plotting by kilometrage (fcd-output.distance). Issue #8799
  - [drtOnline.py](Tools/Drt.md) now supports option **--max-processing** increase processing efficiency. Issue #8793
  - [flowrouter.py](Tools/Detector.md#flowrouterpy) now support output of pedestrian flows via option **--pedestrians**. Issue #8864
  

### Miscellaneous
- tools
  - Function sumolib.xml.parse now sets the attribute `heterogeneous=True` by default. This enables parsing of all attributes even not all elements have the same set of attributes. The value `heterogeneous=False` is useful for conserving memory when all elements are known to have the same set of attributes. Issue #8651
  - Now the tool [drtOnline.py](Tools/Drt.md) contains only the information for DRT scheduling (read fleet and requests, call a DARP solver and dispatch). To find the best routes, different methods can be used to solve the DialARideProblem (DARP), which are defined in the script "darpSolvers.py". Issue #8700

## Version 1.9.2 (18.05.2021)

### Bugfixes

- simulation
  - Vehroute output for persons now writes the correct stopping place type (i.e. parkingArea, busStop, etc.) Issue #8597

- sumo-gui
  - Coloring *by waitingTime* now uses value 0 for stopped persons, consistent with stopped vehicles. Issue #8585
  
- netedit
  - When adding stops to a trip, the route now changes as necessary to pass the stop location. Issue #7364
  - Person stages that end at a busStop can now be defined. Issue #6903
  - Flows and vehicles are now drawn on their configured departLane. Issue #7888
  - Fixed missing 'modes' menu entries in Demand- and Data-supermode. Issue #8486
  
- netconvert
  - Added automated check to prevent disconnected routes due to invalid lane-change permissions in OSM input. Issue #8603
  - Fixed invalid network output when setting **--ignore-change-restrictions ignoring**. Issue #8616
  - Fixed failure of **--tls.guess-signals** in lefthand network. Issue #8635
  
- marouter
  - Input attributes fromJunction and toJunction are now working. Issue #8631

- traci
  - Fixed crash when trying to read parameters for subscriptions that don't have them. Issue #8601 (regression in 1.9.1)
  
### Enhancements

- simulation
  - Vehroute-output now includes stop attributes 'started' and 'ended' and ride attribute 'ended' if option **--vehroute-output.exit-times** is set. Issue #8415, #8600
  - Added option **--use-stop-ended** to use the new stop attribute 'ended' instead of 'until'. Issue #8611

- netedit
  - Vehicle attributes departEdge and arrivalEdge are now supported. Issue #8452

- duarouter
  - Added option **--keep-route-probability** which lets a given proportion of vehicles keep their old routes (selected at random). Issue #8550

- tools
  - [duaIterate.py](Demand/Dynamic_User_Assignment.md#iterative_assignment_dynamic_user_equilibrium) now supports option **--convergence-steps** which forces route choices to converge in the given number of steps (via duarouter option **--keep-route-probability**). This is recommended when using option **--logit** which otherwise may not converge at all. Issue #8550
  - [countEdgeUsage.py](Tools/Routes.md#countedgeusagepy) now allows filtering and grouping counts by vehicle departure time. Issue #8621
  

## Version 1.9.1 (04.05.2021)

### Bugfixes
- Simulation
  - Statistic-output value departDelayWaiting is now in s (was in ms before).
  - Fixed emergency braking with high-duration continuous lane changing. Issue #8489 (regression in 1.9.0)
  - departDelayWaiting (verbose output and statistic-output) no longer includes loaded vehicles that were not scheduled to depart before simulation end. Issue #8490
  - Person stops with duration 0 are now working. Issue #8494
  - Fixed bug where vehicles could ignore connection permissions. Issue #8499
  - Fixed invalid jamming of persons and vehicles on a shared walkingarea. Issue #8417
  - Fixed invalid vehroute.exit-times after loading state. Issue #8536
  - Fixed missing vehicle-class-specific speed limits on internal lane. Issue #8556
  - Fixed bug where person did not exit ride on access edge of destination stop. Issue #8558
  - Fixed error when taxi ride starts or ends at a busStop only reachable via access. Issue #8578
  
- netconvert
  - Loaded road connections are no longer ignored when railway.topology.repair affects a junction. Issue #8505
  - Fixed invalid connections at ramp-like junctions with bike lanes. Issue #8538
  - Phases with identical states are no longer merged if their names differ. Issue #8544
  
- duarouter
  - Option **--weights.priority-factor** is no longer ignored in rail networks with bidirectional tracks. Issue #8561

- netedit
  - Restored polygon exaggeration so that it increases line width rather than growing the whole shape. Issue #8568 (regression in 1.7.0) 
  - Polygon ids can be shown again. Issue #8575 (regression in 1.7.0)
  - Setting size of additional ids is now working. Issue #8574 (regression in 1.8.0)
  - Ids of additional objects are now shown when zoomed out. Issue #8571

- TraCI
  - Fixed crash when calling traci.simulation.loadState. Issue #8477, #8511
  - Fixed invalid traceFile when using traci.<domain>.unsubscribe. Issue #8491
  - Added LiSum option **--lisa-version** to allow compatibility with version below 7.2. Issue #8065
  - Fixed LiSum crash. Issue #8104
  
- tools
  - osmWebWizard.py can now import locations with negative longitude again. Issue #8521 (regression in 1.9.0)
  - Fixed problem with sumolib.xml.parse_fast_nested when an element is missing some of the attributes to be parsed. Issue #8508
  - Fixed invalid characters in ptlines2flow.py output. Issue #8557
  - Fixed invalid constraint output of generateRailSignalConstraints.py when two vehicles stop with parking=true and the same until time at the same stop. Issue #8246

- Miscellaneous
  - Fixed invalid xml output when writing file creation timestamp in exotic locales. Issue #8533
  - Fixed proj.db error message when starting applications with geo-conversion capability (sumo, netconvert, polyconvert) on windows (commonly used functionality was not affected). Issue #8497 (regression in 1.9.0)

### Enhancements
- Simulation
  - Vehicles can now be equipped with the [glosa device](Simulation/GLOSA.md) to adapt their speed at traffic lights. Issue #7721
  - Verbose output now lists total time spent on TraCI when applicable. Issue #8478
  - Statistic-output now includes 'totalTravelTime' and 'totalDepartDelay'. Issue #8484
  - Added option **--tripinfo-output.write-undeparted** to ensure that the number of written tripinfos is independent of insertion success (simplify comparison of different runs). Issue #8475
  - Vehicles can now be [configured to ignore specific foe vehicles at junctions](Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#transient_parameters). Issue #8205
  - Option **--time-to-teleport.disconnect** now supports value 0 for teleporting without waiting. Issue #8537

- sumo-gui
  - Updated container count icon in status bar. Issue #8479

- netconvert
  - Added option **--junctions.higher-speed** to allow higher speed on internal lanes when the speed limit changes between incoming and outgoing edge. When set, speed on junction will be set to the maximum of both edges rather than the average. Issue #8535

- TraCI
  - Added traci.trafficlight functions to work with [railSignalConstraints](Simulation/Railways.md#schedule_constraints): getConstraints, getConstraintsByFoe, removeConstraints, swapConstraints. Issue #8455, #8224
  - traci.simulation.Stage now supports function 'toXML' for python client. Issue #8517

- tools
  - Added new tool [drtOnline.py](Tools/Drt.md) for simulation of demand responsive transport (DRT). It uses the [taxi API](Simulation/Taxi.md#traci) with an LP-Solver to optimize shared dispatch. Issue #8256
  - Added function sumolib.route.addInternal to interpolate internal edges into a route. Issue #1322
  - sumolib.net.getShortestPath now includes internal edges in path cost and supports parameter 'withInternal' for including internal edges in the resulting edge list edges#4994
  - [gtfs2pt.py](Tools/Import/GTFS.md) now supports option **--osm-routes** to improve mapping of gtfs data onto the network. Issue #8251
  - [edgeDataFromFlow](Tools/Detector.md#edgedatafromflowpy) now supports cadyts output. Issue #8516
  - [splitRouteFiles.py](Tools/Routes.md#splitroutefilespy) now handles detector file. Issue #8462
  - [routeSampler.py](Tools/Turns.md#routesamplerpy) now supports option **--pedestrians** to generate persons instead of vehicles. Issue #8523

### Other

- Miscellaneous
  - Fixed pip package contents. Issue #8513, #8514
  - You can use libsumo and libtraci as a dependency in your maven builds by the using package repository https://repo.eclipse.org/content/repositories/sumo-releases/. Issue #7921
- Documentation
  - Documented [statistic-output](Simulation/Output/StatisticOutput.md). Issue #8188

## Version 1.9.0 (13.04.2021)

### Bugfixes
- Simulation
   Option **--xml-validation.net** is now working again. Issue #8107 (regression in 1.5.0)
  - Fixed disappearing vehicle when transporting containers with taxi. Issue #7893
  - Fixed collisions between pedestrians and vehicles on shared space #7960
  - Vehicles with low (desired) decel value will no longer perform an emergency stop when caught in the "Yellow Light Dilemma Zone". Instead they will brake with decel **--tls.yellow.min-decel** (default: 3) as long as they have a sufficiently high emergencyDecel value. Issue #7956
  - Fixed invalid output directory for **--device.taxi.dispatch-algorithm.output**. Issue #8013
  - Fixed error when loading saved state with vehicles that stopped due to collision. Issue #8030, #8063 , #7696
  - Fixed bug where an emergency vehicle does not advance in the rescue lane. Issue #8072
  - When a junction collision is detected, the vehicle with right of way is now classified as the victim. Issue #8094
  - Fixed inconsistent vehicle positioning after collision stop #8109
  - Fixed slow initialization of railway router. Issue #8120
  - Fixed invalid braking of vehicles at traffic light junctions with crossings. Issue #8116
  - Fixed insufficient precision of timestamps when using low step-length (i.e. 0.025). Issue #8129
  - Fixed crash when using junction-taz in combination with taxi device. Issue #8152
  - Fixed emergency braking when approaching zipper link. Issue #8242
  - Members of personFlow and containerFlow are no longer in the same ride unless the group attribute is set. Issue #8396
  - Vehicle type attribute containerCapacity is now supported when using taxi device for container logistics #7892
  - Fixed invalid chargingStation-output and overheadwiresegments-output in subsecond simulation or when multiple vehicles are charging at the same element simultaneously. Issue #8351
  - Fixed invalid depart position when using departPos="stop" with a full parkingArea. Issue #8338
  - Fixed invalid waiting pedestrian count at busStop which caused invalid pedestrian jamming. Issue #8366
  - Fixed crash after junction collision with stoptime. Issue #8359  
  - Attribute departLane is no longer ignored when validating attribute departSpeed. Issue #8226
  - Persons and containers with a `<stop>` stage at a busStop are now assigned to that stop instead of just waiting on the edge. Issue #8436
  - Fixed invalid edgeData output if simulation begin is later than edgeData begin. Issue #8464
  - Sublane model fixes
    - Fixed deadlock in roundabout. Issue #7935
    - Fixed invalid deceleration at intersection due to misinterpreting lateral position of approaching foes #7925
    - Fixed collision in sublane model after parallel internal edges (requires rebuilding the network) #3619
    - Fixed invalid collision warning. Issue #8068
    - Fixed invalid vehicle angle in subsecond simulation. Issue #8070
    - Lateral deviation due to lcSigma > 0 is now independent of step-length when using `latAlignment="arbitrary"`. Issue #8154
    - Lateral acceleration no longer exceeds lcAccelLat. Issue #8195
  - Opposite direction driving fixes
    - Fixed undetected collisions. Issue #8082, #8111
    - Fixed invalid collision warning. Issue #8079
    - Fixed unsafe driving. Issue #8082, #8084, #8112, #8141
    - Fixed late change to the opposite side due to misidentification of oncoming vehicle #8080
    - Fixed premature arrival while on the opposite side. Issue #8199
    - Fixed Error when using continuous lane-changing with a looped route #8471

- meso
  - Fixed invalid jam-front back-propagation speed. Issue #8000 (Regression in 1.7.0)
  - Fixed invalid warning when using stop-output with multiple stops on the same segment. Issue #8001  
  
- sumo-gui
  - Fixed long pause on right-click in large networks. Issue #7927 (Regression in 1.4.0)
  - Routes for vehicles with dark color are no longer colored black. Issue #7934
  - Fixed crash when using network property dialog in meso. Issue #7998
  - Person drawing style "as circles" is now drawing circles as intended. Issue #8130
  - Fixed crash when opening person parameter dialog for a person with depart="triggered". Issue #8164
  - Default coloring now indicates lanes that allow rails and busses. #8315
  
- netedit
  - Fixed invalid E2 detector shape Issue #7895 (Regression in 1.7.0)
  - Fixed invalid rendering layer of polygons below roads. Issue #8316 (Regression in 1.7.0)
  - Access elements can only be placed on lane that permit pedestrian traffic. Issue #5893
  - Fixed issues when adding stops in person plan. #7829
  - Fixed issue where written network (with only invalid crossings) is modified upon reloading. Issue #7765
  - In Traffic Light mode, cycle time is now updated, when new phase is inserted. Issue #7961
  - Fixed problem with invisible data elements. Issue #7643 
  - Fixed problem with invisible vehicles when activating 'spread vehicles'. Issue #7931
  - Polygon and poi now have a visual indication when 'marked as front'. Issue #8331
  - Fixed bug when deleting or changing geometry-points (via dialog) in 3d-networks. Issue #8345
  - When polygons and other network elements overlap, clicks will not correctly go to the top element by default. Issue #8346
  - Fixed invalid "merging junctions" confirmation when moving a selection of junctions. Issue #8373
  
- netconvert
  - Fixed invalid signal plans in network with unusual geometry. Issue #7915
  - Option **--junctions.join-same** no longer fails due to numerical errors when comparing positions. Issue #8019
  - Fixed missing edges in ptline-output when those edges were created by option **--railway.topology.repair**. Issue #8024
  - Fixed invalid ptstop-output when using **--edges.join-tram-dist**. Issues #8035
  - Fixed invalid ptline output when stop edge is removed via option. Issue #8039
  - Fixed duplicate public transport stops when importing public transport lines from OSM. Issue #8060
  - Fixed missing trafficlights when combining options **--tls.discard-simple** and **--junctions.join**. Issue #8219
  - Fixed high running time when using option **--geometry.remove** on large networks. Issue #8270
  - Fixed crash when using option **--heightmap.shapefiles** with unsuitable shape data. Issue #8307
  - Fixed missing bus-permissions when importing OSM. Issue #8310, #8317
  - Option **--geometry.max-segment-length** now takes effect when importing from .edg.xml without edge attribute shape. Issue #8362
  - Fixed invalid z-data when importing geotiff. Issue #8364
  - Fixed invalid error when loading heightmap from geotiff with different color depths. Issue #8365
  - Fixed crash when loaded roundabouts are removed due to option  **--keep-edges.components**. Issue #8465
  
- polyconvert
  - POIs are now correctly imported from VISUM files. Issue #8414

- dfrouter
  - Input networks with pedestrian infrastructure no longer result in invalid output. Issue #8408

- od2trips
  - Fixed invalid begin and end times when writing personFlows. Issue #7885

- duarouter
  - Fixed invalid routes when using option **--remove-loops** with **--with-taz**. Issue #8451
  
- TraCI
  - Function 'vehicle.getSpeedWithoutTraCI' now returns original model speeds after calling moveToXY. Issue #7190
  - Fixed issues with mapping location and speed for function 'person.moveToXY' . Issue #7907, #7908
  - Fixed crash when switching to carFollowModel that requires vehicle variables. Issue #7949
  - Fixed crash when using traci.simulation.getDistanceRoad. Issue #5114
  - Fixed non-deterministic behavior for person.moveToXY. Issue #7933
  - Function vehicle.getNeighbors now correctly handles neighbors that changed lane after the ego vehicle in the same simulation step. #8119
  - Fixed [simpla](Simpla.md) crashes. Issue #8151, #8179
  - Fixed crash when starting traci with option traceFile, closing and starting again without traceFile. Issue #8177
  - Function 'trafficlight.setProgramLogic' new resets phase duration. Issue #2238
  - Function 'trafficlight.setPhaseDuration' now works for actuated traffic lights. Issue #1959
  - Route replacement with internal edge at the start of the edges list no longer causes an error. Issue #8231
  - Fixed failure to add stop when close to the stop position (but not quite too close). Also affected taxi re-dispatch. Issue #8285,#8398  
  - Looped taxi-dispatch now picks up persons in the intended order. Issue #8295
  - Fixed bug where traci.vehicle.dispatchTaxi could generate non-continuous routes. Issue #8424
  - Fixed crash after calling 'person.removeStage' on a riding stage. Issue #8305
  - Fixed crash after removing persons that have an open taxi reservation #8363
  - Fixed invalid traceFile output. Issue #8320, #8323
  - Fixed missing default vehicle types after calling simulation.loadState. Issue #8410
  - Fixed invalid error related to subscriptions after calling simulation.loadState. Issue #8426
  - Fixed memory leak when calling simulation.loadState. Issue #8450
  - Fixed invalid build dependencies for libsumo. Issue #8472
  
- Tools
  - Fixed error in xml2csv.py when loading files names consists only of numbers. Issue #7910
  - Fixed invalid routes when [importing MATSim plans](Tools/Import/MATSim.md) #7948  
  - randomTrips.py now generates multi-stage plans when combining option **--intermediate** with options that generated persons (i.e. **--persontrips**). Issue #8273
  - Fixed duaIterate.py crash when loading multiple route files with particular names. Issue #8411
  
### Enhancements
- Simulation
  - Sublane model can now be used together with overtaking through the opposite direction. Issue #1997
  - Added stop attribute 'permitted' to restrict persons and containers that can enter vehicle at stop #7869
  - In tripinfo-output, the access stage of a person plan now includes attributes depart and arrival. Issue #7822
  - Detectors for actuated traffic lights can now be selectively disabled by setting the special id 'NO_DETECTOR' for a lane. #7919
  - Setting vehicle attribute `arrivalLane="random"` and `"first"` is now supported. Issue #7932
  - Added new option **--collision-output** to write information on collisions to an XML file. Issue #7990.
  - Actuated traffic lights based on detector gaps now support [custom detection gaps per lane](Simulation/Traffic_Lights.md#lane-specific_max-gap). Issue #7997
  - Improved computational efficiency of bluelight device. Issue #7206
  - Added option **--save-state.precision** to configure the numerical precision when saving simulation state. Issue #8115
  - busStop attribute personCapacity can now cause pedestrian jams when the busStop is filled to capacity. Issue #3706
  - Taxi device now supports option **--device.taxi.idle-algorithm** [stop|randomCircling] to control the behavior of idle taxis. #8132
  - The sublane model now supports modeling an inverse relation between longitudinal and lateral speed (higher lateral speed while stopped and lower while driving fast). This is achieved by setting a negative values for attribute 'lcMaxSpeedLatFactor' and by setting 'lcMaxSpeedLatStanding' > 'maxSpeedLat'. #8064
  - Added new vType attribute 'jmIgnoreJunctionFoeProb' to allow ignoring foes (vehicles and pedestrians) that are already on the junction. Issue #8078
  - Added option **--device.ssm.filter-edges.input-file** to filter ssm device output by location. Issue #7398
  - Added vehicle attribute 'arrivalEdge' which can be used to set an arrival edge index ahead of the last edge of it's route. Issue #7609
  - Connection attribute 'visibility' now controls the distance for zipper merge related speed adjustments (default 100m). Issue #8240
  - Added option **--fcd-output.attributes** to set the list of attributes which are included in fcd-output. Issue #7632
  - fcd-output can now distinguish riding and walking persons by adding 'vehicle' to the option **--fcd-output.attributes**. Issue #7631
  - Added option **time-to-teleport.disconnected** which is applied when teleporting vehicles on fully disconnected routes. Issue #8267
  - Now supporting element `<containerFlow>` to define multiple containers.
  - Added option **--save-state.constraints** to include constraints in saved simulation state. Issue #8337
  - PersonFlow now supports attribute `begin="triggered" number="..."` to start a number of persons in the same vehicle. Issue #8165
  - Added option **--device.battery.track-fuel** to track fuel level for other fuels besides electricity #8349
  - Added option **--collision.check-junctions.mingap** to increase/decrease the sensitivity of junction-collision checks. Issue #8312
  - Added option **--stop-output.write-unfinished** to write stops that are not ended at simulation end. Issue #8401
  - Vehicle Class specific speed limits are now interpolated onto junctions. #8380
  - Option **--vehroute-output.exit-times** now applies to walks #8415
  
- sumo-gui
  - Random color for containers is now supported. Issue #7941
  - Added 'Update' button to object selection dialogs to refresh the object list. Issue #7942
  - Multiple gui setting schemes can now be loaded from the same input file. Issue #8012
  - Add vehicle drawing style 'draw as circles'. Issue #5947
  - BusStop parameter dialog now includes a summary of lines which are being waited for. Issue #8138
  - Background images can now be removed using the 'Clear Decals' button. Issue #8144
  - Vehicle lengths will now be scaled according to [custom edge lengths](Simulation/Distances.md) to avoid confusing visual overlap. A new vehicle visualization setting checkbox 'scale length with geometry' is provided to disable scaling. Issue #6920
  - Asymmetrical lane-change restrictions are now indicated by a combination of broken and unbroken divider lines. Issue #3656
  - Improved visualization of containers waiting at containerStop. Issue #8348
  

- netedit
  - Added file menu options 'reload additionals' and 'reload demand'. Issue #6099
  - Route attributes 'repeat' and 'cycleTime' are now supported. Issue #6655
  - Added "Save All" option. Issue #6357
  - In move mode, exact coordinates for a geometry point can be set via right-click menu. Issue #2500
  - Edges can now be selected via context menu. Issue #4549
  - Selected edges and junctions can now be moved in z-direction (absolutely or relatively) via move mode frame controls. Issue #2499
  - Shapes of selected edges can now be shifted orthogonally to their driving direction via move mode frame controls. Issue #2456
  - Polygons can now be moved without changing their shape (with new move mode checkbox). Issue #5268
  - New custom cursors added to the **Inspect**, **Delete**, **Select** and **Move** modes. Issue #4818
  - Added new top-level 'Modes' menu for selecting edit mode. All mode-specific toggle options are now included in the 'Edit'-menu  #8059
  - ParkingArea roadsideCapacity slots are now visible. Issue #6982
  - Configuration dialog for rerouter, calibrator and variableSpeedSign can now be accessed from the inspection frame. #8215
  - Lane attribute 'type' can now be edited. Issue #8230

- netconvert
  - Lanes and connections now support attributes 'changeLeft' and 'changeRight' to configure lane-change restrictions. Issue #3656
  - Lane changing restrictions are now imported from OSM. To achieve legacy behavior, set option **--ignore-change-restrictions all**. Issue #8221
  - Added option **--tls.no-mixed** which prevents building phases where different connections from the same lane have green and red signals. Issue #7821
  - Element `<laneType>` is now supported in an edge `<type>` to pre-configure speed, width and permissions for individual lanes. Issue #7791
  - Merging of overlapping geo-referenced networks with re-occuring ids now works without setting option **--ignore-errors**. Issue #8019
  - When using option **--junctions.join-same** to merge overlapping networks with different junction ids, the option **--edges.join** can now be used to automatically remove duplicate edges. Issue #8019
  - Added option **--railway.topology.repair.minimal**. This works similar to **--railway.topology.repair** but avoids creating bidirectional tracks that are not needed for public transport routes (only applies when using option **--ptline-outut**). Issue #7982
  - Public transport edges that are disconnected from the main road network (in particular railways) are now included in the output when using option **--keep.edges.components 1** as long as they have public transport stops that are written via option **--ptstop-output**. Issue #8061
  - Edge types now support attribute 'spreadType'. Issue #7897
  - The behavior of option **--geometry.remove** (merging subsequent edges with common attributes) no longer depends on written **--ptstop-output** (stops will be remapped onto merged edges). To enable legacy behavior, the option **--geometry.remove.keep-ptstops** may be set. Issue #8155
  - Connection file element `<walkingArea>` now supports attribute 'width' #7968
  - Lane attribute type is now written in OpenDRIVE output. Issue #8229
  - Added option **--default.allow** to set default edge permissions (also applies to netgenerate). Issue #8271
  - Added option **--osm.extra-attributes** to import additional edge attributes (osm tags) as generic parameters. Issue #8322
  - Added option **--osm.lane-access** for import of extra lane access permissions (experimental). Issue #7601
  - Option **--output.street-names** is now working for  VISUM network. Issue #8418
  - MatSim import can now split self loops and handle "half" lanes. Issue #8047, #7947
  
- netgenerate
  - Relaxed restrictions on minimum edge lengths when building grid and spider networks. Issue #8272

- marouter
  - Added option **--ignore-taz** to route individual trips written by [od2trips](od2trips.md) between their assigned edges. Issue #8343
  - Option **--scale** is now applied when loading route files instead of od-matrices. Issue #8352
  
- od2trips
  - Added option **--junctions** which interprets the loaded source and sink IDs as fromJunction and toJunction ids. #8389

- dfrouter
  - Added option **--vclass** to filter eligible edges in a multi-modal network. Issue #8408
  
- polyconvert
  - Added option **--visum.language-file** to support importing shapes from English and French language versions (default is German). Issue #8414

- Routing (duarouter / sumo)
  - The landmark table format changed to a more compressed version (old tables are still readable). Issue #8153
  - Gzipped landmark tables can now be read as well. Issue #8153

- TraCI
  - [libtraci](Libtraci.md) is now the "officially supported" library for the Java, Python and C++ TraCI clients, providing (functional) identical interfaces for all three languages. TraaS and the old C++ TraCI API are considered deprecated.
  - Added function 'traci.simulation.getCollisions' to retrieve a list of collision objects for the current time step. This also includes collisions between vehicles and pedestrians. Issue #7728
  - Can now retrieve vehicle parameters 'device.battery.totalEnergyConsumed' and 'device.battery.totalEnergyRegenerated' when a vehicle has the battery device. Issue #6507
  - vehicle.dispatchTaxi now supports re-dispatching a taxi that is already in pickup or occupied mode. Issue #8148
  - Vehicles that are accumulating insertion delay (because they cannot safely enter the network as schedule) can now be retrieved using the functions 'traci.simulation.getPendingVehicles', 'traci.edge.getPendingVehicles' and 'traci.lane.getPendingVehicles. Issue #8157
  - Taxi customers (including those that shall be picked up but are not yet on board) can now be retrieved using `traci.vehicle.getParameter(vehID, "device.taxi.currentCustomers")`. Issue #8189
  - The reservation objects returned by [traci.person.getTaxiReservations](Simulation/Taxi.md#gettaxireservations) now includes persons that are eligible for re-dispatch and includes the state of the reservation (new, assigned, on board). Issue #8168
  - Added function 'traci.person.splitTaxiReservation' to transport pre-made groups with multiple vehicles. Issue #8236
  - The domains 'simulation', 'junction', 'inductionloop', 'lanearea', 'multientryexit' now support setParameter and getParameter. Issue #4733, #8244
  - The value set by `traci.vehicle.setParameter("lcReason", VALUE)` will now be appended to lanechange-output. Issue #8297
  - Function 'traci.vehicle.replaceStop' now supports deleting stops and optionally teleporting to the replacement stop. Issue #8265

- Tools
  - Added [new tools](Tools/Import/GTFS.md) to support GTFS import. Issue #4596
  - The tool [gridDistricts.py](Tools/District.md#griddistrictspy) can be used to generated a grid of districts (TAZs) for a given network. #7946
  - [netcheck.py](Tools/Net.md#netcheckpy) now supports option **--print-types** to analyze the edge types of the different network components. Issue #8097
  - The tool [generateRailSignalConstraints.py](Simulation/Railways.md#generaterailsignalconstraintspy) can now handle inconsistent schedule input without generating deadlocking constraints when setting option **--abort-unordered**. Issue #7436, #8246, #8278
  - The tool [generateRailSignalConstraints.py](Simulation/Railways.md#generaterailsignalconstraintspy) can now generate redundant constraints by setting option **--redundant TIME**. Issue #8456
  - When loading additional weights in for [duaIterate.py](Demand/Dynamic_User_Assignment.md#iterative_assignment_dynamic_user_equilibrium), the new option **--addweights.once** controls whether the weights are to be effective in every iteration or not. The new default is to apply them in every iteration whereas previously, they were applied only in the first iteration. Issue #8249
  - Added new tool [splitRandom.py](Tools/Routes.md#splitrandompy). to split route files into random subsets. Issue #8324
  - Added new tool [changeAttribute.py](Tools/Xml.md#changeattributepy) to add/change/remove xml attributes. Issue #8339
  - Added new tool [computeStoppingPlaceUsage.py](Tools/Output.md#computestoppingplaceusagepy) to compute running occupancy of parking ares from stop-output. Issue #8405
  - [traceExporter.py](Tools/TraceExporter.md) now supports SSAM trajectories. Issue #8051

### Other

- Miscellaneous
  - Fixed "Error: Cannot get file attributes" when loading large files on Windows and macOS. Issue #6620
  - Clang build now works on Windows (Visual Studio) Issue #8123
  - Python3 is now the default and recommended python Issue #5700
  - The released windows binaries are now built using Visual Studio 2019
  - XML parsing can now handle https references to schemas on Windows

- netedit
  - When using rectangle selection, junctions of selected edges are now added to the selection by default. Issue #8406

- netconvert
  - Parallel turn lanes are no longer written as distinct edges but are instead written as multi-lane edge with different lane lengths. As before, lane-changing on an intersection is not permitted on a turn lane. Issue #7954
  - Written network version is now 1.9.0

- Documentation
  - Added [public transport tutorial](Tutorials/PublicTransport.md) (without web wizard). Issue #8108
  
- Tools
  - Some obsolete tools were moved to tools/purgatory (let us know if you were using them). Issue #1425
  - The following tools were renamed/relocated: (issue #1425)
    - tools/showDepartsAndArrivalsPerEdge.py -> tools/countEdgeUsage.py
    - tools/averageRuns.py -> tools/averageTripStatistics.py
    - tools/shapes/debug2shapes.py -> tools/devel/debug2shapes.py
    - tools/build/timing.py -> tools/devel/timing.py
    - tools/build/rebuild_cscope.sh -> tools/devel/rebuild_cscope.sh
    - tools/xml/schemaCheck.py -> tools/build/schemaCheck.py
    - tools/xml/rebuildSchemata.py -> tools/build/rebuildSchemata.py
    - tools/assign/networkStatistics.py -> tools/output/tripStatistics.py
    - tools/osmTaxiStop.py -> tools/import/osm/osmTaxiStop.py
    - tools/addParkingAreaStops2Routes.py -> tools/route/addParkingAreaStops2Routes.py
    - tools/addStops2Routes.py -> tools/route/addStops2Routes.py
    - tools/route2sel.py -> tools/route/route2sel.py
    - tools/splitRouteFiles.py -> tools/route/splitRouteFiles.py
    - tools/tls_buildTransitions.py → tools/tls/buildTransitions.py
    - tools/generateTurnRatios.py -> tools/turn-defs/generateTurnRatios.py
    - tools/assign/matrixDailyToHourly.py -> tools/district/aggregateAndSplitMatrices.py
  - randomTrips.py option **-c** is no longer a shortcut for option **--vclass**. Instead it is used to load a configuration file (written via **-C**). Issue #8409
  - osmBuild.py option **-c** is no longer a shortcut for option **--vehicle-classes**. Instead it is used to load a configuration file (written via **-C**). Issue #8409

## Version 1.8.0 (02.12.2020)

### Bugfixes
- simulation      
  - Fixed automatic ride sharing for personTrips between the same origin and destination that did not declare a common 'group'. By default, rides are not shared. Automatic ride sharing for personTrips and rides can be enabled by setting option **--persontrip.default.group STR** to an arbitrary value. Issue #7559, #7560  
  - Fixed bug where leader vehicles were ignored during lane-changing when using the sublane mode. Issue #7614
  - Fixed invalid conflict check for vehicles on the same intersection. Issue #7618, #7173, #7825, #7840
  - Fixed failure to create a rescue lane. Issue #7173  
  - Parallel intermodal routing now respects the option **--routing-algorithm**. Issue #7628
  - Fixed oscillating lane changes in roundabout with more than 2 lanes. Issue #7738    
  - Planned vehicle stops are no longer included in edgeData waitingTime output. Issue #7748
  - Option **--ignore-junction-blocker** is now working again. Issue #7650 (Regression in 1.0.0)
  - Fixed "jumping" persons when transfering from car to walking at junction. Issue #7778   
  - Fixed unsafe insertion speed for IDM which was causing emergency braking #7786  
  - When using the sublane model, vehicles will now consider the travel speed on lanes beyond their current neighboring lanes for tactical lane changing. Issue #7620
  - Emergency vehicles that are using a rescue lane can now pass an intersection even if the necessary turning lane is blocked. Issue #7619
  - Fixed invalid negative space and time gap in ssm device output. Issue #7844
  - Now saving output of ssm device relative to the configuration file by default. Issue #7847
  - Fixed invalid speed and acceleration for parked vehicle. Issue #7850
  - Fixed negative timeLoss in edgeData output. Issue #7805
  - battery model now works correctly with subseconds and handles additional power intake better. Issue #7695
  - crashing
    - Fixed crash on parallel intermodal routing. Issue #7627
    - Fixe crash when defining stops on internal edges. Issue #7690
    - Fixed crash when using meanData attribute 'trackVehicles=True' in a pedestrian simulation. Issue #7664
    - Fixed crash when vehicle with ssm device is teleported. Issue #7753  
    - Fixed crash on invalid route input. Issue #7801  
  - state saving/loading fixes
    - Fixed crash when loading simulation state that includes a routeDistribution along with a route file that includes the same distribution. Issue #7539
    - Fixed invalid active-vehicle count after loading state. Issue #7583
    - Routing device is now properly assigned when loading departed vehicles from a saved state. Issue #7810
    - Lanes of the vehicle rear end are now properly restored after loading state. Issue #7828
    - Fixed invalid acceleration after loading state. Issue #7701
    - Fixed invalid odometer value after loading state. Issue #7827
    - Fixed invalid reroute count after loading state. Issue #7811
    - Fixed accumulated waitingtime after loading state. Issue #7657
    - Fixed invalid randomness when loading state saved with option **--save-state.rng**. Issue #7731
  - railway fixes
    - Fixed unwanted influence by stopped trains on insertion and rail signal operation. Issue #7527, #7529 (regression in 1.7.0)
    - Fixed train collision due to unsafe rail signal state. Issue #7534
    - Fixed unsafe train insertion. Issue #7579
    - Rail signal constraints now work correctly after loading simulation state. Issue #7523, #7673
    - Fixed deadlock after loading state. Issue #7837
  - taxi fixes
    - Fixed bug with non-deterministic behavior and aborted rides if person capacity at a busStop was exceeded. Issue #7674, #7662
    - Intermodal routing no longer returns plans that frequently alternate between taxi and walking. The assumed time penalty when starting a taxi ride is now set to 300s configurable with option **--persontrip.taxi.waiting-time**. Issue #7651
    - Grouped rides are now serviced in batches when the group size exceeds maximum taxi personCapacity. Issue #7660  
    
- meso
  - Fixed bug in queue selection which could severely reduce flow at tls controlled intersection with turn lanes. Issue #7794 (regression in 1.7.0)
  - Fixed handling of cyclical stops. Issue #7500
  - Fixed bug where persons would enter a stopped vehicle at the wrong stop. Issue #7866
  - Option **--meso-minor-penalty** is no longer applied on top of **--meso-tls-penalty**. Issue #7802
  - Fixed delayed insertion on on traffic light edge when option **--meso-tls-penalty** is used. Issue #7875
  - Passengers that leave a vehicle are now counted in **--stop-output**. Issue #7865


- netconvert
  - Fixed unsafe traffic light signals when two connections from the same edge target the same lane. Issue #7550
  - Fixed missing right-turn connections from bicycle lanes to mixed traffic road. Issue #7548  
  - Road lanes that prohibit bicycles no longer receive bicycle permissions when a bicycle lane ends. Instead the bicycle lane receives a double-connection to allowed target edges where needed. Issue #2350
  - Fixed invalid right of way for right-turning double-connections at intersections with one incoming road. The rightmost lane now gets priority. Issue #7552
  - Fixed inconsistent OSM-typemap that artificially restricted bicycle driving direction on edge type 'highway.path'. Issue #7615
  - Fixed unsafe intersection rules for double connection with internal junction. Issue #7622
  - Option **--geometry.remove** now longer causes streets with different names to be joined. Issue #637
  - Loading public transport stops with 'access' via option --ptstop-files and removing the stop edge via options is now working. Issue #7658
  - Fixed unsafe position of internal junctions on large junctions with multiple turning connections. Issue #7761
  - Two-lane side roads entering a priority junction now create a left-turn lane instead of a right-turn lane in most cases. Issue #7754
  - Fixed building of unsafe right-of-way rules at traffic light junctions where the highest priority road makes a turn. Issue #7764  
  
- netedit
  - Visual scaling of selected objects (via selection frame) is now working for junctions (regression in 1.7.0). Issue #7571
  - Crossings no longer disappear when changing lane count of crossed edges. Issue #7626
  - Fixed object centering after filtering by substring in object locator. Issue #6361
  - Fixed very slow operation when loading large edgeData sets. Issue #7604
  - Fixed automatic color calibration when coloring edges 'by permission'. Issue #5852
  - Boolean attributes of vehicle stops are now working. Issue #7666
  - Fixed dotted contour of inspected edges in left-hand networks. Issue #7675 (regression in 1.7.0)
  - Fixed dotted contour of bidi rail edges in spread mode. Issue #7569
  - Fixed text angle of name attribute for additional network objects. Issue #6516
  - Flow and stack labels now scale with vehicle exaggeration. Issue #6541
  - Inspecting tls-controlled crossings now always shows the corresponding tls link index. Issue #7747
  - Link indices of connections and crossings can now be reset to default using value '-1'. Issue #4540
  - Fixed invalid warning about unsaved data. Issue #5971
  - Traffic light frame button 'clean states' now properly cleans up last indices #7781
  
- sumo-gui
  - Rail carriages are now drawn next to the track when at a stop with parking=true. Issue #7528
  - Fixed invalid simulation end after reloading. Issue #7582
  - Prevented opening of multiple locator windows for the same object type. Issue #6916
  - Fixed crash when locating vehicles by name while the simulation is running. Issue #7768
  - Fixed crash when loading a network with internal lanes using mesosim and disabling junction shape rendering. Issue #7857 (regression in 1.6.0)
  
- duarouter
  - Fixed crash on parallel intermodal routing. Issue #7627
  - Parallel intermodal routing now respects the option **--routing-algorithm**. Issue #7628
  - Fixed NaN value int output when using option **--logit**. Issue #7621
  - Fixed invalid intermodal plans where switching between riding and walking happened on intersection. Issue #7652
  - Fixed invalid intermodal plans when walk ends at trainStation and an access element is required to reach the starting edge of the ride. Issue #7654
  
- TraCI
  - Fixed crash when using moveToXY in an intermodal network. Issue #7763 (Regression in 1.7.0)
  - arrivalPos is no longer ignored in *traci.vehicle.add*. Issue #7691
  - Function *traci.vehicle.getStopDelay* now returns correct estimates for [waypoints](Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#waypoints). Issue #7752  
  - The server side socket is now closed earlier to avoid race conditions when starting many simulations. Issue #7750
  
- Tools
  - osmWebWizard search now works for IE users. Issue #6119
  - batch files generated by osmWebWizard on linux can now be used on windows. Issue #7667  
  - Tools that support option **-C** for saving their configuration now use proper xml-escaping for their option values. Issue #7633
  - [routeSampler.py](Tools/Turns.md#routesaSimulation/Meso.html#configuration_by_edge_typempler.py) no longer includes routes which do not pass any counting location when using option **--optimize**. This also speeds up execution.Issue #6723

### Enhancements
- simulation
  - Added vehicle attribute 'departEdge' which can be used to [insert a vehicle beyond the first edge of its route](Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#departedge). Issue #1129
  - Added new input elements that can be loaded to influence rail signal operation by providing constraints on train ordering. Issue #7435
  - Train waiting time is now taken into account when resolving conflicts between otherwise equal trains. Issue #7598  
  - edgeData output now includes attribute timeLoss. Issue #1396
  - Pedestrian simulation will no longer deadlock on narrow sidewalks (< 1.28m) Issue #7746
  - Person journey can now include transfer from walking-only edge to car-only edge at junction #7779
  - Option **--ride.stop-tolerance** now applies to all kinds of stops. #6204
  - Added option **--fcd-output.max-leader-distance** which will add attributes `leaderGap, leaderSpeed, leaderID` to fcd-output whenever a vehicle has a leader within the given distance #7788
  - Stop for vehicles and persons can now be specified using attribute 'edge' instead of 'lane'. Vehicles will stop on the rightmost lane that allows their vehicle class (whereas persons ignored the lane index anyway). Issue #5443
  - Taxi device can now be used to simulate on-demand container transport. Issue #7815
  - Option **--fcd-output.params** now supports optional output of device parameters and model parameters with the same [prefix codes as TraCI](TraCI/Vehicle_Value_Retrieval.md#device_and_lanechangemodel_parameter_retrieval_0x7e). Issue #7851
  - Option **--fcd-output.params caccVehicleMode** now outputs the vehicle mode of CACC vehicles, which can be one of `CC`, `ACC`, `CACC`, `CACC_GAP_CL` (gap-closing), `CACC_GAP`, `CACC_CA` (collision avoidance). Issue #6700
  
- meso
  - Model parameters can now be [customized](Simulation/Meso.md#configuration_by_edge_type) for each edge type via additional file input. Issue #7243
  - Stop handling is now similar to microsimulation, pedestrians walk to the correct location and are picked up there, stopinfo is written
  
- netedit
  - Create edge mode now allows selecting from edge types and to inspect/edit edge attributes. Issue #2431
  - Opposite direction lanes can now be edited and selected. Issue #2653
  - Edges can now be split while in 'create edge mode' by shift-clicking. Issue #6754
  - [Generic parameters](Simulation/GenericParameters.md) of vTypes can now be edited. Issue #7646
  - Edge operation 'restore geometry endpoint' can now be applied to edge selections. Issue #7576
  - Junction attribute 'tlLayout' can now be edited. Issue #7812
  - Edit mode specific options are now buttons instead of checkboxes to better fit on the screen. Issue #6498
  
- sumo-gui
  - Rail signal now includes internal state (reason for red) in parameter dialog. Issue #7600
  - Added option **--delay** (shortcut **-d**) to set the simulation delay. Issue #6380
  - Vehicles can now be colored by arrival delay at public transport stops
  - Centering on a vehicle in meso is now accurate. Issue #7871
  
- netconvert
  - Added option **--junctions.join-same** which joins junctions with identical coordinates regardless of edge topology. This is useful when merging networks. Issue #7567
  - Added option **--dlr-navteq.keep-length** to make use of explicit edge lengths in the input. Issue #749
  
- duarouter
  - Added option **--persontrip.taxi.waiting-time** to model the time penalty invovled when calling a taxi. Issue #7651
 
- od2trips
  - Attributes 'fromTaz' and 'toTaz' are now written for walks and personTrips. Issue #7591

- TraCI
  - Added new function *vehicle.getStopArrivalDelay* to return the arrivalDelay at a public transport stop that defines the expected 'arrival'. Issue #7629
  - Added new functions for railway simulation to investigate why a rail signal is red: *trafficlight.getBlockingVehicles, trafficlight.getRivalVehicles, trafficlight.getPriorityVehicles'. Issue #7019
  - Function 'simulation.findIntermodalRoute' now supports mode=taxi. Issue #7757
  - Functon vehicle.moveToXY' now only maps onto lanes compatible with the vClass of the vehicle. To ignore permissions, bit2 of the keepRoute flag can be set. Issue #5383
  - Added [API for coupling taxi dispatch algorithms](Simulation/Taxi.md#traci). Issue #6526

- Tools
  - Added [randomTrips.py](Tools/Trip.md) option **--via-edge-types**. When this option is set to a list of types, edges of this type are not used for departure or arrival unless they are on the fringe. This can be used to prevent departure on the middle of a motorway. Issue #7505
  - Added new tool [generateRailSignalConstraints.py](Simulation/Railways.md#generaterailsignalconstraintspy) to generated constraint input for rain signals from a route file. Issue #7436
  - Added [traceExporter.py](Tools/TraceExporter.md) option **--shift** which allows shifting output coordinates by a fixed amount (i.e. to avoid negative values).
   Added [traceExporter.py](Tools/TraceExporter.md) now supports option **--ipg-output** for generating car-maker tracefiles. Issue #6190
  - [routeSampler.py](Tools/Turns.md#routesampler.py) now supports option **--minimize-vehicles FLOAT** which allows configuring a preference for fewer generated vehicles (where each vehicle passes multiple counting locations). Issue #7635
  - Added new tool [osmTaxiStop.py]() to import taxi stands from OSM data. Issue #7729
  - Added new tool [checkStopOrder.py](Tools/Routes.md#checkstoporderpy) to detect inconsistent times in a public transport schedule. Issue #7458
  - added osmWebWizard option **--output** to set a custom output directory. Issue #7672

### Other
- Changed osmWebWizard default for **--device.rerouting.adaptation-interval** from 1 to 10 to increase performance. The value of **--adaptation-steps** was reduced from 180 to 18 so that the time over which speeds are averaged remains at 180s (with fewer samples). Issue #7640
- [macOS launchers](Downloads.md#application_launchers) added for **sumo-gui**, **netedit** and the **osmWebWizard**. This allows to set **sumo-gui** as the default application to open `.sumocfg` files (to open a simulation by just double clicking a file) on macOS.
- extractTest.py now extracts tests for all variants (sumo, meso) which have specific outputs and got a lot better picking up all options


## Version 1.7.0 (09.09.2020)

### Bugfixes
- Simulation
  - When using the options **--vehroute-output.write-unfinished --vehroute-output.exit-times** all edges of the route are now included (instead of passed edges only) and the exit times for unfinished edges are written as '-1'. Issue #6956
  - Fixed invalid error when using route attribute 'repeat'. Issue #7036
  - Fixed invalid error when using option **--scale** and vehicles are using attribute `depart="triggered"`. Issue #6790
  - Closed roads with changed permissions are no longer ignored by the first loaded vehicles. Issue #6999  
  - Fixed invalid amount of charged energy in subsecond simulation when using battery device. Issue #7074
  - personTrips now support attribute fromTaz. Issue #7092
  - Fixed invalid error when setting vehicle stop. Issue #7059
  - Fixed bugs that were causing collisions. Issue #7131, #7136, #7154
  - Fixed deadlock on intersection. Issue #7139
  - Fixed bug where vehicles at the stop line would fail give way to an emergency vehicle. Issue #7134
  - Fixed invalid tripinfo output when only part of the fleet is generating output. Issue #7141
  - Fixed bugs where pedestrians would step onto the road when it wasn't safe. Issue #7150, #7152
  - Fixed pedestrian routing bug after riding a ship. Issue #7149
  - Fixed invalid insertion lane when using option **--extrapolate-departpos**. Issue #7155
  - Simulation now terminates when specifying a flow without end and attribute 'number'. Issue #7156
  - Fixed issues with rescue-lane creation. Issue #7134
  - Fixed sublane model issues #7181, #7182, #3681
  - Random departPos for personFlow is now working. Issue #7210
  - CarFollowModel 'KraussPS' no longer stalls at steep inclines. Issue #2849
  - Fixed discontinuous acceleration profile and unsafe driving when passing a minor link. Issue #7213
  - Fixed unnecessary waiting at roundabout. Issue #1847
  - Fixed issue where persons would prefer long walks over quick taxi rides. Issue #7216
  - Fixed invalid followSpeed computation in carFollowModel 'W99'. Issue #7229
  - Fixed failing lane-changes when using carFollowModel 'W99'. Issue #7211
  - Fixed invalid SGAP and TGAP output from device.ssm when vehicles have different minGap values. Issue #7233
  - Fixed invali error when doing parallel routing with algorithm 'astar'. Issue #7248
  - Fixed invalid taxi states when two customers exit on the same edge. Issue #7257
  - Fixed crash when using routing algorithm CH with taxis. Issue #7256
  - Person rides with a specified arrivalPos no longer exit their vehicle prematurely. Issue #7258
  - Person ride arrivalPos and stoppingPlace are now included in vehroute-output. Issue #7260, #7261
  - Fixed invalid vehicle angle when using option **--lanechange-duration** with low values of 'maxSpeedLat'. Issue #7263
  - Fixed bug where vehicles would stop at [waypoints](Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#waypoints). Issue #7377, #7419
  - Fixed crash when loading railway network with unregulated junctions. Issue #7421
  - Fixed invalid rail_signal state after loading simulation state. Issue #7451
  - Fixed railway collision on bidirectional track. Issue #7462
  - Fixed railway emergency braking. Issue #7487
  - Fixed missing vehicle line information after loading state. Issue #7469
  - Fixed crash when using bluelight device. Issue #7473
  - Vehicles no longer get stuck when setting 'maxSpeedLatStanding=0'. Issue #3776, #6106, #7474
  - Sorted vehroute output now works when ignoring public transport lines. Issue #7409
      
- Meso
  - Fixed invalid jamming when a long vehicle passes a short edge. Issue #7035
  - Option **--fcd-output.distance** now also applies to Meso. Issue #7069  
  - Fixed invalid exit times in vehroute-output
  - Fixed invalid odometer value. Issue #7380
  - Fixed person arrivalPos value. Issue #7381
  
- netconvert
  - Fixed crash when patching a network with '.tll.xml' file. Issue #7014 (Regression in 1.6.0)
  - Fixed invalid results/crashing when splitting an edge within a joined traffic light. Issue #7017
  - Fixed missing traffic lights when using **--tls.guess-signals** together with **--junctions.join**. Issue #6977
  - Fixed missing busStops when importing public transport lines from OSM. Issue #6963
  - Fixed invalid error when patching network with netdiff patches. Issue #6981
  - Fixed invalid offset when using option **--tls.quarter-offset**. Issue #6988
  - Option **--geometry.avoid-overlap** is no longer lost on reloading a network. Issue #7033  
  - Fixed infinite loop when loading OSM data with 'NaN' values. Issue #7101
  - Fixed invalid 'neigh' lanes when editing network. Issue #7108
  - Fixed bad node shapes at complex intersections with roads and footpaths. Issue #7042
  - Fixed invalid keepClear state for intersections without cross-traffic. Issue #4437
  
- sumo-gui
  - Corrected shape of laneAreaDetector when lanes have a length-geometry-mismatch. Issue #6627
  - Corrected waiting position for persons at busStop in lefthand-network. Issue #6985
  - Fixed crash in mesosim when keeping vehicle parameter window open. Issue #7194
  - Fixed changes to neighboring values when setting color thresholds. Issue #7304
  - Fixed invalid floating point values in saved GUI settings. Issue #7305
  
- netedit
  - Fixed invalid results/crashing when splitting an edge within a joined traffic light. Issue #7018
  - Routes with a single edge can now be created. Issue #7129
  - Fixed bug where all passenger lanes changed their permissions when adding a sidewalk. Issue #7135
  - Fixed crash when changing connection shape or junction shape. Issue #7138, #7351
  - Fixed invalid output when transforming trip to vehicle. Issue #7073
  - Fixed invalid routes when on of it's edges. Issue #6986
  - Fixed invalid count of selected elements. Issue #7113
  - Fixed missing minDur and maxDur attributes after changing tlType. Issue #7188
  - Selection coloring is now working in data mode. Issue #7066
  - Fixed crash when adding multiple busStops to a trip. Issue #7251
  - Fixed invalid handling of stops loaded from route file. Issue #7191
  - Added option "unregulated" to the list of permitted junction types. Issue #7359
  - Fixed bug that caused inconsistent output when modifying the traffic light index of connections. Issue #7311
  - LaneAreaDetectors linked to a traffic light (with attribute 'tl') can now be loaded. Issue #7275
  - Selection of data elements can now be edited in inspect mode. Issue #7067
  - Removed invalid content restriction for shape params. Issue #7379
  - Flows with an embedded route can now be loaded. Issue #7317
  - Size exaggeration for route is now working as in sumo-gui. Issue #7410
  
- duarouter
  - Person ride arrivalPos is now included in route-output. Issue #7260
  - Fixed crash when loading fully defined person plan. Issue #7423
  
- marouter
  - Fixed crash when using routing algorithm 'CH'. Issue #6912, #6935
  
- Tools
  - Fixed invalid connection diff when edges without any connections are removed. Issue #6643
  - [randomTrips.py](Tools/Trip.md) options **--junction-taz** and **--validate** no work as expected when combined. Issue #7002
  - [randomTrips.py](Tools/Trip.md) no longer generates trips were *fromJunction* equals *toJunctoin* when setting option  **--junction-taz**. Issue #7005
  - [randomTrips.py](Tools/Trip.md) option **--vtype-output** now also applies to trip output. Issue #7089
  - turnFile2EdgeRelations.py can now handle empty intervals. Issue #7084
  - XML objects returned by sumo.xml.parse can now add attributes which are python keywords correctly. Issue #7441
  
- TraCI
  - Fixed memory leaks when using libsumo. Issue #7012, #7169, #7220
  - Fixed invalid vehicle placement when using *vehicle.moveToXY* and the lane hint specifies a neighboring lane of the target position. Issue #6870
  - Fixed crash when accessing prior riding stage with *person.getStage*. Issue #7060
  - Fixed crash and delayed effect when setting vehicle parameter "device.rerouting.period". Issue #7064, #7075
  - Polygons and POIs added at runtime now show up in contextSubscriptions. Issue #7057
  - TraaS function getDeltaT is now working. Issue #7121
  - The python client now supports adding polygons with more than 255 shape points. Issue #7161
  - Vehicle type parameters can now be retrieved from vehicles with libsumo. Issue #7209
  - Fixed invalid electric vehicle parameters being used when calling 'traci.vehicle.getElectricityConsumption'. Issue #7196
  - Constructor of traci.simulation.Stage now uses the same default arguments as libsumo.simulation.Stage and an be constructed without arguments. Issue #7265
  - Fixed invalid vehicle lane position when using moveToXY in a network with [custom edge lengths](Simulation/Distances.md). Issue #7401
  - Fixed invalid vehicle lists (i.e. simulation.getDepartedIDList) after loading simulation state. Issue #7454
  
  
- All Applications
  - File names with a `%`-Sign can no be loaded. Issue #6574


### Enhancements
- Simulation
  - Added option **--person-summary-output** which causes statistics on person numbers and their travel modes to be written in every simulation step. Issue #6964
  - **summary-output** now includes number of stopped vehicles. Issue #6966
  - Option **--scale** now also applies to persons. Issue #5564
  - Emission-output is now written only for those vehicles that have an emission device. By default all vehicles get the device when option **--emission-output** is set. This default can be changed by standard device assignment options and configurations. Issue #7079
  - Added option **--device.emissions.period** to control the period in which emission-output is written. Issue #7079
  - Charging stations now work when a vehicle is parking (either with `parking="true"` or when stopped at a parkingArea and a chargingStation simultaneously). Issue #7068
  - The distance at which vehicles react to device.bluelight can now be configured with option **--device.bluelight.reactiondist**. Issue #7112
  - Pedestrians can now be configured to ignore oncoming cars at an unregulated crossing via junction model parameters (jmIgnoreFoeProb, jmIgnoreFoeSpeed). Issue #7148  
  - Strategic lane-changes can now be disabled by setting 'lcStrategic="-1"`. Issue #7180
  - Taxi pickup and drop-off location can now be restricted to public transport stops by setting **--persontrip.transfer.walk-taxi ptStops** and **--persontrip.transfer.taxi-walk ptStops**. Issue #7192  
  - Added new [taxi dispatch algorithm](Simulation/Taxi.md#dispatch) **routeExtension**. Issue #2606 #7373
  - Vehroute-output can now be restricted by assigning 'device.vehroute' with [generic assignment options](Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#devices). Issue #7393
  - Improved loading time of large rail networks. Issue #7403
  - Added option **--railway.max-train-length** to tune the efficiency of the railway router initialization. Issue #7405
  - Calibrators now support attribute 'jamThreshold' to configure automatic jam clearing. Issue #6592
  - **--railsignal-block-output** now includes 'protectingSwitches' to distinguish flanks guarded by signals from flanks guarded by switches. Issue #7489
  
- netconvert
  - Added option **--discard-param KEY1,KEY2,..** which deletes all `<param .../>` elements with the given keys. Issue #6972
  - Added option **edges.join-tram-dist {{DT_FLOAT}}** which can be used to join overlying road and tram edges into a single lane with combined permissions. This is needed for the correct dynamics in OSM-derived networks. Issue #6980
  - When loading **ptstop-files** and filtering the network extend, the loaded stops will be filtered automatically. Issue #7024
  - When specifying a polygon boundary for option **--keep-edges.in-boundary**, the argument may now contain spaces to separate positions. This allows copying a polygon shape attribute. Issue #7025
  - When an intersection is surrounded by connected footpaths, superfluous walkingareas are no longer built. The distance at which pedestrian nodes count as connected can be set by option **--walkingareas.join-dist**. Issue #7120
  - Added new node attribute 'tlLayout' to configure signal plan layout per node. Issue #7187
  - Added new traffic light layout 'alternateOneWay' to model work zones where each direction uses the interior of a joined traffic light exclusively. Issue #7199
  - Added option **--roundabouts.visibility-distance** to set the default visibility distance when entering a roundabout. The new default is 9 (meters) which is twice the default link visibility and this leads to smoother flow when there are no foe vehicles at a roundabout. The old behavior can be restored by setting **--roundabouts.visibility-distance -1**. Issue #4703
  
- netedit
  - Edges can now be colored by edgeData attribute (as in sumo-gui). Issue #6953
  - Added function 'select reachable' to the edge context menu (just like sumo-gui). Issue #6995
  - In create-route-mode, edges can now be colored by reachability from the last selected edge. Issue #6995
  - Added junction context-menu option *select roundabout*. Issue #5606
  - Added junction context-menu option *convert to roundabout*. Issue #7030
  - Vehicles and Flows with their own (unnamed) route can now be created directly. Issue #7103
  - Selection mode now supports Ctrl+click to strictly move the view. Issue #7239
  - Improving loading speed of polygons. Issue #7287
  - Elements can now be marked as 'front element' (F11) to facilitate working with overlapped objects in move mode and traffic light mode. Issue #4903, #5781
  - LaneAreaDetectors with multiple lanes can now be created. Issue #7475

- sumo-gui
  - Improved visualization of long vehicles (i.e. trains) when zoomed out (length/width ratio reduces with zoom). Issue #6745
  - A color legend for vehicle colors can now be enabled via the 'Legend' tab. Issue #6930
  - Vehicles can now be stopped and stops can be aborted via context menu.
  - The hiding-treshold for edge coloring can now also hide edge color value labels (to avoid clutter from irrelevant numbers). Issue #7140
  - Added locator menu entry for containers. Issue #7324
  
- duarouter
  - Route attributes 'repeat' and 'cycleTime' are now supported. Issue #7165
  
- od2trips
  - Can now specify persontrip modes with option **--persontrip.modes**. Issue #7219
  
- meso
  - Added option **--meso-tls-flow-penalty FLOAT**. This can be used to control the headway penalty to model maximum (averaged) flow at traffic lights. Formerly the option **--meso-tls-penalty** was responsible for this but now the latter option only controls the travel time penalty. Issue #7497, #7491

- All Applications
  - Symbolic color definition "random" can now be used (i.e. in vehicle, vType, poi, ...). Issue #7058
  - There is a new emission model "Zero" replacing "HBEFA*/zero" etc. Furthermore the default emission class for a model can now be chosen using something like "HBEFA3/default". Issue #7098

- TraCI
  - Added new function *simulation.loadState* for quick-loading a saved simulation state. Issue #6201
  - Added new optional 'reason' argument to *vehicle.moveTo* which controls how the vehicle interacts with detectors. By default, vehicles will be registered by detectors if the moveTo distance is physically plausible. Issue #3318. 
  - Added new function *vehicle.getStops*. This can be used to retrieve the next or last n stops. The method returns a list of stop objects with extended attributes. Issue #7015, #7249, #7117, #7452
  - Added functionality for retrieving lane, position and stopped vehicles for traci domains 'busstop', 'parkingarea', 'chargingstation' and 'overheadwire'. Issue #5952, #7099
  - The python client now supports the optional argument 'traceFile' in function traci.start which records all traci commands to the given file (also works for libsumo.start). The optional boolean argument 'traceGetters' can be used to switch off tracing of data retrieval functions. Issue#6604, #7481
  - Added function 'vehicle.replaceStop' which replaces the upcoming stop with the given index and automatically adapts the route to the new stop edge. Issue #7226
  - Added function 'vehicle.getFollower' which works like getLeader but looks backwards. Issue #7107
  - Retrieval of prior person stages now includes departure time and travel time. Issue #7274
  - Added 'simulation.getVersion' to libsumo and C++ client. Issue #7282
  - Function ['person.moveToXY'](TraCI/Change_Person_State.md#move_to_xy_0xb4) is now officially supported whenever a person is walking or stopped. It can be used to move a person to an arbitrary place and will update the route accordingly. Issue #2872, #7367, #7382
  - Added function 'person.getLaneID'. Issue #7394
  - Added function 'gui.track' which can be used to track persons. Issue #7400
  - Added functions 'routeprobe.sampleLastRouteID' and 'traci.routeprobe.sampleCurrentRouteID'. Issue #6109
  - Added functions 'gui.isSelected' and 'gui.toggleSelection' to retrieve and modify selection status. Issue #7428
  
- Tools
  - [plot_trajectories.py](Tools/Visualization.md#plot_trajectoriespy) can now filter input data by edges (**--filter-edges**) and vehicle-ids (**--filter-ids**). Issue #6952
  - [plot_trajectories.py](Tools/Visualization.md#plot_trajectoriespy) can now visualize person trajectories using option **--persons**. Issue #6978
  - Added new tool [computeCoordination.py](Tools/Output.md#computecoordinationpy) to compute the fraction of delayed vehicles along a defined corridor. Issue #6987
  - implausibleRoutes.py now works better with routes consisting of a single edge only and respects internal edge lengths. It can also detect routes containing edge loops or node loops. Issue #7071
  - osmWebWizard now exports public transport edges for rail vehicles when setting both of the options 'Car-only Network' and 'Import public transport'. Issue #7081
  - [edgeDataFromFlow.py](Tools/Detector.md#edgedatafromflowpy) now supports time intervals. Issue #7133
  - Added new tool [net2geojson](Tools/Net.md#net2geojsonpy) for converting a .net.xml file into GeoJSON format. Issue #7237
  - [attributeStats.py](Tools/Output.md#attributestatspy) now supports option **--precision** to control output precision. Issue #7238
  - Improved [cutRoutes.py](Tools/Routes.md#cutroutespy) for use with person plans. Issue #6027, #7384
  - Added new tool [addStopDelay.py](Tools/Routes.md#addstopdelaypy) to add random stop delays to a route file. Issue #5209
  - [routeSampler.py](Tools/Turns.md#routesampler.py): Now supports option **--weighted**. This causes routes to be sampled according to their probability. The probabilty can either be specified explicitly using route attribute 'probability' or implicitly if the same sequence of edges appears multiple times in the the route input. Issue #7501
    
### Other
- Simulation
  - Route attribute 'period' was renamed to 'cycleTime'. Issue #7168
  - Total simulation running time is now in seconds rather than milliseconds. Issue #7255
- netconvert
  - Roads of type 'highway.track' (aggricultural roads) are no longer accessible by cars when importing OSM data. Issue #7082

- Netedit
  - Demand creation entries have been renamed to better distinguish the different variants (i.e. flow with embedded route or flow with origin and destination). Issue #7175

- TraCI
  - In the python client, function vehicle.getLeader can now optionally return the pair ("", -1) instead of 'None' if no leader is found. The new behavior can be enabled by calling function traci.setLegacyGetLeader(False). In later versions this will be the default behavior. The return value 'None' is deprecated. Issue #7013
  - The function 'libsumo.vehicle.getNextStops' now returns a list of StopData objects rather than a list of tuples (the same result type returned by getStops) but different from the return type of 'traci.vehicle.getNextStops'. If the python script should be fully compatible between traci and libsumo, the new function 'vehicle.getStops' must be used.

- Tools
  - [randomTrips.py](Tools/Trip.md) now generates repeatable results by default (by using a fixed default seed). To obtain the prior random behavior without specifying a seed, the option **--random** can be used (just like for [sumo](sumo.md)). Issue #6989

- Documentation
  - Dark Mode added (not available on Internet Explorer)
  - Added documentation on [length-geometry-mismatch](Simulation/Distances.md)

- Releases
  - Version numbers of the python packages now follow the 1.6.0.post1234 scheme [see PEP440](https://www.python.org/dev/peps/pep-0440/)
  - Publishing Python packages on https://test.pypi.org/

## Version 1.6.0 (28.04.2020)

### Bugfixes

- Simulation
  - Fixed negative depart delay. Vehicles with a sub-step-length depart time now depart in the subsequent step. Issue #964
  - Fixed sublane model bug that reduced flow on intersections and caused emergency braking. Issue #6649
  - Fixed sublane model bug that was causing cyclical switching between keep-right and sublane-alignment choices. Issue #6084  
  - Fixed sublane model bug that was causing invalid lane alignment in multimodal networks. Issue #6691
  - Fixed crash when depart-triggered vehicles were discarded. Issue #6690    
  - Removed invalid warning for public transport users. Issue #6698
  - Fixed invalid right-of-way at traffic light junctions with right-turn-on-red rules. Issue #6068
  - Fixed bug that caused junction collisions to go unnoticed. Issue #6779  
  - Stopping duration no longer exceeds the planned duration/until time by one simulation step. Issue #6755
  - ACC model no longer uses double minGap when queued. Issue #6728
  - Fixed extremely low speed of W99 model when approaching intersections and stops. Issue #6823
  - Fixed crash after rerouting. Issue #6835
  - Fixed bugs where stops on cyclical routes where ignored after loading simulation state. Issue #6811
  - Fixed invalid 'density' and 'occupancy' values (too high) in edgeData output. Issue #5723
  - Fixed crash when running with multiple threads. Issue #6806
  - Option **--ignore-route-errors** can now be used to ignore the error 'will not be able to depart using the given velocity (slow lane ahead)'. Issue #6853
  - Fixed crash when using option **--no-internal-links** and defining a stop on an internal lane. Issue #6855
  - Fixed invalid vehicle angles when using option **--lanechange.duration** in a left-hand network. Issue #6856
  - Ride arrival stop is now considered when a vehicle stops multiple times on the arrival edge. Issue #2811
  - Fixed unsafe insertion speed for IDM-vehicles. Issue #6904
  - Fixed bug that was causing cooperative speed adjustments to fail. Issue #5124
  - Fixed invalid emission-edgeData when restricting the output time interval. Issue #6944
  - Railway fixes:
    - Train reversal problems. Issue #6692, #6782, #6797, #6851, #6860, #6906, #6908
    - Train routing now considers space requirement for train reversal. Issue #6697, #6743, #6799
    - Train routing is no longer effected by occupied edges that are beyond rail signal control. Issue #6741    
    - Invalid train speed after insertion on short edge. Issue #6801 
    - CarFollowModel 'rail' now correctly uses minGap. Issue #6796
    - Invalid stop position after splitting train. Issue #6788
    - Rail signal allows entering occupied block for joining trains. Issue #6794
    - Joining trains with different minGap values. Issue #6795   
    - carFollowModel 'Rail' no longer ignores attributes 'accel', 'decel' and 'emergencyDecel', 'apparentDecel' and 'collisionMinGapFactor'. Issue #6834
    - Fixed invalid warning "Invalid approach information after rerouting". Issue #6914
    - Fixed invalid warning "Circular block" at dead-end sidings. Issue #6926
    
- MESO
  - Calibrator attribute `vTypes` is now working. Issue #6702
- netconvert
  - Fixed precision when filtering network with a given boundary. Issue #6206
  - Fixed missing connections when importing OpenDRIVE. Issue #6712
  - OpenDRIVE networks where a single edges is connected to more than two junctions are now supported. Issue #6828
  - Fixed invalid link state at railway switch that would cause uncessary deceleration. Issue #6833
  - Fixed invalid link direction in rail networks with unusual geometry. Issue #6924
  - OpenDRIVE import now correctly handles `signalReference` and signal `laneValidity` elements. Issue #2517, #2366
  - Fixed errors when loading custom lane shapes. Issue #6892, #6893, #6894, #6895
  - Fixed bug that could cause an invalid intermodal network to be written. Issue #6901
- sumo-gui
  - Fixed layout of meso edge parameter dialog at high occupancy (regression in 1.5.0)
  - Fixed crash when simulation ends while tracking person attributes. Issue #6784
  - Fixed missing stop annotations in 'show-route' mode. Issue #6814
  - Fixed crash when scaling railway track width. Issue #6859
- netedit
  - Fixed empty route id when creating route from non-consecutive edges with custom id. Issue #6722
  - Fixed invalid displayed connection attribute. Issue #6760
  - Fixed crash when moving a selection of junctions and edges. Issue #4918
- duarouter
  - Attribute `group` of `<ride>` and `<personTrip>` is no longer lost. Issue #6555
  - Train routing now considers space requirement for train reversal. Issue #6697
- od2trips
  - Option **--departpos** is now working for pedestrians and personTrips. Issue #6720
- TraCI
  - Fixed crash when calling vehicle.moveTo for an off-road vehicle with tripinfo-output. Issue #6717
  - Fixed crash when trying add subscription filter for context subscription of persons around an ego vehicle. Issue #6735
  - Fixed crash and invalid results when using 'vehicle.addSubscriptionFilterLateralDistance'. Issue #6770, #6941
  - Fixed crash when adding vehicle with `depart="triggered"`. Issue #6793
  - Fixed invalid routing results when calling 'traci.simulation.findIntermodalRoute(..., routingMode=ROUTING_MODE_AGGREGATED)'. Issue #6827
- Tools
  - [sort_routes.py](Tools/Routes.md#sort_routespy) can now handle trips, triggered vehicles and human-readable times.  
  - [routeSampler.py](Tools/Turns.md#routesampler.py): fixed bias in departure time for the generated vehicles. Issue #6786

### Enhancements
- Simulation
  - Taxi device now supports ride sharing. Issue #6544
  - Added option **--extrapolate-departpos**. When this option is set, vehicles with a departure time that lies between simulation steps have their depart position shifted forward to better reflect their position at the actual insertion time. This can greatly reduce depart delay and increase and insertion flow in high-flow situations. Issue #4277
  - Traffic lights of type 'actuated' and 'delay_based' now support the use of [custom detectors](Simulation/Traffic_Lights.md#custom_detectors). Issue #5125, Issue #6773
  - The new route attribute `period` is now applied repeatedly when rerouting the vehicle and the new route contains stops with attribute `until` to shift those times forward (e.g. on cyclical rerouters for public transport vehicles). Issue #6550
  - The new route attribute `repeat` can now be used to define repeating routes. When the route contains stop definitions with the `until`-attribute, the new route attribute `period` must be used to shift the repeated stop times. Issue #6554  
  - Added option **--fcd-output.params KEY1,KEY2,...** to extend fcd-output with additional vehicle [generic parameters](Simulation/GenericParameters.md). Issue #6685
  - Tripinfo-output attribute vaporized now includes specific description for premature vehicle removal (collision, traci, calibrator, ...). Issue #6718
  - Tripinfo-output now includes timeloss for person rides. Issue #6891
  - Added option **--statistic-output** to write [various statistics](Simulation/Output/index.md#commandline_output_verbose) in XML format. Issue #3751
  - vType attribute 'lcSigma' now always affect lateral position (previously, only sublane model and continuous lanechange modle were affected).
  - Option **--save-state.times** now accepts human-readable time. Issue #6810
  - Added new 'device.toc' parameters ['mrmSafeSpot' and 'mrmSafeSpotDuration'](ToC_Device.md) to control the behavior during minimum-risk maneuvers. Issue #6157
  - Added option **--vehroute-output.stop-edges** to record the edges that were driven ahead of stop as part of the vehroute output. Issue #6815
  - Added option **--weights.priority-factor FLOAT** to factor the priority of edges into the routing decision with a configurable weight (edges with higher priority are preferred increasingly when setting this to a higher value). An application for this is [railway routing](Simulation/Railways.md#routing_on_bidirectional_tracks). Issue #6812
  - Added option **--device.rerouting.bike-speed** to compute aggregated speeds for bicycles separately. Issue #6829
  - Automatic train rerouting by rail signal logic can now be disabled by setting option **--device.rerouting.railsignal false** as well as by vehicle and vType parameters (key="device.rerouting.railsignal"). Issue #6781
  - Trains can now be joined in reverse order (rear part id is kept). Issue #6803
  - Element `<busStop>` now supports attribute 'parkingLength'. This can be used define the available space for stopping vehicles with `parking="true"` independent of the length of the busStop along the lane. Issue #3936
  - edgeData output now includes the new attribute 'laneDensity' which reports the density in vehs per km per lane. Issue #6840
  - edgeData and laneData definitions now support the new attribute 'writeAttributes' which can be used to reduce the list of output attributes to the given list of attribute names. Issue #6847
  - Public transport import from OSM now supports route definitions with abbreviated style. Issue #6862
  - Added option **--save-state.rng**. When this option is set, saved simulation state will include the state of all random number generators and this state will be restored upon loading. This RNG state takes up about 500kb. Issue #1418
      
- netedit
  - Added new 'Data Mode' to edit files with edge and turn-related data (`<edgeData>`, `<edgeRelation>`). Issue #6461
  - Traffic light phase editing function 'Insert Phase' now takes successive green states into account when synthesizing transition phases. Issue #6732
- sumo-gui
  - Missing data values (color by edgeData) can now be distinguished from negative data values. Issue #6660
  - `<edgeRelation>`-data files can now be visualized. Issue #6659
  - Traffic lights of type 'delay_based' can now dynamically toggle detector visualization.
  - Train reversals are now indicated when ''Show Route'' is active. Issue #6744
  - Vehicles can now be colored by 'by stop delay'. This computes the estimated departure delay at the next `<stop>` with an 'until' attribute. Issue #6821
  - Travelled distance (odometer) is now listed in the vehicle parameter dialog.
  - Junction name is now listed in the junction parameter dialog and can optionally by drawn in the view. Issue #6635
  - Lanes that explicitly allow rail and passenger traffic are now highlighted with a distinct color in the default color scheme. Issue #6844
  - When loading a network with internal lanes and setting the option **--no-internal-links**, the internal lanes are still shown. This helps to understand track topology in rail networks. Issue #6852
  - Visualization of a vehicles route can now be configured with the menu option 'Draw looped routes' which can be disabled to reduce clutter in looped public transport routes. Issue #6917
  
- netconvert
  - Edge attribute `spreadType` now supports value `roadCenter` which aligns edges with common geometry so that the geometry reflects the middle of the road regardless of differences in lane numbers (causing natural alignment for turning lanes). Issue #1758
  - Added option **--default.spreadtype** to set the default spread type for edges. Issue #6662
  - Connections now support attribute ''length'' to customize the length of internal lanes. Issue #6733
  - Added option **--default.connection-length** to overwrite the length of internal lanes with a specific value. Issue #6733
  - Added option **--railway.topology.direction-priority** to assign edge priority values based on the [preferred track usage direction](Simulation/Railways.md#routing_on_bidirectional_tracks) (determined heuristically from uni-directional track).
  - Added node attribute 'name' to hold an optional string label. Issue #6635
  - Direction-specific speeds are now imported from OSM (*maxspeed:forward*, *maxspeed:backward*). Issue #6883
  
- duarouter
  - When loading weights from edgeData files, arbitrary attributes can now be used with option **--weight-attribute**. Issue #6817
  - Added option **--weights.priority-factor FLOAT** to factor the priority of edges into the routing decision with a configurable weight (edges with higher priority are preferred increasingly when setting this to a higher value). Issue #6812
   
- TraCI
  - [Meso simulation](Simulation/Meso.md) now supports TraCI
  - Parking vehicles are now picked up by context subscriptions. Issue #6785
  - Added functions 'getFollowSpeed', 'getSecureGap', 'getStopSpeed' for accessing carFollowModel functions to the vehicle domain. Issue #3361
  - Added function 'traci.vehicle.getStopDelay' to retrieve the estimated delay at the next (public transport) stop. Issue #6826
- Tools
  - added osmWebWizard option to import a simplified network that is only for cars. Issue #6595
  - [matsim_importPlans.py](Tools/Import/MATSim.md#matsim_importplanspy) now supports alternative input dialects and sorts its output.  
  - added new tool [net2kml.py](Tools/Net.md#net2kmlpy) to convert sumo networks to KML
  - Function sumolib.net.getShortestPath can now penalize train reversals using the optional argument 'reversalPenalty'
  - [tls_csvSignalGroups.py](Tools/tls.md#tls_csvsignalgroupspy) now supports defining controlled pedestrian crossings. Issue #6861
  - [routeSampler.py](Tools/Turns.md#routesampler.py) improvements
    - supports loading multiple data intervals. Data aggregation can be customized using options **--begin**, **--end**, **--interval**
    - supports loading multiple files (routes, edgedata, turns). The corresponding options were renamed accordingly (**--route-files, --edgedata-files, --turn-files**)
    - supports optimization by linear programming using option **--optimize INT|full** (requires scipy). The integer argument can be used to configure the maximum deviation from the input route distribution.
    - supports output of mismatch between measured counts and sampled counts for visualization in sumo-gui via `edgeData` and `edgeRelation` files using option **--mismatch-output**
    - supports direct optimization of route input (without resampling) by setting option **--optimize-input**
    - supports multi-edge counting data using `<edgeRelation from="A" to="D" via="B C" count="INT"/>`. Issue #6729
    - supports non-consecutive edgeRelations with maximum gap by using the new option **--turn-max-gap <INT>**. Issue #6726
    - supports loading origin-destination relations with the new option **--od-files**. Issue #6888
    - supports writing results with routeIDs, routeDistribution and flows instead of vehicles. Issue #6730

### Other
- Network version is now 1.6 (Issue #1758)
- The turn-file format used by jtrrouter is now deprecated. Instead the new edgeRelation-file format should be used. To convert old files into the new format, the tool turnFile2EdgeRelations.py can be used. The tools jtcrouter.py and routeSampler.py will only accept the new file format as input.
- netconvert
  - Signal references from OpenDRIVE networks are now exported as a mapping from sumo-tls-link-index to OpenDRIVE-signal-id instead of sumo-edge to OpenDRIVE-signal-name. Issue #6863
- netedit
  - supermode hotkeys changed
    - F2: Network mode
    - F3: Demand mode
    - F4: Data mode
  - Default color for edge-geometry points changed to avoid confusion with rail signals when coloring junctions by type. Issue #6749

## Version 1.5.0 (11.02.2020)

### Bugfixes
- Simulation
  - Fixed bug that could cause the simulation to freeze when vehicles approach a right_before_left junction from all directions simultaneously. Issue #6471 (regression in 1.4)
  - Fixed bug that was causing pedestrian jamming. Issue #6457
  - Fixed bug that was causing unsafe situations between bicycles and turning cars. Issue #6437, #6438
  - Fixed unsafe insertion speed when using 'stopOffset'. Issue #6411
  - Extra device output (emissions etc.) is now included in tripinfo-output when using option **--tripinfo-output.write-unfinished**. Issue #6422
  - Time spent parking is now included in tripinfo-output 'stopTime'. Issue #6423
  - Fixed lateral jump when driving across lanes with different width in a left-hand network and using sublane simulation. Issue #6573
  - Fixed bug where rerouting failed for no reason. Issue #6572
  - Fixed invalid error when loading a network where edge-IDs have non-ascii characters. Issue #6597

- sumo-gui
  - Fixed visualisation of containers. Issue #6426
  - Text setting options 'constant size' and 'background' are now working for 'link junction index' and 'link tls index'. Issue #6376
  - Traffic light parameter 'running duration' is now correct when switching phases via TraCI. Issue #6520
  - Fixed bug where Locator-Dialog 'Center' button remained disabled after successfully locating objects via substring filtering. Issue #6593
  
- netedit
  - Creating new traffic light plans where a plan already exists no longer results in invalid plans at joined traffic lights or traffic lights with signal groups. Instead a copy of the existing plan is created. Accordingly the button caption now varies between 'Create' and 'Copy'. Issue #6536, #6537
  - Fixed invalid position of Vehicle with negative departPos. Issue #6473

- MESO
  - Fixed invalid simulation state when vehicles are teleporting. Issue #6408
  - Meso calibrator is now shown in GUI. Issue #2345
  
- netconvert
  - Fixed bug where loaded connection was not created. Issue #6470
  - Fixed bug where turning restrictions from OSM were violated. Issue #6482
  - Option **--flip-y-axis** is now working. In can be used to turn a right-hand network into a left-hand network and vice versa. Issue #3838
  - Fixed bug where option **--tls.guess-signals** failed to detect signals on incoming edges. Issue #6493, Issue #6584
  - Fixed invalid right-of-way rules in left-hand network. Issue #6496
  - Connection attribute 'pass' now overrides conflicts between straight and turning connections from the same edge. Issue #6497
  - Fixed invalid permissions for internal lanes when the incoming lane has more restrictions than the outgoing lane. Issue #5557
  - Fixed bug that was causing missing green phases for some connections at joined traffic lights. Issue #6582
  - Fixed node shape computation for difficult geometries. Issue #6585
  - The status of option **--tls.ignore-internal-junction-jam** is no longer lost when loading a '.net.xml' file. Issue #6559

- TraCI
  - Context subscriptions can now be removed individually from the python client (before every unsubscribeContext removed all)
  - Fixed invalid changing to the opposite side when using changeLaneRelative(vehID, -1) on lane 0. Issue #6576
  - Fixed premature changing from the opposite side when a duration was set for changeLaneRelative. Issue #6577
  - Fixed invalid result when calling traci.lanearea.getLastStepMeanSpeed in subsecond simulation. Issue #6605
  - Fixed invalid stage depart values when calling traci.simulation.findIntermodalRoute. Issue #6600

### Enhancements
- Simulation
  - Added new [model for electric hybrid vehicles and overhead lines and power substations](Models/ElectricHybrid.md). 
  - Railway simulation now supports [splitting and joining trains, also known as portion working](Simulation/Railways.md#portion_working). Issue #6398
  - Added option **--device.fcd.radius** which allows fcd output for persons and vehicles that are within the given range around equipped vehicles. Issue #6410
  - Added new ['taxi' device](Simulation/Taxi.md) to allow simulation of demand-responsive transport. Issue #6418
  - Added option **--step-log.period INT** to set the number of simulation steps between outputs. The new default is 100 which brings a major execution speedup when running sumo on windows with step-log (where command-line output is slow). Issue #6377
  - Added option **--fcd-output.acceleration** to include acceleration values in fcd-output. When sublane simulation is enabled, attribute 'accelerationLat' is added as well. Issue #6448
  - Added option **--lanechange-output.xy** to include coordinates in lanechange-output. Issue #6588
  - [Aggregated tripinfo output](Simulation/Output/index.md#aggregated_traffic_measures) now includes average speed. Issue #6480
  - Trips, flows, personTrips and walks with attributes `fromJunction`, `toJunction` and `viaJunctions` are now supported when setting option **--junction-taz**. Issue #6474
  - Calibrators can now be used to update vehicle types by defining flows with the 'type' attribute and without the 'vehsPerHour' attribute. Issue #6219
  - When defining [automated switching between traffic light programs](Simulation/Traffic_Lights.md#defining_program_switch_times_and_procedure), the new attribute 'period' can now be used to define repeating switch times. Issue #6540
  - Cooperative lane changing at roundabouts can now be configured independently from other cooperation by setting the new vType attribute 'lcCooperativeRoundabout'. Issue #6565
  - Cooperative speed adjustment can now be configured independently from other cooperation by setting the new vType attribute 'lcCooperativeSpeed'. Issue #6534
  - Vehicles can now foresee speed reductions on their lane and change before being slowed down (configurable by new vType attribute 'lcSpeedGainLookahead'. Issue #6547

- sumo-gui
  - Dead-end links in the middle of a road are now colored in magenta to highlight connectivity problems. Issue #6391
  - Vehicles that stop (with a `<stop>`) in the middle of the road now activate emergency blinkers. Issue #6608

- netedit
  - Stacked vehicles (overlapping due to similar departPos) are now indicated by a number. Issue #6485
  - Stacked vehicles can now be "unstacked" by setting the option 'spread vehicles'. This spreads them alonger the lane sorted by depart time. Issue #5602
  - Flows are now distinguished from individual vehicles with a text caption. Issue #6486
  - Junctions with missing connections are now highlighted in magenta. Issue #6391
  - Geometry points are now clearly distinguished from junctions by color and size. Issue #6514
  - Traffic light mode now allows grouping and ungrouping signals. Issue #2958
  - Traffic light mode 'Clean States' function now also compactifies state and removes all unused indices. Issue #3828
  - The background grid size can now be increased and decreased with hotkeys CTRL+PAGE_UP, CTRL+PAGE_DOWN. Issue #6542
  - Demand objects (vehicles, routes, ...) can now be located by ID. Issue #5110

- netconvert
  - Improved connection building heuristic. Issue #6295, #6467, #6468
  - Improved heuristic for option **--junctions.join**. Issue #872, Issue #5398
  - Improved heuristic for option **--tls.guess**. Issue #6512
  - Added options **--numerical-ids.edge-start INT**, **--numerical-ids.node-start INT** to relabel network elements with numbers starting at the given value. Issue #6372
  - Added option **--no-turnarounds.except-turnlane** which causes turnlanes to be built only if there is an exclusive (left) turn lane. Issue #6444
  - Added option **--tls.group-signals** which assigns the same signal index to connections that share the same sequence signal states. Added option **--tls.ungroup-signals** which reverts grouping and assigns a distinct signal index to every connection. Issue #2958
  - Symbolic speed limits in OSM are now supported for all countries. Issue #6566
  - Added option **--fringe.guess** to heuristically mark nodes on the outer network boundary. Issue #6624
  - Added option **--no-turnarounds.fringe** to prevent building of turnaround-connections at the network fringe. Issue #6621
  - Added option **--ptstop-output.no-bidi** to skip automatic generation of a bidi-stop on a bidirectional rail edge. Issue #6598
  
- netgenerate
  - Fringe nodes are now automatically marked in grid networks when setting **--grid.attach-length** > 0. Issue #6622

- duarouter
  - The maximum [map-matching](Demand/Shortest_or_Optimal_Path_Routing.md#mapmatching) distance is now configurable using option **--mapmatch.distance**. Also, mapmatching with large distance is now computationally efficient. Issue #6476
  - Trip, flows, personTrips and walks with attributes `fromJunction`,`toJunction` and `viaJunctions` are now supported when setting option **--junction-taz**. Issue #6474
  - Trips can now be written with attributes `fromJunction` and `toJunction` and `viaJunctions` by setting option **--write-trips.junctions**. Issue #6474
  - When using trip attributes `fromXY`, `fromLonLat`, etc. the given positions can now be mapped to junctions instead of edges by setting option **--mapmatch.junctions**. Issue #6475  

- jtrrouter
  - Added option **--sources-are-sinks** (shortcut **-S**). When this option is set, all flows will terminate upon reaching the from-edge of another flow. Issue #6601
  - Added option **--discount-sources** (shortcut **-D**). When this option is set, the flow departing at an edge will be reduced by the amount of traffic that has reached this edge from upstream flows. When combined with option **--sources-are-sinks**, upstream flows will terminate at a downstream edge when the source flow number has been reached. This can be used to build a scenario from flow counts at arbitrary locations. Issue #6601

- TraCI / Libsumo
  - Added context subscription filter that filters in a given range around the vehicles route 'addSubscriptionFilterLateralDistance'. Issue #6287
  - Libsumo now supports context subscription filters

- Tools
  - [duaIterate.py](Demand/Dynamic_User_Assignment.md#iterative_assignment_dynamic_user_equilibrium) now supports intermodal scenarios. Issue #6403
  - Added [duaIterate.py](Demand/Dynamic_User_Assignment.md#iterative_assignment_dynamic_user_equilibrium) option **--measure-vtypes** to improve accuracy of measured travel times when different vehicle types can use different lanes.
  - Added [tileGet.py](Tools/Misc.md#tilegetpy) to retrieve image tiles from public servers. Issue #6279
  - Added [randomTrips.py](Tools/Trip.md) option **--personrides LINE** to generated person rides with the give line id.
  - Added [randomTrips.py](Tools/Trip.md) option **--junction-taz** to generated trips, flows, personTrips and walks between junctions. Issue #6474
  - Added the ability to download Satellite image backgrounds (using [tileGet.py](Tools/Misc.md#tilegetpy)) in [osmWebWizard](Tools/Import/OSM.md#osmwebwizardpy). Issue #6481
  - Added new tool [jtcrouter.py](Tools/Turns.md#jtcrouterpy) to build a traffic demand scenario based on turn-count data (this uses [jtrrouter](jtrrouter.md) in the background). Issue #6229
  - Added new tool [routeSampler.py](Tools/Turns.md#routesampler.py) to build a traffic demand scenario based on turn-count and edge-count data. This tool samples from a given set of routes to meet the detected counts. Issue #6616
  - Added new tool [turnCount2EdgeCount.py](Tools/Turns.md#turncount2edgecountpy) which converts turn-count data into edge-count data. Issue #6619
  

### Other
- Build
  - removed autotools build 

- sumo-gui
  - the settings file no longer accepts the element breakpoints-file (command line option is still there)
  - renamed attributes in the settings (breakpoint.value to breakpoint.time and decal.filename to decal.file)
  - Default settings values 'show link junction index' and 'show link tls index' changed because their interpretation varies after #6376. Old GUI-setting files must be updated to achieve the default look.
  
- Simulation
  - When defining [automated switching between traffic light programs](Simulation/Traffic_Lights.md#defining_program_switch_times_and_procedure), switch times are no longer wrapped to fit into a single day (modulo). The replace the old behavior, set the new waut attribute `period="86400"`. Issue #6522
  
- TraCI
  - traci.lane.getLinks (python) now returns the extended descriptions by default, the older behavior should not be used anymore (but is still available using a second parameter)


## Older Versions

- [Changes to Versions 1.2.0, 1.3.0, 1.3.1 and 1.4.0 (2019 releases)](Z/Changes_in_2019_releases.md)
- [Changes to Versions 1.0.0, 1.0.1 and 1.1.0 (2018 releases)](Z/Changes_in_2018_releases.md)
- [Changes to Version 0.32.0](Z/Changes_from_Version_0.31.0_to_Version_0.32.0.md)
- [Changes to Version 0.31.0](Z/Changes_from_Version_0.30.0_to_Version_0.31.0.md)
- [Changes to Version 0.30.0](Z/Changes_from_Version_0.29.0_to_Version_0.30.0.md)
- [Changes to Version 0.29.0](Z/Changes_from_Version_0.28.0_to_Version_0.29.0.md)
- [Changes to Version 0.28.0](Z/Changes_from_Version_0.27.1_to_Version_0.28.0.md)
- [Changes to Version 0.27.1](Z/Changes_from_Version_0.27.0_to_Version_0.27.1.md)
- [Changes to Version 0.27.0](Z/Changes_from_Version_0.26.0_to_Version_0.27.0.md)
- [Changes to Version 0.26.0](Z/Changes_from_Version_0.25.0_to_Version_0.26.0.md)
- [Changes to Version 0.25.0](Z/Changes_from_Version_0.24.0_to_Version_0.25.0.md)
- [Changes to Version 0.24.0](Z/Changes_from_Version_0.23.0_to_Version_0.24.0.md)
- [Changes to Version 0.23.0](Z/Changes_from_Version_0.22.0_to_Version_0.23.0.md)
- [Changes to Version 0.22.0](Z/Changes_from_Version_0.21.0_to_Version_0.22.0.md)
- [Changes to Version 0.21.0](Z/Changes_from_Version_0.20.0_to_Version_0.21.0.md)
- [Changes to Version 0.20.0](Z/Changes_from_Version_0.19.0_to_Version_0.20.0.md)
- [Changes to Version 0.19.0](Z/Changes_from_Version_0.18.0_to_Version_0.19.0.md)
- [Changes to Version 0.18.0](Z/Changes_from_Version_0.17.1_to_Version_0.18.0.md)
- [Changes to Version 0.17.1](Z/Changes_from_Version_0.17.0_to_Version_0.17.1.md)
- [Changes to Version 0.17.0](Z/Changes_from_Version_0.16.0_to_Version_0.17.0.md)
- [Changes to Version 0.16.0](Z/Changes_from_Version_0.15.0_to_Version_0.16.0.md)
- [Changes to Version 0.15.0](Z/Changes_from_Version_0.14.0_to_Version_0.15.0.md)
- [Changes to Version 0.14.0](Z/Changes_from_Version_0.13.1_to_Version_0.14.0.md)
- [Changes to Version 0.13.1](Z/Changes_from_Version_0.13.0_to_Version_0.13.1.md)
- [Changes to Version 0.13.0](Z/Changes_from_Version_0.12.3_to_Version_0.13.0.md)
- [Changes to Version 0.12.3](Z/Changes_from_Version_0.12.2_to_Version_0.12.3.md)
- [Changes to Version 0.12.2](Z/Changes_from_Version_0.12.1_to_Version_0.12.2.md)
- [Changes to Version 0.12.1](Z/Changes_from_Version_0.12.0_to_Version_0.12.1.md)
- [Changes to Version 0.12.0](Z/Changes_from_Version_0.11.1_to_Version_0.12.0.md)
- [Changes to Version 0.11.1](Z/Changes_from_Version_0.11.0_to_Version_0.11.1.md)
- [Changes to Version 0.11.0](Z/Changes_from_Version_0.10.3_to_Version_0.11.0.md)
- [Changes to Version 0.10.3](Z/Changes_from_Version_0.10.2_to_Version_0.10.3.md)
- [Changes to Version 0.10.2](Z/Changes_from_Version_0.10.1_to_Version_0.10.2.md)
- [Changes to Version 0.10.1](Z/Changes_from_Version_0.10.0_to_Version_0.10.1.md)
- [Changes to Version 0.10.0](Z/Changes_from_Version_0.9.10_to_Version_0.10.0.md)
- [Changes to Version 0.9.10](Z/Changes_from_version_0.9.9_to_version_0.9.10.md)
- [Changes to Version 0.9.9](Z/Changes_from_version_0.9.8_to_version_0.9.9.md)
- [Changes to Version 0.9.8](Z/Changes_from_version_0.9.7_to_version_0.9.8.md)
- [Changes to Version 0.9.7](Z/Changes_from_version_0.9.6_to_version_0.9.7.md)
- [Changes to Version 0.9.6](Z/Changes_from_version_0.9.5_to_version_0.9.6.md)
- [Changes to Version 0.9.4](Z/Changes_from_version_0.9.3_to_version_0.9.4.md)
- [Changes to Version 0.9.3](Z/Changes_from_version_0.9.2_to_version_0.9.3.md)
- [Changes to Version 0.9.2](Z/Changes_from_version_0.9.1_to_version_0.9.2.md)
- [Changes to Version 0.9.1](Z/Changes_from_version_0.9.0_to_version_0.9.1.md)
- [Changes to Version 0.9.0](Z/Changes_from_version_0.8.x_to_version_0.9.0.md)
- [Changes to Version 0.8.3](Z/Changes_from_version_0.8.2.2_to_version_0.8.3.md)
- [Changes to Version 0.8.2.2](Z/Changes_from_version_0.8.2.1_to_version_0.8.2.2.md)
- [Changes to Version 0.8.2.1](Z/Changes_from_version_0.8.0.2_to_version_0.8.2.1.md)
- [Changes to Version 0.8.0.2](Z/Changes_from_version_0.8.0.1_to_version_0.8.0.2.md)
- [Changes to Version 0.8.0.1](Z/Changes_from_version_0.8.0_to_version_0.8.0.1.md)
- [Changes to Version pre0.8](Z/Changes_from_version_0.7.0_to_version_pre0.8.md)
