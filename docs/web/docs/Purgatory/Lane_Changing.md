---
title: Purgatory/Lane Changing
permalink: /Purgatory/Lane_Changing/
---

# Theory

# Navigation Level

## Best Lanes Structure

### Introduction

The used networks consist of edges of different lengths and connections
- possibilities to reach a consecutive edge - between them. When moving
through the network, a vehicle (driver) has to know which lanes of an
edge may be used in order to continue its ride. Otherwise - if standing
on a lane from which the next edge within its route can not be reached -
it will stay at the edge's end. As a conclusion, lane changing is highly
dependent on the knowledge about which lanes may be used for continuing
the ride. The following nomenclature will be used:

- "*allowed lane*": a lane which allows to move to the next edge
  (without a lane change)
- "*best lane*": the allowed lane which allows the longest continued
  travel on allowed lanes without a lane change

The information about which lanes are the best ones for continuing the
ride is stored for each vehicle individually in
`std::vector<std::vector<LaneQ> > MSVehicle::myBestLanes `. Each
vehicle must store the information for itself as this information
depends on the vehicle's route, and, as later seen, on the density in
front of the vehicle. The storage saves the information about the "best"
lanes along the edges of the route, starting at the edge the vehicle is
currently on. For each edge, the information about each of the edge's
lanes is stored, starting at the rightmost one. To be exact:

- myBestLanes\[0\] describes the lanes of the current edge (the edge
  the vehicle is standing on)
- myBestLanes\[1\] describes the lanes of the next edge in route

etc.

- myBestLanes\[0\]\[0\] describes the rightmost lane of the current
  edge
- myBestLanes\[1\]\[1\] describes the second lane of the next edge

The structure itself - LaneQ - is defined as following:

```
struct LaneQ {
    /// @brief The described lane
    MSLane *lane;
    /// @brief The overall length which may be driven when using this lane without a lane change
    SUMOReal length;
    /// @brief The overall vehicle sum on consecutive lanes which can be passed without a lane change
    SUMOReal occupation;
    /// @brief As occupation, but without the first lane
    SUMOReal nextOccupation;
    /// @brief The (signed) number of lanes to be crossed to get to the lane which allows to continue the drive
    int bestLaneOffset;
    /// @brief Whether this lane allows to continue the drive
    bool allowsContinuation;
    /// @brief Consecutive lane that can be followed without a lane change (contribute to length and occupation)
    std::vector<MSLane*> bestContinuations;
};
```

### Computation

myBestlanes is re-computed under following circumstances:

- The vehicle wants to enter the network - as it needs the information
  about next lanes for computing the allowed speed
- The vehicle moves one edge further - as the whole structure shifts
- The vehicle is leaving a stop; during the computation, the lane the
  vehicle wants to stop at is regarded and when leaving the stop, best
  lanes have to be recomputed as they point no longer to this stopping
  lane but to one that allows to continue the ride

Additionally, the dynamic information contained in LaneQ is updated in
each time step:

- nextOccupation

These steps are always performed before the lane change algorithm
starts.

#### Speed-Up Possibilities

- Do not compute the whole structure, but only necessary edges by
  removing the first ones and appending the missing
- Do not go through all nextLanes for finding a connection, but store
  (type-dependent) next lanes in a lane and go through them
- Compute some more values in the first (forward) step and save them
  for late use in the backwards-step

# Tactical Level

## Special Cases

### Leaving place in case of counter-changes

It may happen that two vehicles, running on neighbor lanes, want both to
change to the one the other is at. As long as they run besides each
other with similar speed, this would not work. The result is a dead-lock
at the lanes' ends - each of both vehicles can not drive further from
his lane, and can not change to the allowed lane as it is occupied by
the other vehicles.

One attempt to solve this problem is to:

- If a vehicle wants to change a lane and is blocked by a leading
  vehicle which wants to change to his lane, it stores this leading
  vehicle's length, and the space left on the allowed lane in internal
  variables
- If it comes to speed adaptation, the vehicle checks whether it wants
  to change and has a blocking leader and whether there is enough room
  for the blocked leader in front. If so, it computes the safe
  velocity to halt at the place which allows the leading vehicle to
  merge in. Now, this velocity is used if it is below the vehicle's
  wanted velocity - if it is approaching the end of the lane, needing
  a lane change.

The leading vehicle is performing normal operations, as it has not the
leading length set.

Currently not implemented / open issues:

- The distance to the lane's end is stored, not regarding the
  occupation in front of the vehicle. Maybe, this even is not
  incorrect - as the vehicles in front would simply continue they
  ride, making the place available for the vehicle to merge.
- There is a magic number (length) of 1m added to the space needed by
  the leading vehicle that shall merge in
- What happens if the leader has a leading length set? Will it be
  resolved as at least one of the vehicles will become a new leader?

# Control Level

As vehicles are always running at the center of a lane and lane changes
are done discrete, the control level is not considered.