---
title: Simulation Loop
---

# Main Vehicle Update Loop

sumo and sumo-gui call both the method `MSNet::simulationStep`. 
This sketch describes the further communication of this method 
with the other elements of the simulation and the update cycle for the vehicles.

```plantuml
@startuml
participant MSNet_simulationStep as simstep
participant TraCIServer
participant MSEdgeControl
participant MSInsertionControl
participant MSInsertionControl
participant ...
participant MSVehicle

group TraCI
simstep -> TraCIServer : processCommandsUntilSimStep
... -> MSVehicle : getSpeed, setSpeed, ...
end

group Events
simstep -> ... : myBeginOfTimestepEvents->execute
... -> MSVehicle : reroute
... -> ... : MSTrafficLightLogic->trySwitch
... -> ... : //move pedestrians//
end

group car-following model
simstep -> MSEdgeControl : planMovements
... -> MSVehicle : planMove
end

group junction model
simstep -> MSEdgeControl : executeMovements
... -> MSVehicle : executeMove
end

group lane-changing model
simstep -> MSEdgeControl : changeLanes
... -> MSVehicle : getLaneChangeModel().wantsChange
end

group insertion
simstep -> MSInsertionControl : execute
... -> MSVehicle : enterLaneAtInsertion
end

group TraCI2
simstep -> TraCIServer : postProcessRemoteControl
... -> MSVehicle : postProcessRemoteControl
end

group output
simstep -> simstep : writeOutput
... -> MSVehicle : getSpeed, getPosition, ...
end

@enduml
```

# Pedestrian Lifecycle and update Loop

Persons in SUMO are modelled by the distinct [stages](../../Specification/Persons.md#simulation_behavior) *walk*, *ride*, *stop* and *access*. 
The simulation behavior of a person in stage *walk* is controlled by the pedestrian model configured via option **--pedestrian.model**.

The model must inherit from [class MSPModel]() and provide instances of [MSTransportableStateAdapter]() for each pedestrian. The adapter instances are used by the simulation to retrieve the state of the pedestrian (i.e. position and speed). The MSPModel is responsible for signaling when the pedestrian changes to another edge. The following sketch describes the interation.

!!! note
    abstract methods that must be overridden by the model implementation are in *italics*

```plantuml
@startuml
participant MSEventControl as events
participant ...
participant MSPerson as person
participant MSPersonStage_Walking as walk
participant MSPModel as model
participant MSTransportableStateAdapter as adapter

group start walk
... -> person : proceed
person -> person : enter walking stage
person -> walk : proceed
walk -> model : //add//
model -> adapter : //constructor//
model -> walk : return adapter
walk -> walk : store pointer to adapter
end

group move pedestrian
events -> model : //execute movements//
model -> ... : MSLink->opend (optional, used by model 'striping')
model -> ... : //retrieve vehicles// (optional, used by model 'striping')
model -> adapter : //update state//
model -> events : //register walk events// (optional, used by event-based model 'nonInteracting')
model -> walk : moveToNextEdge
end

group vehicle update interacts with pedestrians
... -> model : hasPedestrians(lane)
... -> model : blockedAtDist(lane)
... -> model : nextBlocking(lane)
end

group output
... -> person : getPosition, getEdge, getSpeed, ...
person -> walk : getPosition, getEdge, getSpeed, ...
walk -> adapter : getPosition, getSpeed, ...
adapter -> walk : return
walk -> person : return
person -> ... : return
end

group end walk
model -> walk : moveToNextEdge
walk -> model : return true to signal arrival
model -> adapter : //destructor//
model -> model : //clean up pedestrian//
walk -> person : proceed
person -> person : enter next stage
end

@enduml
```
