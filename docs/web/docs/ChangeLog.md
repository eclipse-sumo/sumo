---
title: ChangeLog
---

## Git Main ([nightly development builds](https://sumo.dlr.de/docs/Downloads.php#nightly_snapshots))

### Bugfixes

- sumo
  - Fixed crash in sublane simulation with unusual turning lanes #18030 (regression in 1.27.0)
  - Fixed invalid strategic lane choice when connections have reduced permissions #18034
  - Fixed extreme reduction in simulation speed when setting high road speed limits in a dense road network #18038
  - Fixed insufficient backward safety gaps with respect to minGap #18042
  - Fixed insufficient backward safety gaps on lanes that are shared by road and rail traffic #18041
  - Loading state with an invalid begin time no longer results in an unexpected simulation start time #18048
  - Fixed bug where sampling from vTypeDistribution diverged after calling **--load-state** #16989
  - Fixed diminished energy consumption from radial drag immediately after departure #18059
  - Fixed invalid error when vehicles with stops are affected by a closingReroute #18070
  - Vehicles with departLane "best" or "best_prob" are no longer inserted on forbidden edges #18077
  - Fixed crash when attempting to load networks with NaN in edge lengths or speeds (now raises an error instead) #18094
  - Fixed invalid driveway foes in moving-block mode #18106
  - Fixed invalid traffic light signal phase when loading simulation state (with WAUT) #18105
  - Fixed deadlock on junction with parallel internal lanes #18115
  - Fixed collision on junction with unusual geometry #5013  

- netedit
  - Fixed crash after renaming tlLogic #18116 (regression in 1.27.0)
  - Fixed invalid error message when loading a route with an unknown stopping place #16333
  - TAZs with one shape point no longer writes an invalid center #16845
  - selected connections now remain visible at high zoom when selection scaling is active #17309
  - rerouterDialog now allows to set special destination "terminateRoute" and "keepDestination" in destProbReroute #18072
  - rerouter intervals no longer disappears when editing attribute `edges` in inspect mode #18071
  - Fixed crash after calling undo and 'select parents' #18120
  - Fixed invalid edge removal when using function 'reduce' and having an edge selected together with its lanes #18121
  - Fixed awkward workflow for reducing the network based on edge attributes (i.e. type). It was previously necessary to add junctions to the selection (via a manual 'add parents' step) and easy to mess up connections. Now it works in the same way as the netconvert **--keep-edges...** options #18122

- sumo-gui
  - Fixed invalid color for shown route when activating vehicle id background color (meso only) #18112 (regression in 1.11.0)
  - meso vehicles no do not flicker while waiting in a queue #18045 (regression in 1.27.0)
  - meso vehicles do not overlap while queued on a single lane #18044 (regression in 1.27.0)
  - fixed crash in meso #18100 (regression in 1.27.0)
  - Fixed invalid meso segment markers in network with length/geometry mismatch #18036
  - busStop parameter "waitingDepth" now increases the visual width of the busStop as intended #18080

- meso
  - The default option **--no-internal-links** was changed to 'true' when running with **--mesosim**. This is a soft roll-back of #17842 to avoid #18131. The changed default is intended to be temporary until the model can be fixed to correctly propagate jams over short segments. (regression in 1.27.0)
  - traffic light type "delay_based" (unsupported by meso) now gives a warning #18026
  - fixed unsafe rail insertion #18028
  - fixed invalid rail signal state in network with internal links #6990, #18027

- duarouter
  - Fixed crash when called with **--repair** #18109 (regression in 1.24.0)

- TraCI
  - Fixed reversed driving after calling moveTo on a vehicle that is overtaking through the opposite direction #18051

- tools
  - routeSampler.py: can now load marouter output (routeDistributions) #18079
  - sumolib: getNeighboringEdges and getNeighboringLanes now return consistent results when called with different arguments for `includeJunctions` #18086
  - mapDetectors.py: fixed mapping issues for positions close to a junction coordinate #18085

### Enhancements

- sumo
  - CarFollowModel "Rail', can now include train resistance due to curvature by setting vType attribute `curveResistance="1"`. The underlying Röckl model exposes 5 configuration parameters (i.e. to adapt to different gauges and wheel bases). #18057
  - CarFollowingModels now save their state (VehicleVariables) #18055

- sumo-gui
  - Context menu function 'show cursor geo position in google-maps' now sets a map pin at the cursor position. (existing installations of SUMO must use the Application Settings dialog to change the query string to "https://www.google.com/maps/search/?api=1&query=%lat,%lon&zoom=19") #18114
  - Can now configure the color of parkingAreas and parkingSpaces (also in netedit) #16180

- netconvert
  - Option **--ignore-errors** can now be used to adapt public transport stops and ptlines to a reduced network #18125

- od2trips
  - Added option **--flow-output.poisson** to generate poission distributed insertions when option **--flow-output** is set. refs #18117

- tools
  - createScreenshotSequence.py: Added option **--relative** which interprets zoom/rotate/offset values relatively to the previous state #17974
  - gtfs2pt.py: keep longer stop when merging stops on the same lane, added option **--access-radius** for adding pedestrian access #17858
  - mapDetectors.py: Added option **--poi-output** for easier debugging
  - mapDetectors.py: now reports mapping errors #18084
  - edgeDataFromFlow.py: added options **--time-format** and **--time-offset** to handle arbitrary timestamp/date formats #18087
  - Added function `sumolib.net.node.getTLSID()` #18124

### Miscellaneous

- Changing the view (mouse move, zoom and pan) are now disabled in gaming mode #11112
- Fixed code signing issues for libsumo on macOS #18021
- Fixed problem when importing libsumo and pyarrow simultaneously on Windows #17324
- Installing libsumo no longer depends on the sumo binaries package #16588
- Added new [Railway scenario tutorial](Tutorials/RailwayScenario.md) #17929

## Version 1.27.0 (21.05.2026)

### Bugfixes

- sumo
  - Invalid separator in allow/disallow now results in error instead of warning again (regression in 1.21.0) #17709
  - Fixed bug where vehicles that are blocked from entering a stopping place do not collect enough timeLoss #17914 (regression in 1.26.0)
  - Fixed invalid error when setting option **--device.rerouting.mode 8** #17939 (regression in 1.26.0)
  - Stopping on a long busStop before reaching the designated spot due to jamming now permits passengers to exit if the vehicle is fully within the busStop #17635
  - Fixed invalid parking positions in network with a [length-geometry mismatch](Simulation/Distances.md#length-geometry-mismatch) #17640
  - Fixed bug where imprecise driving caused vehicles to enter a slower lane with excessive speed (this could cause negative timeLoss). #15435
  - Less crashes with parquet output (especially summary output) #17656
  - Fixed invalid error when loading networks with unusual walkingarea shape #17689 (also affected duarouter)
  - Fixed invalid stopping in network with lane-changing prohibition and minor-link-merge #17714
  - Fixed platform dependency in lane changer #17747
  - Rerouters now act according to **device.rerouting.mode** even when vehicles are not equipped with a rerouting device #17815
  - Fixed crash when setting **--default.departlane best** #17854
  - Fixed infinite loop when specifying invalid default depart lane #17850
  - Fixed bug where rerouting after parkingAreaReroute fails to add stops when input contains vias #17892
  - Fixed where vehicles that stop on the end of the lane take one extra step to continue #17916
  - Fixed invalid timeLoss computation for stopping vehicles #17915, #17916
  - Fixed inconsistent behavior of vehicles not stopping in the sublane model when they brake late for not overtaking right #17933
  - Fixed failure when loading routes with edges that have extended ascii characters in their id (also for duarouter) #17935
  - Fixed unsafe insertion at zipper link #17972
  - Option **--aggregate-warnings** is now working for more kinds of warnings #18015
  - Fixed crash in scenarios with bidirectional edges #18009, #18013
  - State-Loading:
    - Fixed invalid signal state when loading from saved state and using WAUT #17675
    - Lanechanging state is now restored after loading #2380
    - Fixed bugs that caused actuated traffic lights to differ in behavior after loading state #17849
    - Fixed miscellaneous bugs that were causing simulations to diverge after loading state #17532, #17887, #17889, #17895, #17897, #17902, #17900, #17920, #17921
    - Past stops are now present in vehroute output after loading state #17886
    - Fixed superfluous startPos, endPos in vehroute-output after loading state #17888
    - Fixed error when loading state with persons that were saved during an access stage #17896
    - Fixed invalid number of aborted rides after loading state #18011
    - Fixed bug where simulation with loaded waiting persons did not terminate #18017
  - Taxis:
    - Fixed invalid estimation of pickup-traveltime during taxi dispatch #17631, #17629
    - Fixed failure to group taxi passengers when the dispatch-period is low #17644
    - Fixed invalid taxi device warnings #17864
    - Fixed failure in stop assignment #17893
    - Taxis no longer block the road when idling if their passenger stops are defined as not-parking #17904
    - Fixed unsafe insertion before junction when using idle-algorithm *randomCircling* #17973
  - Railways:
    - Fixed crash when loading rail signal on road #17847 (regression in 1.22.0)
    - Fixed invalid driveWay foes in moving-block mode #17623, #17683, #71684
    - Fixed invalid driveway assignment involving uncontrolled links at complex junctions of type rail_signal #17681
    - Fixed invalid error when loading railway state #17796
    - Fixed train collision #17821
    - Fixed deadlock when multiple vehicles approach the same siding #17834
    - Fixed deadlock caused by missing foe driveway #17835
    - Fixed deadlock because vehicle is assigned to the wrong subDriveway #17925
    - Rail signals on shared lanes are now working regardless of permissions #17848
    - Cars that share lanes with tram no longer activate rail signals #17862


- sumo-gui
  - Fixed missing text in lane and vehicle dialogs (lane permissions, stop attributes and current driveways) #17700 (regression in 1.21.0)
  - Fixed crash when drawing persons #17616
  - Fixed rare crash when vehicle parameter dialog is open while a vehicle exits the simulation #17637
  - Fixed crash in simulation with rerouting and sorted vehroute-output #17918
  - Fixed invalid lane color when switching from (meso) segment colors to functional color (i.e. TAZ) #17704
  - Shift-click to select lanes now reliably activates edge annotations that are restricted to "only for selected" #17734
  - Fixed misleading rail signal visualization when trams share a multi-lane road with cars #17846
  - Fixed bug where fcd-output differs from non-gui simulation after loading state #17894

- netedit
  - Adding a "smart" next phase in the TLS editor now works even if a program has only a single phase #17680 (regression in 1.6.0)
  - background color for busStop id/name labels is now working #17965 (regression in 1.8.0)
  - Walkingareas no longer intercept clicks #17794 (regression in 1.14.0)
  - Fixed redundant network computation on saving #17881 (regression in 1.16.0)
  - Editing custom lane and junction shapes on top of edges is working again #17806 (regression in 1.20.0)
  - Clicks on invisible edges are ignored again #17762 (regression in 1.20.0)
  - edgedata mode: clicking on intervals works again #17686 (regression in 1.22.0)
  - It is now possible to select edges that belong to a TAZ using selection mode #17701 (regression in 1.22.0)
  - taz mode: clicking edge for membership toggle works again #17697 (regression in 1.23.0)
  - Errors when loading additionals from the command line, are now visible in the message window again #17819 (regression in 1.23.0)
  - Fixed crash when setting option **--railway.signal.permit-unsignalized** to an invalid value #17983 (regression in 1.23.0)
  - python tool dialogs now permit selecting multiple files #17615, #17619 (regression in 1.25.0)
  - Fixed crash when calling python tool and using the 'back' button #17618, #17940 (regression in 1.25.0)
  - Fixed crash after deleting an object from a group of overlapped objects #17795 (regression in 1.25.0)
  - Saving of loaded unmodified additionals to a new file name is working again #17814 (regression in 1.26.0)
  - The network file name shows up in the window title again #17662 (regression in 1.26.0)
  - Fixed failure to load sumocfg in subfolder from command line #17673 (regression in 1.26.0)
  - Fixed invalid default extension when saving plain-xml #17778 (regression in 1.26.0)
  - Fixed invalid save-sumocfg button state after saving the sumocfg #17938 (regression in 1.26.0)
  - Transparent junctions are no longer drawn as white #17751
  - Fixed invalid error when loading stop that ends on lane end #17818
  - Custom Shape color gui setting now takes effect #17828
  - Fixed bug where deleting connections from a multi-lane turn could create an invalid network #17876
  - Setting log output file in sumo options editor is now working #17948
  - Defining multiple files in sumocfg options editor (i.e. additional-files) is now working #17949
  - Fixed problems when setting vehicle attribute departEdge / arrivalEdge #17967
  - Fixed crash when defining trip between junctions with stop #17488
  - Disabling randomTrips options checkbox 'validate' now takes effect #17979
  - vehicle stops are no longer visible in network mode #17899
  - Function *split edge* now takes into account current *front element* #17738
  - Fixed crash when adding and deleting lane types #17315
  - Added missing options for netdiff tool #17531
  - Trips between junctions are now correctly drawn when setting toggle "draw vehicles spread" #17999


- netconvert
  - Fixed superfluous bidi-edges when repairing ptLine #17754 (regression in 1.26.0)
  - NEMA computation now works for 4-arm junction without right-turns (also affects some signal plans of non-NEMA junctions) #17610
  - Zipper junctions no longer feature internal junctions (which could cause deadlock in the simulation) #17650
  - Fixed interpretation of some geo-projection strings #17655
  - Fixed invalid connections in visum import (non-German version) #17660
  - Fixed invalid edge permissions in visum import #17659
  - Fixed invalid changes to loaded edge attributes when patching edge type #17678
  - Fixed invalid permissions in OSM import when bicycles are explicitly forbidden on a pedestrian path #17691
  - Fixed bug where config options were ignored when contrary to options set in the .net.xml #17703
  - Fixed bug where the node type of a `<join>` element was ignored and replaced by "traffic_light" #17732
  - Fixed excessive shifting from **--geometry.avoid-overlap** #17772
  - No longer adding rail signals at network fringe with option **--railway.signal.guess.by-stops** #17779
  - Fixed missing line annotations for added reverse stops #17787
  - OSM: interpretation of railway tag 'highspeed' no longer breaks rail connectivity #17748
  - OSM: fixed invalid spreadType of one-way roads when setting **--default.spreadtype=roadCenter** #17755
  - OSM: fixed bug that was causing invalid rail signals to be generated #17781
  - OSM: fixed invalid distances (kilometrage) #17784
  - OSM: fixed invalid spreadType right in rail network #17823

- meso
  - Fixed bug that prevented taxi simulation from terminating #17908 (regression in 1.13.0)
  - Parking vehicles that cannot enter the road now collect waitingTime #17797
  - Fixed crash when loading state with parked vehicle #17805
  - Fixed crash in rail simulation after teleport #17836

- duarouter
  - Fixed invalid vehicle departure times when defining poisson flow (very noticeable at low rate) #17663
  - Using options **--skip-new-routes --ignore-errors** no longer writes invalid routes #17726
  - Vehicle attributes `departEdge` and `arrivalEdge` are now updated when repairing route #17763
  - Fixed invalid error when using algorithm 'CH' and repairing routes #17927


- TraCI
  - `trafficlight.getSpentDuration` now works correctly after calling `setRedYellowGreenState` #17598
  - Fixed bug where `vehicle.insertStop` shortens looped route #17741
  - Fixed bug where `vehicle.insertStop` ignores nextStopIndex for looped route that stops repeatedly on the same edge #17718

- tools
  - gtfs2pt.py: fixed invalid error when block_id is missing #17750 (regression in 1.26.0)
  - gtfs2pt.py: fixed missing rail edges when mapping routes #17749
  - gtfs2pt.py: fixed invalid output when gtfs input contains double hyphen in stop name #17791
  - gtfs2pt.py: fixed invalid stop mapping on looped route #17953
  - osmWebWizard.py: fixed various platform issues that prevent running. #17503
  - osmWebWizard.py: starting two instances at the same time is now working #16663
  - osmWebWizard.py: fixed bug that was causing OSM data download to fail #17941
  - patchRailConflicts.py: no longer declaring rail signals that do not control any links #17588
  - patchRailConflicts.py: now works for mixed-permission networks (i.e. tram on road) #17682
  - plotXMLAttributes.py: fixed missing labels on barplots with a non-numerical axis #17611
  - countEdgeUsage.py: fixed interpretation of (expected) poisson flow count #17657
  - countEdgeUsage.py: now returns fractional counts for random flows (very noticeable at low rate) #17664
  - netdiff.py: now handles arbitrary network file names, now includes modified edge type in diff. #17676
  - generateRerouters.py: fixed crash when trying to close edges that do not permit vClass #17706
  - generateRerouters.py: fixed invalid placement of notification edges #17707
  - generateRerouters.py: fixed invalid allow / disallow in output when list of vClasses is given #17710
  - remap_additionals.py: fixed crash when object has no position attribute #17711
  - edgeDataDiff.py: fixed crash on empty interval #17715
  - edgeDataDiff.py: fixed invalid output when inputs have different interval times #17716
  - sumollib.net.getShortestPath: fixed bug where no path was found when a route had to loop back to the starting edge #17759
  - generateStationEdges.py: Fixed invalid output when stops have spaces in their name #17932
  - createScreenshotSequence.py can now handle human-readable time #17964
  - net2geojson.py now writes valid polygons (i.e. w.r.t. orientation) #15295


### Enhancements

- sumo
  - Cars no longer enter a bidi-edge if it is occupied or already being approached from a higher-priority road #10941
  - parkingAreas with `onRoad="true"` are now filled from the downstream end if overtaking is not possible (i.e. on rails) #17627
  - Added parkingArea attribute `reservable` ({{DT_BOOL}}) which prevents multiple vehicles from targeting the same spot when [cruising for parking](Simulation/Rerouter.md#rerouting_to_an_alternative_parking_area) #17628
  - carFollowModel *ACC* now supports driverstate device (but it is only active when setting vType attribute `applyDriverState="1"`) #17633
  - Option **--vtk-output** now supports writing data at sub-second simulation step-length #17645
  - edgeData and laneData-output now support attribute `excludeEmpty="modified"` which writes unused edges but only if their speed was modified with calibrators or variableSpeedSigns. #17587
  - fcd-output now supports attribute `stopDelay` #17767
  - fcd-output now supports attributes `speedVec` and `accelerationVec` which write speed and acceleration as 2-dimensional vectors corresponding to the x and y axes #17877
  - fcd-output now supports attribute `speedRelative` (can also be activated by option **--fcd-output.speed-relative**) #17234
  - The symbolic departSpeed values "desired", "speedLimit", "last" and "avg" now automatically adjust to upcoming stops and intersections #17851
  - Stop-output now includes the optional attribute 'state' to distinguish waypoints and skipped on-demand stops #17872
  - Stop-output now includes actType if non-empty #17891
  - Added option **--fcd-output.utm** which writes raw UTM coordinates when simulating in geo-referenced networks #17878
  - When option **--vehroute-output.cost** is set, attribute `savings` now reports the detour cost for a closingReroute #17924
  - Actuated tls with custom conditions now supports expression `p:DETID` to retrieve the public transport delay of vehicles on the detector #17229
  - Bus stops can now be configured to avoid overtaking on the right if a bus is on it #17933
  - Taxis:
    - Intermodal routing now works with taxis of arbitrary vClass. The vClasses that are used for routing depend on all the vehicles with taxi device previously loaded. The default can be set with option **--device.taxi.vclasses** (default *taxi*) #9812
    - Taxis with idle algorithm `taxistand` now advance in queue when parking with `onRoad="true"` and overtaking is not possible #17632
    - Added taxi param `<param key="device.taxi.swapGroup" value="<GROUP_NAME>"/>` which permits idle taxis to receive dispatch tasks before pickup is complete from other taxis in the same swapGroup if that would reduce time to pickup #17639
    - Saving and loading state is now supported for taxi simulation #17874

- meso
  - When the loaded network has internal links they are used instead of jumping across intersections (legacy behavior can be enabled with sumo option **--no-internal-links**) #17842
  - edge-type specific meso parameters now support `edgeLength` #17582
  - now supporting battery device #13843
  - tripinfo-output now tracks waitingCount #17799

- netedit
  - Edge attribute routingType is now supported #17095
  - Element context menu for multiple objects at the same spot now shows selection status (also in sumo-gui) #17731
  - Phase attribute "next" automaticaly updates when adding new phases #16488
  - Added 18 more tools to the menu #17950

- sumo-gui
  - traffic lights now have a higher right-click priority than busstops #17761
  - Made route and locomotive coloring brighter to improve visibility #17859

- netconvert
  - Now importing geo-projection from visum networks #17658
  - visum import now supports option **--type-files** for loading custom interpretation of permissions for TSys codes #17659
  - connection files (*.con.xml) now support attribute `reset="true"` to trigger connection guessing when patching a network #17668
  - Element `<join>` (in *.nod.xml*) now supports attribute `reset` to force recomputation of all connections at the new node. Also added option **--junctions.join-reset** which triggers recomputation of all connections at all joins #17733
  - Added option **--default.junctions.type** to override type-guessing when types are not defined in the input. This option also applies to junctions created in netedit #17736
  - Added option **--railway.topology.ptline-priority** to set railway routingType from ptlines #17558
  - Added option **--railway.signal.guess.by-stops.split** to optionally split edges when setting **--railway.signals.guess.by-stops**. The default behavior of **--railway.signals.guess.by-stops** was changed to allow direct control of switches to ensure smooth operations on single tracks. The new option splits at the stop to give sufficient overlap (Durchrutschweg). #15820
  - OSM: Improved geometry when adding opposite direction bikepath #17699
  - OSM: Improved interpretation of tag `oneway=no` in connection with rail (especially tram) #17690
  - OSM networks now assign routingType *narrow* to one-lane edges to permit post-processing and [routing](Simulation/Routing.md#routing_by_travel_time_and_routingtype) or capacity adaptations. #17661
  - OSM: the railway routingType is now set based on tag `railway:preferred_direction` #17774
  - OSM: tag `placement` is now supported for better geometry of one-way roads #17728
  - OSM: adding more bidi edges based on signal direction #17782
  - OSM: deriving more bidi edges from maxspeed:backward #17785

- duarouter
  - Option **--mapmatch.junctions** now automatically sets option **--junction.taz** #17688


- TraCI
  - Added function `vehicle.getReferenceDistance` to retrieve the position of the vehicle in the underlying [linear reference system](Simulation/Distances.md#defining_and_using_linear_coordinates) #10572

- tools
  - osmWebWizard.py: now automatically saves osmGet configuration for easier updating of a scenario #17570
  - osmWebWizard.py: now permits selection of public transport modes to import #8628
  - osmWebWizard.py: now permits setting a 'verbose' checkbox which gets forwarded to all applications #17573
  - osmWebWizard.py: adds menu for changing the tileset (i.e. OpenTopoMap and öpnvkarte / public transport) #17746
  - osmGet.py: now attempts to download again after timeout and tries to use proxies (also affects osmWebWizard) #17597
  - plotXMLAttributes.py: added option **--join** to configure the separator when joining values or labels #17612
  - generateStationEdges.py: added option **--join-stations** to generate a single access edge for all stops with the same name #17625
  - generateStationEdges.py: added option **--build** to automatically create the extended network #13383
  - visum_convertXMLRoutes.py: added option **--trips** to write unvalidated trips #17666
  - visum_convertXMLRoutes.py: now uses demand time range when interpreting volume #17667
  - countEdgeUsage.py: Now writes option header and supports .gz output #17685
  - generateRerouters.py: Added option **--terminate-unreachable** to configure behavior for unreachable destinations #17708
  - routeSampler.py: Added option **--keep-attributes** to preserve vehicle attributes loaded from route input including departure time (unless the route of that vehicle is used more than once) #16114
  - routeSampler.py: Added option **--timeline** to interpret loaded data intervals that are longer than the **--interval** time #17863
  - routeSampler.py: Now supports **--mismatch-output** for edgeRelations with `via` #17884
  - routeSampler: Added option **--merge-strategy** to configure handling of overlapping data sets #17871
  - patchRailConflicts.py: Added option **--split-offset** which splits rail edges before traffic_light junctions if these junctions also have a rail conflict #17740
  - [instantOutDiff.py](Tools/Output.md#instantoutdiffpy): added tool for comparing instantInductionLoop output from two simulation runs #16524
  - [tls_analyzeSplit.py](Tools/tls.md#tls_analyzesplitpy): added tool to analyze green split of static signal plans (or plan-like logs of dynamic plans) #17730
  - sumolib.net.getOptimalPath now uses caching by default (configurable with `readNet` attribute `maxcache`) and runs much faster in one-to-many routings (i.e. gtfs2pty.py). #17753
  - gtfs2pt.py: Added option **--remove-detour-factor** to filter out trips with implausible routes #17757
  - gtfs2pt.py: Added option **--rail-priority-factor** to consider routingType when mapmatching railways #17560
  - gtfs2pt.py: Added option **--poi-output** to visualize loaded stop coordinates as pois #17812
  - gtfs2pt.py: Added option **--distance-penalty** which makes the (exponent) of the penalty for matching accuracy configurable. The penalty is now also active when using option **--stops** #17813
  - gtfs2pt.py: Added option **--join-blocks** to import circulations (block_id) #17541
  - gtfs2pt.py: Added option **--parking-threshold** to remove vehicles from roads/tracks if they have consecutive stops in the same location with a long in-between time #17960
  - [patchRailPriorities.py](Tools/Railways.md#patchrailprioritiespy): added tool to patch edge routingType in rail networks (and optionally add stops and signals) to ensure smooth operations on single track lines with passing loops. This tools works well as a preparatory stage before **gtfs2pt.py** #17766
  - randomTrips.py: Added option **--persontrip.modes** to simplify persontrip definition #16472
  - randomTrips.py: Added option **--threads** to speed up validation. Uses half the number of available cpus by default #17980
  - addStops2Routes.py: Added option **--length** to define stops that permit a larger valid area for being reached. #18001
  - addStops2Routes.py: Added options **--edges** to restrict the set of edges used for stopping and option **--color** to change the color of stopping vehicles. #17856
  - tileGet.py now supports reprojection to UTM and stitching of the downloaded tiles #14641 #17789

### Miscellaneous

- The default values for options **--default.departspeed** was changed from "0" to "avg". Also, the default of option **--default.departlane** was changed from "first" to "best_prob". This generally leads to higher insertion flow and using all available lanes. #16888
- Simulation of railway-taxis with vClass "taxi" no longer triggers rail signals (this is a consequence of #17862). Instead railway-taxis can now be defined as vClass "rail" and taxi behavior will work as expected (#9812). Alternatively, option **--railsignal.default-classes taxi** can be set to enable rail signal switching for vClass "taxi".
- no more HTML tables in the docs
- the build configuration now uses consistently options like ENABLE_FOX to enable or disable optional features #17677
- Option **--netstate-dump** (also known as raw dump) is now deprecated. fcd-output has been upgraded to permit all attributes formerly only available in netstate dump. #16882
- The OpenStreetMap Attribution of OSMWebWizard is no longer hidden behind the sidebar #17743
- Added link to [A Free Educational Course Based on SUMO](Tutorials/index.md#external_courses_and_tutorials) #17742
- The python modules [sumolib](https://sumo.dlr.de/pydoc/sumolib.html) and [traci](https://sumo.dlr.de/pydoc/traci.html) have now pdoc documentation.
- The python module SUMOPy was superseded by hybridPY which extends the functionality and supports python 3 #17365
- The maximum Parquet/Arrow version to use is 23.0, newer versions require a newer compiler which needs to be enabled explicitly at build time


## Version 1.26.0 (29.01.2026)

### Bugfixes

- sumo
  - Fixed deadlock in roundabout #17330 (regression in 1.24.0)
  - Fixed inconsistent computation of attribute flow in edgeData output #17349
  - Fixed invalid braking when trying to avoid junction blockage #17318
  - Fixed inconsistent lane-changing inside roundabout while on an internal lane #17336
  - Fixed emergency braking in roundabout #17306
  - Fixed invalid warning after train rerouting #17343
  - Fixed problems when configuring calibrator with period > 1 (period now truncated to 1) #6589, #17371
  - Fixed invalid error about stops when configuring vehicles to ignore temporary permission changes #17380
  - Fixed unsafe sublane-change due to missed neighbor vehicles #17388
  - Fixed invalid expected sublane speeds at lane split with forbidden connections #17390
  - Fixed failure to return from outside lane bounds after lane width change in the sublane model #17383
  - Fixed collisions involving pedestrian crossings #17385
  - Fixed invalid braking after lane change #17408
  - Fixed bug where vehicles block each others lane change #10763
  - Fixed right-of-way violation when entering roundabout #17409
  - Fixed invalid braking at intersection with sublane model #17410
  - Fixed unsafe lane-changing ahead of roundabout #17411
  - Fixed broken vehicle flows where total vehicle count overflows a 32-bit integer #17396
  - A closingReroute with configured permissions no longer affects vehicle with an unaffected vehicle class #17426
  - Fixed missing railsignal vehicle-events on sub-driveways #17442
  - Fixed unsafe train insertion when driveways start and end in the middle of the block #17453
  - Fixed missing driveway foes when a sequence of bidi-edges is interrupted by a unidirectional edge #17367
  - Fixed bug where pedestrian walks into vehicle #17462
  - Fixed unsafe lookahead time when computing crossing conflicts between vehicles and pedestrians (now attribute `jmTimegapMinor` takes effect) #17463
  - vehicles approaching a non-priority crossing no longer yield unless a person steps onto the crossing #17465
  - Fixed infinite traffic from poisson flow at specific random seeds #17468
  - Fixed inconsistency where a route with a single edge and departPos > arrivalPos causes no error on loading but rerouting (now results in a warning) #10246
  - Fixed invalid error when combining option **--device.rerouting.mode** with taz-routing #17490
  - Fixed undefined behavior when computing route cost between taz #17489
  - Fixed missing events when vehicle is inserted on `<instantInductionLoop>` #17510
  - Fixed crash when running rail signal simulation with **--tls.all-off** #17516
  - Fixed bug where vehicle stops despite stop attribute `onDemand` when there is no demand #17523
  - Fixed invalid default stop startPos if endPos is defined as negative #17522
  - Fixed unsafe train insertion when combining rail signals with traffic lights #17546, #17547
  - Fixed invalid vehicle mass for personTrip with car #17551
  - Fixed non-deterministic moving-block simulation #17557
  - Fixed invalid signal state when using WAUT and loading state #17526
  - Fixed unsave signal state if a junction where rail lines cross is defined as rail_signal #17580

- netedit
  - lane selection count now updates when selecting with shift-click #17394 (regression in 1.11.0)
  - In move mode, grabbed and merged geometry points are indicated again (with contour instead of color) #17032 (regression in 1.20.0)
  - Fixed crash in calibrator dialog #17398 (regression in 1.25.0)
  - Merging junctions in move mode is working again #17358 (regression in 1.25.0)
  - Creating calibrator flows is working again #17399 (regression in 1.25.0)
  - No longer saving an invalid network when edges from prohibition elements are deleted #17331
  - selection scaling now work for tazRelations #17382
  - Remove some unsupported combinations of taz/junctions for rides, transport, and tranships #17414
  - Fixed bug where paths in the configuration were absolute when they should have been relative #17446
  - Fixed problem when locking lanes #17514
  - Trying to load waypoints with triggers now issues a warning #17534
  - Fixed visualization of parkingArea with onRoad=true or lefthand=true #17499, #17538

- sumo-gui
  - saving selection to file no longer uses **--output-prefix** #17368
  - fixed crash when tracking a vehicle which already left #17472
  - Fixed invalid positioning of parking spaces #17478
  - Fixed crash when person performs a jump #17506
  - Fixed invalid color when vehicle passes a waypoint and coloring *by speed* is active #17524

- netconvert
  - Fixed crash when removing traffic light crossing via xml input #17515
  - Negative split pos is now relative to custom edge length #17527
  - Fixed invalid handling custom edge length when splitting at an existing node #17528
  - Split at position 0 now sets node type #17533
  - Elements written with **--ptstop-output** are now assigned as trainStop where appropriate #17535
  - Elements written with **--ptstop-output** now preserve their 'lines' attribute #17530
  - Fixed invalid busStop direction in ptline-output #17537
  - Fixed invalid ptlines with option --ptstop-output.no-bidi #17571


- duarouter
  - Fixed crash when loading invalid routes with option **--skip-new-routes** and **--ignore-errors** #17348 (regression in 1.25.0)
  - Option **--ignore-errors** now works when origin or destination are prohibited by option **--restriction-params** #17387
  - Any routes that are repaired with option **--repair** no longer trigger an error (and thus do not require option **--ignore-errors** anymore) #17369
  - Fixed invalid route when two stops on the same edge require looping back #17484
  - Fixed invalid route when departPos > arrivalPos and from=to #17482
  - stop arrival times for flows are now shifted #17504

- TraCI / Libsumo
  - function traci.vehicle.rerouteParkingArea now finds looped route from the current edge #17353
  - libsumo macOS wheels work again #15945

- tools
  - osmGet.py: fixed missing road nodes when using option **--shapes** #17293 (regression in 1.20.0)
  - netdiff.py: fixed crash involving removed `<neigh>` attribute #17345
  - generateRerouters.py: Fixed invalid output when no detours are possible #17361
  - generateRerouters.py: now find notification edges for consecutive closed edges #17360
  - tazRel2POI.py: fixed invalid error on skipped taz #17379
  - countEdgeUsage.py: Fixed invalid count for vehroute-output involving replaced routes #17401
  - sumolib.xml.parse_fast: No longer yields records for element names that start with the same string as the requested element #17403
  - gtfs2pt.py: Fixed bug that was causing invalid stop assignments and large detours #17540
  - driveways2poly.py: fixed crash on rail signals without links #17550
  - remap_additionals.py: fixed bug where wrong edge was picked among parallel edges #17556

- Options **--output-prefix** and **--output-suffix** can now be freely combined #17545

### Enhancements

- sumo
  - Routing with randomized weights (**--weights.random-factor**) is now stable with respect to network changes, vehicle composition and state loading. The random noise for each edge only depends on the random seed and vehicle id (also applies to duarouter). If thew new option **--weights.random-factor.dynamic** is set, the randomness in the simulation also varies over time. #17325
  - The new option **--output-suffix** can be used to modify the names of all output files (similar to **--output-prefix**). The suffix will be inserted right before the file name extension. #17338
  - edgeData output definitions now support attribute `aggregate="taz"` which will aggregated data within each loaded taz definition #11104
  - Added option shortcut **-m** for **--edgedata-files** #17400
  - A warning is now given when loading personTrips with mode "public" and no public transport was loaded #2825
  - Departure on closed edge with option to ignore transient permissions (**--device.rerouting.mode 8**) now delays departure instead of raising an error #17461
  - ChargingStation attribute `totalPower` can now be used to limit the total power when charging multiple vehicles at the same time. #17173
  - Timeloss is now discounted when braking/accelerating for planned stops. Thus, a punctual public transport vehicle does not have any timeLoss #5287
  - Tram simulation
    - Tram simulation now defaults to moving-block mode. This can be configured with the new option **--railsignal.moving-block-default-classes** #17542
    - Train insertion in moving-block mode ignores zipper conflicts to improve operations where when rail signals are sparse #17544
    - Rail signals in moving block mode ignore zipper conflicts if they are beyond 200m (configurable with new option **--railsignal.moving-block.max-dist**) #17542

- netedit
  - Automatically sets sumo option **--junction-taz** if at least one vehicle is configured to start/end at a junction #17405
  - The written sumocfg now tracks network file name changes #17314
  - Configurations with unsupported options can now be loaded with warnings instead of errors #17445
  - Added support for junction attribute 'roundabout' #17271

- duarouter
  - The speedFactor configured in a vehicle, trip or flow is now taken into account when computing costs #17424
  - Added option **--max-traveltime** which lets routing fail if traveling takes too long #17422
  - Rerouters with element `closingReroute` can now be loaded from an **--additional-file** to influence routing #12501
  - Rerouters with element `closingLaneReroute` can now be loaded from an **--additional-file** to influence routing #17428
  - consistency of stops and vias is now checked #17485

- netconvert
  - now keeps more stops from OSM despite minor data errors #17575

- TraCI
  - function traci.simulation.findRoute now supports optional attributes departPos, arrivalPos #17352
  - `traci.vehicle.setSpeedMode` bit 2 (ignoring deceleration constraints) now also applies when validating deceleration for `traci.vehicle.setStop` #17477
  - traci and libsumo python libraries now provide the standard __version__ attribute #17366

- tools
  - attributeCompare.py: Now supports special id-attribute \@FILE #17334
  - [visum_convertXMLRoutes.py](Tools/Import/VISUM.md#visum_convertxmlroutespy): new tool for importing VISUM routes file #17347
  - generateRerouters.py: Added option **--closed-edges.input-file** for loading edges to close from a selection file #17359
  - generateRerouters.py: major speed-up #17362
  - tazRel2POI.py: Added option **--reference-taz** to allow filtering relations to those that interact with a specific taz #17375
  - tazRel2POI.py: supports multiple taz input files #17378
  - route2OD.py: supports separating multiple input files with ',' #17377
  - edgesInDistricts.py: now supports geo polygons #17376
  - edgeDataDiff.py: now include mean_abs in output #17404
  - generateDetectors.py: supports generating nextEdges attribute #17509
  - addStops2Routes.py: allows placing stops on vias #14818
  - filterElements.py: Added option **--remove-parent** which filters parent element based on child attributes #17539
  - gtfs2pt.py: now warns about large detours (i.e. implausible  routes) #17567
  - patchRailConflicts.py: new tool for adding tram rail signals

### Miscellaneous

- Fixed invalid meta data in python wheels
- Started Korean Language translation #17420
- add manylinux_2_28 support #16771
- dlr-navteq output no longer defaults to option **--numerical-ids** #17520
- It is no longer possible to end a rail_signal block with a traffic light (this was found to be unsafe in the context of single-track operations). Both types of signaling should only ever be combined in tram simulation which defaults to moving block so no adverse effects are expected #17542
- the default download location for the nightly wheels changed to https://sumo.dlr.de/daily/ciwheels


## Older Versions

- [Changes to versions 1.22.0, 1.23.0, 1.23.1, 1.24.0 and 1.25.0 (2025 releases)](ChangeLog/Changes_in_2025_releases.md)
- [Changes to versions 1.20.0 and 1.21.0 (2024 releases)](ChangeLog/Changes_in_2024_releases.md)
- [Changes to versions 1.16.0, 1.17.0, 1.18.0 and 1.19.0 (2023 releases)](ChangeLog/Changes_in_2023_releases.md)
- [Changes to versions 1.12.0, 1.13.0, 1.14.0, 1.14.1 and 1.15.0 (2022 releases)](ChangeLog/Changes_in_2022_releases.md)
- [Changes to versions 1.9.0, 1.9.1, 1.9.2, 1.10.0 and 1.11.0 (2021 releases)](ChangeLog/Changes_in_2021_releases.md)
- [Changes to versions 1.5.0, 1.6.0, 1.7.0 and 1.8.0 (2020 releases)](ChangeLog/Changes_in_2020_releases.md)
- [Changes to versions 1.2.0, 1.3.0, 1.3.1 and 1.4.0 (2019 releases)](ChangeLog/Changes_in_2019_releases.md)
- [Changes to versions 1.0.0, 1.0.1 and 1.1.0 (2018 releases)](ChangeLog/Changes_in_2018_releases.md)
- [Changes to versions 0.29.0, 0.30.0, 0.31.0 and 0.32.0 (2017 releases)](ChangeLog/Changes_in_2017_releases.md)
- [Changes to versions 0.26.0, 0.27.0, 0.27.1 and 0.28.0 (2016 releases)](ChangeLog/Changes_in_2016_releases.md)
- [Changes to versions 0.23.0, 0.24.0 and 0.25.0 (2015 releases)](ChangeLog/Changes_in_2015_releases.md)
- [Changes to versions 0.20.0, 0.21.0 and 0.22.0 (2014 releases)](ChangeLog/Changes_in_2014_releases.md)
- [Changes to versions 0.17.0, 0.17.1, 0.18.0 and 0.19.0 (2013 releases)](ChangeLog/Changes_in_2013_releases.md)
- [Changes to versions 0.14.0, 0.15.0 and 0.16.0 (2012 releases)](ChangeLog/Changes_in_2012_releases.md)
- [Changes to versions 0.12.3, 0.13.0 and 0.13.1 (2011 releases)](ChangeLog/Changes_in_2011_releases.md)
- [Changes to versions 0.12.0, 0.12.1 and 0.12.2 (2010 releases)](ChangeLog/Changes_in_2010_releases.md)
- [Changes to versions 0.10.0, 0.10.1, 0.10.2, 0.10.3, 0.11.0 and 0.11.1 (2009 releases)](ChangeLog/Changes_in_2009_releases.md)
- [Changes in versions 0.9.0 up to 0.9.10](ChangeLog/Changes_from_version_0.9.0_up_to_0.9.10.md)
- [Changes in versions pre0.8 up to 0.8.3](ChangeLog/Changes_from_version_pre0.8_up_to_0.8.3.md)
