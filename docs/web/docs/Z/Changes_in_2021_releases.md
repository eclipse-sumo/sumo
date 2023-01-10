---
title: Changes in the 2021 releases (versions 1.9.0, 1.9.1, 1.9.2, 1.10.0 and 1.11.0)
---

## Version 1.11.0 (23.11.2021)

### Bugfixes

- simulation
  - crashing
    - Fixed crash in parallel simulation. Issue #9359 (regression in 1.10.0)
    - Fixed freezing in parallel simulation. Issue #9385
    - Fixed crash when using flow with invalid departEdge. Issue #8993
    - Fixed crash caused by rerouters on short edges. Issue #9186
    - Fixed bug where simulation did not terminate after a departSpeed related error. Issue #9211
    - Fix bug where simulation with waypoints at stopping place freezes. Issue #9399
  - safety
    - Fixed invalid approach information with step-method.ballistic that could cause collision. Issue #8955
    - Fixed rare collision on junction when the outbound lane is jammed. Issue #6415
    - Fixed miscounted teleports in vehicle-person collisions. Issue #9018
    - Fixed collision after unsafe lane changing ahead of junction. Issue #8950
    - Fixed emergency braking in sublane simulation. Issue #9051
    - Fixed invalid error after emergency braking at red light with ballistic update. Issue #8978
    - Fixed unsafe sublane changing on junction. Issue #9180
    - Fixed emergency braking during opposite-direction overtaking. Issue #9183, #9184, #9185, #9297, #9530
    - Fixed invalid collision warning during opposite direction driving. Issue #9388
    - Fixed bug where frontal collision between fast vehicles was not detected. Issue #9402
    - Fixed rail_signal bug that could cause a flanking conflict to be ignored. Issue #9612
  - sublane
    - Fixed invalid behavior after using stop posLat in non-sublane simulation. Issue #9013
    - Fixed deadlock when using lcSigma without sublane model. Issue #9395
    - Fix bug where rescue lane briefly closes when two rescue vehicles follow each other. Issue #9494
    - Fixed bug where vehicles stay on the opposite side for too long in sublane simulation. Issue #9548
  - output
    - Fixed invalid edges and exitTimes in **vehroute-output** when using rerouting and looped routes. Issue #9374
    - Fcd-output no longer includes persons in vehicle when person-device.fcd is disabled. Issue #9469
    - Stop attribute 'actType' is now preserved in vehroute-output. Issue #9495
    - Fixed invalid use of busStop instead of containerStop in vehroute-output. Issue #8989
    - Fixed invalid human readable times in tripinfo output (affecting sub-second values). Issue #9580
  - parking / stopping
    - Fixed non-deterministic parkingReroute. Issue #9066
    - Parking search is no longer deterministic when param 'parking.probability.weight' is set. Issue #9364
    - Parking search doesn't drive past an available parkingArea if 'parking.probability.weight' is set. Issue #9371
    - Attribute personCapacity is no longer ignored for trainStop. Issue #9262
    - Fixed lower-than-configured boardingTime when many persons are entering. Issue #9263
  - misc
    - Fixed bug where a vehicle never starts with startPos="stop" departSpeed="max" and ballistic update. Issue #8988
    - Fixed departSpeed related errors when using vehrouter-output as simulation input. Issue #9199, #9205
    - Fixed invalid duplicate ids when using option **--scale**. Added option **--scale-suffix** to deal with unavoidable id collisions. Issue #9055
    - Railway routing now longer creates routes where a train gets stuck on very short reversal edges. Issue #9323
    - Fixed invalid warning in railway simulation. Issue #9398

- netedit
  - network mode: additionals
    - Fixed probability statistics and coloring in taz mode. Issue #9107 (regression in 1.7.0)
    - Fixed crash when creating reverse edge with "both directions" active. Issue #9408 (regression in 1.8.0)
    - Inverting selection of shapes now works even when no edges are loaded. Issue #8951 (regression in 1.9.2)
    - BusStops with '/' in their name can now be loaded gain. Issue #9064 (regression in 1.10.0)
    - Fixed disappearance of connecting lines between E3 detector and its entry/exit elements. Issue #8916
    - trainStops with access can now be loaded. Issue #8969
    - Fixed crash after selecting edges in taz mode. Issue #9128
    - Fixed undo-redo issues after selecting edges in taz mode. Issue #9132
    - Fixed invalid warning about missing color of POI. Issue #9125
    - Loading taz without shape is now supported. Issue #9140
    - Taz are now drawn below roads. Issue #9146
    - Fixed bug where additional objects could not be loaded via command line option. Issue #9166
    - When creating a new TAZ, edges are now assigned based on the polygon shape rather than it's bounding box. Issue #9225
    - Fixed invalid e3detector position when placing entry/exit detectors close to junction. Issue #9421
  - network mode: other
    - Copying one of several traffic light programs now copies the correct one. Issue #7963
    - Adding a bicycle lane via lane context menu now respects option **--default.bikelane-width** Issue #9073
    - Fixed missing turnaround after adding bike lane. Issue #9079
    - Fixed invalid drawing style for lane that allows tram and bus. Issue #9089
    - Fixed invalid edge type attributes in saved network. Issue #9070
    - Fixed invalid linkIndex2 for indirect left turn after modifying an existing turn. Issue #9102    
    - Fixed slow operation when inspecting large objects. Issue #9106
    - Fixed slow loading of large networks. Issue #9207
    - Dotted contour now matches junction shape at reduced size. Issue #9204    
    - Fixed invalid error when loading program in tls frame. Issue #9270
    - Attribute 'opposite' is now updated when changing lane count. Issue #9283
    - Attribute 'opposite' is now preserved when splitting edges. Issue #9316
    - Minor fixes to save-load tls-program dialog. Issue #9269
    - Fixed lost window focus. Issue #9274, #9275
    - Fixed invalid roundabout when using function 'convert to roundabout' before first network computation. Issue #9348    
    - Fixed crash related to convert-to-roundabout and undo. Issue #9449
  - demand mode
    - Multi-parameter speedFactor value can now be set. Issue #8263
    - Flows with embedded route can now be loaded. Issue #8966
    - Fixed lost elements when loading personFlow. Issue #7732
    - Vehicles with embedded routes can now load parameters. Issue #7509
    - Demand mode now respects "show connections" settings. Issue #9087
    - Fixed long delay when switching between editing modes while in demand super-mode. Issue #9088
    - Fixed slow loading of large traffic demand files. Issue #9191
    - Fixed crash when entering non-existing vtype in current vtype field. Issue #9509
  - data mode
    - Fixed various usability issues related to tazRelation editing. Issue #9059, #9086, #9109, #9123, #9114, #9122, #9120, #9121, #9126, #9113, #9152

- sumo-gui
  - Fixed invalid person angle in output. Issue #9014
  - Fixed slow stepping on windows when the simulation has little to do. Issue #6371
  - Fixed unresponsive UI on linux when the simulation has little to do. Issue #9028
  - Speed mode in vehicle parameter dialog now shows all 6 bits. Issue #9078
  - Option **--no-warnings** now suppresses warnings from actuated tls. Issue #9104
  - Fixed crash on pressing "recalibrate rainbow" button when taz files are loaded. #9119
  - Fixed invalid error when defining step-length with human readable time. Issue #9196
  - Coloring by edgeData is now working in meso. Issue #9215
  - Edge color value is now correct when coloring 'by angle'. Issue #9431
  - Color values for 'color by edge data' are now loaded from settings file. Issue #9462
  - Small text placement fixes. Issue #9477, #9476, #9467
  - Fixed freezing in person simulation. Issue #9468
  - Fixed crash when loading edgeData with inconsistent interval times. Issue #9502

- netconvert
  - Connection attribute visibility is now working if the connection has an internal junction. Issue #8953
  - Fixed crash when importing OpenDRIVE with internal lane shapes when the input defines no width. Issue #9009
  - Attribute 'color' and generic params of loaded stops are now kept. Issue #9011, #9048
  - Guessed bicycle lanes are now always placed to the left of an existing sidewalk. Issue #9084
  - Fixed invalid connections after guessing bicycle lanes. Issue #9083
  - Option **--no-turnarounds.geometry** now ignores pedestrian paths and bike paths that attach to the main road. Issue #9068
  - Fixed missing tlType in plain .nod.xml output. Issue #9281
  - Fixed invalid network when importing OSM with 'nan' values for maxspeed. Issue #9302
  - Fixed crash when loading joined traffic lights together with option **--tls.join**. Issue #9299
  - Fixed errors and invalid shape when converting short roads with arc-geometry from OpenDRIVE. Issue #9332
  - Option **--flatten** now works for OpenDRIVE input. Issue #9447
  - Option **--geometry.min-dist** no longer changes OpenDRIVE topology. Issue #9448
  - When importing OSM, footpaths with bike access now default to two-way edges. Issue #9459
  - Option **--speed.factor** now works with lane-specific speeds. Issue #9466
  - Fixed invalid permissions for some bicycle and pedestrian lanes in OSM import. Issue #9483
  - Fixed invalid walkingarea shapes that overlap with vehicular movements. Issue #9485
  - Fixed inconsistencies in assignment of connection directional arrows. Issue #9430

- duarouter
  - Fixed bug where some input flows where ignored when preceded by non-flow elements. Issue #8995
  - tranship with attributes from,to is no longer ignored. Issue #8976
  - Fixed crash when loading containerFlow. Issue #8975
  - Fixed endless loop related to triggered personFlow. Issue #8977
  - Persons and containers with depart=triggered are now written in the correct order: directly after their intended vehicle. Issue #9000
  - Fixed crash when loading transport outside a container #9008
  - Fixed invalid treatment of loaded routeDistribution input. Issue #9229
  - Person params are no longer lost. Issue #9320
  - Fixed bug where vehicles change their route despite option **--keep-route-probability**. Issue #9487
  - Fixed crash when using **--keep-route-probability** with lower-than-previous **--max-alternatives**. Issue #9488
  - Fixed inconsistent railway routing results when using option **--weights.priority-factor** compared to sumo results. Issue #9397
  - Stop attribute 'actType' is now preserved. Issue #9495
  - Fixed infinite loop when loading flow with probability and number but without end attribute. Issue #9504
  - Fixed crash when ignoring missing vType for probabilistic flow. Issue #9503
  - Option **--weights.random-factor** now influences routing results when using **--routing-algorithm** CH or CHWrapper. Issue #9515
  - Fixed several memory leaks

- meso
  - fixed crash when using taxi device #9208 (regression in 1.9.2)
  - Fixed bug where not all passengers unboard. Issue #9556
  - ArrivalEdge is no longer ignored. Issue #8994
  - Fixed invalid departSpeed error. Issue #9201

- marouter
  - Fixed invalid route-not-found error. Issue #9193

- polyconvert
  - Shapefiles will now be interpreted correctly when no projection is defined #8948
  - Fixed hidden buildings due to invalid default layers in OSM typemap. Issue #9061

- TraCI
  - trafficLights.getControlledLinks passes the correct response size (this has no effect for the end user and is only relevant for client developers). Issue #8891
  - The debug-build of libsumo is now working. Issue #8946
  - Removing and reinserting a vehicle in the same step is now working. Issue #5647
  - traci.vehicle.getLeader can no longer return -inf when the leader is on an intersection. Issue #9001
  - Fixed problems related to complex types returned from libsumo in java. Issue #7204
  - Fixed invalid result of vehicle.getDistance after vehicle.moveToXY, and vehicle.moveTo. Issue #9050, #8778
  - Fixed bug where intended teleport after replaceStop failed due to oncoming train. Issue #9175
  - Fixed invalid route after adding vehicle with trip-route and forcing insertion with moveTo. Issue #9257
  - Fixed invalid departedIDList after reloading a libsumo simulation. Issue #6239
  - Subscription filter "turn" now includes foe on junction. Issue #9330
  - Fixed warning "splitting vector close to end" when using subscription filter. Issue #9404
  - Fixed invalid expected vehicle number after failed vehicle.add. Issue #9456
  - Fixed crash when using libtraci repeatedly. Issue #9478
  - 'traci.vehicle.getLaneChangeState' no longer includes 'TraCI' in the 'state-without-traci' component. Issue #9492
  - Fixed bug where command 'traci.vehicle.changeLane' was ignored in sublane simulation. Issue #9147, #9493
  - Function 'traci.vehicle.replaceStop' no longer fails while on junction. Issue #9467
  - Fixed invalid stop duration reported after updating duration with setStop. Issue #9522
  - Added missing convenience functions in traci.vehicle (i.e. 'isAtBusstop' to libsumo and libtraci python bindings. Issue #9591
  - Fixed missing vehicles when using subscriptionFilterLateralDistance. Issue #9603

- tools
  - cutRoutes.py: Fixed mixed usage of trainStop and busStop. Issue #8982
  - cutRoutes.py: Handles flows without end time
  - [generateRailSignalConstraints.py](../Simulation/Railways.md#generaterailsignalconstraintspy)
    - Initial tripId set via vehicle param is now used. Issue #8959
    - Now using correct tripId when generating constraints for intermediate stop. Issue #8960
    - Fixed crash when there are two stops on the same edge. Issue #8958 (regression in 1.10)
  - generateContinousRerouters.py: fixed infinite loop. Issue #9167
  - GTFS import no longer ignores trips with routes not starting in the simulation area. Issue #9224
  - GTFS import now works when crossing day boundaries. Issue #9002
  - GTFS import no longer fails when the optional 'shapes.txt' is missing.
  - plot_trajectories.py no longer crashes when trying to plot kilometrage with missing input distance. Issue #9472
  - generateParkingAreaRerouters.py Fixed problem when there are 2 parkingAreas on the same edge. Issue #9537

- Miscellaneous
  - Xsd schema now permit trips in additional files. Issue #9110
  - Fixed invalid xsd for tazRelations. Issue #9124
  - game runner is now compatible with python3. Issue #9223
  - game '4-junctions' score is now meaningful. Issue #9222

### Enhancements

- simulation
  - Traffic light type "NEMA" is now supported and implements a NEMA compliant dual-ring controller. Issue #9520 (Thanks to Qichao Wang and NREL)
  - Added new electric vehicle model ["MMPEVEM"](../Models/MMPEVEM.md). Issue #8764 (Thanks to Kevin Badalian and the MMP, RHTW Aachen)
  - Detectortype (E1, E2, E3) now support [attribute 'detectPersons'](../Simulation/Pedestrians.md#detectors_for_pedestrians) to detect pedestrians and passengers. Issue #5252
  - When option **--vehroute-output.exit-times** is set, The output for walk,ride, transport and tranship now includes the values 'started' and 'ended.' Issue #9005
  - Added option **--weights.separate-turns FLOAT**. When this is set to values in ]0,1] routing in the simulation will distinguish travel times by turning direction (i.e. to prefer right turns over left turns where the latter are a cause of delay). Issue #2566
  - If a simulation includes bicycles, they will get a separate section in trip statistics for bicycles. Issue #9069
  - Added option **--vehroute-output.speedfactor**. When this is set, the vehicle specific speed factor will be written in the output. If the vehicle defines a departSpeed, this defaults to 'true'. Issue #9199 
  - BoardingDuration / loadingDuration are now also applied when exiting a vehicle. Issue #4216
  - Delays for opening a rail crossing can now be [customized](../Simulation/Railways.md#rail_crossing_parameters) and their values were increased to be more realistic. Issue #9301
  - Loading parkingReroute elements that can potentially cause blockage in parking search now results in a warning. Issue #9363
  - [Parking search](../Simulation/Rerouter.md#memory_in_parking_search) now employs memory of previously visited and occupied parking areas. Issue #9047
  - Option **--vehroute-output.route-length** now also applies to unfinished routes. Issue #9373
  - Option **--vehroute-output.route-length** is now written for all routes of a vehicle. Issue #9375
  - Taxi simulation now supports the use of multiple taxi fleets and customer choice of fleet. Issue #7818
  - Added ssm device output option **--device.ssm.write-positions** for writing global measures. Issue #9230
  - Waypoints can now be used together with the 'until' attribute to force stopping on early arrival. Issue #9489
  - edgeData output now includes optional attribute 'teleported' to count the number of vehicles that teleported from an edge. Issue #5939
  - Added option **--time-to-teleport.remove** for removing vehicles instead of teleporting and re-inserting them along their route after waiting for too long. Issue #9377
  - Added option **--device.ssm.write-lane-positions** to add lane id and lane-pos to all outputs. Issue #9231
  - In simulations with [opposite-direction-driving](../Simulation/OppositeDirectionDriving.md) stops on the opposite direction edge may be defined by setting a lane index beyond the leftmost forward lane. Issue #8876

- sumo-gui
  - An index value is now drawn for each train reversal in 'show route' mode. Issue #8967
  - All stopping places (busStop, parkingArea, ...) now support custom color. Issue #8280
  - The numerical value behind the current edge color can now be plotted in a tracker window. Issue #9049
  - Locator dialog now shows number of available objects. Issue #9075
  - Locator dialog now allows selecting/deselecting all objects in (filtered) list. Issue #5426
  - Improve positioning of persons in vehicles. Issue #9159
  - Taz attribute 'fill' is now supported. Issue #9144
  - Drawing detail of POIs can now be configured. Issue #9203
  - Improved visualization of teleporting vehicles when shown because of active route visualization. Issue #9174
  - Added vehicle context menu function 'Select transported'. Issue #2241
  - Time range and intervals in loaded edgedata are now reported. Issue #9217
  - Meso vehicles are now drawn with interpolated positions
  - Segment boundaries are now drawn in meso simulation. Issue #9227  
  - Breakpoints are now rounded down to reachable step value. Issue #6789
  - Clicking on timestamps in message window now creates breakpoints with a configurable offset. Issue #7617
  - Lane params and street names are now shown in meso edge parameter dialog. Issue #9300
  - Coloring of stop lines can now be switched to "realistic" mode. Issue #9358
  - in meso, stops are now indicated when 'show route' is active. Issue #9514
  - Added extra colors for 'stopped' in vehicle coloring scheme 'by speed' #9547
  - Added number of stopped vehicles to network parameters. Issue #9558
  - Attribute guiShape now supports 'scooter'. Issue #9190

- netedit
  - Added context menu function to reset opposite-lane information for a selection of lanes. Issue #8888
  - Added new 'Lock' menu to protect different types of objects from inspect,move,select and delete operations. Issue #8342
  - Vehicle stop attribute posLat is now supported. Issue #8808
  - Saved busStop attributes now have the same order as netconvert. Issue #7624
  - Data mode now permits attributes with non-numeric values. Issue #9060
  - Drawing detail of POIs can now be configured. Issue #9203
  - Objects within a polygon boundary can now be selected by using the polygon context menu. Issue #9158
  - Improved drawing style of connecting lines between dependent objects (i.e. busstop/access). Issue #8914, #9258
  - Writing shortened xml header for demand and data output. Issue #9261
  - New network is started in create-edge mode. Issue #9272
  - After setting new edge template, the default in create-edge frame is 'use template'. Issue #9289
  - Edge / lane context menu operations are now available in create-edge mode. Issue #9271
  - The number of routes passing an edge can now optionally be drawn. Issue #8584
  - Routes referenced in routeProbReroute are now  written in additional files. Issue #8547
  - TAZ now support a custom center point for showing their ID and attaching tazRelations. Issue #9298
  - Colors for traffic demand elements (routes, stops, ...) can now be customized in the 'Demand' tab of the view settings dialog. Issue #6318
  - Added options **--node-prefix**, **--edge-prefix** and **--edge-infix** to customize the default names for new objects. Issue #4375
    - New default prefix for edges is 'E' and new prefix for junctions is 'J'. Issue #9424
  - Generic parameters (`<param>`) of traffic light programs can now be edited in the traffic light frame. Issue #7659
  - [stopOffset](../Networks/PlainXML.md#stop_offsets) can now be defined. Issue #3799
  - Object attributes that are computed rather than user-defined (i.e. edge length) are now distinguished in blue. Issue #4633
  - Stopping places can now be resized via shift + drag #3966
  - Loaded rerouters are now drawn near their edges rather than (0, 0), when no position is defined. Issue #9365
  - Parking spaces and textured POIs can now be resized with the mouse. Issue #8825
  - Parent e3 detector now remains selected after creating detEntry/detExit. Issue #9420
  - Newly created reverse-direction edges now receive an id based on the forward direction edge and a '-' sign. Issue #9396
  - Local lane angle is now shown in context menu. Issue #9432
  - Intervals and child-elements of rerouters and VSS can now be inspected. Issue #9369
  - `<DEL>`-key can now be used in inspect mode to delete the currently inspected element. Issue #9352
  - Selection mode can now filter for attributes that have/haven't an empty value using '=' and '^' operators. Issue #9507

- netconvert
  - Public transport line colors are now imported from OSM. Issue #7845
  - Added option **--osm.sidewalks**. This permits to import complete sidewalk data from OSM. Issue #9393

- netgenerate
  - Added options **--grid.x-attach-length --grid.y-attach-length** to configure attachments separately by direction. Issue #8991
  - The option **--bidi-probability** can now be used to control the generation of reverse edges for grid and spider networks. When set to 0, pure oneway networks are generated. Issue #9006

- od2trips
  - tazRelation files (as written by netedit) are now supported as OD-matrix definition. Issue #9057

- duarouter & jtrrouter
  - Added option **--named-routes** which writes routes with an id and lets vehicles reference them. Can reduce output size if many vehicles are using the same route. Issue #8643

- meso
  - Tau value of vehicles types now affect simulation (by acting as a multiplier on the segment tau value) Issue #9356

- marouter
  - tazRelation files (as written by netedit) are now supported as OD-matrix definition. Issue #9057
  - **--netload-output** now includes 'density' and 'laneDensity' and 'speedRelative'. Issue #9197

- traci
  - Added function 'traci.simulation.getEndTime' to retrieve the **--end** value that was set when starting sumo. Issue #2764
  - addSubscriptionFilterTurn can now be combined (additively) with addSubscriptionFilterLateralDistance and with addSubscriptionFilterLanes. Issue #9177
  - Added function 'traci.person.remove' Issue #9351
  - Improved warning messages when 'traci.vehicle.replaceStop', or 'changeTarget' fails. Issue #9453
  - Added functions to retrieve aggregated traffic measures from E3-detector. Issue #9501
  - Libtraci now supports 'Simulation::start'. Issue #6466
  - Added functions 'trafficlight.setNemaSplits', 'setNemaMaxGreens', 'setNemaCycleLength' and 'setNemaOffset' to control the split and offset of NEMA-type controllers. Issue #9520  

- tools
  - cutRoutes.py: Can now handle multiple additional and public transport files in input. Issue #8997
  - [generateRailSignalConstraints.py](../Simulation/Railways.md#generaterailsignalconstraintspy) now supports options **--comment.stop --comment.time --comment.all** for extended annotation of generated constraints. Issue #8963
  - Added tool [addTAZ.py](../Tools/Routes.md#addtazpy) for adding taz information to route files. Issue #8884
  - osmWebWizard.py now imports all bicycle lane data when building scenario with bicycle traffic. Issue #9071
  - osmWebWizard.py uses improved pedestrian routing on shared space. Issue #9100
  - osmWebWizard.py now [avoids duplicate sidewalks](../Networks/Import/OpenStreetMap.md#sidwalks_from_osm) when building a simulation with persons. Rebuild network with **--osm.sidewalks False** to replicate the old behavior (for areas that lack OSM sidewalk data). Issue #9444
  - [gridDistricts.py](../Tools/District.md#griddistrictspy) now supports option **--vclass** for filtering taz edges in multi-modal networks. Issue #9127
  - Added tool [route2OD.py](../Tools/Routes.md#route2odpy) which generates a [tazRelation-file (OD-Matrix)](../Demand/Importing_O/D_Matrices.md#tazrelation_format) from a taz-file and route-file. Issue #9117
  - Major speedup in GTFS import with [gtfs2pt](../Tools/Import/GTFS.md). Issue #9136
  - ptlines2flows.py: Added options **--stop-duration-slack** generate schedules that are more robust with respect to traffic delays. The new default value is '10' which means that vehicles can compensate up to 10s of time loss per stop. Issue #9170
  - ptlines2flows.py: Added options **--speedfactor.bus** and **--speedfactor.tram** to allow for relaxed schedules of vehicles which may be affected by road congestion. Issue #9170
  - ptlines2flows.py: Line colors are now supported. Issue #7845
  - generateContinousRerouters.py: added option **--vlcass** to avoid errors in multi-modal networks. Issue #9188
  - generateTurnRatios.py: Added option **--interval** to write time-dependent turn counts / ratios. Issue #9294
  - duaIterate.py: Now supports options **--save-configuration** (**-C**) and **--configuration-file** (**-c**) to save and load configurations. Issue #9314
  - Added tool [computePassengerCounts.py](../Tools/Output.md#computepassengercountspy) to count passenger number in vehicle over time #9366
  - [generateParkingAreaRerouters.py](../Tools/Misc.md#generateparkingarearerouterspy)
    - added option **--opposite-visible** to ensure that parking areas on the opposite direction road are visible. Issue #9372   
    - now runs much faster. Issue #9379    
  - routeSampler.py: added option **--min-count** to set minimum number of counting locations for each used route. Issue #9415
  - Added tool [scheduleStats.py](../Tools/Railways.md#schedulestatspy) to analyze deviations between loaded public transport schedules and simulation timing. Issue #8420
  - Added tool [plotXMLAttributes.py](../Tools/Visualization.md#plotxmlattributespy) to generated 2D-plots from arbitrary attribute of XML files. Issue #9403
  - osmTaxiStop.py: added option **--fleet-size** to generated a taxi fleet along with the stops. Issue #9116
  - sumolib.net.getShortest path now allows using edges in both directions for pedestrian route search (`ignoreDirection="true"`). Issue #9533
  - [addStops2Routes.py](../Tools/Routes.md#addstops2routespy) received new options to define stops at parkingAreas, add stops to persons and to define stationary (parking) traffic. Issue #3607, #9561, #9562
  - library functions 'sumolib.xml.parse_fast' and 'parse_fast_nested' can now handle XML-comments. Issue #8565
  - Added tool [parkingSearchTraffic.py](../Tools/Output.md#parkingsearchtrafficpy) to analyze reroute times and distances for parking search traffic. Issue #9534
  - Added tool [tripinfoByType.py](../Tools/Output.md#tripinfobytypepy) for aggregating tripinfo output by vType #9596

### Other

- Miscellaneous: Renamed the "master" branch in git to "main". Issue #8591
- This will be the last time we officially release a 32bit version for Windows.
- The repository location for the open build service (RPMs for openSUSE, Fedora and derivatives) changed to https://build.opensuse.org/project/show/science:dlr
- Simulation: Default efficiency of chargingStation is now 0.95 (previously 0). Issue #9414
- TraCI: Parameter 'upstreamDist' of function 'traci.vehicle.addSubscriptionFilterTurn' was renamed to 'foeDistToJunction'. Issue #9141
- netconvert: opendrive-output now includes header comment with netconvert version and options. Issue #9454
- netedit
  - Some Undo-Redo operations are now restricted to the supermode of the operation. Issue #9097
  - TAZ and Polygons can no longer have the same id. Issue #9165
- polyconvert: When no network is loaded, output will now be in lon,lat by default (if the input is geo-referenced) in order to be useful with any network. The old behavior of writing raw utm values in this case can be restored by setting option **--proj.plain-geo false**.
- duaIterate.py option **-C** is no longer a synonym for **--continue-on-unbuild**. Instead, it is a shortcut for **--save-configuration**. Issue #9314

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
  - Fixed flickering while vehicles drive on the opposite side. Issue #8887
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
  - [generateRailSignalConstraints.py](../Simulation/Railways.md#generaterailsignalconstraintspy)
     - Fixed crash when loading stops define by edge instead of lane. Issue #8937
     - No longer generates constraint where a train waits for itself. Issue #8935
     - Fixed invalid constraint signal id if a train reverses directly after stopping. Issue #8936
     - Fixed missing constraints when a train skips a stop. Issue #8943

- Miscellaneous
  - Fixed problems with unicode paths on windows. Issue #3973
   
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
  - [cutRoutes.py](../Tools/Routes.md#cutroutespy) now handles vehicle attributes 'arrivalEdge' and 'departEdge'. Issue #8644  
  - Added new tool [stateReplay.py](../Tools/Misc.md#statereplaypy) to visually observe a simulation that is running without gui (i.e. on a remote server).
  - [generateRailSignalConstraints.py](../Simulation/Railways.md#generaterailsignalconstraintspy) can now make use of post-facto stop timing data (attribute 'started', 'ended'). Issue #8610
  - [generateRailSignalConstraints.py](../Simulation/Railways.md#generaterailsignalconstraintspy) now generates insertionPredecessor constraint to fix the insertion order after a parking-stop. Issue #8736
  - Added netdiff.py option **--remove-plain** to automatically clean up temporary files. Issue #8712
  - [gtfs2pt.py](../Tools/Import/GTFS.md) vTypes are now written as a separated output file. The name of the file can be defined with **--vtype-output**. Issue #8646
  - Added option **--dpi** to plot_summary.py and other plotting tools. Issue #8761
  - [plot_trajectories.py](../Tools/Visualization.md#plot_trajectoriespy) now supports plotting by kilometrage (fcd-output.distance). Issue #8799
  - [drtOnline.py](../Tools/Drt.md) now supports option **--max-processing** increase processing efficiency. Issue #8793
  - [flowrouter.py](../Tools/Detector.md#flowrouterpy) now support output of pedestrian flows via option **--pedestrians**. Issue #8864
  

### Miscellaneous
- tools
  - Function sumolib.xml.parse now sets the attribute `heterogeneous=True` by default. This enables parsing of all attributes even not all elements have the same set of attributes. The value `heterogeneous=False` is useful for conserving memory when all elements are known to have the same set of attributes. Issue #8651
  - Now the tool [drtOnline.py](../Tools/Drt.md) contains only the information for DRT scheduling (read fleet and requests, call a DARP solver and dispatch). To find the best routes, different methods can be used to solve the DialARideProblem (DARP), which are defined in the script "darpSolvers.py". Issue #8700

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
  - [duaIterate.py](../Demand/Dynamic_User_Assignment.md#iterative_assignment_dynamic_user_equilibrium) now supports option **--convergence-steps** which forces route choices to converge in the given number of steps (via duarouter option **--keep-route-probability**). This is recommended when using option **--logit** which otherwise may not converge at all. Issue #8550
  - [countEdgeUsage.py](../Tools/Routes.md#countedgeusagepy) now allows filtering and grouping counts by vehicle departure time. Issue #8621
  

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
  - Vehicles can now be equipped with the [glosa device](../Simulation/GLOSA.md) to adapt their speed at traffic lights. Issue #7721
  - Verbose output now lists total time spent on TraCI when applicable. Issue #8478
  - Statistic-output now includes 'totalTravelTime' and 'totalDepartDelay'. Issue #8484
  - Added option **--tripinfo-output.write-undeparted** to ensure that the number of written tripinfos is independent of insertion success (simplify comparison of different runs). Issue #8475
  - Vehicles can now be [configured to ignore specific foe vehicles at junctions](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#transient_parameters). Issue #8205
  - Option **--time-to-teleport.disconnect** now supports value 0 for teleporting without waiting. Issue #8537

- sumo-gui
  - Updated container count icon in status bar. Issue #8479

- netconvert
  - Added option **--junctions.higher-speed** to allow higher speed on internal lanes when the speed limit changes between incoming and outgoing edge. When set, speed on junction will be set to the maximum of both edges rather than the average. Issue #8535

- TraCI
  - Added traci.trafficlight functions to work with [railSignalConstraints](../Simulation/Railways.md#schedule_constraints): getConstraints, getConstraintsByFoe, removeConstraints, swapConstraints. Issue #8455, #8224
  - traci.simulation.Stage now supports function 'toXML' for python client. Issue #8517

- tools
  - Added new tool [drtOnline.py](../Tools/Drt.md) for simulation of demand responsive transport (DRT). It uses the [taxi API](../Simulation/Taxi.md#traci) with an LP-Solver to optimize shared dispatch. Issue #8256
  - Added function sumolib.route.addInternal to interpolate internal edges into a route. Issue #1322
  - sumolib.net.getShortestPath now includes internal edges in path cost and supports parameter 'withInternal' for including internal edges in the resulting edge list edges#4994
  - [gtfs2pt.py](../Tools/Import/GTFS.md) now supports option **--osm-routes** to improve mapping of gtfs data onto the network. Issue #8251
  - [edgeDataFromFlow](../Tools/Detector.md#edgedatafromflowpy) now supports cadyts output. Issue #8516
  - [splitRouteFiles.py](../Tools/Routes.md#splitroutefilespy) now handles detector file. Issue #8462
  - [routeSampler.py](../Tools/Turns.md#routesamplerpy) now supports option **--pedestrians** to generate persons instead of vehicles. Issue #8523

### Other

- Miscellaneous
  - Fixed pip package contents. Issue #8513, #8514
  - You can use libsumo and libtraci as a dependency in your maven builds by the using package repository https://repo.eclipse.org/content/repositories/sumo-releases/. Issue #7921
- Documentation
  - Documented [statistic-output](../Simulation/Output/StatisticOutput.md). Issue #8188

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
  - Fixed [simpla](../Simpla.md) crashes. Issue #8151, #8179
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
  - Fixed invalid routes when [importing MATSim plans](../Tools/Import/MATSim.md) #7948  
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
  - Actuated traffic lights based on detector gaps now support [custom detection gaps per lane](../Simulation/Traffic_Lights.md#lane-specific_max-gap). Issue #7997
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
  - Vehicle lengths will now be scaled according to [custom edge lengths](../Simulation/Distances.md) to avoid confusing visual overlap. A new vehicle visualization setting checkbox 'scale length with geometry' is provided to disable scaling. Issue #6920
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
  - Added option **--ignore-taz** to route individual trips written by [od2trips](../od2trips.md) between their assigned edges. Issue #8343
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
  - [libtraci](../Libtraci.md) is now the "officially supported" library for the Java, Python and C++ TraCI clients, providing (functional) identical interfaces for all three languages. TraaS and the old C++ TraCI API are considered deprecated.
  - Added function 'traci.simulation.getCollisions' to retrieve a list of collision objects for the current time step. This also includes collisions between vehicles and pedestrians. Issue #7728
  - Can now retrieve vehicle parameters 'device.battery.totalEnergyConsumed' and 'device.battery.totalEnergyRegenerated' when a vehicle has the battery device. Issue #6507
  - vehicle.dispatchTaxi now supports re-dispatching a taxi that is already in pickup or occupied mode. Issue #8148
  - Vehicles that are accumulating insertion delay (because they cannot safely enter the network as schedule) can now be retrieved using the functions 'traci.simulation.getPendingVehicles', 'traci.edge.getPendingVehicles' and 'traci.lane.getPendingVehicles. Issue #8157
  - Taxi customers (including those that shall be picked up but are not yet on board) can now be retrieved using `traci.vehicle.getParameter(vehID, "device.taxi.currentCustomers")`. Issue #8189
  - The reservation objects returned by [traci.person.getTaxiReservations](../Simulation/Taxi.md#gettaxireservations) now includes persons that are eligible for re-dispatch and includes the state of the reservation (new, assigned, on board). Issue #8168
  - Added function 'traci.person.splitTaxiReservation' to transport pre-made groups with multiple vehicles. Issue #8236
  - The domains 'simulation', 'junction', 'inductionloop', 'lanearea', 'multientryexit' now support setParameter and getParameter. Issue #4733, #8244
  - The value set by `traci.vehicle.setParameter("lcReason", VALUE)` will now be appended to lanechange-output. Issue #8297
  - Function 'traci.vehicle.replaceStop' now supports deleting stops and optionally teleporting to the replacement stop. Issue #8265

- Tools
  - Added [new tools](../Tools/Import/GTFS.md) to support GTFS import. Issue #4596
  - The tool [gridDistricts.py](../Tools/District.md#griddistrictspy) can be used to generated a grid of districts (TAZs) for a given network. #7946
  - [netcheck.py](../Tools/Net.md#netcheckpy) now supports option **--print-types** to analyze the edge types of the different network components. Issue #8097
  - The tool [generateRailSignalConstraints.py](../Simulation/Railways.md#generaterailsignalconstraintspy) can now handle inconsistent schedule input without generating deadlocking constraints when setting option **--abort-unordered**. Issue #7436, #8246, #8278
  - The tool [generateRailSignalConstraints.py](../Simulation/Railways.md#generaterailsignalconstraintspy) can now generate redundant constraints by setting option **--redundant TIME**. Issue #8456
  - When loading additional weights in for [duaIterate.py](../Demand/Dynamic_User_Assignment.md#iterative_assignment_dynamic_user_equilibrium), the new option **--addweights.once** controls whether the weights are to be effective in every iteration or not. The new default is to apply them in every iteration whereas previously, they were applied only in the first iteration. Issue #8249
  - Added new tool [splitRandom.py](../Tools/Routes.md#splitrandompy). to split route files into random subsets. Issue #8324
  - Added new tool [changeAttribute.py](../Tools/Xml.md#changeattributepy) to add/change/remove xml attributes. Issue #8339
  - Added new tool [computeStoppingPlaceUsage.py](../Tools/Output.md#computestoppingplaceusagepy) to compute running occupancy of parking ares from stop-output. Issue #8405
  - [traceExporter.py](../Tools/TraceExporter.md) now supports SSAM trajectories. Issue #8051

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
  - Added [public transport tutorial](../Tutorials/PublicTransport.md) (without web wizard). Issue #8108
  
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