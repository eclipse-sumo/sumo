---
title: ChangeLog
---

## Version 1.17.0 (25.04.2023)

### Bugfixes

- Simulation
 
  - Fixed crash with option **--device.rerouting.threads**. Issue #12711 (regression 1.16.0)  
  - Fixed collision at parallel lanes with different lengths. Issue #12590
  - Fixed crash when using option **--collision.action remove** and vehicles collide after lane changing. Issue #12583
  - Traffic light type `delay_based` no longer generates overlapping detectors (which could cause invalid switching decisions). Issue #12615  
  - Waypoints with attribute 'triggered' now result in an error rather than undefined behavior. Issue #12665
  - Fixed collision on junction between two conflicting links with internal junction. Issue #12715
  - Fixed invalid error: "Disconnected walk for person". Issue #12744  
  - Fixed invalid duration value in summary-output. Issue #13006
  - NEMA Controller now works with uneven yellow time at barrier. Issue #12989
  - A triggered stop is now always aborted when the vehicle is full. Issue #13015
  - Setting attribute expected or expectedContainer now always marks the stop as triggered (for person or container respectively). Issue #13016
  - Persons during access stage are now counted as walking rather than riding. Issue #13019
  - Fixed bug where the simulation does not abort when the last active transportable is riding inside an inactive (triggered) vehicle. Issue #13017  
  - Setting phase minDur=0 is now working for `actuated` and `delay_based` traffic lights. Issue #12952, #13089
  - Bike lane detector placement for 'actuated' traffic lights now uses correct bike length. Issue #13109  
  - Lane changing
    - Fixed crash related to lane-changing in tight corners. Issue #12618 (regression in 1.13.0)
    - Attribute `lcCooperative` no longer impacts speed adjustments that a vehicle needs for it's personal lane change maneuvers. #9473
    - Fixed invalid interpretation of sublane positioning of junction leaders during lane changing. Issue #12580
    - Fixed unsuitable lateral alignment on bidi edge when preparing for a turning movement. Issue #11436
    - vType attribute `lcTurnAlignmentDistance` now works for indirect left turns. Issue #8500
    - Fixed invalid strategic lane computation when lanes on the same edge differ in length. Issue #13021
    - Fixed invalid collision warning at bike-to-road merge with sublane model. Issue #13101
    - Fixed collisions at reduction in lane width when using the sublane model. Issue #13103
  - Railway simulation:
    - Simulation now terminates even when a stop with `triggered="join"` fails. Issue #12668
    - Stop attribute `extension` now works for `triggered="join"`. Issue #12666
    - Fixed unsafe train insertion that could cause a follower train to collide. Issue #12857
    - Fixed emergency braking when reversing on a red signal. Issue #12868
    - Fixed invalid railway routing result. Issue #12872
    - Fixed deadlock on bidirectional track. Issue #12858
    - Train headings now reflect the heading of the locomotive. Issue #12985

- netedit
  - Fixed bug when showing list of newly created vehicle types in type mode. Issue #12625 (regression in 1.15.0)
  - Fixed bug that prevent creation of crossing at priority junctions with speed above 50k/mh. Issue #12609 (regression in 1.16.0)  
  - The name of the current network is shown in application title bar again. Issue #12702 (regression in 1.16.0)
  - Fixed bug that caused junction shape to change on repeated computations. Issue #12584
  - Fixed running phase duration for rail crossing in parameter window. Issue #12642
  - Centering on TAZ now works if the TAZ was loaded without a shape. Issue #12687
  - Avoiding unnecessary scrollbars in combobox. Issue #12717
  - Lane positions defined before network computation are now handled if they become invalid after computation. Issue #12727
  - Additional objects are now always included in zoomed-out rectangle selection. Issue #12733
  - Fixed invalid description in undolist. Issue #12838
  - Fixed target of "right click" where a road overlaps a polygon. Issue #12819
  - Fixed wrong message in the message window when attempting to save files without permission. Issue #12992
  - Fixed crash when trying to save demand/additionals into non-writable dir. Issue #12984
  - Deleting geometry points that are in the same spot as a junction bubble now works. Issue #12964
  - Can now load TAZ with param and no shape. Issue #12678
  - Changing sumocfg options now always enables the "save sumocfg" button. Issue #12880
  - Shift-click on geometry point can now be used to make it the custom end point. Issue #12716
  - When activating sidewalks in the create edge frame, they now use the configured default sidewalk width. Issue #12449
  - Fixed crash when unjoining a joint traffic light. Issue #12610
  - Fixed "Invalid reference counter" error after recomputing with volatile options with data elements. Issue #13058
  - Netedit now gives a warning when changing user defined junction type 'rail_crossing' to priority. Issue #9273
  - Fixed bug where person plan elements after a stop where not loaded correctly. Issue #13125

- sumo-gui
  - Fixed invalid carriageLength for tram. Issue #13011 (regression in 1.11.0)
  - Fixed random (rare) crash when having an open detector attribute window. Issue #12595 (regression in 1.16.0)
  - Fixed crash when loading abstract projection in .net.xml. Issue #12762
  - Fixed freeze when selecting reachable lanes while the selection editor is open. Issue #12766
  - Fixed crash when switching to game mode in rail network. issue #12779
  - Fixed invisible lane markings at dense geometry. Issue #12837
  - 3D View (OSG view) fixes:
    - Flight mode navigation is now working on Linux. Issue #12503
    - Fixed loading of 2D viewports. Issue #12638
    - Fixed invalid interpretation of background image attribute. Issue #12671
    - Fixed usage of non-Ascii characters. Issue #12628
    - Preset visualisation scheme in GUI settings file is now used. Issue #12682
    - Now clearing background objects on loading a different simulation. Issue #12751

- netconvert
  - OSM: fixed importing lane access for `psv`. Issue #12457
  - OSM: fixed handling of one-way roads that allow buses in reverse direction. Issue #12592
  - Fixed crash when loading split beyond edge length. Issue #12695
  - Bidi status is no longer lost after symmetrical split. Issue #12698
  - Fixed invalid right-of-way rules involving indirect left turns. Issue #12676
  - Fixed invalid edge ordering for strongly curved edge. Issue #12735
  - Relaxed right of way rules in the context of dedicated lanes. Issue #12720
  - Abstract projection is now resolved when loading .net.xml. Issue #12761
  - Fixed tram connections when using option **--edges.join-tram-dist**. Issue #12767
  - Fixed crash when merging networks. Issue #12824
  - Fixed connection building in intermodal networks that could cause dead ends for passenger traffic. Issue #12978, #13087, #10080

- netgenerate
  - Fixed option category for options that apply to all network types but were placed in the `Random Network` category. Issue #12930

- meso
  - Option **--time-to-teleport.remove** is now working. Issue #12797

- TraCI
  - Fixed failure to perform unsafe lane change. Issue #13007 (regression in 1.11.0)
  - Fixed value returned by `person.getMaxSpeed`. Issue #12786 (regression in 1.15.0)
  - Fixed crash when calling `close()` directly on a traci connection. Issue #12996 (regression in 1.16.0)
  - Fixed wrong edgeId in error message of `simulation.findIntermodalRoute`. Issue #12591
  - Error when loading a state file now indicates possible version problem. Issue #12593
  - Fixed problem when loading JAVA bindings for libsumo/libtraci via JNI. Caution: Windows users must update their code to load dependent libraries explicitly due to JAVA issues that cannot be fixed on the SUMO side. Issue #12605
  - Fixed incomplete cleanup of SSM output with repeated libsumo runs. Issue #12587
  - Function `vehicle.setStopParameter` now takes effect when setting "duration". Issue #12630
  - libtraci.lane.setAllowed has been fixed
  - libtraci.simulation.getCollisions returns meaningful results Issue #12861
  - Function `vehicle.setStopParameter` now works correctly with key 'triggered'. Issue #12664
  - Error messages concerning subscription filters now use the correct command id
  - Fixed bug where user-triggered lane-change were not executed when using the sublane model. Issue #12810, #12944
  - `trafficlight.swapConstraints` now returns constraint parameters for derived constraints. Issue #12935
  - Fixed crash when using `vehicle.moveToXY`. Issue #13053  

- tools
  - Fixed invalid error when calling option **--save-template**. Issue #12589
  - Selected python tools now handle 'stdout' and 'stderr' as magic file names (i.e. *gtfs2pt.py* with more tools to follow). Issue #12588
  - Fixed broken routes for public transport from GTFS caused by invalid permissions. Issue #12276
  - tlsCoordinator.py now handles disconnected routes. Issue #11255
  - tlsCycleAdaptation.py: fixed ZeroDivisionError. Issue #12760
  - runSeeds.py: Fixed failure to run multiple configs with a list of seeds. Issue #13116

### Enhancements

- Simulation
  - Vehroute-output now includes the used vehicles for `<driving>` stage. Issue #12520
  - DriverState now optionally affects free flow speed (using param `freeSpeedErrorCoefficient`). Issue #6331
  - Element `<vTypeDistribution>` now supports attribute `probabilities` together with `vTypes` to re-use the same `<Types>` with different probabilities. Issue #12799
  - Some warnings about inconsistent public transport stop times are now avoided when using option **--use-stop-ended** along with stop attribute `ended`. #12825
  - Stop-output now includes optional attribute `usedEnded` to indicate whether a stop was affected by option **--use-stop-ended**. Issue #12863
  - Public transport vehicles may now slow down by a configurable factor when they are ahead of their schedule. (using new vType attribute `speedFactorPremature`). This is based on optional stop attribute `arrival`.  If the new option **--use-stop-started** is set, this is instead based on the optional `started` attribute of the stop. Issue #11899
  - Simulating a Two-way-left-turn-lane is now supported. Issues #12924
  - Added option **--personinfo-output** to separate `<personinfo>` elements from `<tripinfo>` elements. Issue #12929
  - vType attribute `startupDelay` is now applied to scheduled stops when using `carFollowModel="Rail"`. Issue #12943
  - Statistics-output now includes performance metrics (i.e. clockTime). Issue #12946
  - The z value is now included in emission-output if the network has elevation data. Issue #13022
  - The time spent while passing a [waypoint](Definition_of_Vehicles%2C_Vehicle_Types%2C_and_Routes.md#waypoints) is no longer counted as 'stoppedTime' in tripinfo-output. Issue #13037
  - Junction model [params](Definition_of_Vehicles%2C_Vehicle_Types%2C_and_Routes.md#transient_parameters) `ignoreIDs` and `ignoreTypes` can now be used to ignore pedestrians. Issue #13057
  - Vehicle attribute `depart` now supports the new value `begin` referring to the start of the simulation. Issue #12270

- netconvert
  - Added options **--shapefile.width** and **--shapefile.length** to allow importing custom widths and lengths from [shape files](Networks/Import/ArcView.md). Issue #12575
  - Exceptions for turning restrictions are now imported from OSM. Issue #12645
  - Option **--osm.extra-attributes** now also applies to all node attributes. Issue #12677
  - OSM: Supporting bus lane restrictions using `psv`. Issue #6767
  - junction attribute 'radius' now overrides option **--junctions.small-radius**. Issue #12816
  - merging two projected plain-xml networks with different offsets is now working. Issue #12841
  - Option **--numerical-ids** now also applies to traffic light IDs. Issue #12886
  - Building networks with Two-way-left-turn-lane is now supported. Issues #12917
  - OpenDRIVE: lane access declarations are now imported. Issue #12804

- netedit
  - Added menu entry for directly calling netgenerate and and instantly editing the generated network. Issue #2393
  - Added menu and dialogs for calling python tools without using the command line. Issue #4138
  - When calling netdiff from tools menu, the computed differences may be selectively imported for visualization (i.e. differently colored shapes for added, deleted and modified edges). Issue #2736
  - All objects with a name attribute can now be located by name using the locate-dialog. Issue #12686
  - Inspect now allows inspecting individual objects that are part of a selection via ALT+LEFT_CLICK. Issue #12690
  - In data mode, overlapped data elements list is now sorted by interval begin. Issue #11330
  - If a route is selected it will always be shown on top of other overlapping routes. Issue #12582
  - The vehicle type "DEFAULT_RAILTYPE" can now be used for defining trains. Issue #6752
  - The network is automatically recomputed (if needed) when trying to create an E2 multilane detector. Issue #12763
  - Selection files with `node:NODE_ID` are now supported (also in sumo-gui). Issue #13061
  - Delete mode now features a button to toggle all protections. Issue #13073
  - Option dialogs are now resizable. Issue #13099

- sumo-gui
  - Dynamically modified values for `latAlignment` (i.e. when preparing to turn) are now listed in the type-parameter dialog. Issue #12579
  - The attribute value that is used for scaling edge widths can now be drawn as an annotation. Issue #12544
  - Statistic output now includes person teleport count. Issue #12271
  - Option **--alternative-net-file** can now be used to load secondary network shapes. Hotkey CTRL+K can be used to switch between primary and secondary shapes. This is intended to support rail simulations where geographical and abstract shapes are available. Issue #11551
  - Added `.gif`-file support to 3D view. Issue #12672
  - Improved position and scale of vehicle lights in 3D View #12752
  - Add 3D person shapes for 3D View. Issue #11144
  - Background images can now be added via file-dialog. Issue #1627 (also for netedit)
  - Edge context-menu function *select-reachable* now ignores lane direction when selecting vClass "pedestrian". Issue #12801
  - Polygons are moved to the lowest layer upon pressing 'ALT'. Issue #12134

- meso
  - When using option **--meso-lane-queue**, laneData can now be used for lane specific outputs (except trackVehicles=true) #12854

- netgenerate
  - Added option **--spider.attach-length** to create outer extensions to the network. Issue #12928

- TraCI
  - Added function `vehicle.setLateralLanePosition`. Issue #12568
  - Function `vehicle.setStopParameter` now supports "onDemand". Issue #12632  
  - Functions [vehicle.getParameter](TraCI/Vehicle_Value_Retrieval.md#supported_device_parameters) and [simulation.getParameter](TraCI/Simulation_Value_Retrieval.md#device_parameter_retrieval) can now retrieve various aggregated trip statistics. Issue #12631
  - TraCIConstants are now available as static values for the Java bindings. Issue #12371
  - Added function `vehicle.getNextLinks` to retrieve all upcoming intersections. Issue #12551
  - Added functions `lane.setChangePermissions` and `lane.getChangePermissions` to dynamically modify 'changeLeft' and 'changeRight' attributes. Issue #12562
  - Simpla can now report platoon-based information and statistics. Issue #12124
  - Added function `traci.executeMove` to only do vehicle movements and allow further interaction before writing the outputs at `simulationStep`. Issue #11091  

- tools
  - gtfs2pt.py: multiple improvements to route matching. Issue #12834 (Thanks to Gladys McGan)
  - Visualization tools now support option **--alpha** to set background transparency. Issue #12556
  - Added new tool [generateRerouters.py](Tools/Misc.md#generatererouterspy) to generate rerouters for a given set of closed edges including automatic computation of suitable notification edges. Issue #12510
  - Added new tool [split_at_stops.py](Tools/Net.md#split_at_stopspy) to ensure that each bus/train-stop has it's own edge. Issue #12519
  - racing.py: now supports collision sound. Issue #12540
  - randomTrips.py: Added option **--random-factor** to apply random disturbances to edge probabilities (i.e. to break up symmetry in small networks). Issue #12603
  - randomTrips.py: Trips no longer start or end inside a roundabout. To restore the old behavior, option **--allow-roundabouts** can be set. Issue #12619
  - osmWebWizard.py: No longer starts/ends trips on motorways or slip roads (except at the fringe). Issue #12620
  - Improved compatibility between sumolib phase definitions and libsumo phase definitions. Issue #12131
  - Added tool [abstractRail.py](Tools/Net.md#abstractrailpy) to generate an abstract/schematic rail network based on a geodetic rail network. Issue #12662
  - Added tool [stationDistricts.py](Tools/District.md#stationdistrictspy) for segmenting a public transport network based public transport stations. Issue #12662
  - Added tool [attributeCompare.py](Tools/Output.md#attributecomparepy) to compare matching attribute across multiple files. Issue #13026
  - attributeDiff.py: Can now optionally group attributes by one or more id-attributes before comparing. #12794
  - attributeDiff.py: optionally write entries that could not be compared (via new options **-m** and **-M**. Issue #12798
  - attributeStats.py: Now supports option **--human-readable-time** (**-H**) to make attributes with large time values more legible. Issue #12822
  - attributeStats.py: Now supports option **--abs** to include statistics on absolute values. Issue #12899
  - sumolib.miscutils functions `parseTime` and `humandReadableTime` now handle negative values. Issue #12821, #12823
  - tlsCycleAdaptation.py now supports more inputs (i.e. TLS with unused stats). Issue #12777
  - routeSampler.py: Now supports option **--keep-stops** to preserve stops from the route input (*only* when the stops are child element of a route). Issue #12901
  - new research intersection Ingolstadt scenario for the SUMO game
  - Traffic light game now supports showing score for built-in adaptive algorithms. Issue #12915
  - Fixed bug where users could skip yellow phase in traffic light game. Issue #12971
  - netdiff.py: Now writes configuration-style header in diff files. Issue #13036
  - many more python tools now support configuration files
  - plotXMLAttributes.py:
    - can plot by sorting rank with attribute value `@RANK`. Issue #12607
    - can plot by input order with attribute value `@INDEX` (note that this was the behavior of @RANK in 1.16.0). Issue #12607
    - can plot number of occurrences (histogram) with attribute value `@COUNT`. Issue #11971
    - supports binning of data via options **--xbin** and **--ybin** (i.e. for making histograms)
    - supports bar plots via option **--barplot** and **--hbarplot**. Issue #12612, #12833
    - supports box plots via option **-x \@BOX** or **-y \@BOX**. Issue #11994
    - supports clamping of data range via option **--xclamp A:B** and **--yclamp C:D**

- Other
  - Further work on translation. Issue #12611
  - Added "Feedback" dialog in Help menu of netedit and sumo-gui. Issue #12550

### Miscellaneous

- Netedit help menu now links to to netedit shortcuts. Issue #12626
- Removed libsumo / libtraci from the binary windows release to avoid version mismatch. Instead the user must install the bindings for their installed python version via `pip`. Issue #11992
- Stop-output no longer contains attribute `delay` if a stop does not define the `until` attribute. Formerly, the value of *-1* would be written which is ambiguous in conjunction with negative delays caused by TraCI or **--use-stop-ended**. Issue #12883
- Added an installer for the extra version. Issue #8688
- Removed tool `plotXMLAttr.py` since it's functionality is covered by `plotXMLAttributes.py` (tool moved to 'purgatory' folder). Issue #11994.
- Netgenerate default parameters where changed to reduce the default network size for spider and random networks. Issue #12927
- Added translation to Italian. Issue #13004
- plot_net_dump.py: no longer supports setting a default color with option **-c**. Instead the long option **--default-color** must be used. The option **-c** is now used for loading of a tool configuration file instead. Issue #13068
- Added [documentation page](Netedit/attribute_help.md) for all xml elements and their attributes. Issue #12658


## Version 1.16.0 (07.02.2023)

### Bugfixes

- Simulation
  - Improved simulation speed in large networks with simulation routing. Issue #12416 (regression in 1.2.0)
  - Fixed invalid blocked state while decelerating and trying to perform lane change. Issue #12108  
  - Fixed invalid braking at internal junction. Issue #12000
  - Fixed invalid right of way rules for two conflicting connections with internal junctions. Issue #11988
  - Fixed incomplete vehroute-output when using option **--vehroute-output.sorted** and some cars or vehicle do not finish their journey. Issue #12049
  - stop-output now always contains the correct 'started' values even if other values were part of the input. Issue #12125
  - Fixed error when trying to use laneChangeModel *DK2008* with continuos lane change model. Issue #12144
  - Fixed invalid lane choice in the presence of lane change restrictions. Issue #12118, #12461
  - Fixed crash in public transport scenario with looped routes. Issue #12150  
  - Fixed invalid error when loading edgeData with negative electricity consumption. Issue #12172
  - Fixed collision on bidi edge. Issue #12393    
  - Fixed invalid switching in branching 'actuated' traffic light programs. Issue #12265
  - Fixed invalid bike lane detector placement for 'actuated' traffic lights. Issue #12266
  - Fixed vehicle angles when using 'lcSigma' with the continuos lane change mode. Issue #12201
  - Fixed inconsistency in waitingTime definition between tripinfo-output and accumulated waitingTime. Issue #12287
  - Fixed invalid lot assignment for onRoad parkingArea. Issue #12330
  - Fixed invalid change to lane with stopped leader. Issue #12113    
  - Fixed invalid error when using departSpeed values 'avg', 'last', 'desired' or 'speedLimit' near a minor link or speed reduction. Issue #12398, #12401
  - Vehicles no longer become stuck when using a waypoint with parking=true. Issue #12468
  - The simulation now aborts after an unknown stoppingPlace has been loaded for a vehicle. To obtain the old behavior, option **--ignore-route-errors** may be used. Issue #12487
  - Fixed invalid braking on junction. Issue #12511
  - Bluelight device: probabilistic rescue lane formation no longer depends on step-length. Issue #12516
  - Fixed invalid schema error when reading vehroute-output with human readable times. Issue #12545
  - Traffic light type 'delay_based' now adheres to the specified `maxDur` phase length. Previously phases could be prolonged beyond maxLength in the absence of other traffic. The old behavior can be enabled by setting `<param key="extendMaxDur" value="true"/>`. Issue #12553
  - Routing algorithm CH now uses updated travel times from routing device. Issue #9803
  - intermodal simulation
    - Fixed crash when defining ride without lines. Issue #12167 (regression in 1.11.0)
    - Fixed invalid walking distance output related to lengths of crossings and walkingArea paths. Issue #11983
    - Fixed bug where vehicle with short boardingDuration fails to board passengers after deboarding. Issue #12168
    - Fixed crash when letting persons route between identical junctions. Issue #12242
    - Containers and persons are now unloaded in parallel. Issue #12385
    - Fixed premature simulation end with triggered stop and combined loading/unloading. Issue #12386
    - Fixed unsafe deceleration at pedestrian crossing. Issue #12455
    - Pedestrian no longer walk onto crossings which are fully blocked by traffic. Issue #12507
  - railway fixes
    - Fixed invalid insertion delay on bidirectional track. Issue #12423, #12079
    - Fixed unsafe insertion after parking stop. Issue #12425
    - Fixed deadlock on bidirectional track involving trains that arrive within a block. Issue #12184
  - sublane model fixes:
    - Fixed invalid emergency braking for junction foe. Issue #12202
    - Fixed inconsistent computation for vehicle back position. Issue #12146
    - Fixed unsafe follow speed on multi-lane turning edge. Issue #12204
    - Fixed failure in cooperative speed adaptation at low step length. Issue #12283

- netedit
  - Fixed crash when adding green verge to an edge selection. Issue #12446 (regression in 1.10.0)
  - Fixed bug where stops on looped routes where saved in an invalid order. Issue #12054 (regression in 1.12.0)
  - Right click on elements above a polygon now acts on the top element again. Issue #12111 (regression in 1.14.1)
  - Fixed invalid position of start and end edge geometry points after merging geometry points in move mode. Issue #12178 (regression in 1.14.1)
  - Fixed bug where during creation of new edge, sometimes, the candidate "to" junction wasn't drawn wit magenta dotted contour. Issue #12013 (regression in 1.15.0)  
  - Fixed crash when using 'convert to roundabout' from the junction context menu. Issue #12355 (regression in 1.15.0)
  - Fixed invalid selection outline. Issue #12033
  - Fixed bug where right-click object-choice-menu lists the same object twice. Issue #12034
  - Junctions can now be deleted when covered by a walkingarea. Issue #12070
  - Junctions can be merged by moving when the background grid is active. Issue #12080
  - Fixed crash when using option **--prefix**. Issue #12024
  - Fixed bug that permitted invalid combination of stops and and via attribute. Issue #11961
  - The grouping of inspected overlapped elements no longer includes invisible elements. Issue #12126
  - Custom `loadingDuration` and `boardingDuration` are written as s instead of ms (also affects duarouter). Issue #12383
  - Fixed invalid default edge attributes after setting a template. Issue #12392
  - Moving a selected edge with selected junctions now also moves the custom edge end points. Issue #12445
  - Fixed invalid conversion between periodic flow rate and poisson flow rate. Issue #12451
  - Calibrator-flow attributes `begin` and `end` are now loaded correctly. Issue #12470

- sumo-gui
  - Fixed invalid camera position after tracked vehicles exits the simulation. Issue #12137 (regression in 1.13.0)
  - Fixed invalid right-click target when there is a pedestrian crossing on top of a polygon. Issue #12523 (regression in 1.15.0)
  - Pedestrians now follow the exact shape of access lines while in access stage. Issue #12116
  - Aggregated warning summary is now written at simulation end. Issue #12209
  - Fixed invalid objects in right-click disambiguation menu. Issue #12046
  - Fixed initial positions of some dialogs that were too high. Issue #11936
  - Fixed crash on saving gui settings to registry (debug mode only). Issue #11595
  - Pressing Escape key in OSG view no longer makes it unresponsive to further control input. Issue #12313
  - Trailer and Semi-Trailer vehicles are now drawn correctly during opposite-overtaking. Issue #12331
  - Fixed bad default settings when loading a 2D viewport in OSG view. Issue #12348
  - Fixes 3D view boundary coordinates after rotating the view. Issue #11941
  - Fixed crash on quick-reload reloading. Issue #12367
  - Fixed crash when using 'select reachability' (related to negative edge speeds). Issue #12400, #12403
  - Reload hotkey no longer takes effect when running with TraCI. Issue #12431
  - Fixed invalid pedestrian position while passing a short walkingarea. Issue #12456
  - Right-click on vehicles and person no longer fails on very wide lanes. Issue #12505
  - Fixed fluctuating rail width when using edge size-exaggeration at varying zoom levels. Issue #11832

- meso
  - Stopping at pos=0 is now working. Issue #12240
  - Picking up persons and containers with `lines="ANY"` is now working. Issue #12241

- netconvert
  - Fixed xml-validation error when importing MATSim network. Issue #12509 (regression in 1.15.0)
  - Fixed unnecessary dead-end lanes at large intersections. Issue #2472
  - Fixed invalid OpenDRIVE output when writing traffic signals with signal groups. Issue #11980
  - Fixed invalid OpenDRIVE output for lefthand networks. Issue #11995, #12038, #12047  
  - Fixed invalid right of way rules when connections from the same edge merge and both have internal junctions. Issue #11988  
  - Fixed invalid right of way rules causing mutual conflict at multimodal priority-junction. Issue #5609
  - Fixed bug where **--junctions.join** failed in multimodal networks. Issue #6495
  - Fixed bug where **--junctions.join** joins to much. Issue #10589
  - Fixed invalid dead-end when using option **--osm.turn-lanes**. Issue #12042
  - Fixed invalid plain xml output (after reading an invalid network). Issue #12086  
  - Fixed invalid guessed connections. Issue #10771, #10978, #2472, #12181, #12327
  - Fixed missing bidi edges in generated network. Issue #12127
  - OSM import no longer ignores `spreadType` in typemap. Issue #12141
  - Fixed invalid bike lane in OSM import. Issue #12216
  - Fixed invalid bidirectional combined foot/bike path in OSM import. Issue #12214
  - Traffic light building on large intersections now adds extra red time after left-turn phases to ensure safety. Issue #10796  
  - Fixed creation of invalid pedestrian crossings. Issue #12234
  - Added missing (guessed) connections when an outgoing edge has additional lanes. Issue #8899
  - Fixed invalid link state at zipper junction. Issue #7676
  - Prohibitions involving edges with underscore in their name are now working. Issue #12419
  - Turnarounds are now correctly added if the innermost lane prohibits passenger traffic. Issue #12447
  - OpenDRIVE-imported networks no longer include guessed turn-arounds at real intersection by default (option **--no-turnarounds false** can be used to replicate the old behavior). Issue #12448
  - Line name from **--ptline-files** is now exported. Issue #12497
  - A `<split>` at pos=0 no longer ignores speed. Issue #12526
  - Distance (kilometrage) is now preserved when adding `<split>` element. Issue #12527
  - Custom edge length is now preserved when adding `<split>` element. Issue #12529
  
- polyconvert
  - Fixed invalid polygon output for some line based inputs. Issue #12161
    
- duarouter
  - Fixed crash on unknown landmark edge. Issue #12438
  - Fixed invalid routing result by routing algorithm  A* / "astar" (minor difference caused by internal edge lengths). Issue #12463
    
- TraCI
  - Fixed moveToXY failure at parallel internal junction. Issue #12065
  - Foe lanes for crossings can now be retrieved. Issue #12059
  - `Connection.close()` now closes simulation. Issue #12133
  - Fixed invalid behavior when mixing stops on normal and internal lanes. Issue #11885
  - Function `traci.edge.setAllowed` is now working. Issue #12305
  - Context subscriptions to the simulation domain now always return all requested objects regardless of range argument. Issue #12306
  - `trafficlight.swapConstraints` now preserves params (and swaps params for `bidiPredecessor`). Issue #12326
  - Fixed problems when using libsumo with gui. Issue #12285, #12021
  - Fixed crash on rerouting after insertStop/replaceStop. Issue #12387
  - Signal states are now updated immediately after calling `trafficlight.setProgramLogic`. Issue #12414
  - Cycle length is now updated after call to `traci.trafficlight.setProgramLogic`. Issue #12357

- Tools
  - plot_net_dump_file.py: plotting a single measure is working again. Issue #11975 (regression in 1.15.0)
  - generateTurnRatios.py: fixed unsorted intervals in output when using python2. Issue #12019
  - tlsCycleAdaptation.py: Fixed bug where controllers differed in cycle length when setting option **--unified-cycle**. Issue #12045
  - tlsCycleAdaptation.py: Fixed mismatch between optimal and actual cycle duration. Issue #12068
  - generateRailSignalConstraints.py: Fixed inconsistent bidiPredecessors. Issue #12075
  - generateRailSignalConstraints.py: Added missing insertion constraint after invalidated stop. Issue #12474
  - tracemapper.py: Fixed duplicate consecutive edges in route. Issue #12094
  - cutRoutes.py: No longer writes persons without a plan. Issue #12245
  - routesampler.py: Option **--total-count** is now compatible with **--weighted**. Issue #12284
  - net2geojson.py: Fixed crash when trying to import network without geo projection (now gives an error message instead). Issue #12295
  - gtfs2pt.py: Fixed missing transport modes. Issues #12277
  - gtfs2pt.py: Fixed mapping of bus and tram stops in multimodal networks. Issue #11802, #11849
  - gtfs2pt.py: Output is now usable with duarouter. Issue #12333

- All Applications: 
  - Fixed XML-validation error when SUMO_HOME is not set. Issue #12138 (regression in 1.15.0)
  - Fixed crash if gzipped outputfile cannot be opened. Issue #11954

### Enhancements

- Simulation
  - Elements `<ride>` and `<transport>` can now be defined without attribute `lines` and default to a value of `ANY` (taking any eligible vehicle that stops at the destination). Issue #12167
  - Option **--fcd-output.attributes** now supports the attribute `speedLat` for writing lateral speeds. Issue #12213
  - Stops now support attribute `jump="TIME"` to model explicit jumps (teleports) between disconnected locations. Issue #12268
  - The randomness in rescue lane formation can now be configured with bluelight device [parameters](Simulation/Emergency.md#further_parameters). Issue #12437
  - Added option **--pedestrian.striping.reserve-oncoming.max** (default 1.28) to configure an upper limit on stripes to reserve for oncoming pedestrians. Issue #12506

- netconvert
  - The right-of-way rules to take effect when switching a traffic light off, can now be configured as 'allway_stop'. This is the new default for NEMA-type controllers. Issue #12043
  - Improve traffic light programs in networks with separated bicycle paths. Issue #10039
  - OpenDRIVE outputs now supports export of loaded POIs and polygons as road objects. Issue #12060
  - When setting option **--tls.guess**, roads without conflict are excluded from the threshold-heuristic. Issue #6513
  - Improved heuristic for generating rail connections at sharp angles. Issue #12119
  - Warnings of E3 detectors and from the SSM device can now be aggregated. Issue #12149
  - Individual lane widths are now import from OpenStreetMap. Issue #12162
  - Lane divider style can now be exported from OSM to OpenDRIVE. Issue #12158
  - Traffic signs from OSM can now be exported to OpenDRIVE. Issue #12231
  - Pedestrian crossings are now exported to OpenDRIVE. Issue #12229
  - Added option **--osm.crossings** to import pedestrian crossings from OSM. Issue #12238
  - Now separating lanes for bikes and pedestrians according to OSM declarations. Issue #12215
  - Option **--osm.bike-access** now serves to add any additional bike lanes declared in OSM without the need to load another typemap (The typemap may still be used to customize bike lane widths for different categories of lanes). Issue #12228
  - Added option **--tls.rebuild** to rebuild all loaded traffic light plans. Issue #12250
  - Added option **--tls.guess-signals.slack** to identify more controlled intersections by guessing from surrounding *simple* tls nodes. Issue #12249
  - Option **--tls.guess-signals** no longer requires signals on non-passenger edges to interpret an intersection as controlled. Issue #12260
  - Option **--junctions.join** can now join intersections with more than 4 incoming edges. Issue #12261
  - Walkingarea shapes for pure pedestrian intersections now match the junction shape. Issue #12377
  - Added option **--plain-output.lanes** to include all lane attributes in plain-xml output. Issue #12443

- netedit
  - It is now possible to load and save a *.sumocfg* file and also to edit all sumo options (SHIFT-F10). Issue #11896
  - If a .sumocfg is set, it will be used automatically when launching sumo from the menu. Issue #12450
  - Added support for saving and loading *.neteditcfg* files to simplify simulation project management. Issue #12309
  - Added TimeStamp in Undo-Redo list. Issue #11744
  - Now drawing red line between edge geometry points (if the points are not along the edge). Issue #11530
  - Can now disable drawing dotted contours in visualization settings. Issue #11662
  - Now showing a warning dialog if user tries to create a TLS in a junction with all connections set as uncontrolled. Issue #6382
  - Removed dialog-button for permission 'disallow' attribute to avoid confusion. Issue #11940
  - Saved sumocfg now includes version data. Issue #11294
  - Multiple persons or personFlows in the same location are now indicated. Issue #10724
  - Added MeanData mode to create and modify `<edgeData>` and `<laneData>` definitions. Issue #11897
  - Recently used files are now in a sub-menu. Issue #12025
  - Geometry points are now drawn above everything else in move mode. Issue #11725
  - Geometry points now change color in move mode to indicate whether a click would create or merge points. Issue #12177
  - Move mode can now toggle whether closely spaced geometry points shall be automatically removed. Issue #12244
  - Stops now support attribute 'jump'. Issue #12269
  - Crossing mode now ensures that only sensible crossings can be defined. Issue #12366
  - Vehicle inspect mode now allows selecting vehicle type from a drop-down list. Issue #12379
  - Added edge color legend to person and container mode. Issue #11613
  - Added edge color legend when creating e2 detectors along multiple lanes. Issue #11334
  - Moving a single selected edge now moves its whole geometry including endpoints. Issue #12442
  - Parameters for the W99 carFollowModel can now be configured. Issue #12290

- sumo-gui
  - When option **--use-stop-ended** is set, show-route mode now labels the 'ended' time of stops. Issue #11833
  - The view now updates after loading a selection file. Issue #12191
  - Whenever the simulation has tripinfo-devices, the trip based statistics are available in the network parameter dialog. Issue #12207
  - A rainbow color scale can now be defined between upper and lower "hiding" thresholds even when there is no data yet. Issue #11978
  - Aggregated detector values are now listed in their respective parameter dialog. Issue #12031
  - Detector outputs are now flushed at simulation end even while the gui remains open. Issue #12293
  - Added guiShape "aircraft". Issue #12314
  - Added vehicle setting to maintain orientation after reversal. This achieves a more realistic visualisation of reversing trains and (grounded) aircraft. Issue #12140
  - Added settings to show/hide HUD elements in 3D view. Issue #12294
  - Added terrain to 3D view (using flat background color) #12279
  - Added Fly Movement mode in SUMO 3D View for Windows OS (Linux still pending). Issue #11473
  - Color legend decoration now includes the name of the coloring scheme. Issue #11967
  - All dialogs now remember their previous position. Issue #11962
  - Window menu now includes entries for opening a new view. Issue #12417
  - Train insertion that is delayed due to oncoming trains is now indicated in the lane parameter dialog. Issue #12421
  - Added vClass icons in the "Select lanes which allow" and "Select reachable" lists. Issue #12429
  - Scaling edge width by loaded edge data is now supported. Issue #9216

- TraCI
  - Added functions `vehicle.getDeparture` and `vehicle.getDepartDelay`. Issue #3036
  - Added functions to retrieve aggregated inductionLoop detector measures. Issue #12030
  - Added functions to retrieve aggregated lanearea detector measures. Issue #12029
  - Added functions `vehicle.getLoadedIDList` and `vehicle.getTeleportingIDList` to retrieve the corresponding vehicles (some of which could not be retrieved with getIDList). Issue #2338
  - Simpla: The maximum length of a platoon can now be configured. Issue #11426
  - The JAVA bindings now facilitate casting of subscription results. Issue #8930
  
- Tools
  - osmWebWizard.py: Add checkboxes to select/deselect groups of elements in the road types tab. Issue #10692
  - runSeeds.py: Now forwarding unknown options to application call. Issue #12312
  - runSeeds.py: Now supports setting a list of applications and a list of configurations to run all at once (with results in subfolders). Issue #12311
  - routeSampler.py: Major improvement in sampling speed. Issue #12319
  - routeSampler.py: Full optimization now skips initial sampling for further speed-up. Issue #12307
  - [attributeStats.py](Tools/Output.md#attributestatspy): Permit parsing multiple elements and attributes at once. The new default is to parse all elements and attributes. Issue #12317
  - attributeStats.py now includes count in xml-output. Issue #12337
  - gtfs2pt.py: Now writing short route id and headsign as params. Issue #11819
  - plot_trajectories.py: Now support common visualization options. Issue #11991
  - Many visualization tools can now configure linestyle and marker style. Issue #11985
  - countEdgeUsage.py: Now permits loading named routes. Issue #12010
  - implausibleRoutes.py: Added options **--additional-file**, **--unsorted-input** and **--ignore-errors** which are passed to duarouter when needed. Issue #12090  
  - implausibleRoutes.py: Added options to handle unsorted input and additional files when duarouter is being called. Issue #12069
  - tracemapper.py: Added option **--vehicle-class** to guide edge mapping. Issue #12117
  - net2geojson.py: Added option **--boundary** to write polygons instead of center lines. Issue #12296
  - stateReplay.py: Now works on Windows. Issue #12298
  - checkStopOrder.py: now indicating terminal stops and overtaking events in **--stop-table** output. Issue #12471
  - generateRailSignalConstraints.py: Now handling actual times ahead of schedule. Threshold configurable with new option **--premature-threshold**. Issue #12530
  - Added new tool [filterElements.py](Tools/Xml.md#filterelementspy) to filter elements from an xml file (either all instances or filtered by attribute value). Issue #12304
  - Added new tool [attributeDiff.py](Tools/Output.md#attributediffpy) to compute the numerical difference between two xml files with the same structure. Issue #12318
  - Added new tool [fcdDiff.py](Tools/Output.md#fcddiffpy) to compare two fcd-output files (by vehicle id and time). Issue #12233
  - Added new tool [fcdReplay.py](Tools/Visualization.md#visulizing_fcd-data_as_moving_pois) to review a fcd recording as moving POIs (optionally alongside a running simulation). Issue #12433
  - plotXMLAttributes.py
    - can now plot data without assigning ids to the data points. Issue #11969
    - can now plot categorical (non-numerical) data and also a mix of data types. Issue #11970, #11976
    - categorical labels can be sorted and filtered by loading a list of labels with option **--xticks-file** and **--yticks-file**. Issue #12091
    - now supporting additional visualization options (linestyle, markers, grids, ...). Issue #11972
    - plots a marker on a single point if the input data is only one point. Issue #11974
    - Added the possibility to use wildcards with option **--filter-ids**. Issue #11981
    - Attribute options now permit setting a list of attributes. Issue #12015
    - Can now display times as hours by setting **--xtime0** and **--ytime0** (also applies to other plotting tools). Issue #12011
    - csv-output is now directly usable with gnuplot. Issue #12345

- Other:
  - Added option **--language** to select language for warnings and messages of all applications. Issue #11237
  - Added *Language* menu to sumo-gui and netedit to set the interface language persistently. (Can also be set temporarily with **--language**) Issue #12518
  - You can help to complete and improve the translation at https://hosted.weblate.org/projects/eclipse-sumo/ Issue #11859

### Miscellaneous

- Added exemplary plots to many simulation-output documentation pages (with linked example commands to create them).
- Netconvert options for writing polygons are now documented. Issue #12135
- Updated Windows MSVC runtime libraries and Xerces-C to 3.2.4
- Improved pydoc for TraCI functions that modify vType attributes. Issue #11943
- Added all required fmi functions required for FMPy version 0.3.13. Issue #12199
- Removed Netedit options **--SUMOConfig-output, --additionals-output, --demandelements-output, --dataelements-output**. Instead, the corresponding input file path is used for output. Issue #12388
- Some netedit buttons for saving are now grouped in a sub-menu to avoid clutter. Issue #12360
- Test extraction now works for the San Pablo Dam tutorial. Issue #12466
- The netdiff tool accessible via the 'Tools' menu has been temporarily disabled due to stability issues. #12435


## Version 1.15.0 (08.11.2022)

### Bugfixes

- Simulation
  - Fixed crash when using bluelight vehicle and SSM device. Issue #11336 (regression in 1.12.0)
  - Rerouter attribute `timeThreshold` is working again after a vehicle has changed lanes. Issue #11405 (regression in 1.14.0)
  - Taxi drop-off is no longer interrupted by new dispatch. Also, persons only continue their plan after the drop-off duration. Issue #11311  
  - Fixed bug where emergency vehicle fails to overtake. Issue #11345
  - Fixed bug where emergency vehicle performs invalid (unstrategic) lanechange. Issue #11337
  - A persons individual speedFactor is now applied during access stage. Issue #11452
  - Fixed invalid pedestrian routing when using the *striping* model to navigate within an intersection. Issue #11674
  - Fixed invalid parking maneuver times. Issue #11420  
  - Option **--scale** and vType attribute `scale` now apply evenly to all defined `<flow>` elements when set to values below 1. Issue #11441  
  - Fixed invalid braking at intersection in sublane simulation. Issue #11484
  - Fixed emergency braking at lane-width change in sublane simulation. Issue #11467, #11639
  - Fixed junction collision while turning with the sublane model. Issue #11482  
  - Fixed invalid (slightly exaggerated) estimate of junction passing time. Issue #11106  
  - Fixed prolonged waiting at side roads despite sufficient gaps. Issue #11106  
  - Cars generated for a `<personTrip>` now slow down before dropping of the person. Issue #11532  
  - Fixed vehicle collision with pedestrian on shared lane after lane width change. Issue #11602
  - Fixed emergency breaking near shared walkingarea. Issue #11478  
  - Fixed invalid junction collision warning involving short approach edges. Issue #11609
  - Fixed crash on inconsistent opposite-direction edge definitions. Issue #11661
  - Fixed deadlock between car and pedestrian on shared walkingarea. Issue #11734
  - Fixed error when a personTrip with toJunction is followed by another trip. Issue #11820
  - Fixed incorrect/delayed vehicle inserting on bidi-edge. Issue #11419
  - Fixed collisions on bidi-edge. Issue #11477
  - Fixed missing error after routing failure. Issue #11840
  - Fixed invalid slow-down on intersections by vehicles equipped with bluelight device due to `driveRedSpeed`. Issue #11878
  - Fixed bug where person/container stop does not terminate. Issue #11900
  - Fixed emergency braking when using carFollowModel IDM. Issue #11498, #11564, #11564
  - Fixed emergency braking when using carFollowModel CACC. Issue #11679, #11653
  - Fixed emergency braking and collisions when using carFollowModel ACC. Issue #4551
  - EIDM carFollowModel:
    - Fixed multiple EIDM issues related to imprecise driving at stop lines. #11242, #11182, #11183
    - Fixed bug where vehicles did not reach a defined stop. Issue #11364
    - Fixed collision. Issue #11361
    - Slow-to-start now works after stopping. Issue #11374
    - Fixed emergency braking when approaching stop. Issue #11461
  - lane changing
    - Fixed bug where vehicles take to long to overtake pedestrians. Issue #11608
    - Fixed bug where vehicle drives outside lane bound after lane width change. Issue #11468
    - Fixed emergency braking during opposite direction driving. Issue #11481
    - Fixed bug where car blocks itself during continuous laneChange after loading state. Issue #11394
    - Fixed invalid speed adaptations for lane changing while on an intersection. Issue #11507
    - Fixed strategic change happening despite lcStrategic=-1. Issue #11752
    - A stopped leader now triggers strategic changing in the sublane model. Issue #11773
    - Fixed bug where lane-changing deadlock-avoidance caused excessive braking. Issue #11695
  - output
    - fcd-output now includes riding persons even if their vehicle is not equipped with fcd device. Issue #11454
    - fcd-output of persons now respects edge and shape filters. Issue #11455
    - Output of a persons speed in access stage is now correct (was given as 0 before). Issue #11453
    - Fixed several bugs that prevented intermodal vehroute output from being re-used as simulation input. Issue #7006
    - Vehroute-output no longer includes unfinished persons by default. Issue #11730
  - railways:
    - Fixed unsafe train insertion with oncoming vehicle. Issue #11384
    - Fixed invalid error when trying to insert train before red signal with high speed. Issue #11440
    - Fixed emergency braking due to unsuitable rail signal choice between rivaling trains. Issue #11442
    - Fixed railway routing failure. Issue #11835
  
- netedit
  - Fixed missing coordinate indicator in status bar. Issue #11230 (regression in 1.14.0)
  - Loading an additional file and saving modifications no longer prompts for a file name. Issue #11030
  - Clicking over column labels no longer clears the traffic phase table. Issue #11240
  - Flow probability is no longer limited to full percent. Issue #11259
  - Directional arrows are now drawn on top of detectors. Issue #11381
  - Fixed bug that prevented bidi-edges from being defined. Issue #11397
  - Fixed bugs related to object locking (via the lock menu). Issue #10863
  - Additional object chooser no longer lists POIs and polygons. Issue #11580
  - Fixed invalid context menu after clicking on overlapped lanes. Issue #11577  
  - Junctions covered by a walkingarea can now be moved. Issue #11622
  - In tls mode, icons are now drawn on top of walkingarea shapes. Issue #11302
  - Setting a numerical value for departPosLat is now working. Issue #11694
  - Fixed parsing of `<closingReroute>` permissions. Issue #11699
  - Ids of crossings and walkingareas can now be drawn. Issue #11664
  - Visual scaling for selected data elements now works. Issue #10937
  - Fixed crash when using the "undo/redo history" dialog. Issue #11370
  - Setting custom arrivalPos in personPlan frame is now working. Issue #11800
  - Continuous PersonPlans between Junctions can now be created. Issue #11813
  - Walks over routes can now be created. Issue #11845
  - Subsequent stops can now be created for personPlans and containerPlans. Issue #11848
  - Fixed invalid geometry when splitting edge and there is a parallel edge connection the same junctions. Issue #11837
  
- sumo-gui
  - Fixed crash when opening busStop parameters after simulation end. Issue #11499 (regression in 1.13.0)
  - Lane menu functions *select reachable*, *close lane*, *add rerouter* are now working again. Issue #11448 (regression in 1.14.0)
  - Directional arrows are now drawn on top of detectors. Issue #11380
  - 3D view now updates lane colors after updating of selected lanes. Issue #10908
  - Fixed invalid occupancy values caused by vehicles that are partially on a lane (i.e. with the sublane model). Issue #11404
  - Opening the object chooser now brings existing dialogs into focus. Issue #11579
  - Detector names are now shown in the parameter dialog. Issue #11029
  - TAZs parameters are now loaded. Issue #7479
  - Fixed insufficient precision when saving edge scaling scheme. Issue #11711
  - Fixed corrupted 3D view after window resize and minimize operation. Issue #11727
  - Initial camera coordinates are now matching for the 3D view. Issue #11742
  - Fixed invalid displayed settings for asymmetrical background grid. Issue #11809
  - Fixed appearance of guiShape=rail when applied to "normal" cars. Issue #11919
  - Fixed invalid lateral rendering position of vehicles with potential carriages (i.e. 'trailer'). Issue #11901

- netconvert
  - Fixed invalid red phase at traffic lights with very low connection speeds. Issue #11307 (regression in 1.14.0)
  - Fixed invalid turn-around connection at roundabout with unusual geometry. Issue #11344
  - Fixed projection error when importing OpenDRIVE. Issue #11263
  - Fixed too short busStops when import OSM data. Issue #11372
  - Option **--geometry.remove** now works if edges are running on top of each other (i.e. tram and road). Issue #11516
  - Fixed crash when loading ptstops and reducing the network extent. Issue #11546
  - Fixed bug that caused internal junctions for railways. Issue #11558
  - Fixed invalid walkingarea shapes. Issue #11590
  - Lane types are now preserved after `<split>`s. Issue #11592
  - Tram edge merging now preserves origId. Issue #11582
  - Fixed bug where tram stop was assigned to street instead of rail. Issue #11645
  - Fixed invalid bidi edges with **--ptline-output**. Issue #11497
  - Invalid state string length now always triggers a warning. Issue #11637
  - Activating option **--ptline-output** no longer modifies the network. Issue #10732
  - Fixed bug that caused inconsistent opposite-edge declarations to be written. Issue #11731
  - OSM import: fixed public transport stop assignment to edge without proper permissions. Issue #11656
  - Fixed missing bidi edges on long rail edges with multiple stops after topology repair. Issue #11828
  - OSM: Fixed invalid public transport route on bidirectional way. Issue #11829
  - Fixed crash while processing public transport stops. Issue #11839
  - Filtering access from ptstops input. Issue #11963

- duarouter
  - vTypeDistributions with attribute `vTypes` now consider vType-probabilities. Issue #11376

- jtrrouter
  - Fixed infinite loop when loading poisson flow. Issue #11834

- polyconvert
  - Option **--prune.in-net.offsets** can now specify all possible offsets. Also the interpretation was changed, so that positive values now cause enlargement on all sides. Issue #11438

- od2trips
  - XML validation now permits taz-relation intervals with human readable times. Issue #11554
  - Option **--scale** now works when loading tazrelation-files and amitran-files. Issue  #11853

- marouter
  - Option **--scale** now works when loading tazrelation-files and amitran-files. Issue  #11853

- TraCI
  - Function vehicle.highlight is now tracking the vehicle again. Issue #11352 (regression in 1.13.0)
  - Simpla: fixed crash due to unclear speed factor semantics. Issue #11223
  - Simpla: fixed invalid behavior when subsequent edges vary in lane number. Issue #11276
  - Simpla: fixed input error handling. Issue #11789
  - traceFile no longer contains redundant calls to helper method `person.removeStages`. Issue #11418
  - Libsumo can now be compiled without having fox-toolkit installed. Issue #11115
  - Libsumo simulation outputs now include version information. Issue #11808
  - Context subscription filters for vTypes and vClasses are now compatible with all other filters. Issue #11540
  - Function `simulation.getDistance2D(..., isGeo=True)` now works. Issue #11610  
  - Fixed error after Canceling stop via TraCI and then calling moveToXY. Issue #11870
  - Stops may now be defined at position=0. Issue #11891
  - Fixed bug where CACC model had invalid speed while activating [ToC](ToC_Device.md). Issue #6192
  - View boundaries returned for a 3D view now better match the actual field of vision. Issue #11471
  - Fixed memory leak when closing the last connection

- Tools
  - randomTrips.py and duaIterate.py now properly report unknown options again. Issue #11258 (regression in 1.14.1)
  - randomTrips.py: fixed error when combining options **--additional-files, --vtype-output, --vehicle-class**. Issue #11431
  - randomTrips.py: Option **--fringe-factor** is now applied for pedestrians. Issue #11909
  - tlsCoordinator.py: Now handles rail crossings. Issue #11250
  - tlsCycleAdaptation.py: Now handles unsorted route files. Issue #11251
  - plot_net_dump.py: Fixed invalid error message when closing figure window. Issue #11280
  - plot_net_dump.py: fixed error when trying to plot homogeneous edgeData. Issue #11351
  - generateRailSignalConstraints.py: Fixed missing inactive insertion constraints when **--write-inactive** is set. Issue #11375
  - generateRailSignalConstraints.py: Added missing constraints from bidirectional stop usage. Issue #11371
  - generateRailSignalConstraints.py: now detecting swapped ended times. Issue #11687, #11741   
  - Fixed use of `gapPenalty` parameter in function `sumolib.route.mapTrace`. Issue #11292  
  - osmWebWizard.py Now aborts early if the OSM download fails and no longer leaves empty directories behind. Issue #11722
  - osmWebWizard.py Now gives better feedback on certificate errors. Issue #10804
  - turnfile2EdgeRelations.py: Fixed invalid end element, preserving comments. Issue #11748
  - gtfs2pt.py: Now handling empty timetable. Issue #11763
  - gtfs2pt.py: Fixed invalid route references in output. Issue #11797
  - routeSampler.py: Fixed error when using **--min-count**. Issue #11915

### Enhancements

- Simulation
  - Added option **--tls.actuated.detector-length** to set the default length of actuation detectors. Issue #11335
  - Verbose output and **--statistic-output** now always include the count of vehicles for the trip statistics. Issue #11366
  - carFollowModels *ACC* and *CACC* can now be configured with vType attribute 'collisionAvoidanceOverride' (previously hard-coded to *2*). Issue #11383
  - Tripinfo-output for persons now includes the individual speedFactor (in personInfo). Issue #11450
  - The new vType attribute [`desiredMaxSpeed`](Simulation/VehicleSpeed.md#desiredmaxspeed) can be used to configure speed distributions for vehicles that are not constrained by the road speed limit (i.e. bicycles). Issue #11102
  - Bicycles speeds now follow a speed distribution by default (centered on 20km/h, configurable with `desiredMaxSpeed`). Issue #11102
  - Option **--emission-output.precision** now also applies to emissions in tripinfo-output. Issue #11474
  - Added option **--time-to-teleport.bidi** to resolve deadlocks on bidirectional edges. Issue #11435
  - The vType attributes `timeToTeleport` and `timeToTeleportBidi` can now be used to customize teleporting behavior. Issue #9497
  - Pedestrians now react to edge speed limit (i.e. for modelling stairs). Issue #11526
  - Added option **--vehroute-output.internal** to include internal edges in the output. Issue #10601
  - Specifying pedestrian `<walk>` with `departPosLat="random"` is now supported. Issue #10573
  - Option **--devices.ssm.measures** now supports comma-separated values. Issue #10478
  - When using the special string 'TIME' in file names, all written files now use the same time stamp. Issue #10346
  - Added option **--pedestrian.striping.walkingarea-detail INT** to increase the smoothness of pedestrian trajectories. Issue #8797
  - Added option **--summary-output.period** to reduce the frequency of summary output. Issue #2445
  - The slope of a vehicle now takes into account it's length. Issue #8802
  - Rail signal constraints now support loading generic parameters. Issue #11880
  - Added options **--edgedata-output FILE** and **--lanedata-output FILE** to configure a very basic meanData output without the need for additional files. Issue #11939
  - [Wireless simulation](Simulation/Bluetooth.md) (btsender and btreciever devices) can now be enabled for persons. Issue #11950

- netedit
  - Context menus on ambiguously overlapping objects now gives a choice of object. Holding ALT always gives all choices. Issue #10916
  - Saved detector names use descriptive tags instead of the 'E1,E2, ...' tags. Issue #11028
  - Hovering over an attribute now displays it's full value in a tooltip if field is too narrow. Issue #11194
  - Added button to inspect vTypes/vTypeDistributions during vehicle inspection. Issue #10041
  - Added edge color legend in vehicle mode. Issue #10866
  - Now using distinct default id-prefix for `flow`, `personFlow` and `containerFlow`. Issue #11077
  - Adding network-recomputation status in the lower right corner. Issue #4725
  - Adding a reverse edge now supports adding an edge with the same or with shifted geometry for all spreadType values. Issue #11046
  - Add shortcut to open current location in Google Maps and OSM map to context menu. Issue #10506
  - A sumo configuration file can now be saved directly. Issue #11036
  - Joined traffic lights can now be defined for selected junctions via the context menu. Issue #11396
  - Create edge frame has two new checkboxes (*disallow pedestrians,add sidewalk*)  to simplify creation of edges for pedestrian simulation. Issue #10969
  - In networks with pedestrian crossings, create edge frame disallows pedestrians on road lanes by default. Issue #10970
  - In move mode, alt+right-click on overlapped objects now allows marking the front element to be moved. Issue #10900
  - In vehicle mode: alt+right-click on overlapping routes, now allows marking the front element to be used when creating 'vehicle over route'. Issue #11009
  - In delete mode, ambiguous clicks now open a context menu for selecting the intended target. Issue #11606
  - In delete mode, the object to be deleted is now outlined. Issue #11636
  - Button tooltips can be enabled/disabled with a persistent toggle-button. Issue #11550
  - Creating accidental double stops for the same vehicle is now prevented. Issue #10078
  - Holding the middle button now allows panning the view (also in sumo-gui). Issue #11632
  - "Undo-Redo list" dialog now includes color codes and entity ids. Issue #4765
  - Using distinct walkingArea color to distinguish them from other objects. Issue #11724
  - Every vClass has it's own icon now. Issue #9872, #11801
  - Kilometrage at cursor position is now shown in the edge context menu. Issue #11815
  - Added (optional) tooltips in attribute labels. Issue #11490
  - Hierarchical selection can now select connections and crossings as children. Issue #11863
  - Hierarchical selection now uses undo-group. Issue #11917
  - In data mode, the visualized attribute now remains active when changing between intervals. Issue #11881
  - By default, network geometry is now computed before entering data mode (to improve visualization of turn counts). To improve performance in large networks, this can be disabled with a (persistent) setting in the Processing menu. Issue #11918
  - Can now set and apply edge templates via context menu. Issue #2353
  - Traffic light mode:
    - phase table now permits moving phases up and down. Issue #10856
    - Added buttons reset either the current program or all programs of the current traffic light to their default. Issue #9072, #11357
    - phase table now shows index of each phase. Issue #11343
    - phase table has additional options when inserting a phase (i.e. *duplicate*, *all green*). Issue #10672, #3419  
    - traffic lights can be renamed. Issue #11386
    - the list of programs is sorted by programID. Issue #11358
    - phase table shows cumulative time when hovering over the duration column. Issue #7962
    - tlType can now be changed directly. Issue #10757
    - Detectors for actuated traffic lights can now be customized visually. Issue #9598
    - Add buttons for visually creating and changing joined traffic lights. Issue #11367

- sumo-gui
  - EdgeData and laneData values recorded by the current simulation can now be used [for live edge coloring](sumo-gui.md#edgelane_visualisation_settings). Issue #9756
  - 3D view now permits clicking on more objects (lanes, junctions, traffic lights). Issue #10882
  - 3D view now supports realistic drawing of traffic signals for cars and pedestrians. Issue #10913, #11162
  - 3D view brightness can now be configured. Issue #11865
  - Stopped two-wheelers are now drawn without rider. Issue #10917
  - The front of the first rail carriage is now indicated by a black window in 'simple shape' mode. Issue #11369
  - Vehicle class 'taxi' is now indicated by additional decorations in *simple shapes* mode. Issue #11424
  - vClass-specific speed limits are now listed in lane parameter dialog if defined. Issue #11100
  - Fixed overlapping text for stop information (when *show route* is active). Issue #11462
  - The timeSinceStartup (from standing) is now listed in vehicle parameter dialog. Issue #11463
  - Status bar now indicates successful screenshot. Issue #11279
  - If a car has a custom arrivalPos or arrivalLane, these are now indicated when 'show route' is active. Issue #11533
  - Opening a second breakpoint editor is now prevented. Issue #10365
  - An active traci-server is now indicated in the status bar. Issue #5054
  - Kilometrage at cursor position is now shown in the lane context menu. Issue #11815
  - Added 'hide above threshold' to rainbow calibrator. Issue #11814
  - Generated color rainbow now includes MISSING_DATA color if data is missing. Issue #11927

- netconvert
  - Input given via option **--ptline-files** is now filtered when reducing the network extent. Issue #11548
  - Declaring a node as fringe (attribute `fringe`) now disables walkingareas (that would be appropriate at a dead-end). Issue #11614
  - Added extra consistency checks for public transport stop ordering in OSM input (with regard to bidi-edges). Issue #11612
  - Original ids of edges of removed edges are now stored in joined junctions parameters. Issue #11428
  - OSM import now support more symbolic speed restrictions. Issue #11682, #11683
  - OSM import now sets the fringe type of nodes for dead-ends. Issue #6623
  - Option **--fringe.guess** now detects more outer fringe nodes. Issue #11754
  - Node clusters and joint traffic lights now get similar ids of the form "cluster_id0_id1" or "joinedS_...", "joinedG_...". Issue #3871
  - Joined ids are abbreviated with the scheme `"cluster_id0_id1_id2_id3_#5more"` if too many junctions / traffic lights are participating, see also **-max-join-ids**. Issue #10795
  - OSM: monorail import is now supported. Issue #11799
  - OSM: cable car (aerialway) import is now supported. Issue #11798
  - Added new junction type left_before_right (needed for right-hand traffic with special rules, i.e. Madagascar). Issue #11855

- netgenerate
  - Now supports options **--geometry.remove** and **--tls.discard-simple**. Issue #11422

- activitygen
  - Edges are now checked for their permissions before being used for passenger cars origin / destination. Issue #11445

- polyconvert
  - Warning messages are now aggregated by default. Issue #11585

- TraCI
  - Simpla: Added additional platooning criteria via new attributes 'edgeLookAhead', 'distLookAhead' and 'lcMinDist'. Issue #9252, #11236 (Thanks to Jörg Schweizer)
  - Simpla: platooning distances are now time-headways instead of gap-in-meters by default. Configurable with new attributes 'maxPlatoonHeadway', 'catchupHeadway', 'useHeadway'.  Issue #11124
  - Net method `traci.simulation.getOption` can now be used to retrieve any simulation option. Issue #11319
  - person.getTaxiReservation parameter stateFilter now supports setting multiple bits. Issue #11501
  - Added function `traci.trafficlight.updateConstraints` for automated updating of rail signal constraints after rerouting. Issue #10134
  - Added function `traci.gui.setAngle` to change viewport angle. Issue #11239
  - Added functions `traci.gui.addView`, `traci.gui.removeView` to add/remove view windows. Issue #11760  
  - TraCISignalConstraint now includes param data. Issue #11880

- tools
  - routeSampler.py: now supports options **--depart-attribute**, **--arrival-attribute** to set extra constraints. Issue #6727
  - routeSampler.py: added more statistics on processed intervals. Issue #11328
  - routeSampler.py: Added option **--total-count** to set the total number of desired vehicles. Setting this to `input` reproduces counts from the loaded route files. Issue #11895, #11911
  - countEdgeUsage.py: Can now load multiple route files. Issue #11338  
  - generateRailSignalConstraints.py: added constraints for vehicles inserted at the same stop. Issue #11378
  - generateRailSignalConstraints.py: added option **--params** to make all comment information accessible as generic parameters (i.e. and see them in the gui). Issue #11880
  - generateRailSignalConstraints.py: Added option **--bidi-conflicts** to generated conflicts for bidirectional track use. Issue #11709
  - drtonline.py: now supports option **--tracefile** to allow for quick replication of a simulation. Issue #11414
  - Added new tool [drtOrtools.py](Tools/Drt.md#drtortoolspy) to solve DRT problems with the [ortools package](https://developers.google.com/optimization). Issue #11413  
  - osmWebWizard.py: Now prevents turn-arounds at the start and end of routes to improve traffic flow. Issue #10167
  - osmWebWizard.py: Now reliably generates trips in small networks. Issue #11563
  - osmWebWizard.py: Now aborts scenario building if when trying and failing to retrieve satellite data. Issue #11423
  - attributeStats.py: Now includes `stdDev` in outputs. Issue #10869
  - tls_csvSignalGroups.py: Can now extract csv descriptions from a .net.xml file. Issue #10756
  - changeAttribute.py Now handles missing attributes when removing attribute. Issue #11888
  - randomTrips.py: New option **--min-dist-fringe** which allows short fringe-to-fringe trips if trip generation with **--min-dist** fails repeatedly. Issue #10592
  - randomTrips.py: Option **--fringe-factor** now supports value `max` to force all departures and arrivals onto the fringe. Issue #11894
  - plotXMLAttributes.py: Now support plotting by rank (by specifying attribute `@RANK`). Issue #11605
  - Added new tool [vehrouteCountValidation.py](Tools/Output.md#vehroutecountvalidationpy) to evaluate the differenced between counting data (edge counts, turn counts, ...) and traffic recorded by a simulation (**--vehroute-output** with **exit-times**).
  - tlsCycleAdaptation.py: Now supports loading input with named routes. Issue #11879
  
### Miscellaneous

- Translation efforts for the grahpical applications and command line messages have begun and contributions are welcome at [Weblate](https://hosted.weblate.org/projects/eclipse-sumo/#languages). Issue #11237
- Added xsd-schema for battery-export. Issue #11173
- [ACC model description](Car-Following-Models/ACC.md) now has additional information on platoon stability. Issue #11382
- Github source download now includes all tests. Issue #11403
- All applications now support [appending list-type options](Basics/Using_the_Command_Line_Applications.md#options) in a configuration file with extra items via the command line. Issue #405
- osmWebWizard.py now stores network files as `.net.xml.gz` to conserve space. Issue #7713
- Walk attribute departPosLat is now interpreted in the same coordinate system as used by the vehicles. For backward compatibility, the option **--pedestrian.striping.legacy-departposlat** may be set. Issue #11705
- Added more entries to help menus of sumo-gui and netedit. Issue #11604
- The new default **--xml-validation** mode is *local* which only reads the local schema file but does not do network access to prevent XXE attacks. Issue #11054
- Cadyts-files can be downloaded again. Issue #11014
- Fixed the default state of some netedit toggle buttons (but kept their default semantics). Issue #10066
- Improved documentation for [3D-Visualization](sumo-gui.md#3d_visualization). Issue #10926

## Version 1.14.1 (19.07.2022)

### Bugfixes

- Simulation
  - Fixed avoidable simulation slow-down (~20% in some scenarios). #11203 (regression in 1.9.0)
  - RailSignalConstraint-attribute 'active' is now saved in simulation state. Issue #11150
  - Fixed collisions with carFollowModel EIDM when running with option **--ignore-junction-blocker**. Issue #11184
  - Fixed bug where carFollowModel EIDM gets "stuck" at low speed. Issue #11165
  - Fixed emergency braking after lane change on junction. Issue #10988
  - Fixed IDM collision warnings due to imprecise braking when configured with low 'accel'. Issue #11199
  - Fixed bug where person fails to enter vehicle. Issue #11210

- netedit
  - Fixed freezing when deleting an edge that is part of a multi-step person plan. Issue #11176 (regression in 1.14.0)

- netconvert
  - Fixed invalid right of way rules when using custom contPos from side road at straight connection. Issue #11160
  - Fixed generation of rail crossing at junctions that require a traffic light. Issue #10777
  
- TraCI
  - Fixed railSignalConstraints-deadlock after calling `traci.trafficlight.swapConstraints`. Issue #11007
  - Simpla now handles missing vType settings. Issue #11219
  
- tools
  - Restored support for Python version 2 (regression in 1.14.0). Issue #11159
  - generateParkingAreaRerouters.py: fixed distance computation. Issue #11170
  - drtonline.py: now works with Ubuntu 22 stock pulp package. Issue #11179
  - gtfs2pt.py: Fixed import of railway types and usability with python2. Issue #9550
  - traceMapper.py: fixed mapping loops when the first point was off the map

- TraCI / libsumo
  - Function 'traci.vehicle.rerouteTraveltime' now again respects custom travel times if currentTravelTimes is set to True. Issue #5943 (regression in 1.14.0)

- Linux packaging
  - enabled Eigen3 support (needed for the overheadwires) for the RPM packages


### Enhancements

- Simulation
  - Added new laneChangeModel parameter `lcOvertakeDeltaSpeedFactor` to configure the threshold for *not* overtaking a slightly faster leader vehicle. Issue #11010

- netedit
  - Warning dialog for loading the same file twice now permits overwriting duplicate elements. Issue #11058
  - View focus is now kept when transforming vehicles and persons between their trip/flow variants. Issue #11072
  - Improved generation of roundabouts involving edges with custom geometry endpoints. Issue #11045
  - Added junction context menu function for resetting edge endpoints. Issue #11044
  - Now showing a radius preview before converting junction to roundabout. Issue #11178

- tools
  - Objects parsed from XML (sumolib.xml.CompoundObject) can now maintain their attribute order. Issue #11174
  - randomTrips.py: options (**--period**, **--insertion-rate**, **--insertion-density**) now permit a comma-separated list of time values (not only space-separated). Issue #11151
  - runSeeds.py: Now supports option **--threads** to enable parallelization. Issue #11187
  - randomTrips.py: Added validation of vehicle class options. Issue #11195


## Version 1.14.0 (12.07.2022)

### Bugfixes

- Simulation
  - Fixed invalid speed when approaching stop for carFollowModel IDM. Issue #8577
  - Fixed unsafe insertion speed when the insertion lane requires multiple lane changes close to the insertion point. Issue #10761  
  - Fixed invalid warnings about missing green phase for "off" and "stop" links. Issue #10835
  - Fixed pedestrian collision after jamming. Issue #10823
  - Fixed invalid jamming that persists after an overfilled busStop has cleared. Issue #10822
  - Fixed invalid detector placement for joined actuated traffic lights. Issue #10837
  - Fixed bug where taxi fails to drop off customer. Issue #10860  
  - Setting a subsecond `timeThreshold` for `e3detector`s is now working. Issue #10881
  - Coasting decelerations have been implemented for HBEFA models. Issue #2110
  - Fixed invalid emissions for decelerating / standing vehicles for HBEFA and HBEFA3. Issue #2110, #4019
  - inductionLoop near lane end no longer miscounts pedestrians. Issue #10909
  - Fixed bug where person 'jumps' to stop when using option **--persontrip.transfer.walk-taxi ptStops**. Issue #10920
  - Fixed ride arrival position when using option **--persontrip.transfer.taxi-walk ptStops**. Issue #10919
  - routeProbReroute is now triggered only once per edge regardless of lane-changing. Issue #10943
  - Fixed inconsistent value for previous vehicle speed after loading simulation state. Issue #10922
  - Fixed errors when using `departSpeed="avg"` or `departSpeed="last"`. Issue #10868
  - Fixed teleporting taxis when combining randomCircling with automatic rerouting. Issue #11079
  - Attribute `startupDelay` now has higher priority than speed adaptations for lane changing. Issue #11067
  - Fixed emergency breaking on approach to junction. Issue #11130  
  - Lane changing fixes
    - Vehicles waiting at a red light no longer change lanes in response to vehicles passing the intersection. Issue #10665
    - Lane-specific speed limits now influence lane-changing decisions. Issue #8984
    - Inconvenience of slower lanes is now taken into account for cooperative changing. Issue #10714
    - Fixed invalid estimation of the number of required lane changes if multiple lanes are equally suitable. Issue #10769
    - Fixed bug where vehicles would enter lanes that are separated from a required lane by a forbidden lane. Issue #10512
    - Fixed unsafe speed adaptation for lane changing. Issue #10767
    - Fixed bug where cars performed strategic changes despite setting `lcStrategic="-1"`. Issue #11109
    - Fixed collision during opposite direction driving. Issue #11118
    - Fixed issues during opposite-overtaking with the sublane model. Issue #10927
    - Fixed invalid vehicle angle during lane changing on internal edge. Issue #5879
  - NEMA controller fixes
    - Fixed crash when loading NEMA controller with invalid phases. Issue #10704
    - Fixed crash when loading NEMA controller embedded in .net.xml. Issue #10705
    - Fixed invalid initial state (until the first switch). Issue #10743
    - Fixed crash when custom detectors are missing. Issue #10745
    - NEMA Controller now prioritizes permitted left 'y' & 'r' over linked phase 'g'. Issue #10897
  
- netedit
  - Fixed crash when loading a network (on very slow computers / builds). Issue #10750 (regression in 1.9.0)
  - Greatly increased rendering speed. Issue #10425 (regression in 1.11.0)
  - Fixed crash when drawing tazRelations. Issue #10929 (regression in 1.11.0)
  - Loaded named routes are now visible again. Issue #11008 (regression in 1.12.0)
  - Setting allow/disallow in create-edge frame is now working again. Issue #11096 (regression in 1.12.0)
  - Fixed incomplete demand output when defining a `<flow>` and a `<pedestrianFlow>` with the same id. Issue #11049
  - tls mode coloring of 'yellow' phase is now consistent with sumo-gui. Issue #10651
  - Loading a demand file only triggers a 'demand is modified' warning after actual modification. Issue #9529
  - Locate dialog buttons "select all" and "deselect all" now toggle selection status. Issue #10707
  - Editing traffic light parameters now marks it as changed. Issue #10673
  - Fixed various issues related to containers. Issue #10844
  - Fixed crash selecting tractionSubstation with rectangle selection. Issue #10896
  - Fixed handling of disconnected lanes of consecutive edges for e2Detectors and wires. Issue #10903
  - Fixed tazRelation coloring issues. Issue #10930
  - Checkbox 'Draw TAZ fill' now overrides taz attribute fill as intended. Issue #10928  
  - Loading and saving a selection is now working for all data elements. Issue #10936
  - Fixed duplicate TAZ id when rendering filled taz. Issue #10944
  - tlType selection from drop-down list is now working after changing junction type. Issue #10956
  - Fixed bug where lanes could not be clicked after changing attribute numLanes. Issue #10964
  - Fixed narrow drop-down boxes in inspect mode. Issue #10955
  - Fixed wrong shortcuts listed in the Edit menu. Issue #10940
  - Fixed inconsistent traffic light settings when removing node from a joined traffic light by changing its type. Issue #11012
  - Fixed visualization of edge-data elements in dataMode. Issue #11032
  - Fixed inconsistent behavior of allow/disallow dialog in create-edge frame. Issue #11095
  
  
- sumo-gui
  - Background images (decals) and multi-view settings are now restored on reload. Issue #10788 (regression in 1.13.0)
  - Background grid configured in settings is now shown when first opening gui. Issue #10789
  - Fixed invalid tranship color when coloring container by mode. Issue #10849    
  - Fixed inconsistent treatment of missing data when coloring by attribute/param. Issue #10932
  - An opened breakpoint editor is now updated when clicking on 'time' links. Issue #11035
  - Fixed crash when drawing persons that enter a vehicle. Issue #11076
  - Detectors are no longer drawn on top of traffic light signals and right-of-way indicators. Issue #11132
  - 3D View:
    - Fixed 3D rendering of edge geometry with varying incline. Issue #4952
    - Fixed Vehicle orientation on sloped edges in 3D view. Issue #10905
    - Fixed invalid OSG light def written in decal (background image) settings. Issue #10950
    - Exported viewport settings mismatch with actual 3D-view #10949
    - Fixed Crash when setting up GL2 compatible shaders. Issue #10895
    - Visualisation settings now maintain their type (3D/2D). Issue #11000
    - Coloring vehicles by type attribute or randomly is now working. Issue #2120
    - Avoid duplicated background objects after loading/reloading. Issue #11047
    - Fixed rendering of walkingareas (they are now raised above the road level). Issue #10773
   
- netconvert
  - Fixed invalid reduction in edge shape detail at very dense geometry. Issue #10727 (regression in 1.12.0)
  - Fixed crash when importing plain-xml with self-looped edge and re-occurring edge ids. Issue #10700
  - Fixed crash when loading OpenDRIVE road object validity record. Issue #10701
  - Reduced overly verbose ambiguous node type warnings. Issue #10827
  - Fixed missing tram connections at sharper turns on large junctions. Issue #10826
  - Option **--geometry.remove** now works correctly when merging networks. Issue #10853
  - Fixed generation of invalid pedestrian crossings. Issue #7625, #10894
  - Fixed invalid walkingarea shapes. Issue #11087, #11090
  - Patching the type of a loaded junction now has priority over option **--tls.guess**. Issue #11013
  - Fixed invalid right-of-way rules when using custom `contPos` at connections from a side road. Issue #11146
  - Generated traffic light plans now include *all-red* phases at very large junctions to ensure safe transitions between phases. Issue #10796

- TraCI / libsumo
  - Function `vehicle.setAcceleration` now supports negative values. Issue #10693
  - Fixed invalid added stop on previous edge while already on junction. Issue #10859
  - Fixed invalid taxi state after re-dispatch (causing early taxi removal). Issue #10933
  - Function traci.load now resets a previous log file by the same name. Issue #10979
  - Fixed invalid mapping with `vehicle.moveToXY` onto intersections with parallel internal edges. Issue #10952
  - Fixed missing collision detection after `vehicle.moveToXY` where the vehicle makes a large "jump". Issue #10952
  - Fixed incomplete rail signal state reset after calling simulation.loadState. Issue #11001
  - Fixed crash when calling inductionloop.getVehicleData for detected pedestrians. Issue #11011
  - Fixed non-deterministic results for `simulation.convertRoad`. Issue #11002
  - Function 'traci.vehicle.rerouteTraveltime' now behaves the same in traci and libsumo in all cases. Previously, the argument `currentTravelTimes` was ignored by libsumo. The behavior in traci changed slightly: instead of using the current edge speeds and updating them for all vehicles, it now uses the aggregated routing mode (which also reacts to current speeds). Issue #5943
  - Fixed crash when calling `traci.vehicle.updateBestLanes` for vehicles not on the road network. Issue #11121
  - Fixed invalid choice of lane after calling `traci.vehicletype.setVehicleClass`. Issue #11117
  - Fixed some corner case of non-existing dll search paths for libsumo on Windows. Issue #10995
  - Fixed invalid active person count after removing waiting-for-depart stage. Issue #11127
  - Function `traci.person.moveTo` is now working (for pedestrians). Issue #11081 

- tools
  - game/racing.py now uses the intended vehicle scaling and simulation delay. Issues #10783 (regression in 1.0.0)
  - sumolib now raises an exception instead of calling `sys.exit` if rtree module is missing and fallback is disabled. Issue #10666
  - sumolib uses the warnings module if rtree is missing which can be used to disable warnings. Issue #10666
  - [emissionsDrivingCycle](Tools/Emissions.md#emissionsdrivingcycle) now aborts on an unreadable cycle. Issue #10754  
  - generateRailSignalConstraints.py: Fixed bug where constraints for invalid stops were generated. Issue #10843
  - route2OD.py: Fixed invalid handling of TAZ defined with tazSource and tazSink elements. Issue #10873
  - routeSampler.py: now longer writes flows with probability > 1. Issue #10887
  - osmWebWizard.py: No longer creates temporary route files in the base working directory. Issue #10878

### Enhancements

- Simulation
  - Added support for PHEMlight V5. Among other things, this permits modelling of fleet aging. Issue #10237
  - Added support for the [HBEFA4 emission model](Models/Emissions/HBEFA4-based.md) with more than 800 emission classes. Issue #7277
  - Vehicles at longer planned stops now switch off their engine. Issue #10491, #4019
  - Automated engine start/stop can be modelled. Issue #10441
  - Jammed detectors of actuated traffic lights can now be ignored for phase extension after a configurable time threshold. Issue #5212
  - When jam detection is activated (i.e. via option **--tls.actuated.jam-threshold**), all detectors are usable for activation and this eliminates the warnings about "no controlling detectors". Issue #9280, #10682
  - InductionLoop detectors now support optional attribute 'length'. Issue #10668
  - Actuated traffic lights now support param key 'detector-length' to set the default length of it's detectors. Issue #10668
  - Option **--fcd-output.attributes** now supports the value **posLat** to include lateral positions in fcd-output. Issue #10695
  - Setting `lcSpeedGain="0"` now fully disables changing for speedGain. Issue #10709
  - RailSignalConstraints can now be loaded in a deactivated state by setting attribute `active="false"`. They can still be retrieved via TraCI. Issue #10799  
  - Attribute 'period' is now an alias for attribute 'freq' in all detectors and both are now optional. Issue #10390
  - Added option **--device.fcd.begin** to customize begin time of fcd-output. Issue #10996
  - Added option **--device.emissions.begin** to customize begin time of emission-output. Issue #11052
  - Vehicle stops now support attribute `onDemand="true"` which lets them skip stopping if no persons wish to embark or disembark. Issue  #11039
  - Added option **--fcd-output.filter-shapes** to restrict output to custom polygonal areas. Issue #11055
  - VType attribute 'lcOvertakeRight' is now supported by the sublane mode. Issue #11097
  - CarFollowModel *EIDM* now supports attribute `startupDelay`. Issue #10736
  - There is no warning about a missing green phase any longer for traffic light links which show "off" or "stop". Issue #10835
  
- netedit
  - Persons and personFlows can now be transformed into each other via context menu (similar to vehicles and flows). Issue #10607
  - Containers and containerFlows can now be transformed into each other via context menu. Issue #10685
  - Traffic light mode now supports phase attributes `earliestEnd, latestEnd` (tlType=actuated) and `vehExt,yellow,red` (tlType=NEMA). Issue #10656
  - PersonPlan-mode now permits selecting the person via clicking (deselect with ESC). Issue #9606
  - Taz Mode: Added new button to update source and sink weight for fringe edges. Issue #4991
  - Now supports creating and editing NEMA controller. Issue #9599
  - A tooltip is now shown when the mouse is over any button in the button bar. Issue #10085
  - Traffic light type is now show in traffic light mode. Issue #3423
  - All attributes can now be reset to their default value by deleting the attribute content if that attribute has a default. Issue #9350
  - TAZ-source and TAZ-sink elements can now be inspected and selected (upon activating a new toggle button). Issue #10273
  - Stops for containers can now be defined. Issue #10855
  - Walkingareas are now shown. Issue #9168
  - Walkingareas can now be located by id. Issue #8580
  - Added 'confirm relation' button to tazRelation mode. Issue #10733
  - Shift-click in demand-stop mode now sets parent element (i.e. vehicle). Issue #10288
  - Added gzip file endings in file selection dialogs. Issue #10513
  - Create edge mode now visually previews whether a new junction will be created or an existing junction will be re-used. Issue #8454
  - A click-preview-indicator is now provided when creating tazRelations and vehicle-over-route. Issue #10997
  - A click-preview-indicator is now provided when splitting edges in in create-edge-mode. Issue #10998
  - The hide/show-TAZRel buttons can now be used to hide all relations. Issue #10972
  - Added dialog for fixing / reporting network element problems. Issue #10151
  - Improved visualization of trips between junctions. Issue #9901
  - Added warning dialog if the same additional/demand/data file is loaded twice. Issue #11057
  - A custom color rainbow is now supported for all data elements. Issue #10934

- sumo-gui
  - InductionLoop detectors now list the time of continuos occupation in their parameter dialog. Issue #10671
  - 3D-view now permits opening vehicle context menu via right-click. Issue #10191
  - Geometry points of polygons junctions and walkingareas can now be annotated to aid in debugging (activated in the openGL settings). Issue #10594
  - Can now color polygons (and TAZ) randomly. Issue #10938
  - Added gzip file endings in file selection dialogs. Issue #10513
  - Right clicking on a pedestrian crossing now always opens it's context menu (rather than the menu for the junction). Issue #10914
  - Breakpoints set in sumo-gui now override breakpoints from option **--breakpoints**. Issue #11061

- netconvert
  - Now supports generating NEMA controllers. Issue #9599
  - Option **--prefix** now also modifies tlLogic ids. Issue #11062

- netgenerate
  - Add option synonyms **--random.lanenumber** and **--random.priority** for options that apply to all network types. Issue #10775
  - Added option **--random.type** to pick a random edge type from all loaded types. Issue #10774
  - Options **--geometry.split**, **--geometry.max-segment-length** and **--junctions.join-same** are now supported. Together, they allow for generating intermediate junctions along the generated edges (i.e. for merging shifted grids). Issue #10787

- TraCI
  - Each domain now supports the member `DOMAIN_ID` to retrieve the constant for subscriptions (i.e. `traci.vehicle.DOMAIN_ID == traci.constants.CMD_GET_VEHICLE_VARIABLE`). Issue #10963
  - It is now possible to retrieve lanearea detectors using traci.*domain*.subscribeContext. Issue #10960

- tools
  - [tls_csvSignalGroups](Tools/tls.md#tls_csvsignalgroupspy) now supports keyword **actuated** to declare time ranges for shorting/extending phases and causes programs of type *actuated* to be written. Issue #10719
  - osmWebWizard now uses a different projection when activating the "satellite background" option  to ensure that the network matches the background.
  - generateRailSignalConstraints.py: Can now write discarded constraints as inactive by setting option **--write-inactive**. Issue #10800
  - plotTrajectories.py: Now supports plotting type 'g' to plot by leaderGap (requires fcd output with option **--fcd-output.max-leader-distance**). Issue #10839
  - addStops2Routes.py: Added various options to randomize stop assignment (i.e. to model random break-down of cars). Issue #10871
  - route2OD.py: now supports loading trips with attributes fromTaz and toTaz. Issue #10924
  - routeSampler.py: now supports option **--write-flows poisson** to generate flows with poisson arrival pattern. Issue #10886
  - routeSampler.py: Can now load pedestrian walks as candidate routes. Issue #11041
  - randomTrips.py: Can now set explicitly randomized departPos and arrivalPos attributes. Issue #7977
  - randomTrips.py: Can now pass any kind of duarouter option using **--duarouter-option-name VALUE**. Issue #7347
  - randomTrips.py: Can now define a timeline of different demand levels. Issue #5739
  - randomTrips.py: Now supports option **--insertion-rate** to define demand as vehicles-per-hour and option **--insertion-density** to define demand as vehicles-per-hour-per-km. Issue#11053
  - randomTrips.py: All generated trips and route files now record the used randomTrips options. Issue #10973
  - cutRoutes.py: now adapts the departSpeed to 'max' and departLane to 'best' if vehicles start on a cut route. Issue #10611
  - The traffic light games with multiple junctions can now be controlled with the keyboard. Issue #11056
  - The traffic light games now use phase switching instead of program switching. Issue #11063
  - sumolib.shapes.polygon now handles missing attributes. Issue #11092
  - netdiff.py: now support saving and loading configuration files. Issue #10942
  - The new function `sumolib.xml.parse_fast_structured` now provides very fast parsing for heterogeneous nested xml files (i.e. tripinfo-output for vehicles and persons). Issue #9033
  - The new reduceLanes.py script can reduce the number of lanes for multi lane roads in a network splitting the edges before and after junctions

### Miscellaneous

- Outputfile comment header no longer includes options that were set automatically (only user defined options). Issue #10712
- Outputs now use attribute `period` instead of `freq` whenever denoting a time period. Issue #10657
- Updated default bicycle speed on highway.path and highway.cycleway. Issue #10976
- Fuel consumption is now given in mg/s instead of ml/s to achieve consistency across liquid an gaseous fuels. For backward compatibility, the option **--emissions.volumetric-fuel** may be set. Issues #7277, #11026
- The default parameters of the battery model have been changed to that of a KIA Soul EV (formerly the parameters were for a large electric bus). Issue #10883
- The documentation now has a `Copy`-button next to every code block. Issue #11050

## Version 1.13.0 (03.05.2022)

### Bugfixes

- Simulation
  - Fixed crash with departLane=best and given arrivalLane. Issue #10614 (regression in 1.9.0)
  - Fixed invalid switching for actuated traffic light in coordinated mode. Issue #10055  
  - Various NEMA fixes. Issue #10081, #10082, #10090
  - Sorted vehroute output now preserves the loading order of vehicles that depart in the same simulation step. Issue #10087
  - Fixed failing train reversal after waypoint. Issue #10093
  - Fixed invalid route when specifying a trip that loops back onto the start edge with arrivalPos < departPos. Issue #2757
  - Fixed invalid error message when using mismatched parentheses in traffic light switching conditions
  - Fixed invalid arithmetic in custom logic for actuated tls ('-' was working as '+'). Issue #10224
  - Fixed sub-optimal insertion flow with departLane="best". Issue #10137
  - Scaling vehicles with vTypeDistribution now resamples the type for each added vehicle. Issue #10155    
  - Trips with fromJunction, toJunction can now be loaded from additional file. Issue #10306
  - Rerouting now takes empty lanes into account in mean speed calculation. Issue #10345
  - Rerouting now ignores stopped vehicles in mean speed calculation if they can be overtaken. Issue #10336
  - Teleporting of blocked vehicles now works if they are blocked behind a stopping vehicles. Issue #1078  
  - Vehicles that do not park at a stopping place now ignore attribute `parkingLength`. Issue #10463
  - Fixed creeping collision with carFollowModel "W99". Issue #10472
  - Fixed collision with carFollowModel "IDM". Issue #10471
  - Fixed warnings after a long vehicle changes lanes after turning. Issue #10481
  - Added vType attribute 'sigmaStep' to decouple the driving imperfection dynamics (sigma) of Krauss and related Models from the simulation step-length. Issue  #10494  
  - Fixed junction collision in sublane model. Issue #10458
  - Fixed xml schema error when loading data files written by netedit. Issue #1051
  - Fixed error related to NEMA detector naming. Issue #10517  
  - CarFollowModel IDM with low decel values no longer overshoots stop position. Issue #10556
  - Unfinished (started) stops are now included in vehroute-output when setting option **--vehroute-output.write-unfinished**. Issue #10565  
  - Fixed NaN-value in summary-output when setting speed limit to 0. Issue #10603  
  - Fixed negative timeloss in person-walk tripinfo. Issue #10270
  - personFlow now re-samples walks from a routeDistribution. Issue #10632
  - Fixed invalid junction collision error. Issue #10637
  - Fixed bug where vehicles waiting within an intersection violate right-of-way after their green phase ends. Issue #8124
  - Fixed exaggerated precision of non-geodetic fcd-output attributes when using option **--fcd-output.geo**. Issue #10465
  - Fixed vehicle being discard with `departPos="random_free"` due to invalid memory-reuse. Issue #10647
  - opposite-direction driving
    - Can now overtake stopped vehicle when there is only a short gap afterwards. Issue #9994, #10338
    - Fixed failure to overtake fast vehicles. Issue #10194    
    - Fixed error "unexpected end of opposite lane" when the number of lanes changes during overtaking. Issue #10193
    - Fixed invalid lane occupancy after opposite change (could lead to insertion failure). Issue #10314
    - Fixed crash after rerouting during opposite direction driving. Issue #10312
    - Fixed frontal collision. Issue #10340, #10383
    - Fixed deadlock on opposite overtaking slalom #10360
    - Fixed emergency braking in sublane-opposite-direction driving. Issue #10473
  - state loading
    - flows are now fully restored from a state file (without also loading the original route file). Issue #7471
    - route files are now fully reset when loading a state file. Issue #7471
    - Fixed crash on simulation.loadState with persons. Issue #10228, #10261
    - Fixed pedestrians behavior after quick-loading state. Issue #10229, #10245, #10250, #10260, #10257
    - Simulation with persons in loaded state now terminates reliably. Issue #10233
    - stops of named routes are now restored when loading state. Issue #10230
    - Fixed different randomness after loading state. Issue #10251
    - Fixed invalid fcd-output after loading persons from state. Issue #10259
    - Fixed invalid phase of actuated tls after loading state. Issue #10263
    - Fixed undetermined order of tls and pedestrian events #10265
    - Fixed invalid stopping duration of public transport vehicles after loading state. Issue #10266
    - Fixed crash when loading state with calibrators. Issue #10277
    - Fixed crash when loading state that was written with **--vehroute-output.exit-times**. Issue #10410
  - Fixed various crashes
    - Fixed emergency braking / crashing when using **--extrapolate-departpos**. Issue #10294, #10298
    - Fixed crash when defining a trip between junctions and triggered departure. Issue #10188  
    - Fixed crash on parkingAreaReroute. Issue #10201
    - Fixed crash with waypoint on 0-capacity parkingArea #10211
    - Fixed crash when using arrivalEdge and rerouting. Issue #10276
    - Fixed crash when using option **--weights.separate-turns** with intermodal network. Issue #10416
    - Fixed crash when coloring streets by free parking spaces. Issue #10606
    - Fixed crash after teleporting within circular route. Issue #10634
  - Fixed various deadlocks
    - Fixed deadlock at on-off-ramp despite relief connection. Issue #10037 (regression in 1.6.0)  
    - Fixed preventable deadlock in weaving situation by applying stronger braking. Issue #10028
    - Fixed lane-changing related deadlock in sublane simulation. Issue #10054
    - Fixed failure to overtake extremely long vehicle. Issue #10060
    - Fixed sublane model deadlock at width change on junction. Issue #10509
    - Fixed wide vehicle being blocked on junction. Issue #10521, #10523
    - Fixed blocked vehicle when combining opposite driving with parking search. Issue #10625

- sumo-gui
  - Fixed crash when opening phase tracker window on invalid switching conditions. Issue #10121
  - Vehicles in the 3D-view are no longer hidden beneath a colored bubble. Issue #5735
  - Fixed frozen GUI after interactively removing vehicle. Issue #10291
  - param `show-detectors` now takes effect for NEMA controllers. Issue #10519
  - Fixed bug where color threshold names were not restored from settings file. Issue #10507
  - Setting 3D viewing angle in the viewport dialog is now working. Issue #5663
  - Fixed incorrect interpretation of some projection parameters (Windows only). Issue #10526
  - 3D options are no longer shown in viewport editor for 2D view. Issue #10540
  - exaggerated person-as-circle no longer exceeds car size when zoomed out. Issue #10633  
  - Exaggerated trucks are now slightly larger than vehicles when zoomed out. Issue #10635
  - Fixed partially invisible individually defined parking spaces. Issue #10568 (regression in 1.10.0)
  - Fixed missing window focus at startup. Issue #10499
  - Fixed crash in phase tracker when annotating by 'time in cycle'. Issue #10069
  - GUI-defined traffic scaling is now preserved on reload. Issue #10096
  - Fixed several problems when clicking on time links in the message area. Issue #10225
  - Fixed memory leak when drawing polygons. Issue #10232
  - Fixed unnecessary wide parameter dialog for lanes that prohibit all vClasses. Issue #10341
  - Files saved via dialogs now ignore option **--output-prefix**. Issue #10347
  - Fixed red/black GUI on MacOS. Issue #7830
  - Fixed invalid exaggerated vehicle size when drawing vehicle as imgFile. Issue #10381
  - Loading edge data for unknown edges is no longer an error. Issue #10379  
  - Fixed inconsistent gui settings on reload (settings will be kept on reload unless the settings-file was modified). Issue #10398
  - Cancelling a change in viewsettings now takes effect immediately. Issue #10495
  - Fixed x-axis annotation of moused-over value in plot window.
  - Fixed slow simulation when there are many vehicles at a parkingArea. Issue #10570
    
- netconvert
  - Fixed crash when using option **--railway.topology.extend-priority**. Issue #10043
  - Fixed platform dependency in OpenDRIVE export. Issue #10030
  - Internal bicycle lanes which originate from a narrow bicycle lane are now narrow themselves. Issue  #10051
  - removed obsolete multiple-connections warnings. Issue #10089
  - Fixed invalid junction shape when patching node positions to create zero-length edge (also affected netedit). Issue #10150
  - Now importing all types of traffic light signals from OpenDRIVE. Issue #10153
  - Fixed missing attribute in OpenDRIVE export. Issue #10301
  - Improve default shape for diagonal pedestrian crossings. Issue #10287
  - Fixed invalid XML characters in written junction names. Issue #10461
  - Fixed interpretation of OSM key junction=circular. Issue #10479
  - Fixed missing bus connection. Issue #10502
  - Fixed crash when merging overlapping networks with junction params. Issue #10528
  - Connections without conflict now have state *Major* (`M`) at "right_before_left" junctions. Issue #10439

- netedit
  - Fixed undo related crashes. Issue #10588, #10591 (regression in 1.0.0)
  - Fixed invalid geometry when loading geo-polygons. Issue #10101 (regression in 1.10.0)
  - Fixed crash when creating geo-poi. Issue #10505 (regression in 1.10.0)
  - regressions in 1.12.0  
    - Fixed crash when deleting last (or only) personTrip-element. Issue #10192
    - Fixed crash when changing departSpeed for flow. Issue #10165
    - Fixed inconsistent behavior of attributes in flow creation frame. Issue #10075
    - Fixed invalid error when loading shapes with location element. Issue #10112
    - Fixed invalid junction color after creating a trip (from/to). Issue #9980
    - Persons with an stop in their plan can now be created again Issue #10181
    - EdgeData und edgeRelData can be inspected and selected again. Issue #10130
    - When adding stops in demand mode, the list of potential parent elements (i.e. vehicles) now shows all possible candidates again. Issue #10074
  - Fixed invalid route when creating flow (embedded route) with via edges. Issue #10120
  - Vehicles and flows with embedded routes and junctions now appear in locate dialog. Issue #10173
  - Now validating route IDs. Issue #10235
  - Fixed conversion of junction to roundabout in lefthand network. Issue #10258
  - Fixed inconsistent move-mode behavior of E2Detector. Issue #10305
  - Data elements can no longer be given invalid attributes. Issue #10373
  - Rerouters and VSS no longer lose their edges and lanes after recomputing with volatile options. Issue #10386
  - Fixed error when deleting trips/flows between junctions. Issue #10391
  - Fixed crash when setting departLane of trips/flows between junctions. Issue #10396
  - Stops can now load tripId attribute. Issue #10475
  - Stops within embedded routes are now written sorted. Issue #10476
  - Fixed crash in edgeRelation mode. Issue #10485
  - Train visualization parameters (i.e. *carriageLength*) are now written. Issue #10553
  - Empty person plans now trigger a warning when saving. Issue #10566
  - Fixed bad visibility of dotted contour for very short edge. Issue #10555
  - Person drawing style takes now effect. Issue #10636
  - TLS index is no longer shown for non tls junctions. Issue #10649

- duarouter
  - Route errors are now detected when using option **--skip-new-routes**. Issue #6113

- polyconvert
  - Fixed bug that prevented shapefile import without projection. Issue #10420

- meso
  - Fixed missing rerouting after delayed insertion. Issue #10328
  - Fix underestimated penalties when using option **--meso-tls-penalty**. Issue #10415
  - Penalties set by option **--meso-minor-penalty** are no longer affected by initial tls states. Issue #10419
  - Fixed exaggerated speeds at triggered stops. Issue #10488
  - Fixed crash with option **--vehroute-output.stop-edges**. Issue #10600

- traci
  - Fixed invalid length value in TraCI response for context subscription. Issue #10108
  - Fixed crash when calling 'vehicle.replaceStop' to replace stop that was added from a named route before a vehicle departed. Issue #10135
  - libsumo and traci no longer differ on context subscriptions to TRACI_ID_LIST. Issue #7288
  - Fixed superfluous simulation step after traci.load. Issue #10164
  - traci.person.setSpeed is now working. Issue #10166
  - Added missing vType related functions from person domain (i.e. traci.person.getMaxSpeed). Issue #10169
  - Fixed invalid route and errors after removing stops with replaceStop on departure and rerouting. Issue #10209
  - Fixed inconsistent lane change state (left+right at the same time). Issue #10212
  - Fixed missing attributes in vehroute output after adding vehicle. Issue #10282
  - Fixed invalid collision when using moveToXY with high implicit speed. Issue #10367
  - Fixed discontinuous moveToXY mapping at low step length. Issue #10448

- tools
  - generateTurnRatios.py now writes correct closing tag. Issue #10140 (regression in 1.11.0)
  - extractTest.py: now supports complex tests and option CLEAR. Issue #10264, #8473
  - [route2OD.py](Tools/Routes.md#route2odpy) now works if an edge is part of multiple TAZ. Issue #10586
  - [addTAZ.py](Tools/Routes.md#addtazpy) now works if an edge is part of multiple TAZ. Issue #10599
  - [addStops2Routes](Tools/Routes.md#addstops2routespy) now works when no type file is set. Issue #10610

- miscellaneous
  - Fixed missing data files in libsumo wheel installation. Issue #9945****

### Enhancements

- Simulation
  - traffic lights
    - Traffic light type 'NEMA' now supports TS1 and TS2 offsets as well as Type 170. Issue #10013
    - Phase attributes 'minDur, maxDur, earliestEnd, latestEnd' can now be [overridden with condition-expressions](Simulation/Traffic_Lights.md#overriding_phase_attributes_with_expressions). Issue #10047
    - Traffic lights with custom switching rules now support [custom runtime variables](Simulation/Traffic_Lights.md#storing_and_modifying_custom_data). Issue #10049
    - Traffic lights with custom switching rules now support [user-defined functions](Simulation/Traffic_Lights.md#custom_function_definitions). Issue #10123
    - Actuated traffic lights with custom switching rules can now retrieve the current time within the cycle using expression `c:`. Issue #10109
    - Improved error messages for invalid switching conditions of traffic lights to better identify the faulty input.
    - Actuated traffic lights may not omit phase attribute 'maxDur' which defaults to ~24days as long as attribute minDur is set. Issue #10204
    - Detector and condition states can now be included in [tls output](Simulation/Output/Traffic_Lights.md#optional_output). Issue #10065
    - NEMA now supports cross phase switching. Issue #9983
  - Added support for [poisson distributed flows](Simulation/Randomness.md#poisson_distribution) #10302  
  - vType attribute 'startupDelay' can now be used to configure startup loss time. Issue #7832
  - The default lateral alignment of bicycles is now "right" instead of "center". Issue #9959    
  - edgeData output now supports attributes 'edges' and 'edgesFile' to reduce the output to a configurable list of edges. Issues #10025
  - edgeData output now supports attribute 'aggregate' to aggregate data for all (selected) edges. Issue #10026
  - Vehroute-output now includes attribute 'replacedOnIndex' for routes that were replaced after departure to resolve ambiguity for looped routes. Issue #10092
  - Added option **--replay-rerouting** to re-run scenarios from vehroute-output in the same way as the original run. Issue #3024  
  - Added new vehicle attribute 'insertionChecks' that allows forcing vehicle insertion in unsafe situations. #10114  
  - Added option **--save-configuration.relative** to write config-relative file paths when saving configuration. Issue #6578
  - Smoothed the effect size curve of vehicle impatience. Previously, most of the effect occurred at low impatience values and larger values did not matter. To compensate for the reduced gradient, the default of option **--time-to-impatience** was reduced from 300s to 180s. Issue #8507
  - Vehicle flows with equidistant spacing (i.e. `period="x"`) now remain equidistant when the flow is increased via option **--scale**. Issue #10126
  - Added vType attribute 'scale' to allow type-specific demand scaling. Issue #1478   
  - Option **--emission-output.geo** can be used to switch emission location data to lon,lat. Issue  #10216
  - Person attribute 'speedFactor' can now be used to override speed distribution. Issue #10254  
  - Added option **--personroute-output** to separate vehroute output for persons/containers from vehicle routes. Issue #10317
  - Option **--fcd-output.attributes** now supports value 'odometer' to include the odometer value and 'all' to include all values. Issue #10323
  - Option **--time-to-teleport.ride** causes persons and containers to "teleport" after waiting for too long for a ride. Issue #10281
  - Vehicles on long stops should no longer recompute their route. Issue #8851
  - Added warnings when a vehicle has stops with inconsistent timing information. Issue #10460
  - Added option **--time-to-teleport.highways.min-speed** to configure the speed threshold for applying **--time-to-teleport.highways**. Issue #8268  
  - All carFollowModels now support attribute 'collisionMinGapFactor'. Issue #10466
  - Stop attribute 'endPos' can now be used to override automated stop position choice at busStop. Issue #10550
  - Flows now support attribute `begin="triggered"`. Issue #10560
  - Added option **emergency-insert** to permit inserting vehicles at dangerous speeds. This behavior was previously a side effect of option **--ignore-route-errors**. Issue #10581
  - Attribute 'departEdge' can now be used with trips and flows. Issue #8992

- sumo-gui
  - Enabled dpi awareness. Issue #9985
  - Traffic light type 'actuated' now supports parameters 'show-conditions' and 'hide-conditions' to customize visualization in the [Phase Tracker Window](Simulation/Traffic_Lights.md#track_phases) Issue #10046
  - Traffic light type 'actuated' now supports parameters 'extra-detectors' to included additional detectors in the [Phase Tracker Window](Simulation/Traffic_Lights.md#track_phases) Issue #10290
  - Detectors can now be triggered from the context menu even if there are no vehicles on it. Issue #10067
  - Saved configuration now always contains relative file paths. Issue #6578
  - Added menu entry 'Simulation->Load' to quick-load a saved state for the current network.
  - The keys pgdup/pgdown can now be used to change simulation delay.  (their former functionality of quick-panning the view was taken up by alt+arrows). Issue #10199
  - Greatly improved rendering speed of polygons. Issue #10240
  - Hotkey **Ctrl+j** now toggles drawing of junction shapes. Issue #10362
  - Background images (decals) now support environment variable resolution in their paths. Issue #10371
  - The *space* key can be used to toggle run/stop. Issue #10068
  - Improved visibility of vehicles with exaggerated size on multi-lane roads with exaggerated size. Issue #10483
  - Vehicle length no longer affects size when drawing as circles. Issue #10629
  - Improved positioning when drawing persons as circles. Issue #10608

- netedit
  - Added (experimental) support for editing [overhead-wire infrastructure](Models/ElectricHybrid.md). Issue #9480
  - Can now set stop attributes "tripID" and "line". Issue #6011
  - Hierarchy view now contains object ids. Issue #10076
  - Defining waypoints is now supported. Issue #10111
  - Improved accuracy of POI geo-positions. Issue #9353
  - Creating an element with custom id now works without setting a checkbox. Issue #10038
  - Stops of the current vehicle are now distinguished in color. Issue #10079
  - Reducing a network to a selection now works with the new "Reduce" button (instead of the less intuitive Invert+Delete). Issue #10084
  - Writing shortened xml header for additional files. Issue #10247
  - Vehicle stops and waypoints are now annotated with an index when inspecting the vehicle (and zooming in). Issue #10077
  - The context menu for vehicles and routes now includes the current route length. Issue #9354v
  - Lane members (i.e. for variableSpeedSign) can now be set by clicking in the view. Issue #9442
  - Parent elements (i.e. the busStop of an `<access>`) can now be set by clicking in the view. Issue #9652
  - In vType frame, an orange contour is drawn around vehicles with the current selected vType. Issue #10356
  - Improved feedback after reloading additionals. Issue #9362
  - Hotkey **Ctrl+j** now toggles drawing of junction shapes. Issue #10362
  - Added 'smooth shape' to the connections context menu. Issue #10352
  - Deleting lanes in now works in create-edge mode. Issue #10409
  - Saving (compressed) networks with '.net.xml.gz' extension is now possible. Issue #10624

- netconvert
  - Improved speed of OSM import. Issue #8147
  - OpenDRIVE export now includes `<signal>` and `<controller>` information. Issue #2367
  - OpenDRIVE import now uses more information to compute junction shapes. Issue #10337
  - Option **--opposites.guess.fix-lengths** is now enabled by default. Issue #10326
  - The character ':' is now permitted edge and junction ids except as leading character. Issue #10421
  - OSM: now importing additional bus stops. Issue #10503
  - Short roads without streetname can now be merged with longer roads that define a name. Issue #10582
  - A network file can now be patched with geo-coded plain-xml files by setting a projection (i.e. **--proj.utm**). Issue #10628

- polyconvert
  - Shapefile with geometry encoded as linestring2D is now supported. Issue #10100

- duarouter
  - Option **--randomize-flows** now applies to personFlow. Issue #10182

- traci
  - Added function 'traci.inductionloop.overrideTimeSinceDetection' and 'traci.lanearea.overrideVehicleNumber' to trigger the detector without the need for vehicles and facilitate traffic light testing. Issue #10045, #10048
  - function 'traci.vehicle.setPreviousSpeed' now supports an optional parameter to set the previous acceleration. Issue #10097
  - function `traci.simulation.subscribeContext' can now be used to subscribe to all objects in the simulation network. Issue #8388
  - Added function 'vehicle.insertStop' to add stops anywhere in the stop list and reroute automatically. Issue #10132
  - Added function 'vehicle.setStopParameter' to set any possible attribute for any upcoming stop. Issue #7981
  - Added function 'vehicle.getStopParameter' to retrieve any possible attribute for any past or upcoming stop. Issue #10160
  - Added functions 'simulation.getScale' and 'simulation.setScale' to access the global traffic scaling factor. Issue #10161
  - Added functions 'vehicletype.getScale' and 'vehicletype.setScale' to access the type-specific traffic scaling factor. Issue #10161
  - Added functions 'getDetEntryLanes, getDetExitLanes, getDetEntryPositions, getDetExitPositions' to 'multientryexit' domain. Issue #10083
  - Actuated traffic lights now supports the keys *cycleTime, cycleSecond, coordinated, offset* in setParameter and getParameter calls. Issue #10234
  - Actuated traffic lights can now retrieve switching condition states via `trafficlight.getParameter(tlsID, "condition.CONDITION_ID")` #10561
  - Added function 'vehicle.setAcceleration' Issue #10197
  - Function vehicle.replaceStop now supports the flag 'teleport=2' to trigger rerouting after stop removal. Issue #10131
  - Function `vehicle.moveToXY` now "guesses" the next route edges to prevent artifacts when reaching the end of the current route. Issue #4250
  - Experimental support for libsumo with GUI (only works on Linux / Mac yet). Issue #6663

- tools
  - routeStats.py: Can use measures "speed", "speedKmh", "routeLength", can switch to the fast XML parser and can also filter by route length . Issue #10044
  - tls_csv2SUMO.py now supports the same signal states as the simulation. Issue #10063
  - osmGet.py: allow filtering road types and shapes in OSM API query to reduce download size. Issue #7585
  - osmWebWizard.py: can now select desired road types to reduce download size. Issue #7585
  - route2OD.py: added new option **--edge-relations** to write edge-based OD relations (without the need for a TAZ  file). This type of output can be usd with routeSampler.py. Issue #10058
  - randomTrips.py: When setting option **--random-depart**, with a fractional value for option **--period**, the depart times now have sub-second resolution. Issue #10122
  - randomTrips.py: now supports option **--random-routing-factor** to increase the variance of generated routes. Issue #10172
  - added library function `sumolib.net.getFastestPath`. Issue #10318
  - edgeDataDiff.py now supports error propagation for attributes starting with `_std`. Issue #10103
  - countEdgeUsage.py: Option **--taz** now works together with time filtering and **--subpart** filtering. Issue #10404
  - Added tool [mapDetectors.py](Tools/Detector.md#mapdetectorspy) which creates detector definitions based on geographic coordinates.
  - generateParkingAreas.py: now supports option **--keep-all** to keep 0-capacity areas. Issue #10612
  - generateParkingAreas.py: now supports options **--edge-type.keep** and **--edge-type.remove** to filter by edge type. Issue #10615
  - addStops2Routes.py should now preserves vTypes. Issue #10613
  - attributeStats.py now supports reading attributes from multiple files. Issue #10643
  - added new tool [runSeeds.py](Tools/Misc.md#runseedspy) to run a configuration multiple times with different random seeds and collect distinct outputs. Issue #10642

### Miscellaneous

- simulation
  - Rerouter attribute 'file' is no longer supported. Intervals should be child elements of rerouters. Alternatively, element `<include href="FILE"/>`  may be used. Issue #9579
  - Improved error message when using `<stop>` elements and attribute `via` in an inconsistent manner. Issue #10110
  - limit internal precision of random variables (i.e. sampled speedFactor or random departSpeed) to 4 decimal digits and enforced the same minimum output precision. This avoids problems when replicating a scenario based on **vehroute-output**. Issue #10091  

 - The [test extraction page](https://sumo.dlr.de/extractTest.php) now supports downloading a whole directory of tests. Issue #10105
 - The ubuntu package now includes emission tools, header files and libsumo/libtraci.so files. Issue #10136
 - Removed obsolete and broken tool 'personGenerator.py' (use personFlow instead). Issue #10143
 - Added [examples and documentation](Tutorials/FundamentalDiagram.md) for spontaneous break-down of traffic flow jamming. Issue #10244
 - Documented [attribute plotting](sumo-gui.md#plotting_object_properties) and [extended vehicle visualization features](sumo-gui.md#vehicle_visualisation_settings). Issue #10558, #10559


## Version 1.12.0 (25.01.2022)

### Bugfixes

- simulation
  - Fixed bug where persons could enter the wrong vehicle and thereby cause a taxis simulation to crash. Issue #9821, #9733 (regression in 1.11.0)
  - tripId is now updated again when passing waypoints. Issue #9751 (regression in 1.11.0)
  - calibrator speed -1 no longer triggers error. Issue #9767 (regression in 1.11.0)
  - Fixed invalid error on taxi dispatch. Issue #9695, #9867
  - Fix person model bug where person enters jammed state without good reason. Issue #9717  
  - Fixed invalid error on mismatch between ride destination stop and vehicle destination stop. Issue #9712
  - Parking search no longer stalls when all current alternatives are known to be full. Issue #9678
  - Fixed crash when retrieving detected persons with an active vTypes filter. Issue #9772
  - Fixed various issues with emergency vehicle behavior, Issue #9310, #9768
  - Fixed bug where parkingReroute failed due to invalid rerouteOrigin. Issue #9778
  - Fixed inconsistent arrival times for trains approaching red and green signals. Issue #9788
  - Fixed slow sublane simulation on large networks with only a few vehicles. Issue #9816
  - Fixed emergency braking of carFollowModel CACC at small step lengths. Issue #9831
  - Setting **--step-length** > 1 now raises a warning for default tau. Issue #1375
  - Fixed superfluous route extension for taxi with idle-algorithm 'randomCircling'. Issue #9866
  - Fixed collisions and emergency braking for Wiedemann carFollowModel. Issue #1351, #5715, #9832
  - CarFollowModel EIDM now respects emergencyDecel. Issue #9618
  - Fixed bug in driving speed calculation for EIDM. Issue #9878
  - Stop positions between 0 and 0.1 can now be defined. Issue #9915
  - Fixed invalid collider/victim classification for junction collision. Issue #9916
  - Fixed collision on junction when long vehicle cuts a corner. Issue #4431
  - Fixed invalid junction collision warning. Issue #9920
  - Fixed junction collision when bluelight vehicle drives on red. Issue #9919  
  - Fixed missing access stage after ride to busStop. Issue #9958
  - Detector `<param>`s are noW loaded. Issue #9578
  - Various NEMA fixes. Issue #9965, #9971, #9940, #9987
  - Fixed invalid tripinfo for persons that did not finish their plan by simulation end. Issue #8461
  - When setting option **--vehroute-output.sorted**, all persons are now sorted. Issue #9929
  - Fixed deadlock at parkingArea with `onRoad="true"`. Issue #10005
  - opposite direction driving
    - Fixed crash on opposite direction driving (at looped road). Issue #9718
    - Opposite direction overtaking now takes into account slopes. Issue #9719
    - Fixed collision during opposite direction driving near junction. Issue #9721
    - Fixed invalid maneuver distance when returning from opposite side. Issue #9536
  
- netedit
  - Fixed bug preventing inspection of tazRelations. Issue #9728
  - splitting and merging edges with custom length now preserves total length. Issue #9617
  - can now load parkingAreaReroute without probability attribute. Issue #9640
  - Fixed invalid weights of sinks and sources when loading taz from file. Issue #9672
  - Fixed unclickable ParkingArea after moving junction with active grid. Issue #9639
  - Fixed rendering of turn direction arrows for narrow lanes. Issue #9738
  - In create-edge mode: shift-click to split edge now takes into account active grid. Issue #9624
  - Fixed crash on undo after resetting connections. Issue #9673
  - Stop attribute "triggered" now supports symbolic string values. Issue #9563
  - Fixed missing 'end' attribute when converting trip to flow. Issue #9834    
  - Routes now longer have a color unless set by the user. Issue #9512
  - Stops can no longer be created on edges that do not belong to the route. Issue #9923
  - Routes with invalid stops (invalid order or edge) now trigger a dialog for corrections on saving. Issue #9921
  - TLS mode now always shows the correct connection shape. Issue #9962
  - When splitting an edge, all routes passing that edge are now adapted. Issue #9511

- sumo-gui
  - Fixed crash when using guiShape "truck/trailer" or "truck/semitrailer" for short vehicles. #9682 (regression in 1.11.0)
  - Fixed inconsistent behavior when storing gui settings in registry. Issue #9693
  - Fixed meso vehicle tracking focus. Issue #9711
  - Exaggerating stopping place size only increases symbol size. Issue #9370
  - Fixed invisible rerouter on short edge. Issue #9779
  - Fixed invalid detector visibility when switching actuated traffic light program on. Issue #9877
  - Fixed keyboard navigation in locate object dialogs. Issue #9740
  - Rerouter symbols are no longer drawn on sidewalks. Issue #9908
  - Fixed various inconsistencies in the TLS Phase tracker. Issue #9963
  - Fixed occasional freezing during person simulation. Issue #9973

- netconvert
  - Fixed unsafe location of internal junctions that were causing collisions in the simulation. Positioning can be controlled with option **--internal-junctions.vehicle-width** and setting this to 0 restores legacy behavior. Issue #4397  
  - Fixed invalid network when importing public transport and sidewalks. Issue #9701 (regression in 1.10.0)
  - Fixed invalid internal junction location. Issue #9381
  - Fixed unsafe intersection rules that could cause emergency braking at pedestrian crossing. Issue #9671
  - Fixed invalid error when loading projections with '+geogrids' entry on windows. Issue #9766
  - Fixed invalid handling of loaded roundabouts when the network is modified. Issue #9810
  - Network building now aborts when a type file could not be loaded. Issue #9392
  - Option **--ignore-errors** now ignores duplicate node ids in dlr-navteq input. Issue #8581
  - Reduced variation between platforms. Issue #9874
  - Fixed invalid connectivity at motorway ramp when importing OSM lane change prohibitions. Issue #9939
  - Fixed invalid link direction when there are multiple turnaround edges. Issue #9957
  - Options **--tls.group-signals** and **--tls.ungroup-signals** now work for pedestrian crossings. Issue #9521, #9997
  - Fixed invalid permissions when leaving multi--lane-roundabout from inner lane. Issue #10017
  - invalid edge geometry after trying to patch very short edge. Issue #10018
  - OpenDRIVE
    - Fixed invalid LaneLink index in OpenDRIVE export. Issue #9637
    - Fixed too sparse geometry in OpenDRIVE import when using --geometry.min-dist. Issue #10012
    - Fixed geometry affected by spacing of non-driving lanes. Issue #4913
    - Several geometry fixes that mostly affect OpenDRIVE: #10018, #1498

- meso
  - Fixed invalid stop arrival time in meso. Issue #9713  
  - Fixed invalid ride depart time and route length when starting directly after stop. Issue #9560
  - No more warnings about small tau. Issue #9505
  - Dynamically modified road permissions (i.e. closingReroute with disallow and closingLaneReroute) are no longer ignored and can now cause jamming. Issue #9950, #10010
  - Fixed invalid capacity in intermodal scenario. Issue #8167

- duarouter
  - Option **--write-costs** now also applies to walks/rides, Option **--route-length** now applies to normal vehicles. Issue #9698
  - Fixed invalid error on mismatch between ride destination stop and vehicle destination stop. Issue #9730
  - Fixed invalid route output when using option **--remove-loops** on routes with multiple loops. Issue #9837
  - Fixed inconsistent vType defaults for speedFactor. Issue #9864
  - Fixed inconsistent railway routing results for stops on consecutive bidi-edges. Issue #9949
  - Fixed inconsistent handling of personTrips and explicit trip items. Issue #5821
  - Setting **--vtype-output NUL** now discards vtypes. Issue #9991
  
- jtrrouter
  - Unsorted flows now trigger a warning. Issue #9327
  - Fixed inconsistent vType defaults for speedFactor. Issue #9864  

- marouter
  - Fixed wrong begin and end times in marouter output with additive traffic. Issue #10004

- traci
  - New programs created via 'trafficlight.setProgramLogic' now support GUI access. Issue #549
  - turn subscription filter no longer crashes when crossings are present in foe lanes. Issue #9630
  - Fixed crash when calling vehicle.rerouteParkingArea for newly added vehicle. Issue #9755
  - Fixed invalid warnings when adding turn/lanes filter with context domain person. Issue #9760
  - TraCI server no longer hangs when trying to add a subscription filter without previous vehicle subscription. Issue #9770
  - Fixed memory leak in libsumo::TrafficLight::getCompleteRedYellowGreenDefinition. Issue #9818
  - Fixed bug where calling changeSublane with high 'latDist' value, causes exaggerated maneuverDistance. Issue #9863
  - Fixed crash when calling getStopDelay for a vehicle that isn't in the network. Issue #9944

- tools
  - generateParkingAreaRerouters.py: fixed distance bias against long parkingAreas. Issue #9644
  - routeSampler.py: warning message instead of crash when trying to load an empty interval. Issue #9754
  - addStopsToRoutes.py: Now handles disallowed vClass and vTypes with undefined vClass. Issue #9792
  - generateRailSignalConstraints.py: Now handles intermediate parking stops. Issue #9806
  - Fixed encoding problems in osmTaxiStop.py #9893
  - plot_csv_timeline.py now supports python3. Issue #9951
  - splitroutefiles.py can now handle gzipped input on Windows. Issue #8807

- Miscellaneous
  - Specifying NUL output on the command line finally works. Issue #3400
  - Fixed unhelpful error message when defining invalid color in XML input. Issue #9623
  - Time output beyond 25 days now works correctly with all compilers. Issue #8912
  
### Enhancements

- simulation
  - Persons are now included in saved simulation state when setting option **--save-state.transportables**. Issue #2792
  - Traffic lights of type 'actuated' and 'delay_based' now support attributes 'earliestEnd', 'latestEnd' and param 'cycleTime' to configure coordination. Issue #9748, #9885, #9889
  - Traffic lights of type 'actuated' now support [custom logical conditions](Simulation/Traffic_Lights.md#type_actuated_with_custom_switching_rules) for switching. Issue #9890
  - Added attribute speedRelative to edgeData output. Issue #9601
  - Option **--fcd-output.attributes** can now be used to active non-standard attributes (i.e. acceleration). Issue #9625
  - Rerouting period can now be customized via `<param key="device.rerouting.period" value="X"/>` in vType or vehicle. Issue #9646  
  - Detector processing now takes less time if their output file is set to 'NUL'. Issue #7772, #9620
  - Vehicle attribute [departSpeed](Definition_of_Vehicles%2C_Vehicle_Types%2C_and_Routes.md#departspeed) now supports the values 'last' and 'avg'. Issue #2024
  - Traffic light type 'NEMA':
    -  now supports param 'fixForceOff' which permits non-coordinated phases to use the unused time of previous phases. Issue #9848
    -  Added "fully actuated" operation support. Issue #9933
    -  Added "Rest in Green" functionality when not in coordinated mode. Issue #9937, #9968
    -  Supporting phase skipping. Issue #9897
    -  Supporting Green Transfer. Issue #9954
  - parking search:
    - Parking search now supports `<param key="parking.anywhere" value="X"/>` which permit using free parkingArea along the way after doing unsuccessful  parkingAreaReroute x times. Issue #9577
    - Parking search now supports `<param key="parking.frustration" value="X"/>` which increases the preference for visibly free parkingAreas over time. Issue 9657
    - Parking search now supports `<param key="parking.knowledge" value="x"/>` to let driver "guess" the occupancy of invisible parkingAreas with probability x. Issue #9545 
    - Vehicles now collect occupancy information for all parkingareas along the way during parking search. Issue #9645

- sumo-gui
  - All text setting now have the checkbox "only for selected" to display text selectively. Issue #9574
  - Added vehicle setting "show parking info" which labels parking memory (block time and scores) for vehicles with active route visualization. Also, the vehicle is labeled with the number of parking reroutes since the last successful parkingArea stop. Issue #9572
  - Can now color roads "by free parking spaces". Issue #9643  
  - Added context menu entry to open map location in an online map. Issue #9787
  - Vehicle size can now be scaled by attribute. Issue #9567
  - Added speedFactor to vehicle type parameter dialog. Issue #9865
  - The locate dialog has improved keyboard navigation, optional case-sensitivity and optional auto-centering. Issue #9902, #9876
  - Traffic light parameter dialog now includes cycle duration, timeInCycle, earliestEnd and latestEnd. Issue #9784
  - Parameter Tracker window enhancements:
    - The value at the cursor position is now highlighted and printed. Issue #9976
    - When saving data, the time values are now included. Issue #9977
    - Plotting multiple values in the same window is now supported by activating 'Multiplot' and starting more plots. Issue #9984
  - Phase Tracker window enhancements:
    - shows switch times with configure style (seconds, MM:SS, time-in-cycle). Issue #9785
    - optionally shows green phase durations.
    - remembers window position and size. Issue #9826
    - shows phase index or phase name. Issue #9836
    - optionally draws states of actuation detectors. Issue #9887, #9952
    - optionally draws states of custom switching conditions. Issue #9928    

- netedit
  - Trips and flows between junctions (fromJunction, toJunction) are now supported. Issue #9081
  - The name prefixes for all created additional elements can now be configured and their defaults have been shortened. Issue #9666
  - Add images for the guiShapes in the vType attributes editor. Issue #9457
  - All output elements now write 'id' as their first attribute. Issue #9664
  - All elements of a side frame can now be collapsed/expanded. Issue #6034
  - Trips with a single edge can now be created. Issue #9758
  - Added lane context menu function "set custom shape". Issue #9741
  - Loading and setting of vTypeDistributions is now supported. Issue #9435
  - List of issues from 'demand element problems'-dialog can now be saved to file. Issue #7868
  - Lane change prohibitions (changeLeft, changeRight) are now visualized. Issue #9942
  - Selections can now be modified based on object hierarchy (i.e. to selection junctions for all selected edges). Issue #9524
  - Improved visibility for short edges. Issue #9434

- netconvert
  - OSM: import of public transport now supports share_taxi (PUJ) and minibus. Issue #9708
  - OSM: attribute turn:lanes is now used to determine lane-to-lane connections when option **--osm.turn-lanes** is set. Issue #1446
  - OSM: importing airports (aeroways) is now supported. Issue #9800
  - Simplified edge and junction names in OpenDRIVE import. (i.e. '42' instead of '42.0.00'). The option **--opendrive.position-ids** is provided for backward compatibility.  #9463
  - Added option **--opendrive.lane-shapes** which uses custom lane shapes to account for spacing of discarded lanes. Issue #4913  
  - Added option **--railway.topology.extend-priority** which extrapolates directional priorities in an all-bidi network based on initial priorities. Issue #9683
  - OpenDRIVE export and import now supports writing and reading the `<offset>` element for handling shifted geo references. Issue #4417, #10006
   
- duarouter
  - can now write route costs in regular route output. Issue #9667

- TraCI
  - 'traci.vehicle.getParameter' and 'setParameter' now support all laneChangeModel parameters. Issue #10011

- tools
  - routeSampler.py: Option **--prefix** now also applies to route ids. Issue #9634
  - generateParkingAreaRerouters.py: added option **--prefer-visible** which prefers visible alternatives regardless of distance. Issue #9642
  - generateParkingAreaRerouters.py: added option **--distribute** which sets a distance distribution for the given number of alternatives (instead of always using the closest parkingAreas). Issue #9566
  - generateParkingAreaRerouters.py: added option **--visible-ids** to set visible parkingAreas explicitly. Issue #9669
  - addStops2Routes.py: Can now generate stationary traffic to fill each parkingArea to a configurable occupancy. Issue #9660
  - addStops2Routes.py: added option **--relpos** for configuring stop position along edge. Issue #9795
  - Added tool [generateParkingAreas.py](Tools/Misc.md#generateparkingareaspy) to generate parkingAreas for some or all edges of a network. Issue #9659
  - plot_net_dump.py now supports option **--internal** for plotting internal edges. Issue #9729
  - randomTrips.py now supports option **--random-depart** to randomize departure times. Issue #9735
  - tripinfoByType.py: now supports option **--interval** to aggregated data by depart time (or by arrival time with option **--by-arrivals**) Issue #9746
  - netdiff.py: now supports option **--plain-geo** to write locational diffs in geo coordinates. Issue #9808
  - netdiff.py: now also writes diff for edge type file. Issue #9807
  - implausibleRoutes.py: now supports option **--xml-output** to write route scores for post-processing. Issue #9862
  - randomTrips.py: now support option **--fringe-junctions** to determine the fringe from [junction attribute 'fringe'](Networks/PlainXML.md#fringe). Issue #9948
  - [emissionsDrivingCycle](Tools/Emissions.md#emissionsdrivingcycle) now permits loading of [electric vehicle params](Models/Electric.md#defining_electric_vehicles) via the new options **--vtype** and **--additional-files**. Issue #9930
  - aggregateBatteryOutput.py: Tool used for aggregate battery outputs in custom intervals. Issue #10583

### Miscellaneous

- SUMO can now be installed from python wheels. This provides up to date [release](Downloads.md#python_packages_virtual_environments) and nightly versions (via test.pypi.org) for all platforms. #4639
- Added [documentation on road capacity and headways](Simulation/RoadCapacity.md). Issue #9870
- Added [documentation for signal plan visualization](Simulation/Traffic_Lights.md#signal_plan_visualization)
- Traffic light type 'NEMA' now uses attribute 'offset' instead of param key="offset". Issue #9804
- The [generated detector names for actuated traffic lights](Simulation/Traffic_Lights.md#detectors) have been simplified. Issue #9955
- Speed up Visual Studio build with sccache (only works with Ninja not with Visual Studio projects). Issue #9290
- The text "Loading configuration" is printed now only if **--verbose** is given. Issue #9743
- vType attribute 'minGapLat' is now stored for each vehicle ([similar to other lane change model attributes](TraCI/Change_Vehicle_State.md#relationship_between_lanechange_model_attributes_and_vtypes)). Issue #9769
- The download 'sumo-all.zip' is no longer provided (use git clone instead). Issue #9794
- Updated Eigen library Issue #9613
- Updated GDAL library Issue #9614
- Updated gtest library Issue #9616
- The ubuntu ppa now contains ARM builds as well


## Older Versions

- [Changes to versions 1.9.0, 1.9.1, 1.9.2, 1.10.0 and 1.11.0 (2021 releases)](Z/Changes_in_2021_releases.md)
- [Changes to versions 1.5.0, 1.6.0, 1.7.0 and 1.8.0 (2020 releases)](Z/Changes_in_2020_releases.md)
- [Changes to versions 1.2.0, 1.3.0, 1.3.1 and 1.4.0 (2019 releases)](Z/Changes_in_2019_releases.md)
- [Changes to versions 1.0.0, 1.0.1 and 1.1.0 (2018 releases)](Z/Changes_in_2018_releases.md)
- [Changes to versions 0.29.0, 0.30.0, 0.31.0 and 0.32.0 (2017 releases)](Z/Changes_in_2017_releases.md)
- [Changes to versions 0.26.0, 0.27.0, 0.27.1 and 0.28.0 (2016 releases)](Z/Changes_in_2016_releases.md)
- [Changes to versions 0.23.0, 0.24.0 and 0.25.0 (2015 releases)](Z/Changes_in_2015_releases.md)
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
