---
title: netedit
permalink: /netedit/
---

# From 30.000 feet

**netedit** is a graphical network editor for SUMO.

- **Purpose:** create and modify SUMO networks
- **System:** portable (Linux/Windows is tested); opens a window
- **Input (optional):** road network definitions to import
- **Output:** A generated SUMO-road network; optionally also other outputs
- **Programming Language:** c++

# Usage Description

**netedit** is a visual network editor. It can be used to create networks from scratch and to modify all aspects of existing networks. With a powerful selection and highlighting interface it can also be used to debug network attributes. **netedit** is built on top of [netconvert](netconvert.md). As a general rule of thumb, anything [netconvert](netconvert.md) can do, **netedit** can do as well. **netedit** has unlimited undo/redo capabilities and thus allows editing mistakes to be quickly corrected. Editing commands are generally issued with a left-click (according to the current [edit mode](#edit_modes)). The user interface closely follows that of [sumo-gui](sumo-gui.md#interaction_with_the_view). Additional keyboard hotkeys are documented in the *Help* menu.

## Input

**netedit** is a GUI-Application and inputs are chosen via menus. The following input formats are supported:

- SUMO-net files
- OSM-files
- [netconvert](netconvert.md)-configuration files

## Output

- SUMO-net files
- plain-xml files

## Hotkeys

| File shortcuts              | Key              | Description                         |
| --------------------------- | ---------------- | ----------------------------------- |
| New Network                 | Ctrl + N         | Create a new network                |
| Open Network                | Ctrl + O         | Open an existing network            |
| Open configuration          | Ctrl + T         | Open an existing SUMO configuration |
| Import foreign network      | Ctrl + F         | Import a foreign network            |
| Reload                      | Ctrl + R         | Reload network                      |
| Save network                | Ctrl + S         | Save network                        |
| Save network as             | Ctrl + Shift + S | Save network in another file        |
| Save plain XML              | Ctrl + L         | Save network as plain XML           |
| Save joined junctions       | Ctrl + J         | Save joined junctions of network    |
| Load additionals and shapes | Ctrl + A         | Load a file with additionals        |
| Save additionals and shapes | Ctrl + Shift + A | Save additionals in a file          |
| Load demand elements        | Ctrl + D         | Load a file with shapes             |
| Save demand elements        | Ctrl + Shift + D | Save demand elements in a file      |
| Load TLS Programs           | Ctrl + K         | Load a file with shapes             |
| Save TLS Programs           | Ctrl + Shift + K | Save Point Of Interest in a file    |
| Close                       | Ctrl + W         | Close network                       |

| Edit shortcuts | Key      | Description          |
| -------------- | -------- | -------------------- |
| Undo           | Ctrl + Z | Undo the last change |
| Redo           | Ctrl + Y | Redo the last change |

| Supermodes shortcuts | Key  | Description                                      |
| -------------------- | ---- | ------------------------------------------------ |
| Network              | F2   | Change to supermode Network (default )           |
| Demand               | F3   | Change to supermode Demand (implies recomputing) |
| Data                 | F4   | Change to supermode Data (implies recomputing)   |

| Common modes shortcuts | Key | Description              |
| ---------------------- | --- | ------------------------ |
| Delete                 | D   | Change to mode "Delete"  |
| Inspect                | I   | Change to mode "Inspect" |
| Select                 | S   | Change to mode "Select"  |
| Move                   | M   | Change to mode "Move"    |

| Network mode shortcuts | Key | Description                    |
| ---------------------- | --- | ------------------------------ |
| Create Edge            | E   | Change to mode "Create Edge"   |
| Connection             | C   | Change to mode "Connection"    |
| Prohibition            | W   | Change to mode "Prohibition"   |
| Traffic light          | T   | Change to mode "Traffic light" |
| Additional             | A   | Change to mode "Additional"    |
| Crossing               | R   | Change to mode "Crossing"      |
| POI-Poly               | P   | Change to mode "POI-Poly"      |

| Demand mode shortcuts | Key | Description                          |
| --------------------- | --- | ------------------------------------ |
| Create Route          | R   | Change to mode "Create Routes"       |
| Create Vehicle        | V   | Change to mode "Create Vehicles"     |
| Create Vehicle Type   | T   | Change to mode "Create Vehicle Type" |
| Create Stop           | A   | Change to mode "Create Stops"        |
| Create person types   | W   | Change to mode "Create Person Types" |
| Create persons        | P   | Change to mode "Create Persons"      |
| Create person plans   | C   | Change to mode "Create Person Plans" |

| Data mode shortcuts | Key  | Description                                 |
| ------------------- | ---- | ------------------------------------------- |
| Edge data           | E    | Change to mode "Create Edge Datas"          |
| Edge relation data  | E    | Change to mode "Create Edge Relation datas" |

| Processing shortcuts    | Key | Description                            |
| ----------------------- | --- | -------------------------------------- |
| Compute Junction        | F5  | Compute junctions of the network       |
| Clean Junction          | F6  | Clean junctions of the network         |
| Join selected Junctions | F7  | Join selected junctions of the network |
| Options                 | F10 | Open options dialog                    |

| Locate shortcuts | Key       | Description                          |
| ---------------- | --------- | ------------------------------------ |
| Locate Junctions | Shift + j | Open dialog to locate junctions      |
| Locate Edges     | Shift + e | Open dialog to locate edges          |
| Locate TLS       | Shift + t | Open dialog to locate Traffic Lights |

| Help shortcuts       | Key  | Description                                      |
| -------------------- | ---- | ------------------------------------------------ |
| Online documentation | F1   | Open the online documentation in the web browser |
| About                | F12  | Open the about dialog                            |

| Text edition shortcuts | Key      | Description                                      |
| ---------------------- | -------- | ------------------------------------------------ |
| Cut                    | Ctrl + X | Cut selected text of a text field                |
| Copy                   | Ctrl + C | Copy selected text of a text field               |
| Paste                  | Ctrl + V | Paste previously cut/copied text in a text field |
| Select all text        | Ctrl + A | Select all text in a text field                  |

In addition to these hotkeys, [all hotkeys for moving and zooming in
sumo-gui](sumo-gui.md#keyboard_shortcuts) are supported.

- <Button-Left\>: Execute mode specific action
- <Button-Right\>: Open context-menu
- <Button-Right-Drag\>: Change zoom
- <Button-Left-Drag\>: Move the view around (*panning*)
  - in 'Move'-mode pointing at an edge: move or create geometry points
  - in 'Move'-mode pointing at a junction: move the junction
  - in 'Move'-mode pointing at a selected object: move all selected junctions and edges including geometry. If both junctions of an edge are selected, move the whole geometry. Otherwise, move only the geometry near the cursor
- <ESC\>
  - in 'Create Edge'-mode: clear the currently selected source junction
  - in 'Select'-mode: clear the current selection; cancel rectangle selection
  - in 'Connect'-mode: deselect current lane and cancel all changes
  - in 'Traffic Light'-mode: revert changes to current traffic light
  - when editing a junction shape: abort editing the current junction shape
- <DELETE\>: delete all currently selected items
- <SHIFT\>:
  - in 'Select'-mode: hold <SHIFT\> and drag the mouse for rectangle selection
  - in 'Select'-mode: <SHIFT\>-left-click selects lanes instead of edges
  - in 'Inspect'-mode: <SHIFT\>-left-click inspects lanes instead of edges
  - in 'Delete'-mode: <SHIFT\>-left-click deletes lanes instead of edges
- <CTRL\>:
  - in 'Create Edge'-mode, allow moving the view without defining junctions
  - in 'Create Additional'-mode, allow moving the view without adding additionals
  - in 'POI-Poly'-mode, allow moving the view without adding POIs
- <Enter\>
  - in 'Inspect'-mode: confirm attribute changes
  - in 'Connect'-mode: deselect current lane and save all changes
  - in 'Traffic Light'-mode: save changes to current traffic light
  - in 'TAZ'-mode: save changes to current traffic light
  - when editing a junction shape: save the current junction shape

## Processing Menu Options

- **Compute Junctions** (F5): recomputes the geometry and logic of all junctions. This is triggered automatically when entering modes which require this information (Connect, Traffic Lights).
- **Clean Junctions** (F6): removes all junctions that do not have any adjacent edges (These junctions are not included when saving the network. They are kept in the editor until cleaned for potential reuse).
- **Join Selected Junctions** (F7): joins the selected junctions into a single junction (See [joining junctions](Networks/PlainXML.md#joining_nodes)).
- **Options** (F10): inspect and set all options. These are the same options that [netconvert](netconvert.md) accepts on the commandline or in a configuration.

## Customizing Visualization

Most of the visualization options that are available in [sumo-gui](sumo-gui.md#changing_the_appearancevisualisation_of_the_simulation) can also be set in netedit. This includes the facilities for [loading
background images](sumo-gui.md#using_decals_within_sumo-gui). It is also possible to load [POIs and Polygons](Simulation/Shapes.md) using the menu *File-\>Load Shapes*.

### Background Grid

When activating the background grid via the visualization settings dialog ![![](../images/colorwheel.gif](images/Colorwheel.gif "![](../images/colorwheel.gif") or via the *show grid* option in [Create Edge Mode](#create_edges), all clicks that create or move objects will snap to the nearest grid point. The resolution of the grid can be
customized in the *Background*-Tab of the visualization settings dialog. By using hotkey **Ctrl-PageUp** the grid spacing is doubled and with **Ctrl-PageDown** the grid size is reduce by a factor of 2.

## Terminology

The main network elements that are used to represent a road network in SUMO are [edges, lanes, junctions, connections, traffic
lights](Networks/SUMO_Road_Networks.md). [Additional infrastructure elements](#additional_elements) such as bus stops, detectors and variable speed signs are kept in separate files and
loaded at the start of the simulation. In SUMO the terms **node** and **junction** and **intersection** are used interchangeably (in netedit the term junction is used throughout).

# Netedit usage

- [Edit Modes](Netedit/editModes.md)
  - [Common modes](Netedit/editModesCommon.md)
  - [Network specific modes](Netedit/editModesNetwork.md)
  - [Demand specific modes](Netedit/editModesDemand.md)
  - [Data specific modes](Netedit/editModesData.md)
- [Elements](Netedit/elements.md)
  - [Network elements](Netedit/elementsNetwork.md)
  - [Additional elements](Netedit/elementsAdditional.md)
  - [Shape Elements](Netedit/elementsShape.md)
  - [TAZ elements](Netedit/elementsTAZ.md)
  - [Demand elements](Netedit/elementsDemand.md)
  - [Data elements](Netedit/elementsData.md)
- [Popup-Menu functions](Netedit/neteditPopupFunctions.md)
- [Usage examples](Netedit/neteditUsageExamples.md)

# Planned Features

- Support for editing features already supported by
[netconvert](netconvert.md)
  - Editing `<neigh>` information for [Simulation/OppositeDirectionDriving](Simulation/OppositeDirectionDriving.md).
    

!!! note
        Currently, you may either switching on **--opposites.guess** via the [F10-menu](#processing_menu_options) or set lane attribute 'oppositeID'.
    
- Editing [walkingarea
    shapes](Networks/PlainXML.md#walking_areas)
- Integration of [netgenerate](netgenerate.md)
- Integration of [netdiff](Tools/Net.md#netdiffpy)
- Adding / merging of sub-networks to the existing network
- ...
