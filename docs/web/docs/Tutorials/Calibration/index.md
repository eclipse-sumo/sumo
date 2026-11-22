---
title: Calibration
---

# Selecting and Calibrating Vehicle Parameters

## Default Values and Assumptions

SUMO assigns `vClass`-dependent `<vType>` defaults documented at [Vehicle Type Parameter Defaults](../../Vehicle_Type_Parameter_Defaults.md) (for example `length="5.0"`, `minGap="2.5"`, `accel="2.6"`, `decel="4.5"`, `tau="1.0"`, `sigma="0.5"`, and `speedDev="0.1"` for `passenger` vehicles). Parameters such as `accel` and `decel` represent comfortable everyday driving values under the default `Krauss` [car-following model](../../Car-Following-Models/index.md), while `emergencyDecel` represents the physical braking limit.

## Scenario Context Dependencies (Motorway vs. Urban)

Vehicle parameter sensitivity depends strongly on the simulated traffic regime (see [Road Capacity](../../Simulation/RoadCapacity.md)):

- **`minGap` (standstill spacing)**: Dominates queue length, spillback, and saturation discharge at low speeds and signalized urban intersections. Dense urban queues often calibrate `minGap` to `1.5` m to `2.0` m, whereas motorway free-flow throughput is largely insensitive to `minGap`.
- **`tau` (desired time headway) and `sigma` (driver imperfection)**: Govern maximum lane capacity and breakdown probability at arterial and motorway speeds (`grossTimeHeadway = (length + minGap) / speed + tau`). Keep `tau` at or above the reaction interval (`--step-length` or `actionStepLength`) to prevent unsafe following.
- **`speedFactor` and `speedDev`**: Control free-flow speed dispersion and overtaking demand across multi-lane motorways (`speedDev="0.1"` by default on `passenger`; see [speed distributions](../../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#speed_distributions)).
- **`accel` and `decel`**: Determine discharge acceleration from traffic signals, ramp-merging acceptance, and shockwave propagation.

## Calibration Scenarios and Workflows

- **[San Pablo Dam Tutorial](San_Pablo_Dam.md)**: Demonstrates automated calibration of car-following parameters (`vmax`, `accel`, `decel`, `sigma`, `tau`) against real-world detector passing-time series using SciPy Nelder-Mead optimization (`tests/complex/tutorial/san_pablo_dam/`).
- **Online flow and speed calibration**: Use [`<calibrator>`](../../Simulation/Calibrator.md) elements to adapt dynamic flows, speeds, and vehicle types to field detector measurements during a simulation.

# Calibration Tutorials

{{ list_pages("Tutorials/Calibration", recursive=true) }}