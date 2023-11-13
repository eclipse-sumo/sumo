---
title: Friction
---

# Introduction
Thanks to [Thomas Weber et al](https://easychair.org/publications/paper/3S4X) SUMO supports simulation of friction on streets since version 1.14.0
via network annotation and the friction device. This forces vehicles to reduce their
maximum speed when they are on a slippery road.

# Equipping vehicles
To attach a friction device to a vehicle, the [standard device-equipment
procedures](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#devices) can
be applied using `friction` as device name.

For instance, a single vehicle can be equipped as in the following minimal example

```xml
    <vehicle id="v0" route="route0" depart="0">
        <param key="has.friction.device" value="true"/>
    </vehicle>
```

# Network modification
The friction device will work with the lanes friction attribute, which defaults to 1 and can be modified
either directly in the network or dynamically using traci/libsumo.

# Vehicle Behavior
A vehicle with a friction device will determine a perceived friction by adding random noise to friction
coefficient of the lane and then calculate a new maximum speed for the lane by applying the following factor:

```
factor = -0.3491 * fric * fric + 0.8922 * fric + 0.4493
```

# Parameters

The following parameters affect the operation of the friction device.

- device.friction.stdDev: standard deviation when adding gaussian noise (default 1)
- device.friction.offset: constant offset to apply to all friction values (default 0)

All device parameters may be set via sumo option, vType parameter or vehicle parameter

# TraCI

## Friction Modification

Lanes support the setFriction and getFriction functions.

## Parameter Retrieval

Vehicles allow to retrieve `frictionCoefficient` (the perceived friction with noise),
`rawFriction` (the friction value of the current lane), `stdDev`, and `offset` using
the parameter API e.g. `vehicle.getParameter(id, "device.friction.offset")`.
