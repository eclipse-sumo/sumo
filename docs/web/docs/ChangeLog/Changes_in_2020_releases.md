---
title: Changes in the 2020 releases (versions 1.5.0, 1.6.0, 1.7.0 and 1.8.0)
---

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
  - Fixed "jumping" persons when transferring from car to walking at junction. Issue #7778
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
  - Function *traci.vehicle.getStopDelay* now returns correct estimates for [waypoints](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#waypoints). Issue #7752
  - The server side socket is now closed earlier to avoid race conditions when starting many simulations. Issue #7750

- Tools
  - osmWebWizard search now works for IE users. Issue #6119
  - batch files generated by osmWebWizard on linux can now be used on windows. Issue #7667
  - Tools that support option **-C** for saving their configuration now use proper xml-escaping for their option values. Issue #7633
  - [routeSampler.py](../Tools/Turns.md#routesamplerpy) no longer includes routes which do not pass any counting location when using option **--optimize**. This also speeds up execution. Issue #6723

### Enhancements
- simulation
  - Added vehicle attribute 'departEdge' which can be used to [insert a vehicle beyond the first edge of its route](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#departedge). Issue #1129
  - Added new input elements that can be loaded to influence rail signal operation by providing constraints on train ordering. Issue #7435
  - Train waiting time is now taken into account when resolving conflicts between otherwise equal trains. Issue #7598
  - edgeData output now includes attribute timeLoss. Issue #1396
  - Pedestrian simulation will no longer deadlock on narrow sidewalks (< 1.28m) Issue #7746
  - Person journey can now include transfer from walking-only edge to car-only edge at junction #7779
  - Option **--ride.stop-tolerance** now applies to all kinds of stops. #6204
  - Added option **--fcd-output.max-leader-distance** which will add attributes `leaderGap, leaderSpeed, leaderID` to fcd-output whenever a vehicle has a leader within the given distance #7788
  - Stop for vehicles and persons can now be specified using attribute 'edge' instead of 'lane'. Vehicles will stop on the rightmost lane that allows their vehicle class (whereas persons ignored the lane index anyway). Issue #5443
  - Taxi device can now be used to simulate on-demand container transport. Issue #7815
  - Option **--fcd-output.params** now supports optional output of device parameters and model parameters with the same [prefix codes as TraCI](../TraCI/Vehicle_Value_Retrieval.md#device_and_lanechangemodel_parameter_retrieval_0x7e). Issue #7851
  - Option **--fcd-output.params caccVehicleMode** now outputs the vehicle mode of CACC vehicles, which can be one of `CC`, `ACC`, `CACC`, `CACC_GAP_CL` (gap-closing), `CACC_GAP`, `CACC_CA` (collision avoidance). Issue #6700

- meso
  - Model parameters can now be [customized](../Simulation/Meso.md#configuration_by_edge_type) for each edge type via additional file input. Issue #7243
  - Stop handling is now similar to microsimulation, pedestrians walk to the correct location and are picked up there, stopinfo is written

- netedit
  - Create edge mode now allows selecting from edge types and to inspect/edit edge attributes. Issue #2431
  - Opposite direction lanes can now be edited and selected. Issue #2653
  - Edges can now be split while in 'create edge mode' by shift-clicking. Issue #6754
  - [Generic parameters](../Simulation/GenericParameters.md) of vTypes can now be edited. Issue #7646
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
  - Added option **--persontrip.taxi.waiting-time** to model the time penalty involved when calling a taxi. Issue #7651

- od2trips
  - Attributes 'fromTaz' and 'toTaz' are now written for walks and personTrips. Issue #7591

- TraCI
  - Added new function *vehicle.getStopArrivalDelay* to return the arrivalDelay at a public transport stop that defines the expected 'arrival'. Issue #7629
  - Added new functions for railway simulation to investigate why a rail signal is red: *trafficlight.getBlockingVehicles, trafficlight.getRivalVehicles, trafficlight.getPriorityVehicles'. Issue #7019
  - Function 'simulation.findIntermodalRoute' now supports mode=taxi. Issue #7757
  - Function vehicle.moveToXY' now only maps onto lanes compatible with the vClass of the vehicle. To ignore permissions, bit2 of the keepRoute flag can be set. Issue #5383
  - Added [API for coupling taxi dispatch algorithms](../Simulation/Taxi.md#traci). Issue #6526

- Tools
  - Added [randomTrips.py](../Tools/Trip.md) option **--via-edge-types**. When this option is set to a list of types, edges of this type are not used for departure or arrival unless they are on the fringe. This can be used to prevent departure on the middle of a motorway. Issue #7505
  - Added new tool [generateRailSignalConstraints.py](../Tools/Railways.md#generaterailsignalconstraintspy) to generated constraint input for rain signals from a route file. Issue #7436
  - Added [traceExporter.py](../Tools/TraceExporter.md) option **--shift** which allows shifting output coordinates by a fixed amount (i.e. to avoid negative values).
   Added [traceExporter.py](../Tools/TraceExporter.md) now supports option **--ipg-output** for generating car-maker tracefiles. Issue #6190
  - [routeSampler.py](../Tools/Turns.md#routesamplerpy) now supports option **--minimize-vehicles FLOAT** which allows configuring a preference for fewer generated vehicles (where each vehicle passes multiple counting locations). Issue #7635
  - Added new tool [osmTaxiStop.py]() to import taxi stands from OSM data. Issue #7729
  - Added new tool [checkStopOrder.py](../Tools/Routes.md#checkstoporderpy) to detect inconsistent times in a public transport schedule. Issue #7458
  - added osmWebWizard option **--output** to set a custom output directory. Issue #7672

### Other
- Changed osmWebWizard default for **--device.rerouting.adaptation-interval** from 1 to 10 to increase performance. The value of **--adaptation-steps** was reduced from 180 to 18 so that the time over which speeds are averaged remains at 180s (with fewer samples). Issue #7640
- [macOS launchers](../Downloads.md#application_launchers) added for **sumo-gui**, **netedit** and the **osmWebWizard**. This allows to set **sumo-gui** as the default application to open `.sumocfg` files (to open a simulation by just double clicking a file) on macOS.
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
  - Fixed invalid error when doing parallel routing with algorithm 'astar'. Issue #7248
  - Fixed invalid taxi states when two customers exit on the same edge. Issue #7257
  - Fixed crash when using routing algorithm CH with taxis. Issue #7256
  - Person rides with a specified arrivalPos no longer exit their vehicle prematurely. Issue #7258
  - Person ride arrivalPos and stoppingPlace are now included in vehroute-output. Issue #7260, #7261
  - Fixed invalid vehicle angle when using option **--lanechange-duration** with low values of 'maxSpeedLat'. Issue #7263
  - Fixed bug where vehicles would stop at [waypoints](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#waypoints). Issue #7377, #7419
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
  - Fixed invalid routes when on of its edges. Issue #6986
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
  - [randomTrips.py](../Tools/Trip.md) options **--junction-taz** and **--validate** no work as expected when combined. Issue #7002
  - [randomTrips.py](../Tools/Trip.md) no longer generates trips were *fromJunction* equals *toJunction* when setting option **--junction-taz**. Issue #7005
  - [randomTrips.py](../Tools/Trip.md) option **--vtype-output** now also applies to trip output. Issue #7089
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
  - Fixed invalid vehicle lane position when using moveToXY in a network with [custom edge lengths](../Simulation/Distances.md). Issue #7401
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
  - Added new [taxi dispatch algorithm](../Simulation/Taxi.md#dispatch_algorithms) **routeExtension**. Issue #2606 #7373
  - Vehroute-output can now be restricted by assigning 'device.vehroute' with [generic assignment options](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#devices). Issue #7393
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
  - Selection mode now supports <kbd>Ctrl</kbd> + <kbd>click</kbd> to strictly move the view. Issue #7239
  - Improving loading speed of polygons. Issue #7287
  - Elements can now be marked as 'front element' (<kbd>F11</kbd>) to facilitate working with overlapped objects in move mode and traffic light mode. Issue #4903, #5781
  - LaneAreaDetectors with multiple lanes can now be created. Issue #7475

- sumo-gui
  - Improved visualization of long vehicles (i.e. trains) when zoomed out (length/width ratio reduces with zoom). Issue #6745
  - A color legend for vehicle colors can now be enabled via the 'Legend' tab. Issue #6930
  - Vehicles can now be stopped and stops can be aborted via context menu.Issue #2755
  - The hiding-threshold for edge coloring can now also hide edge color value labels (to avoid clutter from irrelevant numbers). Issue #7140
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
  - Function ['person.moveToXY'](../TraCI/Change_Person_State.md#move_to_xy_0xb4) is now officially supported whenever a person is walking or stopped. It can be used to move a person to an arbitrary place and will update the route accordingly. Issue #2872, #7367, #7382
  - Added function 'person.getLaneID'. Issue #7394
  - Added function 'gui.track' which can be used to track persons. Issue #7400
  - Added functions 'routeprobe.sampleLastRouteID' and 'traci.routeprobe.sampleCurrentRouteID'. Issue #6109
  - Added functions 'gui.isSelected' and 'gui.toggleSelection' to retrieve and modify selection status. Issue #7428

- Tools
  - [plot_trajectories.py](../Tools/Visualization.md#plot_trajectoriespy) can now filter input data by edges (**--filter-edges**) and vehicle-ids (**--filter-ids**). Issue #6952
  - [plot_trajectories.py](../Tools/Visualization.md#plot_trajectoriespy) can now visualize person trajectories using option **--persons**. Issue #6978
  - Added new tool [computeCoordination.py](../Tools/Output.md#computecoordinationpy) to compute the fraction of delayed vehicles along a defined corridor. Issue #6987
  - implausibleRoutes.py now works better with routes consisting of a single edge only and respects internal edge lengths. It can also detect routes containing edge loops or node loops. Issue #7071
  - osmWebWizard now exports public transport edges for rail vehicles when setting both of the options 'Car-only Network' and 'Import public transport'. Issue #7081
  - [edgeDataFromFlow.py](../Tools/Detector.md#edgedatafromflowpy) now supports time intervals. Issue #7133
  - Added new tool [net2geojson](../Tools/Net.md#net2geojsonpy) for converting a .net.xml file into GeoJSON format. Issue #7237
  - [attributeStats.py](../Tools/Output.md#attributestatspy) now supports option **--precision** to control output precision. Issue #7238
  - Improved [cutRoutes.py](../Tools/Routes.md#cutroutespy) for use with person plans. Issue #6027, #7384
  - Added new tool [addStopDelay.py](../Tools/Routes.md#addstopdelaypy) to add random stop delays to a route file. Issue #5209
  - [routeSampler.py](../Tools/Turns.md#routesamplerpy): Now supports option **--weighted**. This causes routes to be sampled according to their probability. The probability can either be specified explicitly using route attribute 'probability' or implicitly if the same sequence of edges appears multiple times in the route input. Issue #7501

### Other
- Simulation
  - Route attribute 'period' was renamed to 'cycleTime'. Issue #7168
  - Total simulation running time is now in seconds rather than milliseconds. Issue #7255
- netconvert
  - Roads of type 'highway.track' (agricultural roads) are no longer accessible by cars when importing OSM data. Issue #7082

- Netedit
  - Demand creation entries have been renamed to better distinguish the different variants (i.e. flow with embedded route or flow with origin and destination). Issue #7175

- TraCI
  - In the python client, function vehicle.getLeader can now optionally return the pair ("", -1) instead of 'None' if no leader is found. The new behavior can be enabled by calling function traci.setLegacyGetLeader(False). In later versions this will be the default behavior. The return value 'None' is deprecated. Issue #7013
  - The function 'libsumo.vehicle.getNextStops' now returns a list of StopData objects rather than a list of tuples (the same result type returned by getStops) but different from the return type of 'traci.vehicle.getNextStops'. If the python script should be fully compatible between traci and libsumo, the new function 'vehicle.getStops' must be used.

- Tools
  - [randomTrips.py](../Tools/Trip.md) now generates repeatable results by default (by using a fixed default seed). To obtain the prior random behavior without specifying a seed, the option **--random** can be used (just like for [sumo](../sumo.md)). Issue #6989

- Documentation
  - Dark Mode added (not available on Internet Explorer)
  - Added documentation on [length-geometry-mismatch](../Simulation/Distances.md)

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
  - Fixed invalid link state at railway switch that would cause unnecessary deceleration. Issue #6833
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
  - [sort_routes.py](../Tools/Routes.md#sort_routespy) can now handle trips, triggered vehicles and human-readable times.
  - [routeSampler.py](../Tools/Turns.md#routesamplerpy): fixed bias in departure time for the generated vehicles. Issue #6786

### Enhancements
- Simulation
  - Taxi device now supports ride sharing. Issue #6544
  - Added option **--extrapolate-departpos**. When this option is set, vehicles with a departure time that lies between simulation steps have their depart position shifted forward to better reflect their position at the actual insertion time. This can greatly reduce depart delay and increase and insertion flow in high-flow situations. Issue #4277
  - Traffic lights of type 'actuated' and 'delay_based' now support the use of [custom detectors](../Simulation/Traffic_Lights.md#custom_detectors). Issue #5125, Issue #6773
  - The new route attribute `period` is now applied repeatedly when rerouting the vehicle and the new route contains stops with attribute `until` to shift those times forward (e.g. on cyclical rerouters for public transport vehicles). Issue #6550
  - The new route attribute `repeat` can now be used to define repeating routes. When the route contains stop definitions with the `until`-attribute, the new route attribute `period` must be used to shift the repeated stop times. Issue #6554
  - Added option **--fcd-output.params KEY1,KEY2,...** to extend fcd-output with additional vehicle [generic parameters](../Simulation/GenericParameters.md). Issue #6685
  - Tripinfo-output attribute vaporized now includes specific description for premature vehicle removal (collision, traci, calibrator, ...). Issue #6718
  - Tripinfo-output now includes timeloss for person rides. Issue #6891
  - Added option **--statistic-output** to write [various statistics](../Simulation/Output/index.md#commandline_output_verbose) in XML format. Issue #3751
  - vType attribute 'lcSigma' now always affect lateral position (previously, only sublane model and continuous lanechange model were affected).
  - Option **--save-state.times** now accepts human-readable time. Issue #6810
  - Added new 'device.toc' parameters ['mrmSafeSpot' and 'mrmSafeSpotDuration'](../ToC_Device.md) to control the behavior during minimum-risk maneuvers. Issue #6157
  - Added option **--vehroute-output.stop-edges** to record the edges that were driven ahead of stop as part of the vehroute output. Issue #6815
  - Added option **--weights.priority-factor FLOAT** to factor the priority of edges into the routing decision with a configurable weight (edges with higher priority are preferred increasingly when setting this to a higher value). An application for this is [railway routing](../Simulation/Railways.md#routing_in_bidirectional_networks). Issue #6812
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
  - Traveled distance (odometer) is now listed in the vehicle parameter dialog.
  - Junction name is now listed in the junction parameter dialog and can optionally by drawn in the view. Issue #6635
  - Lanes that explicitly allow rail and passenger traffic are now highlighted with a distinct color in the default color scheme. Issue #6844
  - When loading a network with internal lanes and setting the option **--no-internal-links**, the internal lanes are still shown. This helps to understand track topology in rail networks. Issue #6852
  - Visualization of a vehicles route can now be configured with the menu option 'Draw looped routes' which can be disabled to reduce clutter in looped public transport routes. Issue #6917

- netconvert
  - Edge attribute `spreadType` now supports value `roadCenter` which aligns edges with common geometry so that the geometry reflects the middle of the road regardless of differences in lane numbers (causing natural alignment for turning lanes). Issue #1758
  - Added option **--default.spreadtype** to set the default spread type for edges. Issue #6662
  - Connections now support attribute ''length'' to customize the length of internal lanes. Issue #6733
  - Added option **--default.connection-length** to overwrite the length of internal lanes with a specific value. Issue #6733
  - Added option **--railway.topology.direction-priority** to assign edge priority values based on the [preferred track usage direction](../Simulation/Railways.md#routing_in_bidirectional_networks) (determined heuristically from uni-directional track).
  - Added node attribute 'name' to hold an optional string label. Issue #6635
  - Direction-specific speeds are now imported from OSM (*maxspeed:forward*, *maxspeed:backward*). Issue #6883

- duarouter
  - When loading weights from edgeData files, arbitrary attributes can now be used with option **--weight-attribute**. Issue #6817
  - Added option **--weights.priority-factor FLOAT** to factor the priority of edges into the routing decision with a configurable weight (edges with higher priority are preferred increasingly when setting this to a higher value). Issue #6812

- TraCI
  - [Meso simulation](../Simulation/Meso.md) now supports TraCI
  - Parking vehicles are now picked up by context subscriptions. Issue #6785
  - Added functions 'getFollowSpeed', 'getSecureGap', 'getStopSpeed' for accessing carFollowModel functions to the vehicle domain. Issue #3361
  - Added function 'traci.vehicle.getStopDelay' to retrieve the estimated delay at the next (public transport) stop. Issue #6826
- Tools
  - added osmWebWizard option to import a simplified network that is only for cars. Issue #6595
  - [matsim_importPlans.py](../Tools/Import/MATSim.md#matsim_importplanspy) now supports alternative input dialects and sorts its output.
  - added new tool [net2kml.py](../Tools/Net.md#net2kmlpy) to convert sumo networks to KML
  - Function sumolib.net.getShortestPath can now penalize train reversals using the optional argument 'reversalPenalty'
  - [tls_csvSignalGroups.py](../Tools/tls.md#tls_csvsignalgroupspy) now supports defining controlled pedestrian crossings. Issue #6861
  - [routeSampler.py](../Tools/Turns.md#routesamplerpy) improvements
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
  - Added new [model for electric hybrid vehicles and overhead lines and power substations](../Models/ElectricHybrid.md).
  - Railway simulation now supports [splitting and joining trains, also known as portion working](../Simulation/Railways.md#portion_working). Issue #6398
  - Added option **--device.fcd.radius** which allows fcd output for persons and vehicles that are within the given range around equipped vehicles. Issue #6410
  - Added new ['taxi' device](../Simulation/Taxi.md) to allow simulation of demand-responsive transport. Issue #6418
  - Added option **--step-log.period INT** to set the number of simulation steps between outputs. The new default is 100 which brings a major execution speedup when running sumo on windows with step-log (where command-line output is slow). Issue #6377
  - Added option **--fcd-output.acceleration** to include acceleration values in fcd-output. When sublane simulation is enabled, attribute 'accelerationLat' is added as well. Issue #6448
  - Added option **--lanechange-output.xy** to include coordinates in lanechange-output. Issue #6588
  - [Aggregated tripinfo output](../Simulation/Output/index.md#aggregated_traffic_measures) now includes average speed. Issue #6480
  - Trips, flows, personTrips and walks with attributes `fromJunction`, `toJunction` and `viaJunctions` are now supported when setting option **--junction-taz**. Issue #6474
  - Calibrators can now be used to update vehicle types by defining flows with the 'type' attribute and without the 'vehsPerHour' attribute. Issue #6219
  - When defining [automated switching between traffic light programs](../Simulation/Traffic_Lights.md#defining_program_switch_times_and_procedure), the new attribute 'period' can now be used to define repeating switch times. Issue #6540
  - Cooperative lane changing at roundabouts can now be configured independently from other cooperation by setting the new vType attribute 'lcCooperativeRoundabout'. Issue #6565
  - Cooperative speed adjustment can now be configured independently from other cooperation by setting the new vType attribute 'lcCooperativeSpeed'. Issue #6534
  - Vehicles can now foresee speed reductions on their lane and change before being slowed down (configurable by new vType attribute 'lcSpeedGainLookahead'). Issue #6547

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
  - The background grid size can now be increased and decreased with hotkeys <kbd>Ctrl</kbd> + <kbd>Page Up</kbd>, <kbd>Ctrl</kbd> + <kbd>Page Down</kbd>. Issue #6542
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
  - The maximum [map-matching](../Demand/Shortest_or_Optimal_Path_Routing.md#mapmatching) distance is now configurable using option **--mapmatch.distance**. Also, mapmatching with large distance is now computationally efficient. Issue #6476
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
  - [duaIterate.py](../Demand/Dynamic_User_Assignment.md#iterative_assignment_dynamic_user_equilibrium) now supports intermodal scenarios. Issue #6403
  - Added [duaIterate.py](../Demand/Dynamic_User_Assignment.md#iterative_assignment_dynamic_user_equilibrium) option **--measure-vtypes** to improve accuracy of measured travel times when different vehicle types can use different lanes.
  - Added [tileGet.py](../Tools/Misc.md#tilegetpy) to retrieve image tiles from public servers. Issue #6279
  - Added [randomTrips.py](../Tools/Trip.md) option **--personrides LINE** to generated person rides with the give line id.
  - Added [randomTrips.py](../Tools/Trip.md) option **--junction-taz** to generated trips, flows, personTrips and walks between junctions. Issue #6474
  - Added the ability to download Satellite image backgrounds (using [tileGet.py](../Tools/Misc.md#tilegetpy)) in [osmWebWizard](../Tools/Import/OSM.md#osmwebwizardpy). Issue #6481
  - Added new tool [jtcrouter.py](../Tools/Turns.md#jtcrouterpy) to build a traffic demand scenario based on turn-count data (this uses [jtrrouter](../jtrrouter.md) in the background). Issue #6229
  - Added new tool [routeSampler.py](../Tools/Turns.md#routesamplerpy) to build a traffic demand scenario based on turn-count and edge-count data. This tool samples from a given set of routes to meet the detected counts. Issue #6616
  - Added new tool [turnCount2EdgeCount.py](../Tools/Turns.md#turncount2edgecountpy) which converts turn-count data into edge-count data. Issue #6619


### Other
- Build
  - removed autotools build

- sumo-gui
  - the settings file no longer accepts the element breakpoints-file (command line option is still there)
  - renamed attributes in the settings (breakpoint.value to breakpoint.time and decal.filename to decal.file)
  - Default settings values 'show link junction index' and 'show link tls index' changed because their interpretation varies after #6376. Old GUI-setting files must be updated to achieve the default look.

- Simulation
  - When defining [automated switching between traffic light programs](../Simulation/Traffic_Lights.md#defining_program_switch_times_and_procedure), switch times are no longer wrapped to fit into a single day (modulo). The replace the old behavior, set the new waut attribute `period="86400"`. Issue #6522

- TraCI
  - traci.lane.getLinks (python) now returns the extended descriptions by default, the older behavior should not be used anymore (but is still available using a second parameter)
