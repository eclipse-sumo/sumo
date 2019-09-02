---
title: TraCI/Change Person State
permalink: /TraCI/Change_Person_State/
---

## Command 0xce: Change Person State

|          |           |
| :------: | :-------: |
|  ubyte   |  string   |
| Variable | Person ID |

Changes the state of a Person...

<table>
<caption><strong>Overview Changeable Person Variables</strong></caption>
<thead>
<tr class="header">
<th><p>Variable</p></th>
<th><p>ValueType</p></th>
<th><p>Description</p></th>
<th><p><a href="TraCI/Interfacing_TraCI_from_Python" title="wikilink">Python Method</a></p></th>
</tr>
</thead>
<tbody>
<tr class="odd">
<td><p>add (0x80)</p></td>
<td><p>complex see below</p></td>
<td><p>Inserts a new person to the simulation at the given edge, position and time (in s). This function should be followed by appending Stages or the person will immediately vanish on departure.</p></td>
<td><p><a href="https://sumo.dlr.de/pydoc/traci._person.html#PersonDomain-add">add</a></p></td>
</tr>
<tr class="even">
<td><p>append stage (0xc4)</p></td>
<td><p>complex see below</p></td>
<td><p>Appends a stage (stageObject, waiting, walking or driving) to the plan of the given person.</p></td>
<td><p><a href="https://sumo.dlr.de/pydoc/traci._person.html#PersonDomain-appendStage">appendStage</a> <a href="https://sumo.dlr.de/pydoc/traci._person.html#PersonDomain-appendDrivingStage">appendDrivingStage</a> <a href="https://sumo.dlr.de/pydoc/traci._person.html#PersonDomain-appendWaitingStage">appendWaitingStage</a> <a href="https://sumo.dlr.de/pydoc/traci._person.html#PersonDomain-appendWalkingStage">appendWalkingStage</a></p></td>
</tr>
<tr class="odd">
<td><p>replace stage (0xcd)</p></td>
<td><p>complex see below</p></td>
<td><p>Replaces the nth next stage with the given stage object</p></td>
<td><p><a href="https://sumo.dlr.de/pydoc/traci._person.html#PersonDomain-replaceStage">replaceStage</a></p></td>
</tr>
<tr class="even">
<td><p>remove stage (0xc5)</p></td>
<td><p>int</p></td>
<td><p>Removes the nth next stage. nextStageIndex must be lower then value of getRemainingStages(personID). nextStageIndex 0 immediately aborts the current stage and proceeds to the next stage. When removing all stages, stage 0 should be removed last (the python function removeStages does this automatically).</p></td>
<td><p><a href="https://sumo.dlr.de/pydoc/traci._person.html#PersonDomain-removeStage">removeStage</a><br />
<a href="https://sumo.dlr.de/pydoc/traci._person.html#PersonDomain-removeStages">removeStages</a></p></td>
</tr>
<tr class="odd">
<td><p>reroute (compute new route) by travel time (0x90)</p></td>
<td><p>compound (<empty>)</p></td>
<td><p>Computes a new route to the current destination that minimizes travel time. The assumed values for each edge in the network can be customized in various ways. See <a href="Simulation/Routing#Travel-time_values_for_routing" title="wikilink">Simulation/Routing#Travel-time_values_for_routing</a>. Replaces the current route by the found route.</p></td>
<td><p><a href="https://sumo.dlr.de/pydoc/traci._person.html#PersonDomain-rerouteTraveltime">rerouteTraveltime</a></p></td>
</tr>
<tr class="even">
<td><p>color (0x45)</p></td>
<td><p>ubyte,ubyte,ubyte,ubyte (RGBA)</p></td>
<td><p>sets color for person with the given ID. i.e. (255,0,0,255) for the color red.</p></td>
<td><p><a href="https://sumo.dlr.de/pydoc/traci._person.html#PersonDomain-setColor">setColor</a></p></td>
</tr>
<tr class="odd">
<td><p>height (0xbc)</p></td>
<td><p>double</p></td>
<td><p>Sets the height in m for this person.</p></td>
<td><p><a href="https://sumo.dlr.de/pydoc/traci._person.html#PersonDomain-setHeight">setHeight</a></p></td>
</tr>
<tr class="even">
<td><p>length (0x44)</p></td>
<td><p>double</p></td>
<td><p>Sets the length in m for the given person.</p></td>
<td><p><a href="https://sumo.dlr.de/pydoc/traci._person.html#PersonDomain-setLength">setLength</a></p></td>
</tr>
<tr class="odd">
<td><p>min gap (0x4c)</p></td>
<td><p>double</p></td>
<td><p>Sets the offset (gap to front person if halting) for this vehicle.</p></td>
<td><p><a href="https://sumo.dlr.de/pydoc/traci._person.html#PersonDomain-setMinGap">setMinGap</a></p></td>
</tr>
<tr class="even">
<td><p>speed (0x5e)</p></td>
<td><p>double</p></td>
<td><p>Sets the maximum speed in m/s for the named person for subsequent step.</p></td>
<td><p><a href="https://sumo.dlr.de/pydoc/traci._person.html#PersonDomain-setSpeed">setSpeed</a></p></td>
</tr>
<tr class="odd">
<td><p>type (0x4f)</p></td>
<td><p>string (id)</p></td>
<td><p>Sets the id of the type for the named person.</p></td>
<td><p><a href="https://sumo.dlr.de/pydoc/traci._person.html#PersonDomain-setType">setType</a></p></td>
</tr>
<tr class="even">
<td><p>width (0x4d)</p></td>
<td><p>double</p></td>
<td><p>Sets the width in m for this person.</p></td>
<td><p><a href="https://sumo.dlr.de/pydoc/traci._person.html#PersonDomain-setWidth">setWidth</a></p></td>
</tr>
<tr class="odd">
<td></td>
<td></td>
<td></td>
<td></td>
</tr>
</tbody>
</table>

# compound message details

The message contents are as following:

### add (0x80)

|                       |                 |                     |         |                     |                 |                     |                        |                     |                 |
| :-------------------: | :-------------: | :-----------------: | :-----: | :-----------------: | :-------------: | :-----------------: | :--------------------: | :-----------------: | :-------------: |
|         byte          |     integer     |        byte         | string  |        byte         |     string      |        byte         |         double         |        byte         |     double      |
| value type *compound* | item number (4) | value type *string* | type ID | value type *string* | Initial Edge ID | value type *double* | depart time in seconds | value type *double* | depart position |

A depart time value of -3 is interpreted as immediate departure

### append stage (0xc4)

| byte                | int              | byte           | int        | byte              | string               | byte              | string              | byte              | string                  | byte                  | stringList | byte              | double     | byte              | double | byte              | double | byte              | string                  | byte              | double | byte              | double    | byte              | double     | byte              | string                     |
|---------------------|------------------|----------------|------------|-------------------|----------------------|-------------------|---------------------|-------------------|-------------------------|-----------------------|------------|-------------------|------------|-------------------|--------|-------------------|--------|-------------------|-------------------------|-------------------|--------|-------------------|-----------|-------------------|------------|-------------------|----------------------------|
| value type compound | item number (13) | value type int | stage type | value type string | vType (may be empty) | value type string | line (may be empty) | value type string | destStop (may be empty) | value type stringList | edges      | value type double | travelTime | value type double | cost   | value type double | length | value type string | intended (may be empty) | value type double | depart | value type double | departPos | value type double | arrivalPos | value type string | description (may be empty) |


### append waiting stage (0xc4)

|                       |                 |                  |                |                  |                |                     |                            |                     |                       |
| :-------------------: | :-------------: | :--------------: | :------------: | :--------------: | :------------: | :-----------------: | :------------------------: | :-----------------: | :-------------------: |
|         byte          |       int       |       byte       |      int       |       byte       |      int       |        byte         |           string           |        byte         |        string         |
| value type *compound* | item number (4) | value type *int* | stage type (1) | value type *double* | duration in s | value type *string* | description (may be empty) | value type *string* | stopID (may be empty) |

### append walking stage (0xc4)

|                       |                 |                  |                |                         |            |                     |                  |                  |                                                                                                                      |                     |                                                                                                              |                     |                       |
| :-------------------: | :-------------: | :--------------: | :------------: | :---------------------: | :--------: | :-----------------: | :--------------: | :--------------: | :------------------------------------------------------------------------------------------------------------------: | :-----------------: | :----------------------------------------------------------------------------------------------------------: | :-----------------: | :-------------------: |
|         byte          |       int       |       byte       |      int       |          byte           | stringList |        byte         |      double      |       byte       |                                                         int                                                          |        byte         |                                                    double                                                    |        byte         |        string         |
| value type *compound* | item number (4) | value type *int* | stage type (2) | value type *stringlist* |   edges    | value type *double* | arrival position | value type *double* | duration in s (when a positive value is given, speed is computed from duration and length, otherwise speed is used) | value type *double* | speed (when a positive value is given this speed is used, otherwise the default speed of the person is used) | value type *string* | stopID (may be empty) |

### append driving stage (0xc4)

|                       |                 |                  |                |                     |                     |                     |        |                     |                       |
| :-------------------: | :-------------: | :--------------: | :------------: | :-----------------: | :-----------------: | :-----------------: | :----: | :-----------------: | :-------------------: |
|         byte          |       int       |       byte       |      int       |        byte         |       string        |        byte         | string |        byte         |        string         |
| value type *compound* | item number (4) | value type *int* | stage type (3) | value type *string* | destination edge ID | value type *string* | lines  | value type *string* | stopID (may be empty) |

### replace stage (0xcd)

| byte                | int             | byte           | int               | byte                | int              | byte           | int        | byte              | string               | byte              | string              | byte              | string                  | byte                  | stringList | byte              | double     | byte              | double | byte              | double | byte              | string                  | byte              | double | byte              | double    | byte              | double     | byte              | string                     |
|---------------------|-----------------|----------------|-------------------|---------------------|------------------|----------------|------------|-------------------|----------------------|-------------------|---------------------|-------------------|-------------------------|-----------------------|------------|-------------------|------------|-------------------|--------|-------------------|--------|-------------------|-------------------------|-------------------|--------|-------------------|-----------|-------------------|------------|-------------------|----------------------------|
| value type compound | item number (2) | value type int | replacement index | value type compound | item number (13) | value type int | stage type | value type string | vType (may be empty) | value type string | line (may be empty) | value type string | destStop (may be empty) | value type stringList | edges      | value type double | travelTime | value type double | cost   | value type double | length | value type string | intended (may be empty) | value type double | depart | value type double | departPos | value type double | arrivalPos | value type string | description (may be empty) |