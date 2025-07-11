---
title: netedit
---

# From 30.000 feet

**netedit** is a graphical network editor included in SUMO.

- **Purpose:** create and modify SUMO networks
- **System:** portable (Linux/Windows is tested); opens a window
- **Input (optional):** road network definitions to import
- **Output:** A generated SUMO-road network; optionally also other outputs
- **Programming Language:** C++

# Usage Description

**netedit** is a visual network editor. It can be used to create networks from scratch and to modify all aspects of existing networks. With a powerful selection and highlighting interface it can also be used to debug network attributes. **netedit** is built on top of [netconvert](../netconvert.md). As a general rule of thumb, anything [netconvert](../netconvert.md) can do, **netedit** can do as well. **netedit** has unlimited undo/redo capabilities and thus allows editing mistakes to be quickly corrected. Editing commands are generally issued with a left-click (according to the current [edit mode](editModes.md)). The user interface closely follows that of [sumo-gui](../sumo-gui.md#interaction_with_the_view). Additional keyboard hotkeys are documented in the *Help* menu.

**netedit** can also be used to define and edit:

- [additional simulation infrastructure](elementsAdditional.md)
- [traffic (vehicles, persons, vTypes, routes)](elementsDemand.md)
- [network/TAZ related data](editModesData.md)
- [shape data](elementsShape.md)

## Input

**netedit** is a GUI-Application and inputs are chosen via menus. The following input formats are supported:

- SUMO-net files
- OSM-files
- [netconvert](../netconvert.md)-configuration files

## Output

- SUMO-net files
- plain-xml files

## Hotkeys

The entire list of shortcuts can be found [here](shortcuts.md):

| File shortcuts              | Key              | Description                         |
| --------------------------- | ---------------- | ----------------------------------- |
| New Window                  | <kbd>Ctrl</kbd> + <kbd>Shift</kbd> + <kbd>N</kbd> | Open a new netedit window           |
| New Network                 | <kbd>Ctrl</kbd> + <kbd>N</kbd>         | Create a new network                |
| Open Network                | <kbd>Ctrl</kbd> + <kbd>O</kbd>         | Open an existing network            |
| Open configuration          | <kbd>Ctrl</kbd> + <kbd>T</kbd>         | Open an existing SUMO configuration |
| Import foreign network      | <kbd>Ctrl</kbd> + <kbd>F</kbd>         | Import a foreign network            |
| Reload                      | <kbd>Ctrl</kbd> + <kbd>R</kbd>         | Reload network                      |
| Save network                | <kbd>Ctrl</kbd> + <kbd>S</kbd>         | Save network                        |
| Save network as             | <kbd>Ctrl</kbd> + <kbd>Shift</kbd> + <kbd>S</kbd> | Save network in another file        |
| Save plain XML              | <kbd>Ctrl</kbd> + <kbd>L</kbd>         | Save network as plain XML           |
| Save joined junctions       | <kbd>Ctrl</kbd> + <kbd>J</kbd>         | Save joined junctions of network    |
| Load additionals and shapes | <kbd>Ctrl</kbd> + <kbd>A</kbd>         | Load a file with additionals        |
| Save additionals and shapes | <kbd>Ctrl</kbd> + <kbd>Shift</kbd> + <kbd>A</kbd> | Save additionals in a file          |
| Load demand elements        | <kbd>Ctrl</kbd> + <kbd>D</kbd>         | Load a file with shapes             |
| Save demand elements        | <kbd>Ctrl</kbd> + <kbd>Shift</kbd> + <kbd>D</kbd> | Save demand elements in a file      |
| Load TLS Programs           | <kbd>Ctrl</kbd> + <kbd>K</kbd>         | Load a file with shapes             |
| Save TLS Programs           | <kbd>Ctrl</kbd> + <kbd>Shift</kbd> + <kbd>K</kbd> | Save Point Of Interest in a file    |
| Close                       | <kbd>Ctrl</kbd> + <kbd>W</kbd>         | Close network                       |

| Edit shortcuts | Key      | Description          |
| -------------- | -------- | -------------------- |
| Undo           | <kbd>Ctrl</kbd> + <kbd>Z</kbd> | Undo the last change |
| Redo           | <kbd>Ctrl</kbd> + <kbd>Y</kbd> | Redo the last change |


| Processing shortcuts    | Key | Description                            |
| ----------------------- | --- | -------------------------------------- |
| Compute Junction        | <kbd>F5</kbd>  | Compute junctions of the network       |
| Clean Junction          | <kbd>F6</kbd>  | Clean junctions of the network         |
| Join selected Junctions | <kbd>F7</kbd>  | Join selected junctions of the network |
| Options                 | <kbd>F10</kbd> | Open options dialog                    |

| Text editing shortcuts | Key      | Description                                      |
| ---------------------- | -------- | ------------------------------------------------ |
| Cut                    | <kbd>Ctrl</kbd> + <kbd>X</kbd> | Cut selected text of a text field                |
| Copy                   | <kbd>Ctrl</kbd> + <kbd>C</kbd> | Copy selected text of a text field               |
| Paste                  | <kbd>Ctrl</kbd> + <kbd>V</kbd> | Paste previously cut/copied text in a text field |
| Select all text        | <kbd>Ctrl</kbd> + <kbd>A</kbd> | Select all text in a text field                  |

| Miscellaneous shortcuts | Key      | Description                                      |
| ----------------------- | -------- | ------------------------------------------------ |
| Grid  | <kbd>Ctrl</kbd> + <kbd>G</kbd>        | Toggle background grid
| Grid+ | <kbd>Ctrl</kbd> + <kbd>PageUp</kbd>   | Increase grid spacing               |
| Grid- | <kbd>Ctrl</kbd> + <kbd>PageDown</kbd> | Reduce grid spacing               |


**Toolbar shortcuts**
The mode-depended option checkboxes (such as *Toggle grid* and others) located in the toolbar, can easily be checked an unchecked using the <kbd>Alt</kbd> + <kbd>NUMBER</kbd> shortcut, where NUMBER corresponds to its position within the options (e.g. *Toggle grid* is 1, and so on).

---

In addition to these hotkeys, [all hotkeys for moving and zooming in
sumo-gui](../sumo-gui.md#keyboard_shortcuts) are supported.

- <Button-Left\>: Execute mode specific action
  - in 'Inspect'-mode repeated clicks on the same location cycle through objects at that location
- <Button-Right\>: Open context-menu
- <Button-Right-Drag\>: Change zoom
- <Button-Left-Drag\>: Move the view around (*panning*)
  - in 'Move'-mode pointing at an edge: move or create geometry points
  - in 'Move'-mode pointing at a junction: move the junction
  - in 'Move'-mode pointing at a selected object: move all selected junctions and edges including geometry. If both junctions of an edge are selected, move the whole geometry. Otherwise, move only the geometry near the cursor
- <kbd>ESC</kbd>
  - in 'Create Edge'-mode: clear the currently selected source junction
  - in 'Select'-mode: clear the current selection; cancel rectangle selection
  - in 'Connect'-mode: deselect current lane and cancel all changes
  - in 'Traffic Light'-mode: revert changes to current traffic light
  - when editing a junction shape: abort editing the current junction shape
- <kbd>ESC</kbd> + <kbd>SHIFT</kbd>: clear the current selection
- <kbd>DELETE</kbd>: delete all currently selected items
- <kbd>SHIFT</kbd>:
  - in 'Select'-mode: hold <kbd>SHIFT</kbd> and drag the mouse for rectangle selection
  - in 'Select'-mode: <kbd>SHIFT</kbd>-left-click selects lanes instead of edges
  - in 'Inspect'-mode: <kbd>SHIFT</kbd>-left-click inspects lanes instead of edges
  - in 'Delete'-mode: <kbd>SHIFT</kbd>-left-click deletes lanes instead of edges
- <kbd>CTRL</kbd>:
  - in 'Create Edge'-mode, allow moving the view without defining junctions
  - in 'Create Additional'-mode, allow moving the view without adding additionals
  - in 'POI-Poly'-mode, allow moving the view without adding POIs
- <kbd>ALT</kbd>:
  - in 'Inspect'-mode: <Button-Left\> inspects individual object instead of selection (when object is selected)
  - in 'Inspect'-mode: <Button-Right\> opens a menu for selecting among all objects at the same location
- <kbd>Enter</kbd>:
  - in 'Inspect'-mode: confirm attribute changes
  - in 'Connect'-mode: deselect current lane and save all changes
  - in 'Traffic Light'-mode: save changes to current traffic light
  - in 'TAZ'-mode: save changes to current traffic light
  - when editing a junction shape: save the current junction shape

## Processing Menu Options

- **Compute Junctions** (<kbd>F5</kbd>): recomputes the geometry and logic of all junctions. This is triggered automatically when entering modes which require this information (Connect, Traffic Lights).
- **Compute Junctions with volatile options** (<kbd>Shift</kbd> + <kbd>F5</kbd>): recomputes everything and processes options that can modify the network if set (i.e. **crossings.guess**). This processing step does not support *undo/redo*.
- **Clean Junctions** (<kbd>F6</kbd>): removes all junctions that do not have any adjacent edges (These junctions are not included when saving the network. They are kept in the editor until cleaned for potential reuse).
- **Join Selected Junctions** (<kbd>F7</kbd>): joins the selected junctions into a single junction (See [joining junctions](../Networks/PlainXML.md#joining_nodes)).
- **Options** (<kbd>F10</kbd>): inspect and set all options. These are the same options that [netconvert](../netconvert.md) accepts on the commandline or in a configuration.

## Customizing Visualization

Most of the visualization options that are available in [sumo-gui](../sumo-gui.md#changing_the_appearancevisualisation_of_the_simulation) can also be set in netedit. This includes the facilities for [loading
background images](../sumo-gui.md#showing_background_images). It is also possible to load [POIs and Polygons](elementsShape.md) using the menu *File-> Additionals and Shapes-> Load Additionals*.

### Background Grid

When activating the background grid via the visualization settings dialog ![](../images/icon_colorwheel.png) or via the *show grid* option in [Create Edge Mode](editModesNetwork.md#create_edges), all clicks that create or move objects will snap to the nearest grid point. The resolution of the grid can be
customized in the *Background*-Tab of the visualization settings dialog. By using hotkey <kbd>Ctrl</kbd> + <kbd>PageUp</kbd> the grid spacing is doubled and with <kbd>Ctrl</kbd> + <kbd>PageDown</kbd> the grid size is reduce by a factor of 2.

## Terminology

The main network elements that are used to represent a road network in SUMO are [edges, lanes, junctions, connections, traffic
lights](../Networks/SUMO_Road_Networks.md). [Additional infrastructure elements](elementsAdditional.md) such as bus stops, detectors and variable speed signs are kept in separate files and
loaded at the start of the simulation. In SUMO the terms **node** and **junction** and **intersection** are used interchangeably (in netedit the term junction is used throughout).

# Editing Modes

Netedit can be switched between three major edit modes (also called ***supermodes***) for editing [network](editModesNetwork.md)-related objects, [traffic](editModesDemand.md)-related objects and [data](editModesData.md) objects.
Each of these [supermodes](editModes.md) has a number of sub-modes (simply called ***modes***) for inspecting, deleting and adding the different objects.
Some of these modes are common to all supermodes while others are specific to a particular supermode as listed below.

## [Common modes](editModesCommon.md)

This modes are present in all supermodes and have custom cursors:

- [Inspect](editModesCommon.md#inspect)
- [Delete](editModesCommon.md#delete)
- [Select](editModesCommon.md#select)
- [Move](editModesCommon.md#move) (absent in *Data Supermode*)

## [Network specific modes](editModesNetwork.md)

Modes available in the *Network Supermode*:

- [Edges](editModesNetwork.md#create_edges)
- [Connections](editModesNetwork.md#edit_connections)
- [Prohibitions](editModesNetwork.md#prohibitions)
- [Traffic Lights](editModesNetwork.md#traffic_lights)
- [Additionals (busStops, detectors, ...)](editModesNetwork.md#additionals)
- [Crossings](editModesNetwork.md#crossings)
- [TAZ](editModesNetwork.md#taz_traffic_analysis_zones)
- [Shapes (POI, Poly)](editModesNetwork.md#shapes)

## [Demand specific modes](editModesDemand.md)

Modes available in the *Demand Supermode*:

- [Routes](editModesDemand.md#route_mode)
- [Vehicles](editModesDemand.md#vehicle_mode)
- [Vehicle Types](editModesDemand.md#vehicle_type_mode)
- [Stops](editModesDemand.md#stops_mode)
- [Person Types](editModesDemand.md#person_type_mode)
- [Persons](editModesDemand.md#person_mode)
- [Person Plans](editModesDemand.md#person_plan_mode)

## [Data specific modes](editModesData.md)

Modes available in the *Data Supermode*:

- [Edge Data](editModesData.md#edge_data_mode)
- [Edge Relation](editModesData.md#edge_relation_mode)
- [TAZ Relation](editModesData.md#taz_relation_mode)

# [Elements](elements.md)

Netedit is used to create and edit a variety of different objects. Each object may support additional functionality:

- [Network elements](elementsNetwork.md)
- [Additional elements](elementsAdditional.md)
- [Shape Elements](elementsShape.md)
- [TAZ elements](elementsTAZ.md)
- [Demand elements](elementsDemand.md)

A short description of all [element attributes](attribute_help.md) is available.

# [Popup-Menu functions](neteditPopupFunctions.md)

Right clicking over an element will open a popup-menu with functions and operations relevant to set element. You can right click on:

- [Edge and Lane](neteditPopupFunctions.md#edge_and_lane)
- [Junction](neteditPopupFunctions.md#junction)
- [Connection](neteditPopupFunctions.md#connection)
- [Crossing](neteditPopupFunctions.md#crossing)
- [Modifiable Poly](neteditPopupFunctions.md#modifiable_poly)
- [Additionals](neteditPopupFunctions.md#additionals)
- [Route](neteditPopupFunctions.md#route)

# [Usage examples](neteditUsageExamples.md)

- [Reducing the extent of a network](neteditUsageExamples.md#reducing_the_extent_of_the_network)
- [Specifying the complete geometry of an edge including endpoints](neteditUsageExamples.md#specifying_the_complete_geometry_of_an_edge_including_endpoints)
- [Converting an intersection into a roundabout](neteditUsageExamples.md#converting_an_intersection_into_a_roundabout)
- [Correcting road access permissions](neteditUsageExamples.md#correcting_road_access_permissions)
- [Creating bidirectional railway tracks](neteditUsageExamples.md#creating_bidirectional_railway_tracks)
- [Creating pedestrian infrastructure](neteditUsageExamples.md#building_pedestrian_infrastructure)
- [**+ many others**](neteditUsageExamples.md).

# Planned Features

- Support for editing features already supported by [netconvert](../netconvert.md)
  - Editing `<neigh>` information for [Simulation/OppositeDirectionDriving](../Simulation/OppositeDirectionDriving.md)


!!! note
    Currently, you may either switching on **--opposites.guess** via the [F10-menu](#processing_menu_options) or set lane attribute 'oppositeID'.

- Editing [walkingarea shapes](../Networks/PlainXML.md#walking_areas)
- Ability to graphically edit decals in the "Decals"-Mode
- Adding / merging of sub-networks to the existing network
