---
title: Changes in versions pre0.8 up to 0.8.3
---

## Version 0.8.3 (07.10.2005)

User-relevant changes

- Simulation
  - debugged false tls-offset computation
- Windows-package
  - added libraries missing in 0.8.2.2


## Version 0.8.2.2 (09.09.2005)

User-relevant changes:

- Simulation
  - XML-Trigger bug on using speed triggers removed
  - emission of vehicles on multi-lane source edges debugged (now all lanes are used)
  - Debugged the problem of vehicles stopping before the route is over (reported by Jonathan Davies)
  - setting tls-offsets debugged
- netconvert
  - Computation of left-movers debugged


## Version 0.8.2.1 (22.12.2004)

**Please remind that several option names have changed!** The most
important: "net-files" (router, simulation) is now "net-file"...

Check user documentation and man-pages for this.

User-relevant changes:

- GUI
  - "Clear Message Window" menu added
  - the icons now allow loading instantly after the previous loading has failed
  - Renamed to "guisim"
  - using gradients instead of pure interpolation
  - windows-icons patched
  - added ability to take snapshots
  - using popups instead of comboboxes for finding structures and switching between coloring schemes in order to save place
  - speeded up handling of errors
  - Locators hidden behind a popup
  - ToolTips do now not hide dialog windows
  - Configuration errors are now reported to the window
  - Added the possibility to display a vehicle's route
  - added the possibility to save timelines
- Simulation
  - building dump-files now reports about failures
  - Patched some of the lane-changing bugs
  - time-to-teleport -option added
  - replace "treshhold" by "threshold"
- netconvert
  - Plain output of node and edges added (--plain-output)
  - got rid of the arcview/shapelib-import chaos
  - \--explicite-junctions -\> --explicite-tls
  - \--explicite-no-junctions -\> --explicite-no-tls
  - \--keep-edges.input-file
  - \--keep-edges - option added
  - \--keep-edges.input-file
  - Geometry computation bug patched
- Router
  - Bug on processing all time steps patched
  - Route output reformatted
  - max-alternatives
- Documentation
  - Man-pages added

Developer-relevant changes:

- GUI
  - Have reworked the class dependencies completely in order to make derivation more easy
- Simulation
  - output moved to "output"
  - outputs write to OutputDevices, not ostreams
  - reworked lanechanging



## Version 0.8.0.1

- GUI
  - The gui now remembers the last used folder
  - The gui now remembers his
  - The instance chooser was reworked and shows now which items are selected
  - An selected-items dialog with the ability to save the names of the selected items was implemented
  - New colors for aggregated view (aggregated views are still a prototype)
  - Fixed a bug at optional texture disabling
  - Update of trackers and tables on simulation steps added
  - About-dialog debugged
  - Message window scrolls if new text is appended
- Router
  - Output holding the number of loaded, build, and discarded routes
    added
  - \--move-on-short function implemented
  - Bug on repeatedly emitted vehicles patched
- Simulation
  - Bug on repeatedly emitted vehicles patched
- netconvert
  - Made the geometry of junctions pretty again
  - Too conservative right-of-way rules on importing vissim-networks was reworked
- general implementation issues
  - Replaced most output to cerr by MsgHandler-calls
- Documentation
  - extended the detector-documentation (user)
  - extended the gui-documentation
  - extended the description of the build process
  - added missing pictures
  - resized some pictures
  - improved the visibility of XML-format descriptions

... and some other things ...


## Version pre0.8

- Further applications added: router, od2trips-converter, tagreader
- A graphical user interface
- Further import possibilities added: ArcView, Visum, Vissim (partly in work), ARTEMIS
- A new concept of the microsimulation
- Traffic lights simulation
- Working lane changing
