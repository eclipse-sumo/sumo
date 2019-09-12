---
title: Purgatory/DFROUTER
permalink: /Purgatory/DFROUTER/
---

## Usage

See [DFROUTER](../DFROUTER.md) in the [User Documentation](../SUMO_User_Documentation.md).

## Comments

### Why the route distribution may be different from what is expected

- The time described by the detectors values may end before what shall
  be generated; if, for example, routes for a whole day shall be
  generated (b=0, e=86400, the defaults) but the detectors only
  contain information for one minute, the route distribution is still
  estimated for the whole day while detector values are missing. In
  this case, the distribution may differ from what is expected (being
  the one during the described time).

### Why variable speed signs show velocities of 200m/s

- If the DFROUTER finds a detector entry with v=0, it sets the allowed
  velocity in the according vss to 200km/h. The idea is not to set a
  limit when no valid value exists.

## Behaviour to define

- What shall be done if a detector value is missing?

## Tests

### tri_completely_defined

Simple network:

<http://sumo.sourceforge.net/wiki_images/dfrouter_tricomplete_net.gif>

All detector values are supplied correctly

-   - tri_flows1_1step_straight: the straight direction has 10
    vehicles running
  - tri_flows1_1step_left: the left direction has 10 vehicles
    running
  - tri_flows1_1step_right: the right direction has 10 vehicles
    running
  - tri_flows1_1step_equal: all directions have 10 vehicles
    running
  - tri_flows1_1step_more_straight: the straight direction has
    20, left and right have 10 vehicles running
  - tri_flows1_1step_more_left: the left direction has 20,
    straight and right have 10 vehicles running
  - tri_flows1_1step_more_right: the right direction has 20,
    straight and left have 10 vehicles running

Flows from tri_flows1_1step_more_left (the darker the more):

<http://sumo.sourceforge.net/wiki_images/dfrouter_tricomplete_flow_more_left.gif>

### tri_no_first

The first detectors of the left/right arms are missing

This is meant to verify whether a detector may lie on a later edge.

### tri_last_only

Only the last detectors of the left/right arms exist.

This is meant to verify whether a detector may lie on a later edge.

### tri_missing_first_left_values

All detectors exist (as in tri_completely_defined) but the first
detector to left has no value.

The tests show that:

- it is reported that this detector has no value as soon as the option
  "--report-empty-detectors" is set
- the routes change as missing values for the first detector are
  interpreted as flow=0

### tri_missing_first_left_noempty

Same as (as in tri_missing_first_left_values) but the option
"--remove-empty-detectors" is used.

The tests show that:

- it is reported that the detector that has no values is removed from
  the list of known detectors
- the original routes are restored (thos from
  tri_completely_defined) because the subsequent detectors on the
  left arm have correct flows