---
title: Changes in the 2010 releases (versions 0.12.0, 0.12.1 and 0.12.2)
---

## Version 0.12.2 (07.12.2010)

- all
  - added options to save the xml schema for the configuration file
  - added "--version" option
- Simulation
  - arrivalpos, arrivallane and arrivalspeed parameters for vehicles
  - correcting lots of person handling bugs
  - refactoring vehicle route output and tripinfo into devices
  - clean up of inheritance structure of MSVehicle
- GUI
  - drawing arrows instead of using bitmaps


## Version 0.12.1 (07.10.2010)

- TraCI
  - added a possibility to access a vehicle's signals, wipers, and
    other things; see [Vehicle Value Retrieval](../TraCI/Vehicle_Value_Retrieval.md), [Change Vehicle State](../TraCI/Change_Vehicle_State.md), and
    [Vehicle Signalling](../TraCI/Vehicle_Signalling.md)
- Simulation
  - corrected direction when departing at a lane's begin
  - introduced a new `departpos` named "`base`". The vehicle starts at
    position=vehicle_length+.1, this means at the begin of the
    lane, but being completely on it. This `departpos` is now the default. (see
    [Definition of Vehicles, Vehicle Types, and Routes\#Vehicles and Routes](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#vehicles_and_routes)).
  - addressing teleports
    - corrected "incoming lanes" determination
    - added the option **--lanechange.allow-swap**; if enabled, vehicles blocking each other are exchanged
- GUI
  - debugged traffic lights tracker broken when moving to subseconds
- netconvert
  - OSM-Import
    - osm importer fails fast - furtherly given files are not
      processed in the case of an error
    - assigned default velocities to not-numerical maxspeed values
      ('none':300km/h, 'walk':5km/h); was: [defect 344: The TAPAS Cologne scenario does not work with the current(0.12.0) SUMO version - maxspeed not numeric](https://github.com/eclipse-sumo/sumo/issues/344)
    - parsing maxspeed values given as '50 km/h'; was:
      [defect 344: The TAPAS Cologne scenario does not work with the current(0.12.0) SUMO version - maxspeed not numeric](https://github.com/eclipse-sumo/sumo/issues/344)
    - new option **--osm.skip-duplicates-check** added which, well, skips checks for duplicate nodes and edges


## Version 0.12.0 (27.05.2010)

- Simulation
  - solved [regarding red light late blocks vehicles](https://sourceforge.net/apps/trac/sumo/ticket/173)
  - corrected the documentation on using [an abstract vehicle class](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#abstract_vehicle_class);
    the attribute which defines it is named `vclass` not `class`.
  - introducing flows as replacement for vehicles with repno and
    period
  - introducing stops in routes
  - corrected unmotivated deceleration in front of intersections
    reported by Björn Hendriks (thanks\!)
  - added option **--sloppy-emit** for not trying to emit on full edges (speeds up
    the simulation a lot)

- TraCI
  - moved to representation of time in milliseconds
  - Induction Loop occupancy is now given in % as defined

- sumo-gui
  - Improved layering (correct order of element drawing)
  - solved bug 2872900: same title/naming for different dialog boxes
  - solved bug 2872824: fox related sumo crash
  - Viewport settings can now be saved/loaded
  - Added new visualisation options
    - lanes: by vclass
    - vehicles: HSV by depart position/arrival position/direction
      and distance

- ROUTER
  - removed random routes generation; introduced a [script for generating random trips](../Tools/Trip.md#randomtripspy)
    instead

- netconvert
  - removed the **--tls-poi-output**; instead, [poi_atTLS.py](../Tools/Shapes.md) can be used
  - OpenStreetMap import
    - removal of edges and nodes which have the attribute `action='delete'`
    - recognition and removal of duplicate edges (all values but
      ids are same) added
    - opposite direction edge is built if oneway==-1
  - edge and node attributes can be overridden

- Tools
  - [tools for easier open street map import](../Networks/Import/OpenStreetMap.md) added
