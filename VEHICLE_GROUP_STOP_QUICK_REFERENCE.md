#!/usr/bin/env python3
"""
Quick Reference: Vehicle Group Sequential Stop Feature

This document provides a quick reference for using the new Vehicle Group
Sequential Stop feature in SUMO/libsumo.

FEATURE SUMMARY
===============
Allows multiple vehicles to stop at the same position sequentially with:
- Automatic queue formation
- Configurable spacing between vehicles
- Automatic overflow to upstream edges when space is limited

FILES CREATED
=============
1. src/libsumo/VehicleGroupStop.h
   - Header with API declarations
   - Class definition and method signatures
   
2. src/libsumo/VehicleGroupStop.cpp
   - Implementation of sequential stop logic
   - Queue management and position calculation
   
3. src/libsumo/examples_VehicleGroupStop.py
   - Python usage examples
   - Multiple use case scenarios
   
4. VEHICLE_GROUP_STOP_README.md
   - Comprehensive documentation
   - Implementation details and use cases

FILES MODIFIED
==============
1. src/libsumo/Vehicle.h
   - Added 4 new public static methods
   
2. src/libsumo/Vehicle.cpp
   - Added implementations delegating to VehicleGroupStop
   - Added #include for VehicleGroupStop.h
   
3. src/libsumo/CMakeLists.txt
   - Added VehicleGroupStop.cpp and VehicleGroupStop.h to build sources

API REFERENCE
=============

1. SET SEQUENTIAL STOP
   void Vehicle::setSequentialStop(
       const std::vector<std::string>& vehIDs,
       const std::string& edgeID,
       double pos,
       int laneIndex = 0,
       double duration = INVALID_DOUBLE_VALUE,
       int flags = STOP_DEFAULT,
       double startPos = INVALID_DOUBLE_VALUE,
       double until = INVALID_DOUBLE_VALUE,
       double spacing = 2.5,
       bool allowUpstreamOverflow = true
   );
   
   Purpose: Stop multiple vehicles at same position with queue formation
   
   Key Features:
   - Automatic position calculation for each vehicle
   - Configurable spacing (gap) between vehicles
   - Upstream edge overflow handling
   - Same stop parameters for all vehicles
   
   Python Usage:
   traci.vehicle.setSequentialStop(
       vehIDs=["v1", "v2", "v3"],
       edgeID="edge1",
       pos=100.0,
       duration=10.0,
       spacing=2.5
   )

2. GET STOP SEQUENCE
   std::vector<std::string> Vehicle::getStopSequence(
       const std::string& edgeID,
       double pos
   );
   
   Purpose: Retrieve list of vehicles at a stop location
   
   Returns: Vector of vehicle IDs in stop order
   
   Python Usage:
   sequence = traci.vehicle.getStopSequence("edge1", 100.0)
   # Returns: ["v1", "v2", "v3"]

3. GET SEQUENTIAL STOP POSITION
   double Vehicle::getSequentialStopPosition(
       const std::string& vehID,
       const std::string& edgeID,
       double basePos,
       double spacing = 2.5
   );
   
   Purpose: Get calculated stop position for vehicle in sequence
   
   Returns: Position in meters on the edge
   
   Python Usage:
   pos = traci.vehicle.getSequentialStopPosition(
       vehID="v2",
       edgeID="edge1",
       basePos=100.0,
       spacing=2.5
   )
   # Returns: ~95.3 (depends on vehicle lengths)

4. CLEAR STOP SEQUENCE
   void Vehicle::clearStopSequence(
       const std::string& edgeID,
       double pos,
       double tolerance = 1.0
   );
   
   Purpose: Clear stop sequence information for a location
   
   Python Usage:
   traci.vehicle.clearStopSequence("edge1", 100.0)

HOW IT WORKS
============

Position Calculation:
- First vehicle: stops at base position
- Second vehicle: stops at base_pos - (length + spacing + minGap)
- Third vehicle: stops at base_pos - 2*(length + spacing + minGap)
- And so on...

Example with vehicles of 5m length, 2.5m spacing, 1m minGap:
- v1: position 100.0
- v2: position 100.0 - (5 + 2.5 + 1) = 91.5
- v3: position 100.0 - 2*(5 + 2.5 + 1) = 83.0
- v4: position 100.0 - 3*(5 + 2.5 + 1) = 74.5

Upstream Overflow:
- If calculated position < 0, vehicle moves to upstream edge
- Upstream edge selection: nearest predecessor edge
- Position on upstream edge: near end of edge (close to current edge)

USAGE PATTERNS
==============

PATTERN 1: Simple Queue at Bus Stop
vehicles = ["bus1", "bus2", "bus3"]
traci.vehicle.setSequentialStop(
    vehIDs=vehicles,
    edgeID="bus_stop_street",
    pos=50.0,
    duration=15.0,  # 15 second stop
    spacing=2.5
)

PATTERN 2: Delivery Vehicles with Overflow
vehicles = ["truck1", "truck2", ..., "truck15"]
traci.vehicle.setSequentialStop(
    vehIDs=vehicles,
    edgeID="distribution_center",
    pos=100.0,
    spacing=3.0,
    allowUpstreamOverflow=True  # Overflow to upstream streets
)

PATTERN 3: Emergency Vehicle Staging
vehicles = ["fire1", "fire2", "ambulance1", "ambulance2"]
traci.vehicle.setSequentialStop(
    vehIDs=vehicles,
    edgeID="incident_edge",
    pos=75.0,
    duration=20.0,
    spacing=2.0
)

PATTERN 4: Monitoring Vehicle Positions
# Get sequence first
seq = traci.vehicle.getStopSequence("edge1", 100.0)

# Check each vehicle's actual position
for i, veh in enumerate(seq):
    stop_pos = traci.vehicle.getSequentialStopPosition(
        vehID=veh,
        edgeID="edge1",
        basePos=100.0,
        spacing=2.5
    )
    print(f"Vehicle {veh} (#{i+1}): {stop_pos}m")

ERROR HANDLING
==============

Common Errors and Solutions:

1. "Vehicle not found"
   Cause: Vehicle ID doesn't exist
   Solution: Check vehicle IDs before calling
   
2. "Edge not found"
   Cause: Edge ID is invalid
   Solution: Verify edge exists in network
   
3. "Invalid lane index"
   Cause: Lane index exceeds edge lanes
   Solution: Check edge has required lanes
   
4. "Stop insertion failed"
   Cause: Vehicle already stopped or invalid parameters
   Solution: Check vehicle state and stop parameters

BEST PRACTICES
==============

1. Validate Input
   - Check all vehicle IDs exist
   - Verify edge and lane indices
   - Ensure reasonable spacing values

2. Use Appropriate Spacing
   - Tight parking: 1.5m (risky, may overlap)
   - Normal spacing: 2.5m (safe, recommended)
   - Relaxed spacing: 5.0m (very safe)

3. Handle Overflow Gracefully
   - Always enable allowUpstreamOverflow=True for safety
   - Verify enough upstream edges exist if needed
   - Monitor overflow situations

4. Clean Up When Done
   - Call clearStopSequence() after vehicles depart
   - Prevents memory accumulation
   - Allows sequence reuse at same location

TESTING CHECKLIST
=================

□ Basic functionality with 2-3 vehicles
□ Larger queue with 10+ vehicles
□ Verify position calculations
□ Test upstream overflow
□ Test with different spacing values
□ Test edge and lane validation
□ Test sequence retrieval
□ Test with vehicle departures
□ Integration with vehicle stops
□ Performance with many vehicles

PARAMETERS EXPLAINED
====================

vehIDs (required)
- List of vehicle IDs to stop
- Order determines queue order
- All vehicles must exist

edgeID (required)
- Edge where vehicles stop
- Must be valid edge in network

pos (required)
- Stop position in meters
- Position of first vehicle
- Others calculated relative to this

laneIndex (optional, default=0)
- Which lane of the edge
- 0 = rightmost, increases left
- Must be valid for edge

duration (optional, default=-1)
- How long each vehicle stops
- In seconds
- -1 = indefinite

flags (optional, default=STOP_DEFAULT)
- Stop control flags
- See TraCI documentation

startPos (optional, default=-1)
- Alternative start position

until (optional, default=-1)
- Stop until specific time

spacing (optional, default=2.5)
- Gap between vehicles in meters
- Actual gap = spacing + vehicle_length + min_gap
- Increase for larger vehicles

allowUpstreamOverflow (optional, default=true)
- Allow overflow to upstream edges
- Essential for large queues
- Disable if strictly limiting to one edge

LIMITATIONS & FUTURE WORK
=========================

Current Limitations:
- All vehicles get same duration (no per-vehicle durations yet)
- Position calculation assumes one direction only
- Queue order is fixed at creation time

Future Enhancements:
- Dynamic queue reordering
- Per-vehicle duration configuration
- Priority-based vehicle ordering
- Real-time queue monitoring
- Integration with demand-responsive services
- Advanced prediction and smoothing

TROUBLESHOOTING
===============

Issue: Vehicles not stopping
Solution: 
- Check vehicle is on correct route
- Verify stop duration is positive or -1
- Ensure edge is on vehicle route

Issue: Positions seem wrong
Solution:
- Calculate manually to verify
- Check vehicle lengths in vtype
- Verify spacing parameter

Issue: Overflow not working
Solution:
- Enable allowUpstreamOverflow
- Check upstream edges exist
- Verify no route constraints block access

Issue: Memory grows over time
Solution:
- Call clearStopSequence() when done
- Don't create new stops at same location without clearing

For detailed information, see VEHICLE_GROUP_STOP_README.md
"""

# Print the documentation
if __name__ == "__main__":
    print(__doc__)
