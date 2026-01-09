# Vehicle Group Sequential Stop Feature

## Overview

The Vehicle Group Sequential Stop feature enables SUMO to manage multiple vehicles stopping at the same position with automatic sequential queue formation. Vehicles are placed in a queue-like arrangement with configurable spacing, and if space is insufficient on the current edge, excess vehicles automatically overflow to upstream edges.

## Files Added/Modified

### New Files:
1. **src/libsumo/VehicleGroupStop.h** - Header file with API declarations
2. **src/libsumo/VehicleGroupStop.cpp** - Implementation of sequential stop functionality
3. **src/libsumo/examples_VehicleGroupStop.py** - Python examples and documentation

### Modified Files:
1. **src/libsumo/Vehicle.h** - Added public API methods for sequential stopping
2. **src/libsumo/Vehicle.cpp** - Added wrapper implementations that delegate to VehicleGroupStop

## Core Functionality

### 1. Sequential Vehicle Stopping

```cpp
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
```

**Parameters:**
- `vehIDs`: List of vehicle IDs to stop
- `edgeID`: Edge where vehicles should stop
- `pos`: Base stop position on the edge (meters)
- `laneIndex`: Lane number (default: 0)
- `duration`: Stop duration in seconds (default: -1 for indefinite)
- `flags`: Stop control flags (default: STOP_DEFAULT)
- `startPos`: Alternative start position
- `until`: Stop until this simulation time
- `spacing`: Gap between consecutive vehicles (default: 2.5m)
- `allowUpstreamOverflow`: Enable automatic overflow to upstream edges

**Behavior:**
- Vehicles are positioned in reverse order along the edge
- First vehicle stops at `pos`, next at `pos - (length + spacing + minGap)`, etc.
- If a vehicle's calculated position goes below 0, it overflows to upstream edges
- Each vehicle gets the same stop parameters (duration, flags, etc.)

### 2. Query Stop Sequence

```cpp
std::vector<std::string> Vehicle::getStopSequence(
    const std::string& edgeID,
    double pos
);
```

Returns the list of vehicles stopped at a specific location in order.

### 3. Get Sequential Stop Position

```cpp
double Vehicle::getSequentialStopPosition(
    const std::string& vehID,
    const std::string& edgeID,
    double basePos,
    double spacing = 2.5
);
```

Returns the actual calculated stop position for a specific vehicle in the sequence.

### 4. Clear Stop Sequence

```cpp
void Vehicle::clearStopSequence(
    const std::string& edgeID,
    double pos,
    double tolerance = 1.0
);
```

Clears the stop sequence information for a location (should be called when processing is complete).

## Vehicle Positioning Algorithm

The algorithm places vehicles sequentially in reverse order:

```
Vehicle Index 0: position = pos
Vehicle Index 1: position = pos - (veh_length + spacing + min_gap)
Vehicle Index 2: position = pos - 2 * (veh_length + spacing + min_gap)
...and so on
```

If the calculated position becomes negative (before the edge start), the vehicle is placed on an upstream edge.

## Upstream Overflow Mechanism

When vehicles overflow to upstream edges:

1. The system traverses the predecessors of the current edge
2. For each upstream edge, it checks available space at the end (near the current edge)
3. Vehicles are placed on upstream edges in order
4. The process continues until all vehicles are assigned stops

## Usage Examples

### Basic Usage

```python
import libsumo as traci

# Connect to SUMO
traci.connect()

# Define vehicles and stop location
vehicles = ["bus1", "bus2", "bus3", "bus4"]
edge_id = "main_street"
stop_pos = 100.0

# Set sequential stop
traci.vehicle.setSequentialStop(
    vehIDs=vehicles,
    edgeID=edge_id,
    pos=stop_pos,
    duration=10.0,  # stop for 10 seconds
    spacing=2.5     # 2.5m gap between vehicles
)

# Run simulation
while traci.simulation.getMinExpectedNumber() > 0:
    traci.simulationStep()

traci.close()
```

### With Overflow Handling

```python
# Set sequential stop with upstream overflow enabled
traci.vehicle.setSequentialStop(
    vehIDs=vehicles,
    edgeID=edge_id,
    pos=stop_pos,
    spacing=3.0,
    allowUpstreamOverflow=True  # Allow overflow to upstream edges
)
```

### Querying Stop Information

```python
# Get vehicles at a stop location
sequence = traci.vehicle.getStopSequence(edge_id, stop_pos)
print(f"Vehicles at stop: {sequence}")

# Get specific vehicle's stop position
pos = traci.vehicle.getSequentialStopPosition(
    vehID="bus1",
    edgeID=edge_id,
    basePos=stop_pos,
    spacing=2.5
)
print(f"Bus1 stop position: {pos}")
```

## Use Cases

### 1. Public Transit
- **Scenario**: Multiple buses stopping at same bus stop
- **Benefit**: Automatic queue formation with consistent spacing
- **Example**: 5 buses scheduled for same stop form queue automatically

### 2. Delivery Operations
- **Scenario**: Multiple delivery vehicles at distribution center
- **Benefit**: Sequential pickup/delivery with controlled spacing
- **Example**: 10 delivery trucks queuing for loading docks

### 3. Parking Management
- **Scenario**: Multiple vehicles parking in same area
- **Benefit**: Overflow to adjacent parking areas automatically
- **Example**: Parking lot fills, vehicles overflow to adjacent streets

### 4. Emergency Response
- **Scenario**: Multiple emergency vehicles staging
- **Benefit**: Coordinated positioning with clear queue order
- **Example**: Fire trucks and ambulances coordinating at incident scene

### 5. Traffic Management
- **Scenario**: Vehicles stopping at intersection
- **Benefit**: Organized queuing with automatic overflow
- **Example**: Congestion management at busy intersections

## Implementation Details

### Data Structures

The implementation uses static maps to track stop sequences:

```cpp
static std::map<std::string, std::vector<std::string>> gStopSequences;
static std::map<std::string, std::queue<std::string>> gStopQueues;
```

Stop locations are identified by a unique key: `edge_id_position`

### Private Helper Functions

1. **findUpstreamStoppingPositions()** - Locates available positions on upstream edges
2. **hasSpaceAtPosition()** - Checks if sufficient space exists for a vehicle

## Error Handling

The implementation includes comprehensive error handling:

- **std::invalid_argument** - For invalid vehicle IDs, edges, or lanes
- **std::runtime_error** - For stop insertion failures or network issues
- **Exception propagation** - Errors are reported to the caller

## Performance Considerations

- **Time Complexity**: O(n) for n vehicles, where work is proportional to vehicle count
- **Space Complexity**: O(n) for storing stop sequences
- **Optimization**: Stop sequences are cached for quick lookup

## Future Enhancements

Potential improvements for future versions:

1. **Dynamic Queue Reordering** - Reorder vehicles as they arrive
2. **Priority-Based Stopping** - Different spacing for different vehicle types
3. **Prediction** - Estimate arrival times and adjust positions
4. **Monitoring** - Real-time queue status and analytics
5. **Integration with Parking Areas** - Enhanced overflow to parking infrastructure

## Testing Recommendations

1. **Basic Functionality**
   - Test with 2-5 vehicles
   - Verify correct positioning
   - Check sequence retrieval

2. **Edge Cases**
   - Empty vehicle list
   - Single vehicle
   - Very large spacing values
   - Very small edges

3. **Overflow Scenarios**
   - Many vehicles on short edge
   - Multiple upstream edges
   - Blocked upstream paths

4. **Integration**
   - With other stop types (bus stops, parking)
   - With routing changes
   - With vehicle departures mid-stop

## Building and Integration

To build SUMO with this feature:

1. The new files are automatically compiled if present in src/libsumo/
2. Include VehicleGroupStop.h in Vehicle.cpp (already done)
3. Standard CMake build process applies

```bash
cd /path/to/sumo
mkdir build
cd build
cmake ..
make
```

## API Compatibility

The new API is:
- **Backward compatible** - Existing code unaffected
- **Consistent** - Follows SUMO's existing Vehicle class patterns
- **Extensible** - Can be expanded without breaking changes

## References

- SUMO Documentation: https://sumo.dlr.de/
- TraCI API: https://sumo.dlr.de/docs/TraCI/
- Vehicle Stopping: https://sumo.dlr.de/docs/Simulation/Vehicles/#stopping
