---
title: JuPedSim Introduction
template: jupedsim.html
---

## Pedestrian Dynamics with JuPedSim

The Juelich Pedestrian Simulator ([JuPedSim](https://www.jupedsim.org/)) is an open-source tool for simulating pedestrian dynamics offering different models for the interaction of pedestrians with their neighbors in a 2D environment. The simulations results provide valuable insights into crowd dynamics usually evaluated by the occurrence of jam, local densities and speed. 

For the interaction of pedestrians with their surroundings (neighbors, obstacles, walls, ...) several microscopic [models](https://www.jupedsim.org/stable/pedestrian_models/) are available in JuPedSim. These microscopic models allow to reproduce phenomena in large crowds such as arching in front of bottlenecks and lane formation in bi-directional streams. 

![JuPedSim arching](../../images/JuPedSim_arching.gif)

*Example of a simulation with JuPedSim for a bottleneck scenario. Arching occurs.*

The movement of the agents is restricted to 2D space which is referred to as *walkable area*. The [walkable area](https://www.jupedsim.org/stable/concepts/geometry.html) is a closed polygon with holes respresenting obstacles such as trees, pillars or stalls. Agents can move in all directions within the area. This results in a high computational effort, which increases with the number of agents (and thus neighbors) and the size of the walkable area, as wayfinding becomes more complex. Due to its underlying model, JuPedSim operates with a higher temporal resolution than SUMO - typically 0.01 seconds — which makes it the runtime-limiting factor in coupled simulation scenarios.

For [route planning](https://www.jupedsim.org/stable/concepts/routing.html) a journey must be defined for each agent. The journey can be a composition of various stages, e.g. exits as a final goal, waypoints that must be passed or waiting positions that are organized as queues or sets. Journeys can be shared by several agents but also multiple journeys can be defined within one simulation scenario.

## Technical Background

*JuPedSim* is provided as a Python package with a C++ core behind and a C-API that is used by SUMO. The *SUMO-JuPedSim* simulation loop starts with *JuPedSim* independently updating pedestrian positions ...
