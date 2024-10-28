---
title: Changes in the 2009 releases (versions 0.10.0, 0.10.1, 0.10.2, 0.10.3, 0.11.0 and 0.11.1)
---

## Version 0.11.1 (23.11.2009)

- All
  - configuration xml format changed again to `<section><key value="val"/></section>` (use {{SUMO}}/tools/10to11.py to convert from old to new
    representation)
  - Bugs in Dijkstra implementation fixed (affecting at least
    duarouter, routing with traci and automatic rerouting)
    (01.10.2009)

- Simulation
  - debugged problems when loading TLS definitions with T=0
  - implemented a variable car-following model API; many thanks to Tobias Mayer
    and Christoph Sommer for the collaboration who did most of the work.
  - debugged occurrence of negative vehicle speeds during emit
  - reworked the mean data output (handling multiple lanes
    correctly)

- sumo-gui
  - implemented [request 1641989: No view reset on reload](https://sourceforge.net/tracker/?func=detail&aid=1641989&group_id=45607&atid=443424).
  - the number of colors used for range visualization is now variable
  - debugged problems with not shown junction names

- netconvert
  - making projection mandatory for OSM and DLR-Navteq networks
    (03.09.2009)
  - removed option **--rotation-to-apply** - was not properly working anyway (02.09.2009)
  - removed support for "old" TIGER networks - assuming the current
    ones are given as shape files; **--tiger** {{DT_FILE}} is now mapped onto **--shapefile** {{DT_FILE}} (02.09.2009)
  - removed support for "split" Elmar networks (option **--elmar** {{DT_FILE}}) (02.09.2009)
  - renamed **--elmar2** {{DT_FILE}} to **--dlr-navteq** {{DT_FILE}} (02.09.2009)
  - renamed **--arcview** {{DT_FILE}} to **--shapefile** {{DT_FILE}} (02.09.2009)
    - please note, that all subsequent options have been adapted
      (but the old names still work as aliases)
  - added the possibility to define [changes of lane numbers along an edge](../Networks/PlainXML.md#road_segment_refining)
    in XML-descriptions (03.09.2009)
  - replaced **--use-projection** by **--proj.utm** and **--proj.dhdn** which determine the correct parameters for
    the two widely used projections from the input data
  - for developers: using same loading procedure for all imported
    networks, see [Developer/How To/Net Importer](../Developer/How_To/Net_Importer.md)

- TraCI
  - debugging sending/receiving messages with length\>255 bytes
  - added further [traffic lights variable retrieval options](../TraCI/Traffic_Lights_Value_Retrieval.md)
  - added further [induction loops variable retrieval options](../TraCI/Induction_Loop_Value_Retrieval.md)
  - added further [multi-entry/multi-exit detectors variable retrieval options](../TraCI/Multi-Entry-Exit_Detectors_Value_Retrieval.md)
  - added further [junction variable retrieval options](../TraCI/Junction_Value_Retrieval.md)
  - added further [vehicle variable retrieval options](../TraCI/Vehicle_Value_Retrieval.md)
  - added further possibilities to [change vehicle values](../TraCI/Change_Vehicle_State.md)
  - added possibilities to [retrieve edge variables](../TraCI/Edge_Value_Retrieval.md) and [change edge values](../TraCI/Change_Edge_State.md)
  - added possibilities to [retrieve lane variables](../TraCI/Lane_Value_Retrieval.md) and [change lane values](../TraCI/Change_Lane_State.md)
  - added possibilities to [change PoI values](../TraCI/Change_PoI_State.md) and to [change polygon values](../TraCI/Change_Polygon_State.md)
  - added the possibility assign a vehicle a new route via TraCI

- dfrouter
  - debugged problems with induction loop measure with time\>end
    time
  - debugged problems with spaces in induction loop measures

- od2trips
  - debugged problems with O/D matrices that have no comments, was:
    [defect 148: od2trips breaks on matrices without comments](https://github.com/eclipse-sumo/sumo/issues/148);
    thanks to Wilson Wong for pointing us to it


## Version 0.11.0 (29.07.2009)

- All
  - moved to xerces 3.0
  - moved to FOX1.6
  - changes in network format (use {{SUMO}}/tools/net/0103to0110.py to
    convert from old to new representation)
    - traffic light descriptions (18.05.2009)
    - descriptions of giving lanes free / prohibiting lanes for
      certain vehicle classes (10.06.2009)
    - moved character sections to attributes (22.-24.07.2009), see
      [SUMO Road Networks](../Networks/SUMO_Road_Networks.md)
  - configuration xml format changed from `<section><key>value</key></section>` to `<section key="value"/>` (use {{SUMO}}/tools/10to11.py to convert from old to new
    representation)
  - The data-folder was removed; instead the examples got a part of
    the tests and are generated using these.

- Simulation
  - Collisions
    - Collisions are now checked per default (removed option **--check-accidents**;
      added option **--ignore-accidents** with "reverse meaning" instead)
    - Removed option **--quit-on-accident**
    - Debugged buggy dealing with false vehicle leaving order
      (what should in fact not happen anyway)
  - Cleaning
    - Removed unused/undescribed lane-change output (**--lanechange-output**)
    - Removed unused/undescribed lane-change options **--lanechange.min-sight** and **--lanechange.min-sight-edges**
  - Outputs
    - patched problems with the intervals of detector outputs
      (including [defect 73: false end step information in detectors output](https://github.com/eclipse-sumo/sumo/issues/73))
  - removed TrafficOnline classes; new approaches will be
    described/discussed at
    TrafficOnline; was: [enhancement 57: remove TrafficOnline classes from simulation core](https://github.com/eclipse-sumo/sumo/issues/57)
    - [edgelane traffic](../Simulation/Output/Lane-_or_Edge-based_Traffic_Measures.md),
      [edgelane hbefa](../Simulation/Output/Lane-_or_Edge-based_Emissions_Measures.md),
      and [edgelane harmonoise]../(Simulation/Output/Lane-_or_Edge-based_Noise_Measures.md)
      are no longer writing empty intervals (13.05.2009)
    - corrected behavior of using intervals in [edgelane traffic](../Simulation/Output/Lane-_or_Edge-based_Traffic_Measures.md),
      [edgelane hbefa](../Simulation/Output/Lane-_or_Edge-based_Emissions_Measures.md),
      and [edgelane harmonoise](../Simulation/Output/Lane-_or_Edge-based_Noise_Measures.md)
      (solved [bug 2504162: multiple begins / ends should be defined in separate dumps](https://sourceforge.net/tracker/?func=detail&aid=2168007&group_id=45607&atid=443424)
      and [bug 2504144: dump intervals are mandatory](https://sourceforge.net/tracker/?func=detail&aid=2168007&group_id=45607&atid=443424))
    - [edgelane hbefa](../Simulation/Output/Lane-_or_Edge-based_Emissions_Measures.md)
      now additionally writes per-vehicle values
  - fixed re-emission check for edges with multiple waiting vehicles

- sumo-gui
  - Added the possibility to load view settings and
    [decals](../sumo-gui.md#showing_background_images) from
    a configuration file (implemented [request 2168007: add possibility to load view settings file with start of gui](https://sourceforge.net/tracker/?func=detail&aid=2168007&group_id=45607&atid=443424)).
    Added the possibility to generate snapshots on startup, defined
    in a configuration file.
  - Removed the "additional weight" dialog - was not known to be
    used

- NETGEN
  - random networks may now contain bidirectional edges; the
    probability to have an edge being bidirectional is controlled by
    the **--rand-bidi-probability** {{DT_FLOAT}} with default=1 (all edges are bidirectional)
  - The default junction type (option **--default-junction-type**) may now be only one of
    "priority", "right_before_left", or "traffic_light". Building
    agent-based and actuated traffic lights from within NETGEN will
    no longer be supported.

- netconvert
  - Nodes which have same or almost same positions are no longer
    joined per default.
  - Type descriptions allow a certain edge type to be discarded from
    being imported using the `discard="x"` attribute.
  - netconvert can guess roundabouts; force using **--guess-roundabouts**
  - corrected usage of the `spread_type` attribute in edges; was: [defect 111 correct usage of the spread_type attribute](https://github.com/eclipse-sumo/sumo/issues/111)
  - patching problems with RoboCup Rescue League import

- od2trips
  - The default departure/arrival behavior can be controlled on the
    command line, now. The following options were introduced for
    this purpose: **--departlane**, **--departpos**, **--departspeed**, **--arrivallane**, **--arrivalpos**, **--arrivalspeed**. See also
    [Specification](../Specification/index.md)
  - solved [defect 67: false interpretation of the second time field (od2trips)](https://github.com/eclipse-sumo/sumo/issues/67)

- duarouter
  - The default departure/arrival behavior can be controlled on the
    command line, now. The following options were introduced for
    this purpose: **--departlane**, **--departpos**, **--departspeed**, **--arrivallane**, **--arrivalpos**, **--arrivalspeed**. See also
    [Specification](../Specification/index.md)

- jtrrouter
  - The default departure/arrival behavior can be controlled on the
    command line, now. The following options were introduced for
    this purpose: **--departlane**, **--departpos**, **--departspeed**, **--arrivallane**, **--arrivalpos**, **--arrivalspeed**. See also
    [Specification](../Specification/index.md)

- TraCI
  - changed name of "areal detectors" to "multi-entry/multi-exit
    detectors"
  - fixed crashes occurring when using the distance command
    ([defect 70: TraCI does not recognize edges with'\['...](https://github.com/eclipse-sumo/sumo/issues/70))
  - added [APIs for getting information about vehicles, vehicle types, lanes, routes, polygons, PoIs, and junctions](../TraCI.md).
  - added the possibility to change the phase and retrieve the
    current phase and complete definition of a tls using the new
    phase definition


## Version 0.10.3 (30.03.2009)

- All
  - A daily build for windows is available at:
    <https://sumo.dlr.de/docs/Downloads.php#nightly_snapshots>
  - Made tests tolerant for floating point imprecision
  - First Unit Tests added by Matthias Heppner (thanks\!\!)
  - Xerces 3 may now be used optionally

- Simulation
  - further work on simulation of inner-junction traffic in dense
    scenarios

- netconvert
  - prunning on a bounding box was revalidated; tests were added

- dfrouter
  - made reading of detector positions more fault-tolerant

- TraCI
  - tests revalidated
  - Tutorial on interaction with traffic lights added by Lena
    Kalleske (thanks\!\!)
  - added the possibility to retrieve a complete tls definition via
    TraCI


## Version 0.10.2 (16.03.2009)

- All
  - If configuration is written using **--write-config**, the
    application stops after this step
  - The devices now also allow "**NUL**" or "**/dev/NULL**" as
    output file name. In this case, the output is redirected to
    **NUL** (MS Windows) or **/dev/null** (Linux)

- sumo-gui
  - solved bug 2519476: vss are not shown properly
  - solved bug 2510002: Crash on vss manipulator
  - debugged size info ("legend") drawing
  - solved bug 2519761: guisim crashes on "show all routes"
  - changed how decals are aligned (see
    [sumo-gui\#showing_background_images](../sumo-gui.md#showing_background_images))
  - Consolidated the startup-options **--suppress-end-info** and
    **--quit-on-end**: **--suppress-end-info** was removed, the
    application quits after performing the simulation when
    **--quit-on-end** is given. This is also done even if errors
    occur
  - added the possibility to copy the cursor position to clipboard
  - debugged false position information within the popup-menu of a
    lane
  - Tests based on SUMO tests are now also performed for sumo-gui.
    Thanks to Michael Behrisch for this very nice work

- Simulation
  - solved bug 2524031: sumo xml parser relies on sequence order
    instead IDs;
    thanks to Andrey Gursky for supplying the example
  - solved bug 2519762: rerouter are not shown
    properly
  - solved bug 2105526: Subsecond simulation does not
    compile
  - changed default simulation end time to INT_MAX
  - simulation stops on running empty only if there is no end time
    specified
  - closed work on pollutant emission modelling based on HBEFA

- netconvert / Netgen
  - trying to deal with problems on network building; networks are
    more correct for most cases, but may look weird sometimes
  - solved [defect 38: speed instead of length while importing XML](https://github.com/eclipse-sumo/sumo/issues/38)
  - changed the tls-guessing procedure (unverified, yet); tls which
    span over multiple junctions can now been "guessed" if both,
    nodes are marked as being controlled by traffic lights or not
    within the imported data; see also: [task 42: (TAPAS) add possibility to generate joined tls if tls are given](https://github.com/eclipse-sumo/sumo/issues/42)

- polyconvert
  - solved [defect 14: polyconvert does not report about double ids](https://github.com/eclipse-sumo/sumo/issues/14)
  - renamed option **--xml-points** to **--xml**
  - renamed option **--shape-files** to **--shape-file**

- Tools
  - Rebuilt traceExporter; added tests

- Documentation
  - solved bug 1993983: Errors in User Documentation

- TAPAS
  - see [TAPASCologne ChangeLog](../Data/Scenarios/TAPASCologne.md#changelog)


## Version 0.10.1 (11.01.2009)

- sumo-gui
  - debugged right-click on an empty cell

- Simulation
  - removed bug in collision detection
  - added warning about old route format


## Version 0.10.0 (09.01.2009)

- Build
  - removed obsolete configuration option **--enable-speedcheck**

- All
  - using a reduced, faster XML parser (not validating)
  - default vehicle length is set to 7.5m (including gap)
  - route format changed ("edges" attribute)
  - (junction) internal lanes are the default now
  - copyright changed (removed obsolete ZAIK reference)

- sumo-gui
  - solved bug 2163422: Simulation does not start automatically
  - refactored the visualization speed-up; instead of using a
    self-made hack that uses a grid, an rtree structure is now used.
    This should solve problems with disappearing edges and nodes and
    with a slow selection
  - Added the possibility to save and load decals (int an XML-file)
  - View settings are now saved loaded using XML
  - implemented feature 1906106: make lines of bus stops always visible
  - implemented feature 2103556: Remove "Add Successors To Selected"

- Simulation
  - Refactored mean data; instead of having it as an additional item
    that has to be touched by vehicles, it is a MSMoveReminder
  - changed the definition of triggers; the old definition is still
    possible, but prints a warning.
    - <trigger objecttype="lane" attr="speed" objectid="<LANE\>\[;<LANE\>\]\*" ... --\> <variableSpeedSign lanes="<LANE\>\[;<LANE\>\]\*" ...
    - <trigger objecttype="rerouter" objectid="<EDGE\>\[;<EDGE\>\]\*" ... --\> <rerouter edges="<EDGE\>\[;<EDGE\>\]\*" ...
    - <trigger objecttype="emitter" objectid="<LANE\>" ... --\> <emitter lane="<LANE\>" ...
    - <trigger objecttype="bus_stop" objectid="<LANE\>" ... --\> <busStop lane="<LANE\>" ...
  - new "route probe" output added
  - solved bug 2350888: vehicle on false lane with cyclic routes
    (thanks to Daniel Janusz for supplying the example)
  - solved bug 2082959: bus stops that end at edge end confuse vehicles
    (thanks to Gerrit Lammert for supplying the example)
  - solved bug 2468327: log files together with input errors let sumo crash
  - solved bug 2227272: Random routes do not work with simple net
  - distributions for routes and vehicle types
  - changed format of dump definitions from command line to xml
    configuration

- netconvert
  - solved bug 2137657: edges disappear in combination with --keep-edges
  - changed APIs for setting lane-2-lane connections
  - work on Vissim-import
  - Now, the node positions are not added to an edge's geometry if
    one exists (if the edge has no explicit geometry, the node
    positions are still used). **--add-node-positions** was added,
    so that the old behavior is still available
  - Changed features of **--plain-outoput**: plain-nodes now contain
    tls information, additionally, plain-connections are written
  - changed computation whether a link is a left-mover
  - changed traffic lights computation
  - removed unneeded option **--all-logics**
  - removed "FileErrorReporter" usage
  - too complicated junctions (\#links\>64) are catched and set to
    unregulated
  - corrected computation of right-of-way rules
  - solved bug 2392943: netconvert fails when removing edges and guessing ramps
  - solved bug 2171355: turnarounds even with --no-turnarounds
  - building turnarounds may be now skipped for tls-controlled edges
    (using **--no-tls-turnarounds**)

- NETGEN
  - removed unneeded option **--all-logics**

- Router
  - solved bug 2149069: dfrouter does not work with internal lanes

- dfrouter
  - removed support for elmar's detector descriptions
  - removed **--fast-flows** option
  - solved bug 2165708: False speed conversion in dfrouter?
  - named classes properly
  - applied changes to trigger definitions (see "Simulation")
  - solved bug 2165666: dfrouter does not regard ''--all-end-follower"

- polyconvert
  - refactoring
  - in-line documentation added
  - added OSM-import functionality
  - added shape-file - import functionality (imports points, line
    strings, polygons, and their multi-pendants)
    - import is done via **--shape-files**
    - the id-field is given via **--shape-file.id-name *<NAME\>* **
    - optional projection guess is done via
      **--arcview.guess-projection**
  - renamed **--visum-file** to **--visum-files**; debugged
