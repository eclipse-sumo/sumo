#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Example usage of VehicleGroupStop functionality in SUMO.

This script demonstrates how to use the sequential vehicle stopping feature
to manage multiple vehicles stopping at the same position with automatic
queue handling and overflow to upstream edges.

Author: Generated
Date: 2025-01-09
"""

import libsumo as traci
from typing import List


def example_basic_sequential_stop():
    """
    Basic example: Stop 5 vehicles sequentially at the same position.
    
    Vehicles will be arranged in a queue-like formation with automatic spacing.
    """
    print("Example 1: Basic Sequential Stop")
    print("-" * 50)
    
    # Define vehicles to stop
    vehicle_ids = ["veh1", "veh2", "veh3", "veh4", "veh5"]
    
    # Edge and position where vehicles should stop
    stop_edge = "edge1"
    stop_position = 100.0  # meters
    
    # Set sequential stop with default spacing of 2.5m
    traci.vehicle.setSequentialStop(
        vehIDs=vehicle_ids,
        edgeID=stop_edge,
        pos=stop_position,
        laneIndex=0,
        duration=10.0,  # stop for 10 seconds
        spacing=2.5     # 2.5m gap between vehicles
    )
    
    print(f"Set sequential stop for {len(vehicle_ids)} vehicles on {stop_edge} at position {stop_position}")
    
    # Get and display the stop sequence
    sequence = traci.vehicle.getStopSequence(stop_edge, stop_position)
    print(f"Stop sequence: {sequence}")
    print()


def example_sequential_stop_with_overflow():
    """
    Example with upstream edge overflow.
    
    When too many vehicles are provided, they automatically overflow
    to upstream edges if there's not enough space on the current edge.
    """
    print("Example 2: Sequential Stop with Upstream Overflow")
    print("-" * 50)
    
    # Large number of vehicles that may not fit on a single edge
    vehicle_ids = [f"vehicle_{i:02d}" for i in range(1, 21)]  # 20 vehicles
    
    stop_edge = "main_edge"
    stop_position = 50.0
    
    try:
        traci.vehicle.setSequentialStop(
            vehIDs=vehicle_ids,
            edgeID=stop_edge,
            pos=stop_position,
            laneIndex=0,
            duration=30.0,
            spacing=3.0,
            allowUpstreamOverflow=True  # Enable overflow to upstream edges
        )
        
        print(f"Set sequential stop for {len(vehicle_ids)} vehicles")
        print(f"Vehicles will overflow to upstream edges if necessary")
        print(f"Stop sequence: {traci.vehicle.getStopSequence(stop_edge, stop_position)}")
        
    except Exception as e:
        print(f"Error setting sequential stop: {e}")
    print()


def example_get_stop_position():
    """
    Example: Query individual vehicle stop positions in sequence.
    
    Get the actual stop position for each vehicle based on its
    position in the queue.
    """
    print("Example 3: Query Sequential Stop Positions")
    print("-" * 50)
    
    vehicle_ids = ["bus_1", "bus_2", "bus_3", "bus_4"]
    stop_edge = "bus_stop_edge"
    base_position = 75.0
    spacing = 4.0
    
    # First set the sequential stop
    traci.vehicle.setSequentialStop(
        vehIDs=vehicle_ids,
        edgeID=stop_edge,
        pos=base_position,
        duration=20.0,
        spacing=spacing
    )
    
    # Then query each vehicle's stop position
    print(f"\nIndividual stop positions for each vehicle:")
    print(f"Base position: {base_position}m, Spacing: {spacing}m")
    print("-" * 50)
    
    for i, veh_id in enumerate(vehicle_ids):
        stop_pos = traci.vehicle.getSequentialStopPosition(
            vehID=veh_id,
            edgeID=stop_edge,
            basePos=base_position,
            spacing=spacing
        )
        print(f"{veh_id:12s} -> Position: {stop_pos:7.2f}m (queue position: {i+1})")
    print()


def example_dynamic_queue_management():
    """
    Example: Dynamically manage the queue as vehicles arrive and depart.
    
    This example shows how to monitor and manage a vehicle queue
    as vehicles reach the stop position.
    """
    print("Example 4: Dynamic Queue Management")
    print("-" * 50)
    
    vehicle_ids = ["truck_01", "truck_02", "truck_03", "truck_04", "truck_05"]
    stop_edge = "delivery_edge"
    stop_position = 120.0
    
    # Set the sequential stop
    traci.vehicle.setSequentialStop(
        vehIDs=vehicle_ids,
        edgeID=stop_edge,
        pos=stop_position,
        duration=15.0,
        spacing=2.5
    )
    
    print(f"Initial stop sequence: {vehicle_ids}")
    print(f"Vehicles will stop with 2.5m spacing")
    
    # Simulate progression: as vehicles depart, we can clear the stop
    # and reset for remaining vehicles
    print("\nSimulated progression:")
    print("- Vehicle 'truck_01' arrives and stops at position 120.0m")
    print("- Vehicle 'truck_02' arrives and stops at position ~115.5m") 
    print("- Vehicle 'truck_03' arrives and stops at position ~111.0m")
    print("- After truck_01 departs, clear stop sequence and reorder")
    
    # Clear the stop sequence when done
    traci.vehicle.clearStopSequence(stop_edge, stop_position)
    print("\nStop sequence cleared after processing")
    print()


def example_custom_spacing():
    """
    Example: Use different spacing values for different scenarios.
    
    Demonstrates how spacing affects vehicle positioning.
    """
    print("Example 5: Custom Spacing Scenarios")
    print("-" * 50)
    
    vehicle_ids = ["taxi_1", "taxi_2", "taxi_3", "taxi_4"]
    stop_edge = "taxi_rank"
    base_pos = 60.0
    
    # Scenario 1: Tight parking (small spacing)
    tight_spacing = 1.5  # 1.5m gap
    print(f"\nScenario A: Tight parking (spacing = {tight_spacing}m)")
    traci.vehicle.setSequentialStop(
        vehIDs=vehicle_ids,
        edgeID=stop_edge,
        pos=base_pos,
        duration=25.0,
        spacing=tight_spacing
    )
    
    for veh_id in vehicle_ids:
        pos = traci.vehicle.getSequentialStopPosition(
            vehID=veh_id,
            edgeID=stop_edge,
            basePos=base_pos,
            spacing=tight_spacing
        )
        print(f"  {veh_id}: position {pos:.1f}m")
    
    traci.vehicle.clearStopSequence(stop_edge, base_pos)
    
    # Scenario 2: Relaxed spacing (safer distance)
    relaxed_spacing = 5.0  # 5m gap
    print(f"\nScenario B: Relaxed spacing (spacing = {relaxed_spacing}m)")
    traci.vehicle.setSequentialStop(
        vehIDs=vehicle_ids,
        edgeID=stop_edge,
        pos=base_pos,
        duration=25.0,
        spacing=relaxed_spacing
    )
    
    for veh_id in vehicle_ids:
        pos = traci.vehicle.getSequentialStopPosition(
            vehID=veh_id,
            edgeID=stop_edge,
            basePos=base_pos,
            spacing=relaxed_spacing
        )
        print(f"  {veh_id}: position {pos:.1f}m")
    
    traci.vehicle.clearStopSequence(stop_edge, base_pos)
    print()


def main():
    """
    Run all examples.
    """
    print("=" * 70)
    print("SUMO Vehicle Group Sequential Stop Examples")
    print("=" * 70)
    print()
    
    # Note: These examples assume SUMO is running and TraCI is connected
    # In real usage, you would:
    # 1. Start SUMO with TraCI server
    # 2. Connect using traci.connect()
    # 3. Run the simulations with traci.simulationStep()
    # 4. Use these functions as part of your simulation
    
    print("NOTE: These are code examples showing how to use the API.")
    print("To run them, you need an active SUMO simulation with TraCI.\n")
    
    # Example functions (in actual use, these would be called within
    # a running simulation loop)
    
    print("Available functions in VehicleGroupStop API:")
    print("-" * 50)
    print("""
1. setSequentialStop(vehIDs, edgeID, pos, ...)
   - Stop multiple vehicles sequentially at same position
   - Automatically handles spacing between vehicles
   - Supports upstream edge overflow
   
2. getStopSequence(edgeID, pos)
   - Retrieve list of vehicles stopped at this location
   - Returns the order of vehicles in the queue
   
3. getSequentialStopPosition(vehID, edgeID, basePos, spacing)
   - Get the actual stop position for a vehicle in the sequence
   - Useful for monitoring vehicle locations
   
4. clearStopSequence(edgeID, pos, tolerance)
   - Clear stop sequence information for a location
   - Should be called when processing is complete

Parameters:
- vehIDs: List of vehicle IDs to stop
- edgeID: Edge where vehicles should stop
- pos: Base stop position on the edge (in meters)
- laneIndex: Lane number (default: 0)
- duration: Stop duration in seconds (default: -1 for indefinite)
- flags: Stop control flags (default: STOP_DEFAULT)
- startPos: Alternative start position
- until: Stop until this simulation time
- spacing: Gap between consecutive vehicles (default: 2.5m)
- allowUpstreamOverflow: Auto-place overflow on upstream edges
    """)
    
    print("\nUse Cases:")
    print("-" * 50)
    print("""
1. Bus/Transit Queue Management
   - Multiple buses stopping at same stop
   - Automatic queue positioning
   
2. Delivery/Pickup Operations
   - Multiple delivery vehicles at same location
   - Sequential loading/unloading
   
3. Parking Management
   - Group parking with sequential placement
   - Overflow to nearby parking areas
   
4. Traffic Control
   - Coordinated stopping at intersections
   - Dynamic queue reorganization
   
5. Emergency Services
   - Multiple emergency vehicles staging
   - Controlled sequential deployment
    """)
    
    print("\n" + "=" * 70)
    print("For detailed implementation, see VehicleGroupStop.h and .cpp files")
    print("=" * 70)


if __name__ == '__main__':
    main()
