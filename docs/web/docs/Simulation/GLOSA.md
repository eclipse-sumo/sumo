---
title: GLOSA
---

# Introduction
Since version 1.9.1 SUMO supports simulation of Green Light Optimal Speed
Advisory (GLOSA) via the glosa device. This allows vehicles to avoid stops at
traffic lights when the phase is about to change.

!!! note
    While the glosa capabilities are under development, their status can be checked via Issue #7721.

# Equipping vehicles
To attach a glosa device to a vehicle, the [standard device-equipment
procedures](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#devices) can
be applied using `<device name>=glosa`.

For instance, a single vehicle can be equipped with glosa as in the following minimal example

```xml
    <vehicle id="v0" route="route0" depart="0">
        <param key="has.glosa.device" value="true"/>  
    </vehicle>
```

# GLOSA functions

## Slowing Down
When a vehicle is approaching a red light the time until reaching the stop line
is compared with the time until the signal (for the intended movement) changes to green.
If the vehicle would reach the stop line before the switch, a slow-down maneuver
may be undertaken.

If the speed can be reduced within reasonable bounds (glosa.min-speed), the
speed is reduced to the maximum possible value so that the vehicle reaches the
stop line at the time of the phase change.

## Speeding up

When a vehicle is approaching a green light and the time until reaching the stop
line is higher than the remaining green phase duration, a speed-up maneuver may
be undertaken.

If driving with a higher [speedFactor]() (configured with glosa.max-speedfactor)
allows to reach the stop line within the remaining green time, the speedFactor
is set to a higher value.

The speedFactor is changed back to the default after passing the traffic light.

# Traffic lights

All traffic lights types supply time-to-switch information via the duration
attribute of their phases. 

!!! caution
    This may cause invalid maneuvers with traffic-actuated traffic lights that
    may vary their phase duration.
    
The communication range may be customized by setting param "device.glosa.range" on the tlLogic. This may also be used to disable glosa functions for specific intersections.
To update the parameter for a program that is embedded in the .net.xml file the following element can be loaded via **--additional-files**:

```xml
    <tlLogic id="C" programID="0">
        <param key="device.glosa.range" value="60"/>
    </tlLogic>
```

# Parameters

The following parameters affect the operation of the glosa device.

- device.glosa.range: (maximum range from stop line at which glosa functions
  become active (default 100). If the current traffic light also sets this parameter, the minimum value of the device and tls parameter is used.
- device.glosa.min-speed: minimum speed for slow-down maneuver (default 5m/s)
- device.glosa.max-speedfactor: maximum speedFactor for trying to reach a green
  light (default 1.1) Setting a value of 1 will ensure perfect compliance with the speed
  limit but may still cause slow drivers to speed up.
- jmDriveAfterYellowTime: This junction model parameter encodes willingness to
  continue driving at yellow depending on running duration of the yellow phase
  (default 0). The value will be taken into account when checking the
  feasibility of a speed-up maneuver

All device parameters may be set via

- sumo option: **--device.glosa.range 150**
- vType parameter: `<param key="device.glosa.min-speed" value="6"/>`
- vehicle parameter: `<param key="device.max.speedfactor" value="1.2"/>`

# TraCI

# Outputs

## Parameter Retrieval
