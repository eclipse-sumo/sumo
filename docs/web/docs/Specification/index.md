---
title: Specification
---

# Traffic simulation specification

## Vehicles

Use the [vehicle attribute reference](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#available_vehicle_attributes) to define individual vehicles and their routes.

### Flows (repeated insertion)

Use [flows](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#repeated_vehicles_flows) to generate multiple vehicles from one demand definition.

### Types

Set shared vehicle properties with [vehicle type attributes](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#available_vtype_attributes).

### Type distributions

Use [vehicle type distributions](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#vehicle_type_distributions) to assign vehicle types with specified probabilities.

### Categories

See [vehicle classes](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#abstract_vehicle_class) for road access permissions and [emission classes](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#vehicle_emission_classes) for emissions modeling.

## Junctions

Define junctions in [PlainXML node files](../Networks/PlainXML.md#node_descriptions). See [junctions and right-of-way](../Networks/SUMO_Road_Networks.md#junctions_and_right-of-way) for their representation in a SUMO network.

## Streets

Define streets with [PlainXML edge files](../Networks/PlainXML.md#edge_descriptions). See [edges and lanes](../Networks/SUMO_Road_Networks.md#edges_and_lanes) for their representation in a SUMO network.

### Lanes

See [lane-specific definitions](../Networks/PlainXML.md#lane-specific_definitions) to configure individual lanes.

## Routes

Define a route by its [sequence of edges](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#routes), or let SUMO calculate a route from a [trip or flow](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#incomplete_routes_trips_and_flows).

### Route distributions

Use [route distributions](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#route_distributions) to assign alternative routes with specified probabilities.

### Stops

Use [stops and waypoints](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#stops_and_waypoints) to schedule stops or changes in speed along a route.

## How the vehicle drives

### Before start

See [vehicle loading](../Simulation/VehicleInsertion.md#loading) and [speed distributions](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#speed_distributions).

### Departure

See [vehicle insertion](../Simulation/VehicleInsertion.md#insertion_departure) for how SUMO selects a lane, a position, and a speed and retries or delays departures.

### Driving

See [vehicle speed](../Simulation/VehicleSpeed.md) and [integration methods](../Simulation/Basic_Definition.md#defining_the_integration_method) for how SUMO computes speed and position.

### Stops

See [stops and waypoints](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#stops_and_waypoints) for stop positions, timing, and triggers.

### End

See [arrival parameters](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#arrivalpos) and [trip information output](../Simulation/Output/TripInfo.md) for the recorded arrival time, lane, and position.

## Related specifications

- [Persons and multimodal trips](Persons.md)
- [Containers](Containers.md)
- [Routing](Duarouter.md)
