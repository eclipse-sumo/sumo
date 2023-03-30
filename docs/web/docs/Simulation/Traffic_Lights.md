---
title: Traffic Lights
---

Normally, [netconvert](../netconvert.md) and
[netgenerate](../netgenerate.md) generate traffic lights and
programs for junctions during the computation of the networks. Still,
these computed programs quite often differ from those found in reality.
To feed the simulation with real traffic light programs, it is possible
to run [sumo](../sumo.md)/[sumo-gui](../sumo-gui.md) with
additional program definitions. Also,
[sumo](../sumo.md)/[sumo-gui](../sumo-gui.md) allow loading
definitions which describe when and how a set of traffic lights can
switch from one program to another. Both will be discussed in the
following subchapters. Another possibility is to edit traffic light plans
visually in [netedit](../Netedit/index.md#traffic_lights).

# Automatically Generated TLS-Programs

- By default, all traffic lights are generated with a fixed cycle and a
  cycle time of 90s. This can be changed with the option **--tls.cycle.time**. 
  The [osmWebWizard](../Tutorials/OSMWebWizard.md) generates *actuated* traffic lights by default (see below).
- The green time is split equally between the main phases
- All green phases are followed by a yellow phase. The length of the
  yellow phase is computed from the maximum speed of the incoming
  roads but may be customized with the option **--tls.yellow.time**
- If the speeds at the intersection are below the threshold of 70km/h
  (configurable via option **tls.minor-left.max-speed**), left-turns are allowed at the same time
  as oncoming straight traffic but have to yield. This is called
  *green minor* and indicated with a lower-case **g** in the state
  definition. Otherwise, the left-turning stream must use a protected
  left-turn phase (see below). If no such phase can be built because
  there is no dedicated turning lane, *green minor* is allowed anyway
  but a warning is issued.
- If a green phase allows for partially conflicting streams (i.e.
  straight going and left-turning from the opposite direction) and
  there is a dedicated turning lane, it is succeeded by another green
  phase with full priority to the partially conflicted streams (this
  would typically be a left-turning phase). The duration of this phase
  defaults to 6s and can be customized (or disabled) by setting the
  option **--tls.left-green.time**.
- The generated cycle starts at time 0 by default with a green phase for the first main direction 
  (sorted by road priority, lane count and speed). This can be influenced for a specified list of traffic light ids using the options **--tls.half-offset TLS1,TLS2,..** and **--tls.quarter-offset TLS3,TLS4,...**. (shifting the start of the first phase by the indicated fraction of the cycle time).
- In reality there are often phases where all streams have red to
  allow clearing an intersection. SUMO does not build these phases by
  default. To have each green phase preceded by an all-red phase, the
  option **--tls.allred.time** can be used.
- It is also possible to generate
  [Actuated Traffic Lights](#type_actuated)
  by setting the option **--tls.default-type**. This will generated the same signal plans as
  above but with green phases that have a variable length of 5s-50s
  (both values can be set using the options **--tls.min-dur, --tls.max-dur**).
    - default type **actuated**: traffic light actuation is based on gaps measured by automatically generated induction loops
    - default type **delay_based**: actuation is based on vehicle delays
- The generated phase layout can be selected setting option **--tls.layout** to
  *opposites* (default) or *incoming* (see below).
- The generated phase layout is also influenced by the [node
  type](../Networks/PlainXML.md#node_types) which may be either
  *traffic_light* or *traffic_light_right_on_red* (explained
  below)

## Default 4-arm intersection (layout *opposites*)

By default, programs are generated with 4 green phases:

1.  a straight phase
2.  a left-turning phase (only if there is a dedicated left-turn lane)
3.  a straight phase for the direction orthogonal to the first one
4.  a left-turning phase for the direction direction orthogonal to the
    first one (only if there is a dedicated left-turn lane)

Due to the default timings explained above, the green phases usually
have a duration of 31s.

If the node type is set to *traffic_light_right_on_red* rather than
*traffic_light*, then right turns from all sides are permitted in all
phases (after coming to a stop before entering the traffic stream that
currently has the green light.

## Intersection layout *incoming*

- each incoming edge of the intersection gets a separate green phase
where all directions of movement are allowed
- If the node type is set to *traffic_light_right_on_red* rather
than *traffic_light*, then compatible right-turns are allowed
simultaneously

## Intersection layout *alternateOneWay*
This layout can be used to model alternating access to a road section which can only be used by one direction at a time.
To make use of this layout, a [joint traffic light](#controlling_multiple_junctions_with_the_same_controller) must be defined for all junctions that border the restricted section. Alternating green phases are separated by an all-red phase that is long enough to clear the interior section.

## Other Intersections

- If there are more than 4 roads meeting at an intersection,
additional green phases are generated
- If the roads meeting at an intersection have the highest road
priorities on a turning stream, then layout *incoming* may be
generated instead of layout *opposites*.
- If a traffic-light junction does not have any conflicting roads
(i.e. it models a pedestrian crossing) it will not get a red phase
by default. A single red phase may be generated by setting the
option **--tls.red.time**.

## Improving Generated programs with knowledge about traffic demand

### Using actuated traffic lights

To get traffic lights that adapt to demand dynamically, built the
network with option **--tls.default-type actuated**. This will automatically generate [actuated
traffic lights](#actuated_traffic_lights).

!!! note
    If the network was created by [osmWebWizard.py](../Tutorials/OSMWebWizard.md) the traffic lights are 'actuated' by default

To convert an existing .net.xml file so that all traffic lights are actuated,
perform the following steps:

```
netconvert -s orig.net.xml --plain-output-prefix plain
netconvert -e plain.edg.xml -n plain.nod.xml -x plain.con.xml -o updated.net.xml --ignore-errors.edge-type --tls.default-type actuated
```

### Adapting fixed timings (green split) to known traffic

[tlsCycleAdaptation.py](../Tools/tls.md#tlscycleadaptationpy)
can be used to modify green phase durations to accommodate a given
traffic demand.

### Coordinating fixed timings

The tool [tlsCoordinator.py](../Tools/tls.md#tlscoordinatorpy)
can be used to modify program offsets to generated green waves for a
given traffic demand.

# Defining New TLS-Programs

You can load new definitions for traffic lights as a part of an {{AdditionalFile}}. When
loaded, the last program will be used. Switching between programs is
possible via WAUTs and/or TraCI. Also, one can switch between them using
the GUI context menu. A definition of a traffic light program within an {{AdditionalFile}}
looks like this:

```xml
<additional>
  <tlLogic id="0" programID="my_program" offset="0" type="static">
    <phase duration="31" state="GGggrrrrGGggrrrr"/>
    <phase duration="5"  state="yyggrrrryyggrrrr"/>
    <phase duration="6"  state="rrGGrrrrrrGGrrrr"/>
    <phase duration="5"  state="rryyrrrrrryyrrrr"/>
    <phase duration="31" state="rrrrGGggrrrrGGgg"/>
    <phase duration="5"  state="rrrryyggrrrryygg"/>
    <phase duration="6"  state="rrrrrrGGrrrrrrGG"/>
    <phase duration="5"  state="rrrrrryyrrrrrryy"/>
  </tlLogic>
</additional>
```

!!! note
    To get started you can copy the **tlLogic** elements for all traffic lights you wish to edit from a .net.xml file and put them into a new file. Then you only need to change the programID attribute and the program is ready to be modified and loaded

## <tlLogic\> Attributes

The following attributes/elements are used within the tlLogic element:

| Attribute Name | Value Type                            | Description      |
| -------------- | ------------------------------------- | ---------------- |
| **id**         | id (string)                           | The id of the traffic light. This must be an existing traffic light id in the .net.xml file. Typically the id for a traffic light is identical with the junction id. The name may be obtained by right-clicking the red/green bars in front of a controlled intersection. |
| **type**       | enum (static, actuated, delay_based) | The type of the traffic light (fixed phase durations, phase prolongation based on time gaps between vehicles (actuated), or on accumulated time loss of queued vehicles (delay_based) )                                                                                  |
| **programID**  | id (string)                           | The id of the traffic light program; This must be a new program name for the traffic light id. Please note that "off" is reserved, see below.                                                                                                                             |
| **offset**     | int                                   | The initial time offset of the program |

## <phase\> Attributes

Each phase is defined using the following attributes:

| Attribute Name | Value Type            | Description                |
| -------------- | --------------------- | -------------------------- |
| **duration**   | time (int)            | The duration of the phase                                                                                                                                    |
| **state**      | list of signal states | The traffic light states for this phase, see below                                                                                                           |
| minDur         | time (int)            | The minimum duration of the phase when using type **actuated**. Optional, defaults to duration.                                                              |
| maxDur         | time (int)            | The maximum duration of the phase when using type **actuated**. Optional, if minDur is not set it defaults to duration , otherwise to 2147483.                                                              |
| name           | string                | An optional description for the phase. This can be used to establish the correspondence between SUMO-phase-indexing and traffic engineering phase names.     |
| next           | list of phase indices (int ...)           | The next phase in the cycle after the current. This is useful when adding extra transition phases to a traffic light plan which are not part of every cycle. Traffic lights of type 'actuated' can make use of a list of indices for selecting among alternative successor phases. |

!!! caution
    In a SUMO-TLS definition, time is on the vertical axis and each phase describes all signal states that last for a fixed duration. This differs from typical traffic engineering diagrams where time is on the horizontal axis and each row describes the states for one signal. Another crucial difference is, that in SUMO a new phase is introduced whenever at least one signal changes its state. This means that transitions between green phases can be made up of multiple intermediate phases.

## Signal state definitions

Each character within a phase's state describes the state of one signal
of the traffic light. Please note, that a single lane may contain
several signals - for example one for vehicles turning left and one for
vehicles which move straight. This means that a signal does not control
lanes, but links - each connecting a lane which is incoming into a
junction and one which is outgoing from this junction. In SUMO, a
one-to-n dependency between signals and links is implemented, this means
each signal may control more than a single link - though networks
generated by [netconvert](../netconvert.md) or
[netgenerate](../netgenerate.md) usually use one signal per link.
Please note also, that a traffic light may control lanes incoming into
different junctions. The information about which link is controlled by
which traffic light signal may be obtained using the "show link tls
index" option within [sumo-gui](../sumo-gui.md)'s visualization
settings or from the according `linkIndex`
attribute of the `<connection>` elements in the *.net.xml* file.

The following signal colors are used:

| Character | GUI Color                                                  | Description          |
| --------- | ---------------------------------------------------------- | -------------------- |
| r         | <span style="color:#FF0000; background:#FF0000">FOO</span> | 'red light' for a signal - vehicles must stop     |
| y         | <span style="color:#FFFF00; background:#FFFF00">FOO</span> | 'amber (yellow) light' for a signal - vehicles will start to decelerate if far away from the junction, otherwise they pass    |
| g         | <span style="color:#00B300; background:#00B300">FOO</span> | 'green light' for a signal, no priority - vehicles may pass the junction if no vehicle uses a higher priorised foe stream, otherwise they decelerate for letting it pass. They always decelerate on approach until they are within the configured [visibility distance](../Networks/PlainXML.md#explicitly_setting_which_edge_lane_is_connected_to_which) |
| G         | <span style="color:#00FF00; background:#00FF00">FOO</span> | 'green light' for a signal, priority - vehicles may pass the junction                                                                                                                                                                                                                                                                                                                                 |
| s         | <span style="color:#800080; background:#800080">FOO</span> | 'green right-turn arrow' requires stopping - vehicles may pass the junction if no vehicle uses a higher priorised foe stream. They always stop before passing. This is only generated for junction type *traffic_light_right_on_red*.                                                                                                                                                             |
| u         | <span style="color:#FF8000; background:#FF8000">FOO</span> | 'red+yellow light' for a signal, may be used to indicate upcoming green phase but vehicles may not drive yet (shown as orange in the gui)                                                                                                                                                                                                                                                             |
| o         | <span style="color:#804000; background:#804000">FOO</span> | 'off - blinking' signal is switched off, blinking light indicates vehicles have to yield                                                                                                                                                                                                                                                                                                              |
| O         | <span style="color:#00FFFF; background:#00FFFF">FOO</span> | 'off - no signal' signal is switched off, vehicles have the right of way|


![traci_tutorial_tls.png](../images/Traci_tutorial_tls.png
"traci_tutorial_tls.png")

Example: traffic light with the current state
**"GrGr"**. The leftmost letter "G" encodes the green light for link 0,
followed by red for link 1, green for link 2 and red for link 3. The
link numbers are enabled via [sumo-gui settings](../sumo-gui.md) by
activating *show link tls index*.

### Default link indices

For traffic lights that control a single intersection, the default
[indices generated generated by
netconvert](../Networks/SUMO_Road_Networks.md#indices_of_a_connection)
are numbered in a clockwise pattern starting with 0 at 12 o'clock with
right-turns ordered before straight connections and left turns.
[Pedestrian
crossings](../Networks/PlainXML.md#pedestrian_crossings) are always
assigned at the very end, also in a clockwise manner.

If traffic lights are joined so that a single program controls multiple
intersections, the ordering for each intersection stays the same but the
indices are increased according the order of the controlled junctions in
the input file.

### Accessing indices programmatically

TLS Link indices can be access using either

- [sumolib](../Tools/Sumolib.md) using
  [tls.getConnections()](http://sumo.dlr.de/daily/pydoc/sumolib.net.html#TLS)
- or [TraCI](../TraCI.md) using
  [traci.trafficlight.getControlledLinks()](../TraCI/Traffic_Lights_Value_Retrieval.md#structure_of_compound_object_controlled_links)

## Viewing TLS-Programs graphically

In the sumo-gui you can right-click on a red/green stop bar and select *show
phases*.

## Interaction between signal plans and right-of-way rules

Every traffic light intersection has the right-of-way rules of a
priority intersection underneath of it. This becomes obvious when
switching a traffic light off (either in [sumo-gui](../sumo-gui.md)
with a right-click on the traffic light or by [loading the "off"
program](#loading_a_new_program)).

The right-of-way rules of this priority intersection come into play when
defining signal plans with simultaneous conflicting green streams (by
using *g* and *G* states). These signal plans only work correctly if the
right-of-way rules force vehicles from the *g* stream to yield to the
*G* stream. In most cases, this relationship was set correctly by the
default signal plan.

However, when introducing new *g*/*G* relationships, correctness is only
ensured by loading the network and the new signal plan into
[netconvert](../netconvert.md) and thus updating the right-of-way
rules.

# Traffic Lights that respond to traffic

Generally, the designation "actuated" refers to traffic lights that switch in response to traffic (or lack thereof). The different controllers and their features are described below. SUMO supports several algorithms with this property and they are described below.
!!! note
    [Mesoscopic simulation](Meso.md) does not support actuated traffic lights.

## Type 'actuated'

SUMO supports *gap-based* actuated traffic control. This control scheme
is common in Germany and works by prolonging traffic phases whenever a
continuous stream of traffic is detected. It switches to the next phase
after detecting a sufficient time gap between successive vehicles. This
allows for better distribution of green-time among phases and also
affects cycle duration in response to dynamic traffic conditions.

To use this type of control, the `<tlLogic>`-element needs to receive the attribute
`type="actuated"`. It also requires the usage of the `phase`-attributes `minDur` and `maxDur` instead of `duration` to
define the allowed range of time durations for each phase (if these
values are equal or only `duration` is given, that phase will have constant
duration). Additional parameters may be used to configure the control
algorithm further. These may be given within the `<tlLogic>`-Element as follows:

### Detectors
The time gaps which determine the phase extensions are collected by induction loop detectors.
These detectors are placed automatically at a configurable distance (see below). If the incoming lanes are too short and there is a sequence of unique predecessor lanes, the detector will be placed on a predecessor lane at the computed distance instead.

Each lane incoming to the traffic light will receive a detector. However, not all detectors can be used in all phases.
In the current implementation, detectors for actuation are only used if all connections from the detector lane gets the unconditional green light ('G') in a particular phase. This is done to prevent useless phase extensions when the first vehicle on a given lane is not allowed to drive.
A simple fix is often the provide dedicate left turn lanes.

!!! note
    When setting option **--tls.actuated.jam-threshold** to a value > 0 (i.e. 30), all detectors will be usable, because useless phase extension is automatically avoided if a detector is found to be jammed. Alternatively, this can be configured for individual tls or even individual lanes using `<param>`.

The detector names take the form `TLSID_PROGRAMID_EDGEINDEX.LANEINDEX` where

- **TLSID** is the id of the tlLogic element
- **PROGRAMID** is the value attribute 'programID'
- **EDGEINDEX** is a running index that starts at 0 for edge that approaches tls linkIndex 0 (typically the northern approach)
- **LANEINDEX** is a running index for the current edge that starts at the first vehicular lane (sidewalks do not count)

!!! note
    Sumo will issue a warning of the form "... has no controlling detector" if a phase or link index does not have usable detectors.

Detector activation states can optionally be written to the [TLS output](Output/Traffic_Lights.md#optional_output).

### Example

```xml
<tlLogic id="0" programID="my_program" offset="0" type="actuated">
  <param key="max-gap" value="3.0"/>
  <param key="detector-gap" value="2.0"/>
  <param key="passing-time" value="2.0"/>
  <param key="vTypes" value=""/>
  <param key="show-detectors" value="false"/>
  <param key="file" value="NULL"/>
  <param key="freq" value="300"/>
  <param key="jam-threshold" value="-1"/>
  <param key="detector-length" value="0"/>  

  <phase duration="31" minDur="5" maxDur="45" state="GGggrrrrGGggrrrr"/>
  ...
</tlLogic>
```
### Parameters
Several optional parameters can be used to control the behavior of actuated traffic lights. The examples values in the previous section are the default values for these parameters and their meaning is given below:

- **max-gap**: the maximum time gap between successive vehicles that will cause the current phase to be prolonged
(within maxDur limit)
- **detector-gap**: determines the time distance between the (automatically generated) detector and the stop line in seconds (at
each lanes maximum speed). 
- **passing-time**: estimates the headway between vehicles when passing the stop line. This sets an upper bound on the distance between detector and stop line according to the formula `(minDur / passingTime + 0.5) * 7.5`. The intent of this bound is to allow all vehicles between the detector and the stop line to pass the intersection within the minDur time. A warning will be issued if the minDur gives insufficient clearing time.
- **linkMaxDur:X** (where X is a traffic light index): This sets an additional maximum duration criterion based on individual signals green duration rather than phase duration.
- **linkMinDur:X** (where X is a traffic light index): This sets an additional minimum duration criterion based on individual signals green duration rather than phase duration.
- **show-detectors** controls whether generated detectors will be visible or hidden in [sumo-gui](../sumo-gui.md). The default for all traffic lights can be set with option **--tls.actuated.show-detectors**. It is also possible to toggle this value from within the GUI by right-clicking on a traffic light.
- parameters **vTypes**, **file** and **freq** have the same meaning as for [regular
induction loop detectors](../Simulation/Output/Induction_Loops_Detectors_(E1).md).
- **coordinated** (true/false) Influence there reference point for time-in-cycle when using [coordination](#coordination)
- **cycleTime** sets the cycle time (in s) when using [coordination](#coordination). Defaults to the sum of all phase 'durations' values.
- **jam-threshold**: ignore detected vehicles if they have stood on a detector for the given time or more (activated by setting a position value)
- **detector-length**: set detector length to the given value (to ensure robust request detection with varying gaps and vehicle positions)

Some parameters are only used when a signal plan with [dynamic phase selection](#dynamic_phase_selection_phase_skipping) is active:

- **inactive-threshold** (default 180): The parameter sets the time in s after which an inactive phase will be entered preferentially.
- **linkMinDur:X** (where X is a traffic light index): This sets an additional minimum duration criterion based on individual signals rather than phase duration

### Visualization
By setting the sumo option **--tls.actuated.show-detectors** the default visibility of detectors can be
set. Also, in [sumo-gui](../sumo-gui.md) detectors can be
shown/hidden by right-clicking on an actuated traffic light and
selecting the corresponding menu entry.

The detectors used by an actuated traffic light will be colored to indicate their status:
- green color indicates that the detector is used to determine the length of the current phase
- white color indicates that the detector is not used in the current phase
- red color indicates that a vehicle was detected since the last time at which the controlled links at that lane had a green light (only if these links are currently red)
- magenta color indicates and active [override](../sumo-gui.md#activating_detectors)


### Custom Detectors
To use custom detectors (i.e. for custom placement or output) additional parameters can be defined where KEY is a lane that is incoming to the traffic light and VALUE is a user-defined inductionLoop (that could also lie on another upstream lane).
```xml
   <param key="gneE42_2" value="customDetector1"/>
```

By assigning the special value `NO_DETECTOR`, the detector for a given lane key can be completely disabled.

!!! caution
    Custom detectors only work when the 'tlLogic' is loaded from an additional file.
    
Custom detector activation states can optionally be written to the [TLS output](Output/Traffic_Lights.md#optional_output).

To include further detectors in the output and in the [phase tracker dialog](#track_phases) (i.e. when a custom logic uses laneArea detectors or multiple detectors on the same lane) the following declaration can be used to list all extra detectors:

```xml
   <param key="extra-detectors" value="customDetector1 customDetector2 ..."/>
```
    
### Lane-specific detector settings 

To define a max-gap value that differs from the default you can use a param with `key="max-gap:<LANE_ID>"` where LANE_ID is a lane incoming to the traffic light (the detector might lie further upstream).
```xml
   <param key="max-gap:gneE42_2" value="2"/>
```

In the same way, a custom jam-threshold or detector-length may be set:

```xml
   <param key="jam-threshold:LANE_ID" value="5"/>
   <param key="detector-length:LANE_ID" value="2.5"/>
```

### Coordination

Actuated phases (minDur != maxDur) can be coordinated by adding attributes 'earliestEnd' and 'latestEnd'.
If these values are used, each step in the traffic light plan is assigned a 'timeInCycle' value depending on the value of param 'coordinated' (default 'false').

- coordinated=true:  timeInCycle = *(simulationTime - offset) % cycleTime*  (where cycleTime is taken from the param with key=cycleTime)
- coordinated=false: timeInCycle = *time since last switching into phase 0*

If 'earliestEnd' is set, a phase can not end while *timeInCycle < earliestEnd* (effectively increasing minDur)
If 'latestEnd' is set, a phase cannot be prolonged when *timeInCycle = latestEnd* (effectively reducing maxDur).

When setting 'latestEnd' < 'earliestEnd', the phase can be extended into the next cycle.
If both values are defined and a phase has already started and ended in the
current cycle, both values will be shifted into the next cycle to avoid having a
phase run more than once in the same cycle (this only happens when param
'coordinated' is set to 'true').

```xml
<tlLogic id="0" programID="my_program" offset="10" type="actuated">
  <param key="coordinated" value="true"/>
  <param key="cycleTime" value="60"/>

  <phase duration="31" minDur="5" maxDur="45" state="GGggrrrrGGggrrrr" earliestEnd="10" latestEnd="50"/>
  ...
</tlLogic>
```

### Dynamic Phase Selection (Phase Skipping)
When a phase uses attribute 'next' with a list of indices. The next phase is chosen dynamically based on the detector status of all candidate phases according to the following algorithm:

- compute the priority for each phase given in 'next'. Priority is primarily given by the number of active detectors for that phase. Active means either of:
  - with detection gap below threshold
  - with a detection since the last time where the signal after this detector was green
- the current phase is implicitly available for continuation as long as its maxDur is not reached. Detectors of the current phase get a bonus priority
- the phase with the highest priority is used with phases coming earlier in the next list given precedence over those coming later
- if there is no traffic, the phases will run through a default cycle defined by the first value in the 'next' attribute
- if a particular phase should remain active indefinitely in the absence of traffic it must have its own index in the 'next' list as well as a high maxDur value
- if an active detector was not served for a given time threshold (param **inactive-threshold**), this detector receives bonus priority according the time it was not served. This can be used to prevent starvation if other phases are consistently preferred due to serving more traffic 

Examples for this type of traffic light logic can be found in [{{SUMO}}/tests/sumo/tls/actuated/multiNext/dualring_simple]({{Source}}tests/sumo/tls/actuated/multiNext/dualring_simple).

The helper script [buildTransitions.py](../Tools/tls.md#buildtransitionspy) can be used to generate such logics from simplified definitions.

## Type 'actuated' with custom switching rules

By default, all traffic light programs are governed by the same pre-defined rules
that determine which detectors are used or ignored in each phase. If more
flexibility is needed, custom conditions can be defined by using the phase
attributes 'earlyTarget' and 'finalTarget' to define logical expressions.

If the controller is in an actuated phase (minDur < maxDur) and could switch
into a new phase, the attribute 'earlyTarget' of the new phase is evaluated. If
the expression evaluates to 'true', the controller switches into the new phase.
Otherwise it remains in the current phase. If the current phase has multiple successors (attribute 'next'),
the candidates are evaluated from left to right and the first candidate where 'earlyTarget' evaluates to true is used.

If the controller has reached the maximum duration of its current phase and
multiple successor phases are defined with attribute 'next', the
attribute 'finalTarget' of all candidate phases is evaluated from left to right.
The first phase where the expression evaluates to 'true' is used.
Otherwise, the rightmost phase in the next-list is used.

The following elements are permitted in an expression for attributes
'earlyTarget' and 'finalTarget':

- numbers
- comparators >,=,>=
- mathematical operators +,-,*,/,**,%
- logical operators 'or', 'and', '!'
- parentheses (,)
- pre-defined functions:
  - 'z:DETID': returns the time gap since the last vehicle detection for inductionLoop detector with id 'DETID' or id 'TLSID_PROGRAMID_DETID' (DETID may omit the the [prefix 'TLSID_PROGRAMID_'](#detectors))
  - 'a:DETID': returns number of vehicles on detector with id 'DETID'. Supports inductionLoop and laneAreaDetectors. Also supports omitting the prefix of the detector id. (see 'z:')
  - 'g:TLSINDEX': returns current green duration in seconds for link with the given index
  - 'r:TLSINDEX': returns current red duration in seconds for link with the given index
  - 'c:': returns the time within the current cycle
- [use-defined functions](#custom_function_definitions) FNAME:arg1,args2,...,argN  where arg may be any expression that does not contain spaces (except within parentheses)
- Symbolic names for [pre-defined expressions](#named_expressions)

The following constraints apply to expressions:

- all elements of an expression must be separated by a space character (' ')
  with the exception of the operator '!' (logical negation) which must precede its operand without a space.
  
!!! note
    The comparators '<' and '<=' are also supported but must be written as xml-entities `&lt;` and `&lt;=` respectively.

### Named Expressions

To organize expressions, the element `<condition>` may be used as a child element
of `<tlLogic>` to define named expressions that can be referenced in other expressions:

```xml
<tlLogic id="example" type="actuated" ...>
   <condition id="C3" value="z:det5 > 5"/>
   <condition id="C4" value="C3 and z:det6 < 2"/>
   <condition id="C5" value="g:3 > 20"/>
   ...
```

- **id** must be an alphanumeric string without spaces and without the ':'-character
- **value** may be any expression which is permitted for 'earlyTarget' or 'finalTarget'

Condition values can be [visualized](Traffic_Lights.md#track_phases) while the simulation is running. By default all conditions are listed (if the corresponding visualization option is active). If many conditions are defined, it may be useful to list only a subset in the tracker window. For this purpose *either* one of the following `<param>`-definitions may be used as child element of the `<tlLogic>`:

- `<param key="show-conditions" value="C1 C4"/>`: shows only the conditions with listed id.
- `<param key="hide-conditions" value="C3 C4"/>`: shows only the conditions which are not listed

The values of (visible) named expressions can optionally be written to the [TLS output](Output/Traffic_Lights.md#optional_output).

### Examples

#### Diverse Logical Conditions

```xml
<tlLogic id="example" type="actuated" ...>
   <condition id="C3" value="z:Det2.0 > 5"/>
   <condition id="C4" value="C3 and z:Det0.0 < 2"/>
   <condition id="C5" value="r:0 > 60"/>
   <phase ... next="1 2"/>
   <phase ... earlyTarget="C3" finalTarget="!C4"/>
   <phase ... earlyTarget="(z:D2.0 > 3) and (z:D3.1 <= 4)" finalTarget="C5 or (z:Det3.1 = 0)"/>
</tlLogic>
```

#### Default Gap Control Logic

The default gap control logic, replicated with custom conditions. A complete scenario including network and detector definitions can be downloaded [here](https://sumo.dlr.de/extractTest.php?path=sumo/basic/tls/actuated/conditions/replicate_default):

```xml
<tlLogic id="C" type="actuated" programID="P1" offset="0">
        <phase duration="33" state="GgrrGgrr" minDur="5" maxDur="60" />
        <phase duration="3"  state="ygrrygrr" earlyTarget="NS"/>
        <phase duration="6"  state="rGrrrGrr" minDur="5" maxDur="60" />
        <phase duration="3"  state="ryrrryrr" earlyTarget="NSL"/>
        <phase duration="33" state="rrGgrrGg" minDur="5" maxDur="60" />
        <phase duration="3"  state="rrygrryg" earlyTarget="EW"/>
        <phase duration="6"  state="rrrGrrrG" minDur="5" maxDur="60" />
        <phase duration="3"  state="rrryrrry" earlyTarget="EWL"/>

        <condition id="NS"  value="z:D0.0 > 3 and z:D2.0 > 3"/>
        <condition id="NSL" value="z:D0.1 > 3 and z:D2.1 > 3"/>
        <condition id="EW"  value="z:D1.0 > 3 and z:D3.0 > 3"/>
        <condition id="EWL" value="z:D1.1 > 3 and z:D3.1 > 3"/>
    </tlLogic>
```

!!! note
    The the expression 'z:D0.0' retrieves the detection gap of detector 'C_PI_D0.0' but the prefix 'C_PI_' may be omitted.

#### Bus prioritization

```xml
    <!-- detect only buses -->
    <inductionLoop id="dBus" lane="SC_0" pos="-90" vTypes="busType" file="NUL"/>

    <tlLogic id="C" type="actuated" programID="P1" offset="0">
        <phase duration="33" state="GgrrGgrr" minDur="5" maxDur="60" />
        <phase duration="3"  state="ygrrygrr"/>
        <phase duration="6"  state="rGrrrGrr" minDur="5" maxDur="60" />
        <phase duration="3"  state="ryrrryrr"/>
        <phase duration="33" state="rrGgrrGg" minDur="5" maxDur="60" />
        <phase duration="3"  state="rrygrryg" earlyTarget="EW or NSbus"/>
        <phase duration="6"  state="rrrGrrrG" minDur="5" maxDur="60" />
        <phase duration="3"  state="rrryrrry"/>

        <!-- the default switching rules (prolong phase depending on observed gaps) -->
        <condition id="EW" value="z:D1.0 > 3 and z:D3.0 > 3"/>
        <!-- prioritization for buses coming from the south -->
        <condition id="NSbus" value="3 > z:dBus"/>
    </tlLogic>
```

### Overriding Phase Attributes with Expressions

By default, the phase attributes 'minDur', 'maxDur', 'earliestEnd' and 'latestEnd' are defined numerically (or left undefined).
It may be desireable to redefine these attributes with expressions (i.e. condition ids or condition values) for the following reasons:

- the switching logic may be expressed more succinctly if these values can change dynamically during the signals operation
- the phase definitions shall be reused for multiple programs and all variability shall be expressed in table of constants (defined via `<conditions>`s)

To override these attributes, their value in the `<phase>` must be defined as `-1`. For each phase and attribute a corresponding condition must be defined with the `id = <ATTRNAME>:<PHASEINDEX>` as in the example below:

```xml
<tlLogic id="C" type="actuated" programID="P1" offset="0">
        <phase duration="33" state="GgrrGgrr" minDur="10" maxDur="65" name="NS"/>
        <phase duration="3"  state="ygrrygrr" earlyTarget="NS"/>
        <phase duration="6"  state="rGrrrGrr" minDur="10" maxDur="65" name="NSL"/>
        <phase duration="3"  state="ryrrryrr" earlyTarget="NSL"/>
        <phase duration="33" state="rrGgrrGg" minDur="-1" maxDur="-1" name="EW" earliestEnd="-1" latestEnd="-1"/>
        <phase duration="3"  state="rrygrryg" earlyTarget="EW"/>
        <phase duration="6"  state="rrrGrrrG" minDur="10" maxDur="65" name="EWL"/>
        <phase duration="3"  state="rrryrrry" earlyTarget="EWL"/>

        <condition id="minDur:4"      value="10"/>
        <condition id="maxDur:4"      value="65"/>
        <condition id="earliestEnd:4" value="60"/>
        <condition id="latestEnd:4"   value="80"/>

    </tlLogic>
```

### Storing and modifying custom data

The `<condition>` elements described above can be used to define complex expressions as well as numerical constants that control program operation.
It may sometimes be useful to store and modify numerical values that persist over consecutive invocations of the control logic. To this end the element `<assignment>`  may be used as a child element of `<tlLogic>` to define conditional assignment of new values to [named expressions](#named_expressions):

```xml
<tlLogic id="example" type="actuated" ...>

        <condition id="NS" value="0"/>
        <condition id="nSw" value="0"/>

        <assignment id="nSw" check="1" value="nSw + 1"/>
        <assignment id="NS"  check="1" value="0"/>    
        <assignment id="NS"  check="z:D0.0 > 3 and z:D2.0 > 3" value="1"/>
   ...
</tlLogic>
```

- **id** may be any alphanumeric id
- **check** may be any expression which is permitted for condition values
- **value** may be any expression which is permitted for conditions values

Every time the control logic is executed, all `assignment`s are executed in the order they are defined: If the the 'check'-expression evaluates to true (a non-0 value), the 'value'-expression is evaluated and the result is stored under the given id:

- if **id** is the id of a condition element, the value of that conditions is replaced by a string representation of the result (The accuracy of this representation is limited by simulation option **--precision**)
- if **id** is not the id of a condition element, a double valued variable with that id is created / updated in the current scope. If the assignment is not part of a [use-defined functions](#custom_function_definitions), this is the global scope

The test case [find_primes](https://sumo.dlr.de/extractTest.php?path=sumo/basic/tls/actuated/conditions/assignments/find_primes) computes all prime numbers below 100 inside the traffic light controller as a capability demonstration.

### Custom function definitions

Custom functions are a mechanism that allows to execute multiple assignments with custom arguments.
They are defined with the `<function>` element within a `<tlLogic>` as shown below:

```xml
<tlLogic id="example" type="actuated" ...>

  <function id="FNAME" nArgs="2">
     <assignment id="COND1" check="1" value="$1 + $2"/>
     <assignment id="$0" check="1" value="COND1 * COND1"/>
  </function>

  <condition id="COND2" value="FNAME:3,4"/>
   ...
</tlLogic>
```

- **id** may be any alphanumeric string
- **nArgs** is the number of arguments required by the function
- **$0** is the value returned by the function
- **$1 ... $n** are the values of the functions arguments in the order they are supplied after the **:**
- when a function is evaluted, all its assignments are evaluted in definition order
- functions may not assign to any defined `<condition>` id
- assignments are local to the function
- a function call takes the form **id:arg_1,arg_2,...arg_n** and there must be no spaces between the arguments and the commas (except within parentheses)
- a function is evaluated within the calling scope (ids assigned to by a function are accessible in a nested function call but assignments are not propagated back to the caller)

in the above example COND2 receives a value of 49

## Type 'delay_based'

Similar to the control by time gaps between vehicles, a phase
prolongation can also be triggered by the presence of vehicles with time
loss. A TLS with this actuation type can be defined as follows:

```xml
<tlLogic id="0" programID="my_program" offset="0" type="delay_based">
  <param key="detectorRange" value="100" />
  <param key="minTimeLoss" value="1" />
  <param key="file" value="NULL"/>
  <param key="freq" value="300"/>
  <param key="show-detectors" value="false"/>
  <param key="extendMaxDur" value="false"/>

  <phase duration="31" minDur="5" maxDur="45" state="GGggrrrrGGggrrrr"/>
  ...
</tlLogic>
```

Here, the `detectorRange` specifies the upstream detection range in meters measured
from the stop line. Per default (if the parameter is left undefined) the
underlying [E2
detector](../Simulation/Output/Lanearea_Detectors_(E2).md) is
assumed to cover the first approaching lanes completely. The time loss
for a vehicle is accumulated as soon as it enters the detector range. If
its accumulated time loss exceeds the value of `minTimeLoss` (current default is one
second) seconds a prolongation of the corresponding green phase is
requested if it is active. The instantaneous time loss of a vehicle is
defined as `1 - v/v_max`, where `v` is its current velocity and `v_max`
the allowed maximal velocity. See \[Oertel, Robert, and Peter Wagner.
"Delay-time actuated traffic signal control for an isolated
intersection." Transportation Research Board 2011 (90th Annual Meeting).
2011.\] for details.

### Parameters
Several optional parameters can be used to control the behavior of delay_based traffic lights. The example values in the previous section are the default values for these parameters and their meaning is given below:

- **detector-range**: the upstream detection range in meters measured from the stop line
- **minTimeLoss**: the minimum timeLoss of a vehicle (in s) that triggers phase prolongation
- **extendMaxDur**: whether phases may be prolonged beyond `maxDur` in the absence of traffic in other arms (this was the default behavior until 1.16.0)
- **show-detectors** controls whether generated detectors will be visible or hidden in [sumo-gui](../sumo-gui.md). It is also possible to toggle this value from within the GUI by right-clicking on a traffic light.
- parameters **vTypes**, **file** and **freq** have the same meaning as for [regular
lane area detectors](Output/Lanearea_Detectors_(E2).md).

### Custom Detectors
To use custom detectors (i.e. for custom placement or output) additional parameters can be defined where KEY is a lane that is incoming to the traffic light and VALUE is a user-defined laneAreaDetector.
```xml
   <param key="gneE42_2" value="customDetector1"/>
```
!!! caution
    Custom detectors only work when the 'tlLogic' is loaded from an additional file.

## Type 'NEMA'

Since version 1.11.0, SUMO supports defining controllers with the naming
convention and control logic according to the 'National Electrical Manufacturers
Association' commonly used throughout the United States.
Detailed documentation is at [NEMA](NEMA.md).

### Custom Detectors
To use custom detectors (i.e. for custom placement or output) additional parameters can be defined where KEY is a lane that is incoming to the traffic light and VALUE is a user-defined laneAreaDetector.
```xml
   <param key="gneE42_2" value="customDetector1"/>
```
!!! caution
    Custom detectors only work when the 'tlLogic' is loaded from an additional file.

Detector activation states (for default and custom detectors) can optionally be written to the [TLS output](Output/Traffic_Lights.md#optional_output).

# Loading a new Program

After having defined a tls program as above, it can be loaded as an {{AdditionalFile}}; of
course, a single {{AdditionalFile}} may contain several programs. It is possible to load
several programs for a single tls into the simulation. The program
loaded last will be used (unless not defined differently using a WAUT
description). All subkeys of the additional programs must differ if they
describe the same tls.

Assuming the program as defined above is put in a file called
*tls.add.xml* it can be loaded in
[sumo](../sumo.md)/[sumo-gui](../sumo-gui.md) like this

```
sumo -a tls.add.xml ...<other options for network and routes>
```

## Switching TLS 'off'

It is also possible to load a program which switches the tls off by
giving the `programID` the value
"`off`".

```xml
<tlLogic id="0" type="static" programID="off"/>
```

!!! note: The 'off' program can always be used from [TraCI](#switching_between_pre-defined_programs).

An alternative way to switch all traffic lights to the 'off' program is to set sumo option **--tls.all-off**.

### Default behavior when 'off'

Once a traffic light has been switched off, its lights will change to to values of `O` (off, no signal) and `o` (off, blinking) and it will behave like a priority intersection. The connections (links) with state `O` will have priority whereas the connections `o` will yield.

The rules for configuring the priority direction in the off-state are the same as for [an intersection without a traffic light](../Networks/PlainXML.md#right-of-way).

### All-way-stop when 'off'

By setting junction (node) attribute `rightOfWay="allwayStop"`, when building / editing the network file, the behavior when switched off will correspond to that of junction type `allway_stop`.

!!! note
    Traffic lights with `tlType="NEMA"` will default to `allwayStop` behavior when switched off.  To change this, the attribute `rightOfWay="mixedPriority"` can be used.

# Tools for Importing TLS Programs

Description from real-world traffic light systems do not arrive in form
of SUMO-traffic light descriptions normally. The main difference in
presentation comes from the fact that SUMO defines a new `<phase>` whenever any
of the controlled signals changes its state, whereas traffic engineers
differentiate between phases and phase transitions.

For an easier import than editing the XML by hand, some tools exists in
{{SUMO}}/tools/tls.

- [tls_csv2SUMO.py](../Tools/tls.md#tls_csv2sumopy): this tool
  simplifies descriptions because it allows to define the duration of
  phases for each controlled edge-to-edge connection. Splitting into
  smaller SUMO-phases because other signals change is done
  automatically.
- [tls_csvSignalGroup.py](../Tools/tls.md#tls_csvsignalgrouppy):
  this tool further simplifies descriptions because it allows to
  define the start and end times of green-phases per signal group (up
  to 2 green phases, actually) and the transitions (yellow,
  red-yellow) are added automatically. The splitting into smaller
  SUMO-phases is also done automatically.

Alternatively [netedit](../Netedit/index.md) can be used to edit programs
using a graphical user interface.

# Modifying Existing TLS-Programs

To modify the program of a traffic light it is generally necessary to
[load a new program](#loading_new_tls-programs). However, in
the special case that only the offset shall be modified it is also
possible to specify a new offset for an existing traffic light id and
programID:

```xml
<additional>
  <tlLogic id="0" programID="0" offset="42"/>
</additional>
```

# Controlling multiple Junctions with the same controller

In SUMO, a traffic light controller can control an arbitrary number of
junctions simultaneously. This is accomplished by one of the following
methods:

- Defining the same *tl* attribute (the controller ID) for a set of
  nodes in *.nod.xml* file when building the network with
  [netconvert](../netconvert.md)
- Setting the same *tl* attribute for multiple nodes in
  [netedit](../Netedit/index.md)
- Setting the option **--tls.join** when building the network with
  [netconvert](../netconvert.md). This will automatically join
  the traffic lights of nodes in close proximity within a default
  distance of 20m (customizable by setting option **tls.join-dist** {{DT_FLOAT}}).

Note, that in this case the state vector for each phase will be as long
as the total number of controlled intersections. Also, the tls indices
for the connections will differ from the link indices (as the latter
starts with 0 for each intersection whereas the tls indices are unique
within a controller).

!!! caution
    The generated TLS program will attempt to identify the main directions but usually this needs some manual corrections.

# Defining Signal Groups

In SUMO every lane-to-lane connection at a controlled intersection is
assigned an index called *link tls index*. This index is used to
determine the state for each phase by looking up the character code at
that index in the state vector. The link index can be shown in
[sumo-gui](../sumo-gui.md) by setting the junction visualization
option *show link tls index*. By default link indices are unique for
each connection and assigned in a clockwise manner starting at the north
(the same as the junction link index which is used for defining
right-of-way rules). When [defining joint
TLS](#controlling_multiple_junctions_with_the_same_controller),
the indices continue their numbering in the order of the controlled
junctions.

The tls index for each connection can be [freely assigned in a
*.tll.file*](../Networks/PlainXML.md#controlled_connections)
or by setting attribute *linkIndex* in [netedit](../Netedit/index.md). By
assigning the **same** index to multiple connection they form a signal
group and always show the same state (simply because they reference the
same state index). This allows shortening and thus simplifying the state
vector.

## Automatic Creation of Signal Groups

[netconvert](../netconvert.md) supports automatic definition of signal groups by setting option **--tls.group-signals**.
To replace existing signal groups with a 1-to-1 assignment of connections to indices, the option **--tls.ungroup-signals** can be used.

[netedit](../Netedit/index.md) also supports creation and removal of signal groups using the functions 'Group Signals' and 'Ungroup Signals' in traffic light mode frame.

# Defining Program Switch Times and Procedure

In practice, a tls often uses different programs during a day and maybe
also for weekdays and for the weekend days. It is possible to load a
definition of switch times between the programs using a WAUT (short for
"Wochenschaltautomatik" \~ weekly switch automatism).

Given a tls which knows four programs - two for weekdays and two for
weekend days where from 22:00 till 6:00 the night plan shall be used and
from 6:00 till 22:00 the day plan, and already defined programs, named
"weekday_night", "weekday_day", "weekend_night", "weekend_day". To
describe the switch process, we have to describe the switch at first,
assuming our simulation runs from monday 0.00 (second 0) to monday 0.00
(second 604800):

```xml
<WAUT refTime="0" id="myWAUT" startProg="weekday_night">
    <wautSwitch time="21600" to="weekday_day"/>    <!-- monday, 6.00 -->
    <wautSwitch time="79200" to="weekday_night"/>  <!-- monday, 22.00 -->
    <wautSwitch time="108000" to="weekday_day"/>   <!-- tuesday, 6.00 -->
... further weekdays ...
    <wautSwitch time="453600" to="weekend_day"/>   <!-- saturday, 6.00 -->
... the weekend days ...
</WAUT>
```

The fields in WAUT have the following meanings:

| Attribute Name | Value Type | Description                                                                                      |
| -------------- | ---------- | ------------------------------------------------------------------------------------------------ |
| **id**         | string id  | The name of the defined WAUT                                   |
| **startProg**  | string id  | The program that will be used at the simulation's begin     |
| refTime   | time   | A reference time which is used as offset to the switch times given later (in simulation seconds or D:H:M:S) |
| period  | time  | The period for repeating switch times. Disabled when set to <= 0, default 0     |

and the fields in wautSwitch:

| Attribute Name | Value Type | Description                                              |
| -------------- | ---------- | -------------------------------------------------------- |
| **time**       | int        | The time the switch will take place                      |
| **to**         | string id  | The name of the program the assigned tls shall switch to |

Of course, programs with the used names must be defined before this
definition is read. Also, the switch steps must be sorted by their
execution time.

Additionally, a definition about which tls shall be switched by the WAUT
must be given, as following:

```xml
<wautJunction wautID="myWAUT" junctionID="RCAS" [procedure="Stretch"] [synchron="t"]/>
```

Here, the attributes have the following meaning:

| Attribute Name | Value Type  | Description                                                                                                     |
| -------------- | ----------- | --------------------------------------------------------------------------------------------------------------- |
| **wautID**     | string id   | The id of the WAUT the tls shall be switched by                                                                 |
| **junctionID** | string id   | The name of the tls to assign to the WAUT                                                                       |
| **procedure**  | string enum | The switching algorithm to use ("GSP" or "Stretch"). If not set, the programs will switch immediately (default) |
| **synchron**   | bool        | Additional information whether the switch shall be done synchron (default: false)                               |

It is possible to assign several tls to a single WAUT. It is also
possible to assign several WAUTs to a single junction in theory, but
this is not done in reality.

A complete definition within an {{AdditionalFile}} is shown below. It would trigger
switching between programs **S1** and **S2** for traffic light logic
**X** with an initial program called **0**.

```xml
<additional>

  <tlLogic id="X" type="static" programID="S1" offset="0">
    <phase duration="50" state="Gr"/>
    <phase duration="50" state="rG"/>
  </tlLogic>

  <tlLogic id="X" type="static" programID="S2" offset="0">
    <phase duration="30" state="Gr"/>
    <phase duration="80" state="rG"/>
  </tlLogic>


  <WAUT startProg="0" refTime="100" id="w1">
    <wautSwitch to="S1" time="300"></wautSwitch>
    <wautSwitch to="SS" time="800"></wautSwitch>
  </WAUT>

  <wautJunction junctionID="X" wautID="w1"></wautJunction>

<additional>
```

!!! note
    If a traffic light program called "online" is loaded, this program will interrupt WAUT switching at that traffic light. This can be used to override WAUT behavior via TraCI.

# Evaluation of Traffic Lights Performance

## Tools For Automatic Detector Generation

Some tools are available which help generating detector definitions for
the evaluation of traffic lights. All are located in {{SUMO}}/tools/output.

!!! note
    The actuated traffic lights do not require detector definitions to be added as they generate their own detectors for internal use.

- ***generateTLSE2Detectors.py*** generates a file which includes
  areal detectors. All lanes incoming into an intersection are covered
  with these detectors. The offset from the intersection may be given
  using the option **--distance-to-TLS** {{DT_FLOAT}} (or **-d** {{DT_FLOAT}}), the default is .1m. The generated detectors
  end either after a given length, defined using **--detector-length** {{DT_FLOAT}} (or **-l** {{DT_FLOAT}}) where the
  default is 250m, or at the lane's end if the lane is shorter than
  this length.
- ***generateTLSE3Detectors.py*** generates a file which includes
  multi-entry/multi-exit detectors. Detectors are built for each edge
  incoming to the traffic light. All lanes of each of these edges are
  covered with exit points. These point's offset from the intersection
  may be given using the option **--distance-to-TLS** {{DT_FLOAT}} (or **-d** {{DT_FLOAT}}), the default is .1m. The
  incoming edges are followed upstream, either until a given length,
  defined using **--detector-length** {{DT_FLOAT}} (or **-l** {{DT_FLOAT}}) where the default is 250m, or another traffic
  light is reached or no further upstream edge exists. Entry points
  are generated at these points.

In both cases, the network must be given using the option **--net-file** {{DT_FILE}} (or **-n** {{DT_FILE}}). The
file including the detector definitions to generate may be given using
the option **--output** {{DT_FILE}} (or **-o** {{DT_FILE}}), default is "e2.add.xml" for areal detectors, and
"e3.add.xml" for multi-entry/multi-exit detectors. Per default, the
areal detectors generated by ***generateTLSE2Detectors.py*** are writing
their measures to "e2output.xml", the multi-entry/multi-exit detectors
generated by ***generateTLSE2Detectors.py*** to "e3output.xml". The
output file name can be changed for both scripts using the option **--results-file** {{DT_FILE}} (or **-r** {{DT_FILE}}). The frequency of generated reports is 60s per default. It can be
changed using the option **--frequency** {{DT_INT}} (or **-f** {{DT_INT}}).

# Controlling traffic lights via TraCI

[TraCI provides various functions for controlling traffic
lights](../TraCI/Change_Traffic_Lights_State.md). And the basic
approaches are outline below

## Setting the phase

A common pattern for implementing adaptive control via TraCI is to load
a program (usually from an
[additional-file](#defining_new_tls-programs)) where all
green phases have a long duration (i.e. 1000s) to avoid switching by
SUMO and then use the **setPhase** function to switch to the next phase
(typically a yellow phase) whenever the green phase should end. The
nice thing about this approach is, that yellow phase and all-red phases
can still be handled automatically by SUMO and the control script simply
determines the duration of the current green phase.

To implement controllers with branching phase transitions, provide
multiple transitions to subsequent green phases and when calling
setPhase, select the yellow phase that starts the transition to the
target green phase.

These additional transition phases can be added to the end of the
program and the optional phase attribute "next" can be used to indicated the
next phase after the transition ends.

Tutorials for controlling traffic lights in this way can be found at
[Tutorials\#TraCI_Tutorials](../Tutorials/index.md#traci_tutorials).

## Setting the duration

Using the function **setPhaseDuration**, the remaining duration for the
current phase can be modified. Note, that this does not take effect when
encountering the same phase again in the next phase cycle.

## Setting the state

Another method for controlling the traffic light is to set the signal
state for all links directly using the function
*setRedYellowGreenState*. After using this function, SUMO will not
modify the state anymore (until switching to another program using
*setProgram*). Consequently, the script must handle all phases and
transitions.

## Setting the complete Program

Using the method **setProgramLogic**, a static
signal plan can be loaded. Since this method requires a complex data
structure as argument, it is recommend to first obtain a data structure
using **getAllProgramLogics** and then modify it.

## Switching between pre-defined Programs

SUMO can load multiple traffic light programs from the *.net.mxl* file
and from [additional files](#defining_new_tls-programs). Using
the TraCI function **setProgram**, a script can switch between them.

# Signal Plan Visualization

In [sumo-gui](../sumo-gui.md), right-clicking on a traffic light allows opening up a menu that contains the items 'Show Phases' and 'Track Phases'.
These items open up new windows which are explained below

## Show Phases

This shows the signal states of all controlled links for the complete list of defined phases. Here, time is on the x-axis and there is one row for each link index. The time axis in the bottom shows the time of each phase change starting at 0.

The time can be switched between the following styles
- **Seconds**: absolute simulation time in seconds
- **MM::SS**: current minute and second (values repeat every hour)
- **Time in Cycle**: current second within the traffic light cycle (resets either when starting phase 0 or in some alignment to absolute simulation time).

Optionally, the green phase durations can be written for every phase. 
The top row contains the phase index but it is possible to change this so it shows phase names instead. (phase names are optional, and only the names of 'Green' phases are shown for brevity)

![show_phases.png](../images/show_phases.png
"Show Phases Window")

!!! note
    All phases will be shown in definition order. This may be different from their operational sequence if phase attribute 'next' is used.

## Track Phases

This shows the evolution of signal states for all controlled links for the last X seconds of operation (set via the 'Range' value). The basic layout is the same as for the 'Show Phases' Window. 

The following additional features may be activated via checkboxes:

- **detector**: shows activation states of all [detectors that are controlling this traffic light](Traffic_Lights.md#detectors)
- **conditions**: shows the boolean value of [conditions that are defined for this traffic light](Traffic_Lights.md#named_expressions). A colored block wil be drawn when the numerical value of the condition is different from zero.

!!! note
    When the mouse is placed over an active condition block, the numerical value of the condition will be shown.

![track_phases.png](../images/track_phases.png
"Track Phases Window")
