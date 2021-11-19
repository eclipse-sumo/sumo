---
title: MMPEVEM
---

# Overview

Since version 1.11.0 SUMO includes a model for electric vehicles energy
consumption from Teaching and Research Area Mechatronics in Mobile Propulsion (MMP), RWTH
Aachen.  It was implemented by Kevin Badalian.
The model uses detailed characterstic maps to describe the a specific vehicle
powertrain.

You can find test cases for example vehicle type configurations at {SUMO_HOME} /
data/emissions/MMPEVEM


# Using the model

To use the model define a `<vType>` with emissionClass "MMPEVEM".

To also track the battery state, the vehicle must be equipped with a [battery
device](Electric.md#defining_electric_vehicles).  The model is fully compatible
with charging infrastructure and related outputs as described in [Electric
Vehicles](Electric.md).

Additional properties of the vehicle and its electrical components must
then be defined via [parameters of the vehicle or its
type](../Simulation/GenericParameters.md).

These values have the following meanings:

| key                     | Value Type | Default    | Description                                             |
| ----------------------- | ---------- | ---------- | ------------------------------------------------------- |
| maximumBatteryCapacity  | float      | 0 (Wh)     | Maximum battery capacity *E<sub>max</sub>*              |



## Publications

"Accurate physics-based modeling of electric vehicle energy
  consumption in the SUMO traffic microsimulator" (DOI: 10.1109/ITSC48978.2021.9564463).
