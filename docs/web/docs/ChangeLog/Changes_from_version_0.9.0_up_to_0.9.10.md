---
title: Changes in versions 0.9.0 up to 0.9.10
---

## Version 0.9.10 (16.09.2008)

- Building
  - debugged [bug 2042630 --enable-traci disables traci?](https://sourceforge.net/tracker/index.php?func=detail&aid=2050900&group_id=45607&atid=443421)

- All
  - solved [feature request 1725623 comma separated option lists](https://sourceforge.net/tracker/index.php?func=detail&aid=1725623&group_id=45607&atid=443424)
  - Network format has changed\! The elements 'cedge' (net/edge/cedge) and 'edges' (net/edges) do not longer exist. You do NOT have to rebuild your networks.

- Simulation
  - solved <b>"Simulation reports 'Error: The number of edges in the
      list mismatches the edge count.'"</b>
  - solved several issues reported by Gerrit Lammert (thank you):
      false ordering of loaded stops, problems with stops near to an
      end edge

- Guisim
  - vehicles now again have a default color

- TraCI
  - patched building tracitestclient_main.cpp under Fedora Core 9
      (thanks to Niek Sanders)

- Tools
  - patched dua-iterate.py - **--move-on-short** does no longer
      exist

- duarouter
  - patched behavior on missing files
  - debugged [bug 2042630 remove-loops breaks application](https://sourceforge.net/tracker/index.php?func=detail&aid=2042630&group_id=45607&atid=443421)


## Version 0.9.9 (05.08.2008)

- All
  - debugged [bug 1976405 Saving invalid dump
    configuration](https://sourceforge.net/tracker/index.php?func=detail&aid=1976405&group_id=45607&atid=443421);
    Was a false encoding in Option_IntVector
  - builds under 64bit Linux (solved bugs
    [bug 1927822](https://sourceforge.net/tracker/index.php?func=detail&aid=1927822&group_id=45607&atid=443421),
    [bug 1928370](https://sourceforge.net/tracker/index.php?func=detail&aid=1928370&group_id=45607&atid=443421),
    [bug 1930444](https://sourceforge.net/tracker/index.php?func=detail&aid=1930444&group_id=45607&atid=443421),
    [bug 1931468](https://sourceforge.net/tracker/index.php?func=detail&aid=1931468&group_id=45607&atid=443421),
    [bug 1932961](https://sourceforge.net/tracker/index.php?func=detail&aid=1932961&group_id=45607&atid=443421),
    [bug 1933569](https://sourceforge.net/tracker/index.php?func=detail&aid=1933569&group_id=45607&atid=443421),
    [bug 1934635](https://sourceforge.net/tracker/index.php?func=detail&aid=1934635&group_id=45607&atid=443421));
    thanks to Ronald Nippold for being persistent in supplying the
    build logs
  - encapsulated xerces-attributes; makes value retrieval so much
    more comfortable; retrieve a attribute using
    attrs.getTYPENAME(id) instead of getTYPENAME(attrs, id)

- Simulation
  - Hidden state loading/saving completely from the microsim
  - Vehicle may now end their route at a certain position of the
    route's last edge (see
    [Specification](../Specification/index.md))
  - The route may be now given as an attribute (*route*) within the
    vehicle definition (see
    [Specification](../Specification/index.md))
  - The vehicle now stores most of its definition parameters in
    *myParameter*, not as atomar values
  - Solved problems with multiple stop on same edge and with stops
    on lanes which can not be used for continuing the route; thanks
    to Friedemann Wesner and Gerrit Lammert for reporting them
  - Reworked mean data output. The vehicle movements are now
    weighted by the time the vehicle was on an edge/lane. Additional
    constraints for output
    (**--exclude-empty-edges**/**--exclude-empty-lanes**) allow to
    dump only information about edges/lanes which were occupied by
    vehicles.
  - debugged [bug 1964433 different simulation results for different starting times](https://sourceforge.net/tracker/index.php?func=detail&aid=1964433&group_id=45607&atid=443421);
    The issue was a false computation of WAUT switching and TLS
    offset times when choosing a different simulation begin time
    than 0
  - replaced MSVehicle::getInTransit and MSVehicle::running by
    MSVehicle::isOnRoad
  - removed MSEdge::isSource; was used only once and can be easily
    replaced by comparing the result of MSEdge::getPurpose()
  - debugged [bug 1932105 second stop on the same edge confuses vehicles](https://sourceforge.net/tracker/index.php?func=detail&aid=1932105&group_id=45607&atid=443421)
  - debugged [bug 1941846 repeatable crash with sumo r5378](https://sourceforge.net/tracker/index.php?func=detail&aid=1941846&group_id=45607&atid=443421)
  - debugged [bug 1912547 linux 64 bit compile error for sumo svn 5143](https://sourceforge.net/tracker/index.php?func=detail&aid=1912547&group_id=45607&atid=443421)
  - debugged [bug 1913655 command line sumo throws unknown exception](https://sourceforge.net/tracker/index.php?func=detail&aid=1913655&group_id=45607&atid=443421)
  - debugged [bug 1924989 sumo r5186 crashes by pure virtual function call](https://sourceforge.net/tracker/index.php?func=detail&aid=1924989&group_id=45607&atid=443421)
  - begun work on [Specification](../Specification/index.md)
    - all vehicles are emitted at position=0, with speed=0
  - removed usage of NLLoadFilter
  - added an option to quit the simulation if a number of vehicles
    is exceeded (**--too-many-vehicles <INT\>**)
  - fully integrated Tino Morenz's MSVTypeProbe; moved it from
    microsim/trigger to microsim/output; tests added; a vtypeprobe
    is now defined within additional files this way:
    `<vtypeprobe id="test1" type="" freq="10" file="vtypeprobe.xml"/>`
    ("test1" is the vehicle type to report, leave empty in order to
    get information about all vehicles)
  - replaced usage of "Command" by encapsulating methods in
    "WrappingCommand" wherever possible
  - "debugged" usage of actuated/agent-based tls; should be covered
    by tests, in fact
  - further work on devices-consolidation
  - implemented a one-shot / multi-shot routing device

- sumo-gui
  - Removed opening an additional command line window from the
    release version
  - Title is now set properly
  - debugged [bug 2021454 Discarding View settings resets them](https://sourceforge.net/tracker/index.php?func=detail&aid=2021454&group_id=45607&atid=443421);
    The editor now uses the settings that were used when it was
    opened as the one to apply when "cancel" is pressed
  - debugged [bug 2021464 view settings are not saved](https://sourceforge.net/tracker/index.php?func=detail&aid=2021464&group_id=45607&atid=443421);
    The button is now removed, all the interaction will be done via
    the top buttons
  - debugged [bug 2015792 Save List of selected Items](https://sourceforge.net/tracker/index.php?func=detail&aid=2015792&group_id=45607&atid=443421);
    The proper extension (mostly ".txt") is now appended if no
    extension is given. Affects saving breakpoints, additional
    weights, tracked parameter, and selections.
  - [bug 1919509 guisim crashed](https://sourceforge.net/tracker/index.php?func=detail&aid=1919509&group_id=45607&atid=443421)
      is invalid, the issue was a buggy opengl driver

- netconvert
  - debugged [bug 1993928 --keep-edges.input-file does not remove empty nodes](https://sourceforge.net/tracker/index.php?func=detail&aid=1993928&group_id=45607&atid=443421);
    as soon as one edge-removal(keeping) option occurs, nodes are
    rechecked whether they contain any further edge. If not, the
    node is removed.
  - VISUM-import: debugged failures due to false district node
    position computation
  - Connection building: made storing outgoing edge connections
    sane; replaced three strange storages by one
  - Disabled the support for setting an "edge function" - because
    vehicles are emitted the same (parametrised) way on each edge,
    we do not need a distinction between source and normal edges;
    sink edges were never distinguished.
  - Refactored the way edge types are stored in netconvert
  - removed "name" attribute from NBEdge; was not written anyway
  - work on SUMO-networks reimport (unfinished)
  - documentation: debugged [bug 1909685 option "omit-corrupt-edges" not recognized](https://sourceforge.net/tracker/index.php?func=detail&aid=1909685&group_id=45607&atid=443421)
    (use **--dismiss-loading-errors** instead of
    **--omit-corrupt-edges**)
  - removed usage of NLLoadFilter
  - thanks to Christoph Sommer, the OSM importer now handles speed
    and lanes information; default OS values are now used, too

- duarouter / jtrrouter
  - Removed option **--move-on-short**. It got obsolete as vehicles
    may start on edges which are shorter than they are, now.

- duarouter
  - debugged [bug 1981921 duarouter truncates routes](https://sourceforge.net/tracker/index.php?func=detail&aid=1981921&group_id=45607&atid=443421);
    Now, **--remove-loops** only prunes the route if the vehicle
    would have to take a turnaround at the begin or end.
  - reworked handling of broken routes; longer routes are not
    replaced; incomplete routes may be corrected using the new
    **--repair** option

- jtrrouter
  - Removed possibility to specify turning percentages using CSV
    files

- dfrouter
  - Definition of emitters, vss, etc. are now written into the same
    folder as the declaration

- TraCI
  - TraCI is now enabled as default

- od2trips
  - due to being used for macroscopic demand definitions, od2trips
    now emits vehicles using departlane="best" and departspeed="max"
  - removed option to parse list of tables from a vissim file; they
    may be given at the command line (**--vissim** is no longer
    supported)
  - added the possibility to omit writing the vehicle type
    (**--no-vtype**)

- [Tools](../Tools/index.md)
  - added some tools for output visualization
    (mpl_dump_twoAgainst.py, mpl_tripinfos_twoAgainst.py,
    mpl_dump_timeline.py) (see
    [Tools/Visualization](../Tools/Visualization.md))


## Version 0.9.8 (07.02.2008)

- Building
  - MSVC8: extracted properties for build configurations into
    separate property files.
  - many coarse changes in directory/project due to resolving
    request 1641440
  - gui/dialogs, gui/drawerimpl, /utils/gui/events,
    /utils/gui/windows, /utils/gui/globjects, /utils/gui/tracker
    have now own projects
  - microsim/traffic_lights - project was named properly
  - netbuild/nodes, microsim/lanechanging, and all netimport classes
    but vissim import were moved one folder up
  - SUMOFrame was moved to microsim (renamed to MSFrame!)
  - utils/helpers and utils/router were moved to utils/common

- all
  - added inline-documentation for utils/importio

- simulation
  - debugged tls-coupled e2-output
  - debugged [bug 1886198 Segmentation
    Fault](https://sourceforge.net/tracker/index.php?func=detail&aid=1871037&group_id=45607&atid=443421);
    see comment; thanks to Helcio Bezerra de Mello for the bug
    report
  - further work on detectors
    - values revalidated and described output has changed
    - marked "measures" and "style" as deprecated
  - while looking for next links to use, the vehicle is now aware of
    his best lanes
  - simplified computation of best lanes
  - debugged [bug 1871859 vehicles do not wait on lane
    end](https://sourceforge.net/tracker/index.php?func=detail&aid=1871859&group_id=45607&atid=443421);
    thanks to Jensen Chou for the bug report
  - debugged [bug 1871964 left-moving vehicles do not fill
    junction](https://sourceforge.net/tracker/index.php?func=detail&aid=1871964&group_id=45607&atid=443421)
  - patched error handling in emitters; tests added
  - debugged [bug 1873155 vehicles with depart lower than begin are
    emitted](https://sourceforge.net/tracker/index.php?func=detail&aid=1873155&group_id=45607&atid=443421);
    tests added
  - tests for rerouter added; partially debugged
  - added inline-documentation for microsim/output
  - debugging inner-junction simulation
  - refactoring detectors
    - MSDetector2File into MSDetectorControl
    - complete e1-refactoring
    - complete e2-refactoring
    - complete e3-refactoring
    - MSMeanData_Net_Cont is now simply a
      std::vector\<MSMeanData_Net\*\>
    - improved documentation
    - spell checking
- removed MSUnit - the usage was bogus, often same functions were
  defined twice. Rather confusing. Added
  utils/common/PhysicalTypeDefs.h instead, as a new begin.
- Removed MSNet's deltaT - was falsely used and duplicate with
  utils/common/SUMOTime

- guisim
  - debugged [bug 1830296 guiemitter destroys event
    que](https://sourceforge.net/tracker/index.php?func=detail&aid=1830296&group_id=45607&atid=443421)
    by introducing a thread-aware event control version for guisim
    (GUIEventControl)
  - implementing [feature request 1645216 Indicator / blinker
    depending on junction
    distance](https://sourceforge.net/tracker/index.php?func=detail&aid=1645216&group_id=45607&atid=443424)
  - debugged [bug 1862073 view is not refreshed when (de)selecting
    item](https://sourceforge.net/tracker/index.php?func=detail&aid=1862073&group_id=45607&atid=443421)
  - added doxygen-documentation for utils/gui/div
- work on problems with guisim (undeterministic termination)

- netconvert
  - debugged [bug 1886198 User reports unknown error while importing
    shapefile](https://sourceforge.net/tracker/index.php?func=detail&aid=1886198&group_id=45607&atid=443421);
    see comment; thanks to Maxim Raya for the bug report

- dfrouter
  - trying to make it more usable for inner-city scenarios with many
    connections
- spell checking
  - renamed 'succeding' to 'succeeding'
  - guisim now shows internal lanes less wide
  - false left closure of leftmost lane shown in "real world" scheme
    debugged
- removed utils/importio/CSVHelpers (was used once and contained a
  two-lines-method (sic!))
- working on doxygen documentation (and adding throw-declarations)
  - utils/options
  - utils/iodevices
  - utils/importio
- removing unneeded debug-helpers in microsim
- debugged problems with repetition of vehicles that have a color
  within guisim (application terminated)
- patched XMLSubSys documentation (was
  [bug 1828803](https://sourceforge.net/tracker/index.php?func=detail&aid=1828803&group_id=45607&atid=443421))
- fixed vehicle decelerations when being on a higher priorised road
  (bug
  [1830266](https://sourceforge.net/tracker/index.php?func=detail&aid=1830266&group_id=45607&atid=443421))
- fixed netconvert bugs occurring when connected nodes are too close
  together (bugs
  [1639460](https://sourceforge.net/tracker/index.php?func=detail&aid=1639460&group_id=45607&atid=443421),
  [1639436](https://sourceforge.net/tracker/index.php?func=detail&aid=1639436&group_id=45607&atid=443421))
- moved color out of MSVehicle (affects loading classes)
- router import classes now process input colors as strings
- consolidated XML-definitions
  - vehicle type is now called "type" in all cases
  - removed unneeded elements and attributes
- (almost complete) rework of the microsimulation loop
What was done, basically is to change the way vehicles move over
junctions and how other vehicles get informed about vehicles
approaching on incoming lanes. Quite heavy changes, in fact, I hope
of benefit. You may read some further information on this in the
following tracker items: [bug 1840935: exchange of vehicles over
lanes disallow fails with two
vehs](https://sourceforge.net/tracker/index.php?func=detail&aid=1840935&group_id=45607&atid=443421),
[bug 1840938: lane exchange mechanism ignores undefined lane
order](https://sourceforge.net/tracker/index.php?func=detail&aid=1840938&group_id=45607&atid=443421),
[bug 1840952: drop vehicle exchange in lane
changer](https://sourceforge.net/tracker/index.php?func=detail&aid=1840952&group_id=45607&atid=443421),
[bug 1840944: replace approaching information in
lanes](https://sourceforge.net/tracker/index.php?func=detail&aid=1840944&group_id=45607&atid=443421)

- Further work on inner-junction traffic (still not 100% complete)
  - bug [1840950: take into account inner-junction
    state](https://sourceforge.net/tracker/index.php?func=detail&aid=1840950&group_id=45607&atid=443421)
- speeding up simulation by not touching empty lanes
- C2C was made optional (using defines) in order to reduce memory
  footprint; was: feature 1843267: make extensions optional


## Version 0.9.7 (14.11.2007)

- 23.05.07: sumo-gui now contains storages for recent configs AND
  networks, usage of "RecentFiles" reworked
- 23.05.07: DOCS: generated pdfs are copied to <SUMO_DIST\>/docs
- 24.05.07: DOCS: removed references to man-pages in docs section
- 24.05.07: DOCS: links to FAQ and Publications point to the wiki now
- 24.05.07: duarouter/SIM: removed the snipplet embedding prototype
  (was not yet used)
- 24.05.07: TOOLS: added script to apply Astyle on the complete code
- 24.05.07: TOOLS: flowrouter.py and flowFromRoutes.py can deal with
  non-integer flows
- 24.05.07: TOOLS: flowrouter.py has an option to ignore detector
  types
- 24.05.07: NETGEN: replaced an occurrence of rand() with randSUMO()
  and reworked the randomness code a little
- 30.05.07: dfrouter: moved to the new exception-handling concept (see
  [Developer/Implementation Notes/Error Handling](../Developer/Implementation_Notes/Error_Handling.md))
- 30.05.07: dfrouter: removed bug "\[ 1724214 \] no user information
  about missing file description header"
- 31.05.07: declaring ";" list divider as deprecated
- 06.06.07: large code recheck: moved warning pragmas to
  windows-config; patched inclusion of the correct configuration file
- 07.06.07: started to rework exception handling (no more
  XMLBuildingExceptions, mostly ProcessErrors)
- 12.06.07: changed API for xml-handlers (now only supplying the
  numerical element ids); made handler functions not pure virtual
- 12.06.07: made PROJ and GDAL optional
- 20.06.07: new XML header in most output files
- 22.06.07: Version info added automatically
- 27./29.06.07: reworked computation of lane-to-lane directions and
  main direction (see also OpenProblems)
- 02.07.07: Added tests for error handling while loading broken
  networks (sumo, jtrrouter, duarouter, dfrouter)
- somewhere between: reworked application subsystem (removing
  OptionsSubSys)
- renaming variables to the myVar scheme
- 16.07.07: removed -l to be used as a shortcut for --lane-weights
  because -l is already used as a shortcut for --log-file (duarouter,
  jtrrouter)
- 16.07.07: removed the omit-unbuild-edges - option; there should be
  no other functionality than for --dismiss-loading-errors
  (netconvert, NETGEN)
- 17.07.07: refactored VISUM-net importer; now using 2 classes instead
  of 18 (netconvert)
- 17.07.07: debugged sumo-gui's options handling
- 17.07.07: removed support for ARTEMIS (never used since 2003)
- OutputDevice supports TCP sockets
- skipping MSVC6 / 7 support
- DFRouter beautification
- JTRRouter uses vehicle classes
- possibility to switch traffic lights off
- preparing subsecond simulation
- Cell/Fastlane support removed
- unsupported applications removed
- starting detector refactorization
- emitting at the end of the step
- ITM-RemoteServer renamed to TraCI
- Visual Studio 64bit build (without Proj, GDAL and Fox)
- Route handling changed (multiref deprecated)


## Version 0.9.6 (23.05.2007)

- polyconvert: possibility to prune the polygons to import on a
  bounding box added
- DUA: Simulation may now load only a part of defined vehicles
- netconvert:
  - debugged connections for on-ramps which already have the right
    number of lanes
  - debugged problems when using networks built without internal
    lanes in conjunction with (\!\!\! use-internal in simulation)
  - lanes split definition added to lane definitions (\!\!\!
    undescribed)
  - added error messages when false lanes are defined in connections
- SIMULATION:
  - c2c wth rerouting
- sumo-gui:
  - debugged memory leak in polygon visualization
  - removed simple/full geometry options; everything is now drawn
    using full geometry
  - the names of nodes, additional structures, vehicles, edges, pois
    may now be shown
- All: help/config subsystem changed
- Netz hat sich geändert
- added the possibility to load a network only
- lane expansions
- iodevices reworked partially
- "Action" removed\* was the same as Command
- changed the Event-interface
  - execute now gets the current simulation time
  - event handlers are non-static
- .icc-files removed
- class-documentation added/patched within dfrouter
- made access to positions in Position2DVector c++ compliant
- xml-types are now read prior to other formats (other Xml-description
  afterwards)
- double projection in XML-import debugged
- vehicle actors added
- shape layers added
- extracted drawing of lane geometries
- added projection information to the network
- edge types may now store the edge function
- removed the unneeded slow lane changer
- VISUM: lane connections for unsplitted lanes implemented, further
  refactoring
- retrieval of microsim ids is now also done using getID() instead of
  id()
- complete od2trips rework
- all structures now return their id via getID()
- rerouter: patched the distribution usage; added possibility o load
  predefined routes
- unifying threads
- beautifying: output consolidation
- removal of loops added (undescribed\!\!\!)
- got rid of the static RONet instance
- got rid of the cell-to-meter conversions
- began with the extraction of the car-following-model from MSVehicle
- began the work on a network editor
- added consecutive process messages
- debugged splitting of matrices
- added the possibility to stretch/change the current phase and
  consecutive phases
- made static comboboxes really static
- VISUM: patching problems on loading split lanes and tls
- added blinker visualisation and vehicle tracking (unfinished)
- retrieval-methods have the "get" prependix; EmitControl has no
  dictionary; MSVehicle is completely scheduled by MSVehicleControl;
  new lanechanging algorithm
- tls switches added
- made the assertion that two consecutive edge geometry points must not
  be same less aggressive
- made MSVehicleControl completely responsible for vehicle handling\*
  MSVehicle has no longer a static dictionary
- dump-begin/dump-end renamed to dump-begins/dump-ends
- E3 detectors refactored partially
- added the possibility to make a polygon being filled or not
- cartesian and geocoordinates are shown; changed the APIs for this
- error checks for dump-begins/dump-ends added
- simulation-wide cartesian to geocoordinates conversion added
- ROUTER: faster access to weight time lines added;
- removed build number information
- API for drawing not filled circles
- first try to make junctions tls-aware
- changed the way geocoordinates are processed
- changed vehicle class naming
- patching junction-internal state simulation
- added vehicle class support to microsim
- added vehicle class support to microsim
- removed deprecated c2c functions, added new made by Danilot Boyom
- c2c visualisation added (unfinished)
- added the possibility to copy an artefact's (gl-object's) name to
  clipboard (windows)
- removed unneeded id member in MSMoveReminder
- added an error handling for adding a tls definition for an unknown
  tls
- added the option to not emit vehicles from flows using a fix
  frequency
- tls tracker now support switches between logics
- first steps towards car2car-based rerouting
- variable vehicle type usage



## Version 0.9.4

This release is rather a snapshot. Many things have changed and some
will require some further revalidation. There are three major
changes/extensions:

**1.** The TLS-subsystem now allows having more than a single logic
for a traffic light. One can describe the switch times and switch
between them on the GUI. The user-description on this may be found under [Traffic Lights](../Simulation/Traffic_Lights.md). There is no developer documentation on this, yet.
If you are parsing the networks by your own, you will see that traffic
lights definitions have changed slightly. Instead of the sgmltag
"logicno" a tag named "subkey" is used by now. Also, "inclanes" has been
removed.

**2.** A full support to import networks with positions encoded in
geocoordinates. This makes the usage of two further libraries necessary,
PROJ.4 and GDAL. The new building process is described [here](../Installing/Windows_Build.md).
A description about how to import geocoordinated files (despite of
ArcView-files) can be found [here](../Geo-Coordinates.md).

Together with this extension, the import of ArcView-files has been
rechecked and now allows to import networks stored in other schemes than
the one NavTeq uses. The new options are described [here](../Networks/Import/ArcView.md).

Some further comments on importing ArcView-files are available [here](../Networks/Import/ArcView.md#arcview_import_options). By now, they include only some comments on importing an open-source network of Osnabrück. You can take a look at this [here](../Networks/Import/ArcView.md#frida_network_city_of_osnabruck) and also download it from [here](https://frida.intevation.org/download.html).


**3.** The developer documentation has been split into several parts.
It seems to be unmanageable to write a complete developer documentation.
Due to this, we now offer some additional documentation on certain
topics. Topics that were previously a part of the developer
documentation may be now found within the [More Docs](../Developer/index.md) section.


There are also some further changes which do not yet show their
potentials, but are quite promising:

**1.** The routing interface has been made abstract. This allows to
use routing algorithms in both the simulation as in the routing
applications and will hopefully yield implementations of some further
routing algorithms - by now Dijkstra and a turning ration algorithm are
available.

**2.** Some first steps towards making SUMO be aware of vehicle types
have been done. It is now possible to allow/disallow vehicles by class
to use certain lanes. This is now implemented in the netconvert
application and within duarouter, but not yet in the simulation itself
and due to this not yet documented. But, as you can see, we are working
towards making SUMO multi-modal...

The complete list of changes:

- Multimodality
  - made lanes storing vehicle classes (additional XML-definition
      for lanes, Router is aware of vehicle classes) (undocumented)
- Routing Interface
  - Router have been made abstract
- GUI
  - the Grid now handles networks with negative positions
  - debugging the visualization grid (unfinished)
  - glu is now used to draw polygons in order to allow concave
    polygons
  - pois may now be added and moved (using shift)
  - Emitter may show their routes
  - added option to change the exaggeration of POIs
- Simulation
  - detector position may now be "friendly" (see
    [E1-Detectors (Induct Loops)](../Simulation/Output/Induction_Loops_Detectors_(E1).md))
  - documentation on variable speed signs added (see [Variable Speed Signs (VSS)](../Simulation/Variable_Speed_Signs.md))
- netconvert
  - Improved ArcView-import
  - Additional option for importing NavTeq-networks (using
    ArcView/Elmar/Elmar2): new navteq lane number information may be
    now only used if not differing too much from the previous \!\!\!
  - ramp-guessing improved: no lanes are appended if there are
    enough
  - Vision import added (see [Importing VISUM-networks](../Networks/Import/VISUM.md))
  - Positions of guessed TLS may now be saved as POIs (undocumented, yet)
  - The edge function can be given (undocumented, yet)
- jtrrouter
  - default-percentages may now contain more than three items
  - Examples added
  - renamed "percentage" to probability
  - Documentation updated (see [Using the Junction Turning Ratio - Router](../jtrrouter.md))
- All
- documentation on meta information added


## Version 0.9.2

User-relevant changes

- GUI
  - version information in gui patched
  - segfaults on loading broken configs patched
  - catching opening a second file using recent files added
  - subwindows are now deleted on (re)loading the simulation
  - some improvements on visualisation (unfinished (GridBuilder))
  - viewport changer debugged
  - problems on loading geometry items patched
  - list of selected items is now cleared on closing/reloading (was
    buggy before)
  - table editing debugged
  - rerouters are now correctly named
- Simulation
  - debugging numeric issues in MSVehicle
  - Emitters reworked
- Netconversion
  - output during netbuilding patched
  - network geometry reworked (unfinished)
  - "speed-in-km" is now called "speed-in-kmh"
  - "type-file" is now called "xml-type-files"
  - removed two files definition for arcview
  - allowed further information to be stored in arcview-files
- Documentation
  - User docs, netconversion partly updated
  - further work on tls-api-description
- All
  - copyright information patched

Developer-relevant changes

- GUI
  - unused addTimeout/removeTimeout-APIs removed
  - debugged building of the gui-application with MSVC7
- Simulation
  - (MS|GUI)TriggeredEmitter is now (MS|GUI)Emitter
  - TLS-API: MSEdgeContinuations added
  - usage of internal lanes is now optional at building
- All
  - got rid of the old MSVC memory leak checker
  - several memory leaks removed
  - made checking the geometry for correctness optional on building
   (CHECK_UNIQUE_POINTS_GEOMETRY)
  - made memory checking optional on building (CHECK_MEMORY_LEAKS)
  - foreign libraries will now be moved to src/foreign


## Version 0.9.1

User-relevant changes

- GUI
  - bug on not loading junction geometry patched
- Simulation
  - actuated tls-lights bug patched

Developer-relevant changes

- GUI
  - unused addTimeout/removeTimeout-APIs removed
- Simulation
  - renamed NLNetHandler to NLHandler
  - each MSJunction has a reference to the net; net allows retrieval of the tls-logics container (see helpers/NamedObjectCont.h)
  - removed several memory leaks, mainly occurring after broken loading
  - bugs in actuated traffic lights logic patched
- Router
  - renamed all ROJP\*-classes to ROJTR\*


## Version 0.9.0 (07.10.2005)

User-relevant changes:

- GUI
  - Selections
    - Flag icons added to Popup-Menues
    - Shift adds (de)selects the whole street, not only a lane
    - Selection-Editor now saves to \*.txt-files
  - Centering of objects has been revalidated
  - Variable Speed Signs
    - Visualisation of vss added
    - Added the possibility to interact with vss'
  - "Reload Simulation" option added
  - Status-Bar message patched
  - Rotation removed (was buggy)
  - Error on closing the gui if parameter tracker are opened patched
  - Value tracker may now be aggregated and are able to save the tracked values
  - POlygon visualisation added
  - Breakpoints added
  - Setting and Saving of additional net weights added (preliminary, subject to improve)
  - Coloring by type and by route added
- Simulation
  - Reports why the simulation ended if in verbose mode
  - emissions-output error due to a division by zero patched
  - VSS may now control several lanes
- general
  - Options
    - Errors on default value usage patched
- Documentation
  - trying to separate XML-code (green) from command line options (red)
- netconvert
  - \--map-output <FILE\> builds a mapping that shows which edges have been removed while joining
  - guess-tls - options added
  - extended the FastLane-import by the -l parameter for the number of lanes
  - \--explicite-junctions <JUNCTION_NAME_LIST\> allows to say which junctions shall be explicitely tls-controlled
  - \--explicite-no-junctions <JUNCTION_NAME_LIST\> allows to say which junctions shall be explicitely non-tls-controlled
  - \--edges-min-speed <SPEED_MS\> allows to remove all edges from the input on which a lower speed than <SPEED_MS\> is allowed
  - \--elmar allows to import NavTech-GDF-files parsed using Elmar Brockfelds parser
  - \--tiger allows to import tiger files (preliminary, does not look THAAAAT good for an unknown reason, yet)
- Router
  - Generation of random route after the first depart fixed
  - Warnings about missing weights are printed only once per edge
  - Supplementary Weights
- Examples have now a new folder structure

Developer-relevant changes:

- GUI
  - Selected structures are now in a separate class
  - Additional structures are now displayed using the same "API"
- Simulation
  - removed the MSNet::dictionary
  - refactored the MSMeanData-output
