---
title: Changes in the 2019 releases (versions 1.2.0, 1.3.0, 1.3.1 and 1.4.0)
---

## Version 1.4.0 (10.12.2019)

### Bugfixes

- Simulation
  - Fixed crash when setting a negative value for option **--device.rerouting.pre-period** Issue #6012
  - Avoiding unnecessary rerouting while trips and flows are delayed from insertion. Issue #6013
  - Lanechange reason is now written in lanechange-output when using the [simple continuous lanechange model](../Simulation/SublaneModel.md#simple_continous_lane-change_model). Issue #6014
  - Fixed invalid mode change from car during intermodal routing. Issue #6070
  - Fixed invalid cooperative speed adjustment in sublane model. Issues #6266, #6267
  - Fixed crash related to parkingAreaReroute. Issue #6283
  - Fixed startup delay in railway simulation when begin time is set to a high value. Issue #6321
  - Fixed invalid state file names when using human-readable times. Issue #6365
  - Fixed invalid energy usage (battery parameters ignored) in emission-ouput, full-output, tripinfo-output and edgeData-emissions. Issue #6364, #6359
  
- netedit
  - Fixed crash when defining walks between disconnected locations. Issue #5369
  - Fixed crash transforming trips to flows. Issue #6050
  - Now blocked shapes can be moved again. Issue #6053
  - Fixed invalid snap to grid when trying to move junction. Issue #6067
  - Coloring edges by generic parameter is now working. Issue #6062
  - "Open in sumo-gui" option now works correctly when there are spaces in the path. Issue #6227
  - Fixed bug where multiple junctions could mistakenly be created in the same place. Issue #6228
  - Fixed invalid lane shapes while moving edge geometry in a lefthand network. Issue #6234
  - Fixed invalid edge shape when using 'replace junction' ahead of junction with custom shape. Issue #5777
  - Connection mode function 'select dead starts' now correctly selects lanes without predecessor (regression in 1.0.0). Issue #6278
  - Fixed slowdown clicking over edges with high concentration of routes. Issue #6276
  - Fixed crash when clicking in message window with invalid crossing. Issue #6368

- netconvert
  - Fixed bugs in classification of roundabouts. Issue #6032
  - Preventing invalid double tracks when using typemap [osmNetconvertRailUsage.typ.xml]({{Source}}data/typemap/osmNetconvertRailUsage.typ.xml). Issue #6056
  - Disabling invalid handling of railway track count in OSM input. Issue #6057
  - Missing track numbers in bidirectional railway edges. Issue #6058
  - Corrected handling of user-specified edge length when adding bidirectional railway edges. Issue #6280
  - Fixed invalid junction and connection shapes in plain-xml-output when using option **--proj.plain-geo** Issue #6066
  - Fixed invalid geometry when using option **--geometry.max-segment-length** Issue #6074
  - Fixed invalid network when using option **--geometry.split** Issue #6075
  - Fixed invalid handling of custom crossing shape when network coordinates are shifted. Issue #4621
  - Fixed invalid error message when loading .tll-file with tlLogid-programID other than '0'. Issue #6160
  - Fixed missing bidirectional edges due to invalid geometry-manipulations. Issue #6370, #6366
  - Avoiding superfluous nodes when using options **--geometry.remove** and **--ptline-output**
  
- MESO
  - device.emissions now collects data. Issue #6086
  - Fixed crash when using device.btsender or device.btreceiver. Issue #6087
  - Fixed crash when loading network with internal lanes that have neighbor ('neigh') data. Issue #6205
  - Fixed crash when saving state while using calibrators. Issue #6369

- sumo-gui
  - Sublane-borders are no longer drawn for railways and sidewalks (where the sublane model does not apply).
  - Fixed crash when using vehicle popup-menu after the vehicle has left the simulation. Issue #6107
  - Fixed crash after removing view with active vehicle visualization once the vehicle leaves the simulation. Issue #6112
  - "Open in netedit" option now works correctly when there are spaces in the path. Issue #6227

- duarouter
  - Fixed invalid mode change from car during intermodal routing. Issue #6070
  - Fixed invalid car routes during intermodal routing. Issue #6077
  - Fixed crash when loading network imported from VISUM. Issue #6358

- TraCI
  - TraaS functions *Trafficlight.getControlledJunctions*, *Edge.setEffort*, *Edge.getEffort*, *Edge.adaptTraveltime*", *Edge.getAdaptedTraveltime* are now working. Issue #6019  
  - Vehicles being controlled by moveToXY are no longer ignored by cross-traffic while on an intersection. Issue #6140
  - Traffic light programs of type 'actuated' and 'delay_based' can now be created. Issue #4324


### Enhancements
- All Applications
  - Environment variables can now be referenced in configuration files using a shell-like syntax: `<net-file value="${NETFILENAME}.net.xml"/>`. Issue #1901
  - The new option **--aggregate-warnings** can be used to group warnings of the same type and give only their count after the first N warnings occured.
  
- Simulation
  - Simulation of parking vehicles now optionally takes into account [maneuvering times](../Simulation/ParkingArea.md#modelling_maneuvering_times_when_entering_and_leaving_the_parking_space). Issue #5956
  - Vehicle stop elements now support attribute 'speed'. This can be used to force a vehicle to drive with a fixed speed on a particular stretch of lane. Issue #1020
  - When defining a person ride, setting attribute `lines="ANY"` can now be used to take any vehicle that stops at the destination busStop or destination edge. Issue #6039
  - when defining vehicle stops, the new attribute 'extension' can be used to limit delays when too many people are boarding or when expected persons do not show up. Issue #6072 #3635
  - Improved smoothness of acceleration / deceleration for carFollowModel 'IDM' when speed limits change and there is no leader vehicle. Issue #6080
  - Persons can now start their plan in a driving vehicle by setting `depart="triggered"`. Issue #4042
  - Traffic light type 'actuated' now supports skipping phases by defining a list of alternative successor phases in phase attribute 'next'. Issue #5042
  - stopinfo-output now includes optional stop attributes 'tripId' and 'line'. Issue #6320
  - Vehicles now support the new symbolic departPos value 'stop' which lets the start at the first stop along their route. Issue #6322
  - Permissions of internal lanes (connections) are now taken into account independent of the lanes before and after the intersection. Issue #6210
  - Option **--duration-log.statistics** can now be set with shortcut **-t**. Issue #6378
  - added new vType parameter *lcLaneDiscipline* to control whether vehicles will stay between lanes to drive faster (sublane model only)
  - added new vTypeParameter *lcSigma* to control lateral imperfection (sublane model and simple continuous lanechange model)
  - Option **--lanechange-duration** can now be used together with opposite-direction-driving. Issue #6270

- MESO
  - Vehicles that reach maximum impatience can now pass junctions from an unprioritized road regardless of foe traffic. The old behavior can be restored by disabling impatience growth via **--time-to-impatience 0**. Issue #5830
  
- netconvert
  - Railway network input with sharp angles along an edge is now corrected by default. (The previous behavior can be enabled with option **--geometry.min-angle.fix.railways false**). Issue #6921
  - Pedestrian paths are no longer checked for minimum turning angles (greatly reducing superfluous warnings). Issue #6022
  - The option **--opposites.guess.fix-length** is no longer required when using networks with default lengths (no user-set values). The resulting networks will automatically adapt their edge lengths when changing geometry. Issue #6028
  - When importing public transport lines (**--ptline-output**), edge permissions are now adapted to allow the respective public transport vehicle. Issue #3437
  - When importing public transport lines from OSM, stops that are not part of the road network are now mapped to the nearest line edge (previously, these stops were discarded). Issue #6114
  - Permissions can now be set for individual connections independent of incoming and outgoing lanes. Issue #6217
  - Regional road names (A9, I-95) are now exported as edge params. They are also exported as Regional Name ID in dlr-navteq-output. Issue #6256
  - Added option **--offset.z** to apply a constant z-offset to a simulation network. Issue #6274
  - Added option **--tls.ignore-internal-junction-jam** to force building traffic light right-of-way rules without mutual response flags. Issue #6269
  - Now VISUM networks in English, French and Italian can be imported (only German networks were supported previously). Issue #1767
  - Improved handling of VISUM connector-edges. By default, no extra edges are generated anymore and instead the regular network edges are assigned to the TAZ. Issue #6356
  - Now TAZ imported from VISUM can be written to a separate file using option **--taz-output**. Issue #6349

- netedit
  - Now additionals and routes can optionally be loaded using option "Open in sumo-gui"). Issue #6049
  - Added shortcuts (Shift F1,F2,F3) for edge templates. Issue #6033
  - VTypes can be edited in multiple selected vehicles. Issue #6051
  - Improved support of GNEStops. Issue #5367
  
- sumo-gui
  - Added 'select reachable' function to the lane context-menu to select all lanes that can be reached by a given vehicle class from the origin lane. After such an operation, the coloring scheme 'by reachability' can be used to render an isochrone travel time map. Issue #3650
  - The currently loaded options can now be saved to a configuration via the file menu. Issue #5707  
  - The object locator dialog now allows filtering the object list by matching all ids/names that include a given text string.
  - Added option for rendering the (relative) route index for each edge when drawing a vehicles route. Issue #6199
  - Traffic light parameter dialog now includes more values describing the current phase (phase, phase name, duration, minDur, maxDur, running duration)
  - Shift-left-clicking on a vehicle now starts tracking
  - The speedFactor for the currently tracked vehicle can now be set via a new slider
  - The probabilities for routeProbReroute can now be toggled between 100% and 0% by shift-clicking on the rerouter symbol or on the new route-direction arrows.
  
- TraCI
  - Added function 'traci.trafficlight.getServedPersonCount' to return the number of pedestrians that would be served by all the crossings for a given phase index. Issue #6088
  - Can now access vehicle device parameters 'device.ssm.minTTC', 'device.ssm.maxDRAC', 'device.ssm.minPET'. Issue #4983
  - Added function 'traci.simulation.writeMessage' to append a custom message to the log file (and sumo-gui message window). Issue #6249
  - Added context subscription filter for viewing angle 'traci.vehicle.addSubscriptionFilterFieldOfVision'. Issue #6273
  
- Tools
  - When importing public transport traffic with osmWebWizard, vehicles from incomplete lines now start and end their routes at the network border instead of starting at the first stop. Issue #6025
  - Added new tool [analyzePersonPlans.py](../Tools/Routes.md) to count the different types of plans in a route file. Issue #6083
  - Added new tool [matsim_importPlans.py](../Tools/Import/MATSim.md) to import intermodal traffic plans from MATSim
  - Added new tool [generateContinuousRerouters.py](../Tools/Misc.md#generatecontinuousrerouterspy) to easily create a simulation where vehicles circulate according to configured turning ratios without ever arriving. #6310
  
### Other
- sumo-gui, netedit
  - The hotkey for 'Edit Visualization' was changed to 'F9' to avoid conflicts when pasting clipboard.
  - Zooming now centers on the mouse-position by default instead of on the screen center. As before, the behavior can be toggled using the icon in the view menu bar and persists across application restarts. Issue #5992
- netconvert
  - By default repeated warnings of the same type are aggregated (only their number is given beyond the initial 5). To see all warnings, the option **--aggregate-warnings -1** can be used. Issue #6335
  - The default number of lanes when importing 'highway.secondary' from OSM was changed from 2 to 1. Issue #5690
- Documentation 
  - moved from MediaWiki to MkDocs
  - Added ['Hello World' tutorial](../Tutorials/Hello_World.md) that shows how to create a simulation by only using netedit and sumo-gui (no command line or text editor necessary).
  - Added new example simulation in 'docs/examples/sumo/model_railroad'. The speed of the train as well as the railroad switches can be controlled while the simulation is running. Merry Christmas!

## Version 1.3.1 (27.08.2019)

### Bugfixes

- netedit
  - Fixed crash when defining vehicle stops. Issue #5963 (regression in 1.3.0)
  - Fixed crashing and invalid errors when editing vehicle type attributes. Issue #5957 (regression in 1.3.0)
  - Fixed crash after defining flow on unusable edges. Issue #5958
  - Fixed crash after loading flow with invalid route. Issue #5967
  - Fixed bug where new vehicles could not be created if trips with a specific id already existed. Issue #5969
  - Fixed invalid application focus after activating checkboxes Issue #5966 (regression in 1.2.0)
  - Attribute *trainType* for carFollowModel *Rail* can now be set. Issue #5959

- Simulation
  - Fixed crash when loading rail signals with long gaps in between. Issue #5983
  - Fixed invalid restrictions on vType parameters. Issue #5991 (regression in 1.3.0)

- netconvert
  - Fixed high running time when using edge-removal options with long edge lists. Issue #5954
  - Fixed creation of invalid network after defining invalid splits. Issue #5980

- duarouter
  - Fixed crash on empty vType in input

- TraCI / libsumo
  - Function *vehicle.changeLaneRelative* is now working with a negative lane offset. Issue #5990
  - several functions were adopted to have the same interface in libsumo and the python client, see Issue #5523

- Tools
  - flowrouter.py can now route vehicles on networks with isolated edges and pedestrian structures. Issue #5951

### Enhancements

- netedit
  - Added menu option for recomputing routes of trips and flows in demand mode (F5) Issue #5978

- TraCI / libsumo
  - You can now trigger the usage of libsumo instead of TraCI in Python by setting the environment variable LIBSUMO_AS_TRACI to any value.

- Tools,
  - [traceExporter.py](../Tools/TraceExporter.md) now supports conversion of fcd-output to KML. Issue #1989

### Other

- Miscellaneous
  - Updated SUMOLibraries structure. To build on windows, the libraries must be updated as well.
  - removed doxygen docu from the distribution

## Version 1.3.0 (20.08.2019)

### Bugfixes

- Simulation
  - Fixed invalid default parameters for the electrical vehicle model that were causing too high energy consumption. Issue #5505
  - Fixed deadlock on controlled intersection with pedestrian crossing. Issue #5487 (regression in 1.1.0)
  - Fixed invalid emergency braking in jammed scenario. Issue #5632
  - Fixed inappropriate lane choice when approaching a multi-lane roundabout in dense traffic. Issue #2576, Issue #2634
  - Rail signals ahead of uncontrolled switches now properly take the vehicle route into account (this was causing invalid red states previously). Issue #5137
  - Options **--tripinfo-output.write-unfinished, --vehroute-output.write-unfinished** now also apply to persons. Issue #3939
  - Fixed bug that could corrupt pedestrian routes when walking across very short lanes. Issue #5661
  - Fixed bug that could cause sumo to freeze when using opposite-direction driving.
  - Fixed crash when using option **--device.rerouting.pre-period 0** with a **--scale** lower than 1. Issue #5704
  - Fixed emergency braking after lane changing when using `carFollowMode="Wiedemann"`. Issue #5711
  - Fixed exaggerated braking to avoid overtaking on the right. Issue #5713
  - Fixed invalid junction rules when using sublane simulation on lefthand-networks Issue #5738
  - Fixed invalid edgeData and tripinfo output in the [mesoscopic model](../Simulation/Meso.md) when vehicles are completely jammed. Issue #5722
  - Fixed crash when using SSM-Device with opposite-direction-driving Issue #5231, #5839
  - FullOutput now returns all speeds as m/s. Issue #5787
  - Fixed invalid error when using calibrators with a fixed departLane. Issue #5794
  - Fixed invalid jamming at occupied parkingAreas close to the start of a lane. Issue #5864
  - Fixed detector placement at actuated traffic light (avoiding warnings of the form *minDur X is too short for detector for a detector gap of Y*) Issue #5119
  - Access edges to stops from lanes which do not allow pedestrians are now ignored. Issue #5890
  - Fixed crash on using ToC device with an unknown vehicle type. Issue #5761
  - Route length in tripinfo output is now correct also when rerouting happens. Issue #5755
  - When combining `<closingReroute>` and `<destProbReroute>`, all vehicles that cannot reach their target due to the closing are rerouted. Issue #5904
  - Fixed bug where vehicles could enter a fully occupied parkingArea. Issue #5905
  - Fixed bug where vehicles would fail to exit from a parkingArea. Issue #5907
  - Fixed unusable person plan when loading a personTrip with a non-zero departPos. Issue #5927

- sumo-gui
  - Fixed crash when rendering short vehicles as *simple shapes* with guiShape *truck/trailer* and *truck/semitrailer*. Issue #5510 (regression in 1.2.0)
  - Pedestrian crossings and walkingareas are no longer listed in the edge locator dialog by default (they can still be enabled by checking *Show internal structures*. Issue #5613
  - Fixed invalid image position and size when rendering vehicles as raster images. Issue #5688 (regression in 1.2.0)
  - Fixed invalid vehicle coloring in mesoscopic mode. (regression in 0.32.0).
  - Fixed invalid vehicle angle when using the sublane model in lefthand-networks. Issue #5737
  - Fixed 3D-View rotation when holding middle-mouse button.

- netconvert
  - Fixed handling of custom shapes in lefthand networks (for connections, crossings, walkingareas). Issue #5507
  - Fixed missing connections when importing OpenDRIVE networks with short laneSections in connecting roads. Issue #5504
  - Fixed invalid link direction at intermodal intersection and at intersections with sharp angles. Issue #5511, Issue #5512
  - Fixed invalid large traffic light clusters when using options **--tls.guess --tls.join**. Issue #5524
  - Fixed invalid custom edge lengths when using option **--geometry.remove** Issue #5612
  - Zipper junctions with multiple incoming edges are now supported. Issue #5657
  - Fixed error when guessing ramps which are close to each other. Issue #5681
  - Option **--tls.guess** now works correctly at junctions with 2 incoming edges. Issue #5684
  - Fixed invalid junction shape at geometry-like nodes where only the lane-width changes. Issue #5749
  - Fixed invalid lane geometry when defining `<split>` for lefthand network. Issue #5856
  - VISSIM networks can now also have multiple lanes connected to one. Issue #5560
  - Option **--junctions.scurve-stretch** is now working for junctions with non-default radius. Issue #5881
  - Fixed exaggerated lane widths when importing briefly-orthogonal border lanes from OpenDRIVE. Issue #5888

- netedit
  - Crossing tls indices are now properly reset when deleting traffic light. Issue #5549
  - Controlled pedestrian crossings at a *rail_crossing* nodes are no longer lost when saving the network with netedit. Issue #5559 (regression in 1.0.0)
  - Snap-to-grid is now working in networks with elevation data. Issue #5878
  - Fixed bug where neigh-edges could get lost upon loading and saving a network. Issue #5916
  - Demand mode issues Issue #5576
    - Creating routes from non-consecutive edges is now working
    - Flow attribute route is now correctly saved
    - Fixed bug where some demand attributes could not be edited

- MESO
  - Fixed invalid *departPos* in tripinfo-output when loading saved state. Issue #5819

- duarouter
  - Option **--write-trips** is now working for persons. Issue #5563

- jtrrouter
  - Vehicle types with PHEMlight emission class can now be handled. Issue #5669

- TraCI / Libsumo
  - TraCI does **not** execute another step when close() is called. Please recheck your scripts, they might finish one step earlier than before.
  - Fixed TraaS method *Simulation.convertRoad*. Issue #5478
  - Fixed bug where vehicle is frozen after removing stop at stopping place via TraCI. Issue #5561
  - Fixed invalid distance to far-away TLS returned by *vehicle.getNextTLS* Issue #5568
  - Fixed crash due to invalid person list at busStop. Issue #5620
  - Persons are no longer listed before their departure in *traci.person.getIDList()* Issue #5674 (regression in 1.1.0)
  - Fixed crash when using parallel rerouting with TraCI. Issue #5553
  - Function *traci.vehicle.setStop* now correctly updates an existing stop when setting a new *until*-value. Issue #5712
  - TraaS function *SumoTraciConection.close()* now cleanly disconnects from the SUMO server. Issue #4962
  - Fixed several issues with position conversions when the edge has a length different from the geometrical distance. Issue #5780
  - Libsumo now supports getAllSubscriptionResults
  - Function *traci.vehicle.moveTo* can now move vehicles onto internal lanes. Issue #5932
  - Traffic lights for junction types rail_signal and rail_crossing can now be switched off with *trafficlight.setProgram("off")* and remote-controlled with *trafficlight.setRedYellowGreenState*. Issue #5946

- General
  - The build version number is updated correctly after all git updates. Issue #3963

### Enhancements

- Simulation
  - Added junction model parameter *jmDriveAfterYellowTime* to configure driving at yellow behavior. Issue #5474
  - calibrators now accept attribute *vTypes* to restrict their application (insertion/removal) to selected vehicle types. Issue #3082
  - Vehicle [`<stops>`s](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#stops) now support the optional attributes tripId and line to track the current state when serving a cyclical public transport route. Issue #5280, Issue #5514
  - Added vehicle class *rail_fast* to model [High-Speed-Rail](https://en.wikipedia.org/wiki/High-speed_rail) Issue #5525
  - Netstate-output now includes attribute *speedLat* when using a [model for lateral dynamics](../Simulation/SublaneModel.md). Issue #5636
  - Pedestrians now switch to *jam resolving* behavior earlier when jammed on a pedestrian crossing. The time threshold can be configured with the new option **--pedestrian.striping.jamtime.crossing** {{DT_TIME}} (default 10s) Issue #5662
  - Extended [ToC Model](../ToC_Device.md#configuration) by various functionalities (4 new parameters: dynamicToCThreshold, dynamicMRMProbability, mrmKeepRight, maxPreparationAccel). Issue #5484
  - Detectors for actuated traffic lights can now be placed on upstream lane to achieve the desired distance to the stop line. Issue #5628
  - Added lane-change model parameter *lcOvertakeRight* to model violation of rules against overtaking on the right side. Issue #5633
  - Lanechange-output now includes the speeds of surrounding vehicles at the time of lane-change. Issue #5744
  - Added new departSpeed values desired (departure with speedLimit * speedFactor) and speedLimit (departure with speedLimit). Issue #2024
  - Added new carFollowModel *W99* which is a 10-Parameter version of the Wiedemann model. Issue #5765
  - Saved states now include lane ids and only the lanes with traffic on them. This reduces state file size and also permits loading state files with a modified network. Issue #5872
  - Initial "pseudo routes" (consisting of start and end edge only) are only written optionally. Issue #5639
  - Traffic lights with type delay_based now use a default detection range of 100m (instead of using the whole lane length). This greatly improves efficiency by avoiding interference from far-away upstream junctions. The default range can be set using the new option **--tls.delay_based.detector-range** {{DT_FLOAT}}. Issue #5897
  - Added battery model parameter "recuperationEfficencyByDecel" to vary the recuperation efficiency based on the deceleration level. Thanks to Sagaama Insaf for the contribution. Issue #5675

- netconvert
  - Now importing High-Speed-Rail tracks from OSM Issue #5525
  - Now importing attribute railway:bidirectional from OSM Issue #5531
  - Now importing track number (track_ref) parameter from OSM Issue #5533
  - Railway usage information (main, branch, industrial, ...) can now be imported from OSM by loading the new typemap [osmNetconvertRailUsage.typ.xml](../Networks/Import/OpenStreetMap.md#recommended_typemaps). Issue #5529
  - Vehicle class *rail_electric* is now correctly assigned depending on railway electrification. Issue #5528
  - Now distinguishing more junction types in verbose summary. Issue #5541
  - Added experimental multi-language support for VISUM import using the new option **--visum.language-file** and language mapping files in [{{SUMO}}/data/lang]({{Source}}data/lang). Issue #1767
  - Added option **--junctions.right-before-left.speed-threshold** {{DT_FLOAT}} to influence the heuristic for guessing junction type (priority/right-before-left) from edge speeds. Issue #5799
  - Added option **--join-lanes** {{DT_BOOL}} to allow merging of adjacent non-vehicle lanes (sidewalk, green verge). Issue #5880
  - Added option **--geometry.remove.width-tolerance** {{DT_FLOAT}} to allow removal of geometry-like nodes when lane widths of merged edges differ no more than the given threshold. Issue #5883
  - Option **--junctions.scurve-stretch** is no longer applied for non-vehicular connections. Issue #5887
  - Added options for generating bicycle lanes: **--bikelanes.guess --bikelanes.guess.max-speed --bikelanes.guess.min-speed --bikelanes.guess.from-permissions --bikelanes.guess.exclude --default.bikelane-width** [These options work the same as the corresponding sidewalk options](../Simulation/Bicycles.md#generating_a_network_with_bike_lanes) and can also be used with [netgenerate](../netgenerate.md). Issue #5924

- netedit
  - Inverting selection now takes selection locks into account. Issue #5615
  - When setting tltype to *actuated*, suitable values for *minDur* and *maxDur* will be added to the phases. Issue #5637
  - Added support for Vehicles, Flows and Trips. Issue #5585 Issue #5086 Issue #5050
  - Partial support of GNEPersons Issue #5369

- sumo-gui
  - Added new visualization presets *selection* (coloring everything according to selection status) and *rail* (analyze rail networks by showing allowed driving directions for all tracks and highlighting rail signals when zoomed out). Issue #5660
  - Added context menu option to show/hide detectors for actuated traffic lights Issue #5627
  - Added visualization option for drawing an edge color legend. Issue #5682
  - Vehicles and persons can now be removed via right-click menu (*remove*). Issue #5649
  - All Vehicle and person parameters that change during the simulation are now updated in any open parameter window. (previously string parameters were always static). Issue #4210
  - The parkingAreas parameter dialog now includes the number of alternative parkingAreas due to loaded *parkingAreaReroute* definitions.
  - The *Recalibrate Rainbow* functionality now generates a distinct color for every permission code when coloring *by permissions*. Issue #5852

- polyconvert
  - Importing railway entities from OSM is now supported and [a new typemap](../Networks/Import/OpenStreetMap.md#railway-specific_objects) was added for this. Issue #5572
  - Shapefile import now supports typemaps and also image files in type maps. Issue #5891

- TraCI
  - Added function *simulation.getBusStopWaitingIDList* to retrieve
    waiting persons. Issue #5493
  - Added function *getPersonCapacity* to vehicle and vehicletype
    domains. Issue #5519
  - Added function *addDynamics* to polygon domain. Issue #5396
  - Added function *highlight* to poi and vehicle domain. Issue #5396
  - TraaS function *Vehicle.setStop* now supports the arguments
    *startPos* and *until* in line with the other clients. Issue #5710
  - Added subscription filtering to C++ client. Issue #5772
  - Added function *person.appendStage* which allows adding a stage object directly to a person plan. #5498
  - Added function *person.replaceStage* which allows replacing an upcoming stage with a stage object. #5797
  - Added function *vehicle.getLateralSpeed* to the python and C++ client. #5010
  - StepListeners in the python client can now be added to a single connection (also allows using them with Flow). Issue #5863
  - The python client now tries again to start sumo and connect with a different port if the port chosen was in use. Issue #5802
  - Function *vehicle.moveTo* can now be used to move the vehicle to any lane on its route including those that were already passed.

- Tools
  - Added new toolbox [{{SUMO}}/tools/contributed/saga]({{Source}}tools/contributed/) (SUMO Activity Generation) to create intermodal scenarios from OSM data. This includes building a virtual population and generating mobility plans for a while day.
  - [Public transport import from
    OSM](../Tutorials/PT_from_OpenStreetMap.md) (also used by
    [osmWebWizard](../Tools/Import/OSM.md#osmwebwizardpy) now
    filters out lines that only run at night. If the option **--night** is set,
    only night-service lines are exported. Issue #5548
  - Improving UTF8 support for xml2csv.py and csv2xml.py. Issue #5538, Issue #5588
  - Added a new tool
    [generateTurnRatios.py](../Tools/Misc.md#generateturnratiospy)
    for generating turning ratios according to a given route file. Issue #1666
  - The option cost modifier in
    [duaIterate.py](../Tools/Assign.md#dua-iteratepy) is
    removed, since it is used for specific projects. Ticket #5594 is open
    to check the respective content for publication and to extend
    the cost modifier function.
  - Added [randomTrips.py](../Tools/Trip.md#edge_probabilities)
    options **--angle-factor** {{DT_FLOAT}} and **--angle** {{DT_FLOAT}} to modify trip probabilities by direction of
    travel. Issue #5642
  - checkStyle.py now also checks Java files and can be used for single files. Issue #5653, Issue #5652
  - extractTest.py can now download tests directly from sumo.dlr.de. Issue #1092
- General
  - All applications can now read and write gzipped files (except for configurations). Issue #5448
  - The Windows build now automatically installs the needed DLLs in the bin dir (if SUMOLibraries are present). Issue #4887

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

- netconvert
  - Network version is now 1.3
  - In the previous version, setting options **--tls.guess --tls.join** activated a heuristic
    that could create joint controllers for clusters of uncontrolled
    nodes (which would not be guessed as controlled individually).
    Now this heuristic must be activated explicitly using option **--tls.guess.joining**.

- netedit
  - Changed button in allow/disallow-dialog from *allow only
    non-road vehicles* to *allow only road vehicles* vClasses. Issue #5526

- netgenerate
  - Option **--rand.min-angle** {{DT_FLOAT}} now expects and argument in degrees instead of radians. Issue #5521

## Version 1.2.0 (16.04.2019)

### Bugfixes

- Simulation
  - Fixed too high insertion speeds for the ACC car following model leading to emergency braking events. Issue #4861
  - Fixed emergency deceleration after insertion when using IDM model. Issue #5040
  - Reduced emergency braking for the Wiedemann carFollowModel. Issue #3946
  - Vehicles approaching an occupied parkingArea where other vehicles wish to exit, now cooperate with the exiting vehicle instead of rerouting to another parkingArea. Issue #5128
  - Fixed error when loading a `<flow>` from saved simulation state. Issue #5131
  - Fixed crash when loading a `<flow>` from saved simulation state and vehroute-output is active Issue #5132
  - ParkingAreaReroute now takes subsequent stops into account when computing a new route. Issue #3659
  - ParkingAreaReroute now adapts subsequent person stages if the person returns to the same vehicle later. Issue #5164
  - Fixed a program crash potentially occurring when different car-following models were used in the same situation. Issue #5185
  - Fixed invalid route when adding trip with cyclical stops on the same edge. Issue #5206
  - Fixed invalid route and crashing when re-routing trip with cyclical stops. Issue #5205
  - Fixed freeze when setting `<vType>`-attribute *lcOpposite="0"* to disable opposite direction driving. Issue #5238
  - Person plans that start with a `<stop busStop="..."/>` and continue with a `<personTrip>` are now working. Issue #5298
  - Fixed invalid public transport routing when a `<personTrip>` is not the first item in the plan. Issue #5329
  - Simulation now termines if triggered vehicles are not inserted due to **--max-depart-delay** Issue #5420
  - Intersection behavior
    - Fixed routing bug where the cost of some left-turns was underestimated. Issue #4968
    - Fixed deadlock on intersection related to symmetrical left turns on multi-modal intersections. Issue #5004
    - Fixed invalid right-of-way behavior on multi-modal intersections Issue #5014
    - Fixed bug where vehicles would sometimes drive onto the intersection despite downstream jamming.
    - Fixed bugs where actuated traffic light would switch too early or too late. Issue #5161, Issue #5187 Issue #5119
    - Fixed deadlock at multi-lane roundabout Issue #5055
    - Fixed emergency deceleration when approaching a zipper node Issue #5080
    - Fixed collisions between vehicles and persons on walkingareas. Issue #3138, Issue #5423
  - Railway simulation
    - Update of averaged edge speeds within *device.rerouting* is now working correctly for bidirectional tracks. Issue #5060
    - Fixed behavior at *railSignal* when using *endOffset* for signal placement. Issue #5068
    - Fixed invalid behavior at *railSignal* when using bidirectional tracks (deadlocks / collisions). Issue #5069, Issue #5073, Issue #5062
    - Fixed detection of railway collisions. Issue #3686, Issue #5312
    - Railway insertion on bidirectional tracks now checks for presence of oncoming vehicles. Issue #5074

- sumo-gui
  - Fixed crash when using the 3D-View. Issue #4944
  - Pedestrian crossing traffic-light indices can be drawn again (regression in 1.1.0). Issue #4960
  - Loading breakpoints from a gui settings file using the settings dialog is now working. Issue #5015
  - Fix visual glitch when drawing rail carriages with exaggerated length. Issue #3907
  - Fixed invalid *pos* value in lane popup for lanes with a strong slope. Issue #5272
  - Stopped vehicles no longer have active brake signals.

- netedit
  - *split junction* is now working reliably in intermodal networks. Issue #4999
  - Fixed crash when [copying edge template](../Netedit/index.md#edge_template) with lane-specific attributes. Issue #5005
  - Fixed index of created lanes when adding restricted lanes with context menu. This is partly a regression fix and partly an improvement over the earlier behavior. Issue #5006
  - Inspection contour now works correctly for spread bidirectional rail edges. Issue #5064
  - Now showing correct edge length when using *endOffset*. Issue #5066
  - Defining e1Detector with negative position (counting backwards from the lane end) is now working Issue #4924
  - Setting connection attribute *uncontrolled* now takes effect. Issue #2599
  - Fixed crossing geometries after editing left-hand network. Issue #5265

- netconvert
  - Custom node and crossing shapes are now correctly shifted when using options **--offset.x, offset.y**. Issue #4621
  - Fixed invalid right of way rules at traffic light intersection with uncontrolled connections. Issue #5047, Issue #5048
  - Connection attribute *uncontrolled* is no longer lost when saving as *plain-xml*. Issue #2956
  - Normal right-of-way rules are never used for unsignalized railway switches where all edges have the same *priority* value. (all links will have linkstate *M*). Issue #5061
  - Fixed bug where connections were not imported from OpenDRIVE networks if junction internal edges have lane sections with different lane numbers. Issue #4812
  - Fixed bugs where superfluous traffic light phase were generated. Issue #5175, Issue #5174
  - Fixed bug where generated traffic light phases had unnecessary red lights. Issue #5177
  - Fixed missing connection at roundabout when importing *dlr-navteq networks*. Issue #5169
  - Fixed bugs in connection to option **--tls.guess-signals** Issue #5179, Issue #5117
  - Nodes that were joined due to a `<join>` declaration are no longer joined with further nodes when option **junctions.join** is set.
  - Fixed invalid street names in dlr-navteq output. Issue #5204
  - Fixed invalid edge geometry when importing VISSIM networks Issue #5218
  - Fixed bug where joined traffic lights could get lost when importing a .net.xml file with option **--tls.discard-simple**. Issue #5229
  - Fixed bug where traffic lights could get lost when importing a .net.xml file containing joined traffic lights with option **--tls.join**. Issue #5229
  - Fixed low-radius connection shapes for left turns at large intersections Issue #5254
  - Joining junctions now preserves the prior connection topology. Issue #874, Issue #1126
  - Loading patch files with `<crossing>` elements is now working. Issue #5317
  - Fixed invalid turnaround-lane in multimodal networks Issue #5193
  - Patching attributes of existing connections is now working. Issue #2174
  - The default edge priorities for link-roads imported from OSM (e.g. on- and offRamps) have been changed to avoid invalid right-of-way rules at intersections. Issue #5382

- duarouter
  - Fixed routing bug where the cost of some left-turns was underestimated. Issue #4968
  - vType attribute `speedDev="0"` is no longer lost in the output. Issue #5167
  - Person plans that start with a `<stop busStop="..."/>` and continue with a `<personTrip>` are now working. Issue #5298
  - Fixed invalid public transport routing when a `<personTrip>` is not the first item in the plan. Issue #5329

- dfrouter
  - Fixed duplicate vehicle ids in generated output. Issue #5381

- TraCI
  - Fixed crash when calling moveToXY for a vehicle that is driving on the opposite direction lane. Issue #5029
  - Fixed *getParameter* and added *setParameter* calls to TraaS client library. Issue #5099
  - Fixed *vehicle.setStop* in TraaS client library. Issue #5105
  - Fixed crash when calling *simulation.getDistance* to compute the driving distance between unconnected parts of the network Issue #5114
  - Fixed invalid driving distance result when calling *simulation.getDistanceRoad* and one of the edges is an internal edges after an internal junction. Issue #5114
  - TraaS functions *Trafficlight.setPhaseDuraton, Trafficlight.getPhaseDuration, Trafficlight.getNextSwitch, Trafficlight.setCompleteRedYellowGreenDefinition, Vehicle.setAdaptedTraveltime, Vehicle.getAdaptedTraveltime, Lanearea.getLastStepHaltingNumber* and *Lanearea.getLastStepVehicleIDs* are now working (regression due to protocol change in 1.0.0).
  - Fixed C++ client function *vehicle.getStopState* (regression due to protocol change in 1.0.0)
  - Fixed estimation of speed and acceleration after using moveToXY. Issue #5250
  - *traci.vehicle.getLaneChangeState* now returns correct information while controlling the vehicle with moveToXY in sublane simulation. Issue #5255
  - Fixed invalid vehicle position after mapping with *vehicle.moveToXY* onto a lane with strong slope. Issue #5272
  - Fixed invalid lane change when moving to a new edge with different lane number after calling *traci.vehicle.changeLane*. Issue #5309

- Tools
  - restored python3 support for [osmWebWizard.py](../Tools/Import/OSM.md#osmwebwizardpy) (regression in 1.1.0) Issue #5007
  - Fixed error when using sumolib functions *net.getNeighboringEdges* and *net.getNeighboringLanes* with the same net object. Issue #5246
  - [netdiff.py](../Tools/Net.md#netdiffpy) now handles `<param>` and `<crossing>`-elements.Issue #5156, Issue #5317

### Enhancements

- Simulation
  - added new input element `<personFlow>` Issue #1515
  - lanechange-output now includes *maneuverDistance* when using the sublane model. Issue #4940
  - Traffic light phases now support the optional attribute *name*. This attribute can be set and retrieved via TraCI and makes it easier to establish the correspondence between phase indexing in SUMO and phase descriptions used by other traffic engineering methods Issue #4788
  - Time losses due to passing intersections from a non-priority road are now anticipated when routing even when no vehicle has driven there previously. The anticipated loss can be configured using the new option **--weights.minor-penalty** {{DT_FLOAT}} (new default 1.5, previous behavior when set to 0). Issue #2202.
  - Vehicles approaching a stop with *parking=true* now signal their intention by activating the blinker. When exiting from such a stop in dense traffic they signal their intention by activating the blinker. Issue #5126
  - Vehicles approaching a signalized junction now either stop at the given stop position (stopOffset) at red or they drive on to cross the junction if they already passed the stop position. Issue #5162
  - Person [rides](../Specification/Persons.md#rides) now allow the special value *ANY* for the *lines* attribute. When this is set, persons may enter any vehicle that will stop at the destination *busStop* of the ride.
  - Option **--weights.random-factor** {{DT_FLOAT}} now also applies when routing persons. Issue #1353
  - Added output of harmonic mean speed to induction loops, which approximates the space mean speed. Issue #4919
  - Added controlability of reaction time (actionStepLength) to DriverState Model. Issue #5355
  - `<edgeData>`-output now supports attribute `detectPersons="walk"` which can be used to record pedestrian traffic instead of vehicular traffic. Issue #5252
  - Railway simulation improvements:
    - When a railSignal is placed on a short edge at the incoming network fringe, the signal will regulate train insertion for all trains longer than the insertion edge. Issue #5135
    - Trains can now reverse on bidirectional edges without the definition of a `<stop>`. Issue #5213
    - RailSignal logic no longer requires all railway switch nodes to be of type *railSignal*. Instead they can be guarded by other railSignal nodes along the incoming tracks. Issue #5091
    - Added new output option **--railsignal-block-output** {{DT_FILE}} to support analysis of the [generated signal block structure](../Simulation/Output/index.md#additional_debugging_outputs). Issue #5109

- sumo-gui
  - Major improvement in rendering speed
  - Can [now load edge-data](../sumo-gui.md#newer_versions) ([edgeData-output](../Simulation/Output/Lane-_or_Edge-based_Traffic_Measures.md), [marouter-output](../marouter.md#macroscopic_outputs.md), [randomTrips-weights](../Tools/Trip.md#customized_weights)) for visualization (time-varying edge colors). Issue #4916
  - The current phase index can now optionally be shown for traffic-light controlled junctions. If a name was set for the current phase it is shown as well. Issue #4927
  - The current lane-changing state is now included in the vehicle parameter dialog. Issue #5038
  - Reloading is now disabled while running as TraCI-server. Issue #5052
  - When *show-detectors* is set for actuated traffic lights, the detector outline will switch to green for detectors that are used to control the active phase. Issue #5168
  - Added option **--tls.actuated.show-detectors** {{DT_BOOL}} to set the default for actuated detector visibility.
  - Vehicles with guiShape *truck/trailer* and *truck/semitrailer* now bend when cornering. Issue #3100
  - Added vehicle route visualisation *Show Future Route* to the vehicle context menu. This only shows the remaining portion of the route.
  - Persons riding in a vehicle now have distinct seat position. Issue #1628
  - Persons waiting at a busStop can now wait in multiple rows according the specified stop length and personCapacity.
  - Added person drawing style *circles*
  - Added openGL gui settings option *FPS* to enable a frames-per-second display. Issue #5253
  - Can now locate objects by their name (streetname or generic parameter key="name"). Issue #5270
  - Railway simulation improvements:
    - Improved visibility of railSignal-state when zoomed out and junction-exaggeration is active. Issue #5058
    - railSignal state indicator is now drawn with an offset to indicate the applicable track direction. Issue #5070
    - Junction shapes are now longer drawn for railway switches at default GUI settings. Issue #1655
    - Drawing bidirectional railways in *spread* style is now supported. Edge IDs are also drawn at an offset to improve readability.
    - The visual length of railway carriages and locomotive [can now be configured](../Simulation/Railways.md#visualisation). Issue #1233

- netconvert
  - Improved junction joining heuristic to prevent superfluous joins. Issue #4987
  - [OpenDrive road objects](../Networks/Import/OpenDRIVE.md#road_objects) can now be imported Issue #4646
  - Road objects can now be embedded when [generating OpenDRIVE output](../Networks/Further_Outputs.md#embedding_road_objects).
  - Attribute *endOffset* can now be used to move the signal position for bidirectional rail edges. Issue #5063
  - Minimum phase duration for actuated traffic lights now takes road speed into account to improve traffic light efficiency. Issue #5127
  - all `<node>`-attributes are now also supported within a `<join>` element to affect the joined node. Issue #1982
  - Various improvements to the generation of traffic light plans. Issue #5191, Issue #5192, Issue #5194, Issue #5196
  - If a custom ID was assigned to a traffic light (different from the junction ID), it will no longer be joined with other traffic lights when option **--tls.join** is set. Issue #5198
  - Lane widths are now imported from VISSIM networks. Issue #5216
  - Changed option default for **--opendrive.advance-stopline** to 0 to improve generated geometries in most cases.
  - Roundabouts can now be disabled and removed by setting the node type to *right_before_left* Issue #2225

- netedit
  - Major improvement in rendering speed
  - Junction context menu function *split junctions* now restores original node ids Issue #4992
  - The new function *split junctions* and *reconnect* now recreates edges heuristically. Issue #4998
  - can now edit tls phase attributes *next* and *name*. Issue #4788
  - can now load additionals and shapes using -a "file.xml" or --additionals "file.xml". Issue #5049
  - Connection attributes *dir* and *state* can now be inspected and used as selection filter. Issue #3858
  - Can now locate objects by their name (streetname or generic parameter key="name"). Issue #5270
  - Roundabouts can now be removed by setting the node type to *right_before_left* Issue #2225

- duarouter
  - added new input element `<personFlow>` Issue #1515
  - Added option **--write-trips** {{DT_BOOL}} to output trips instead of routes. This is useful for validating trip input that shall be routed during simulation. Issue #4971
  - Added option **--write-trips.geo** {{DT_BOOL}} to write trips with attributes *fromLonLat, toLonLat* instead of *from* and *to*
  - Reading trips with attributes *fromLonLat, toLonLat, fromXY, toXY, viaLonLat, viaXY* is now supported. Issue #2182
  - Time losses due to passing intersections from a non-priority road are now anticipated. The anticipated loss can be configured using the new option **--weights.minor-penalty** {{DT_FLOAT}} (new default 1.5, previous behavior when set to 0). Issue #2202.
  - Added option **--weights.random-factor** {{DT_FLOAT}} to randomize routing results with bounded deviation from optimality. Issue #1353

- TraCI
  - Added multi-client support to [TraaS](../TraCI/TraaS.md) (*setOrder*) Issue #4957
  - *traci.vehicle.getNextTLS* now returns upcoming traffic lights for the whole route of the vehicle (before, only traffic lights until the first required lane change were returned). Issue #4974
  - Added functions *trafficlight.getPhaseName* and *trafficlight.getPhaseName* to all clients. Issue #4788
  - Extended the function *traci.vehicle.openGap()* to include an optional parameter specifying a reference vehicle. Issue #5151
  - Added function *traci.vehicle.getNeighbors()* and convenience wrappers (getLeftFollowers(), etc) to retrieve neighboring, lanechange relevant vehicles. Issue #5170
  - Added function *person.getSlope* to all clients. Issue #5171
  - Function *vehicle.changeLaneRelative(vehID, 0)* can now be used to a pin a vehicle to the current lane. Issue #5178

- Tools
  - [Generic parameters](../Simulation/GenericParameters.md) of edges, lanes, junctions and traffic lights are now supported in [sumolib](../Tools/Sumolib.md). Issue #4972
  - [randomTrips edge probabilities](../Tools/Trip.md#edge_probabilities) can now be scaled by [Generic edge parameters](../Simulation/GenericParameters.md). Issue #4963
  - Added new tool [tripinfoByTAZ.py](../Tools/Output.md#tripinfobytazpy) for aggregating tripinfo attributes by origin/destination pair.
  - [netdiff.py](../Tools/Net.md#netdiffpy) now supports the new option **--patch-on-import** to generate patch files which can be applied during initial network import. Issue #5293

- All Applications
  - The option **--help** now supports filtering by topic (e.g. **--help input**. Issue #5303)

### Other

- Simulation
  - The default [algorithm for averaging road speed to estimate
    rerouting travel
    times](../Demand/Automatic_Routing.md#edge_weights) was
    changed from *exponential average (0.5)* to *moving average
    (180s)*. This is better suited for averaging traffic light
    cycles in urban road networks. To restore the old behavior, the
    option **--device.rerouting.adaptation-weight 0.5** can be used. Issue #4975

- netconvert
  - Turn-arounds are no longer built at geometry-like nodes by
    default (when there is a node but no intersection so there would
    only be straight connections and turn-arounds). The old behavior
    can be restored by setting option **--no-turnarounds.geometry false**. Issue #5121
  - The default priorities for different railway edge types imported
    from OSM haven been changed so that all types use a different
    priority value.
  - The assigned edge speed for unrestricted motorways was lowered
    to 142km/h (from 300km/h) to better reflect real world speed
    distributions (together with the default speed factor
    distribution). Also, the default motorway speed was lowered from
    160km/h to 142km/h for the same reason. Issue #5434

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
    API) instead of LinkedList<SumoStringList\>

- Documentation
  - Added Tutorial for [Manhattan
    Mobility](../Tutorials/Manhattan.md).
  - The [TraaS](../TraCI/TraaS.md) library is now documented
    within the wiki.