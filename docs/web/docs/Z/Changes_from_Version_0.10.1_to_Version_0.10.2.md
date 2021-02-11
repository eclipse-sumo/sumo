---
title: Changes from Version 0.10.1 to Version 0.10.2
---

**Release date: 16.03.2009**

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
    [sumo-gui\#Using_Decals_within_sumo-gui](../sumo-gui.md#using_decals_within_sumo-gui))
  - Consolidated the startup-options **--suppress-end-info** and
    **--quit-on-end**: **--suppress-end-info** was removed, the
    application quits after performing the simulation when
    **--quit-on-end** is given. This is also done even if errors
    occured
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
  - solved [defect 38: speed instead of length while importing XML](https://github.com/eclipse/sumo/issues/38)
  - changed the tls-guessing procedure (unverified, yet); tls which
    span over multiple junctions can now been "guessed" if both,
    nodes are marked as being controlled by traffic lights or not
    within the imported data; see also: [task 42: (TAPAS) add possibility to generate joined tls if tls are given](https://github.com/eclipse/sumo/issues/42)

- polyconvert
  - solved [defect 14: polyconvert does not report about double ids](https://github.com/eclipse/sumo/issues/14)
  - renamed option **--xml-points** to **--xml**
  - renamed option **--shape-files** to **--shape-file**

- Tools
  - Rebuilt traceExporter; added tests

- Documentation
  - solved bug 1993983: Errors in User Documentation

- TAPAS
  - see [TAPASCologne Change Log](../Data/Scenarios/TAPASCologne.md#change_log)