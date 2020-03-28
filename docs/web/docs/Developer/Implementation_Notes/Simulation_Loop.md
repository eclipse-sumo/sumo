---
title: Developer/Implementation Notes/Simulation Loop
permalink: /Developer/Implementation_Notes/Simulation_Loop/
---

# Basic

SUMO and SUMO-GUI call both the method `MSNet::simulationStep`. 
This sketch describes the further communication of this method 
with the other elements of the simulation.

```plantuml
@startuml
participant MSNet_simulationStep as simstep
participant MSEdgeControl
participant ...
participant MSVehicle

group car-following model
simstep --> MSEdgeControl : planMovements
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
@enduml
```
