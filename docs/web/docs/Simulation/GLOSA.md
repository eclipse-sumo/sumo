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
may be undertaken (speedFactor is changed to a lower value).

If the speed can be reduced within reasonable bounds (glosa.min-speed), the
speed is reduced to the maximum possible value so that the vehicle reaches the
stop line at the time of the phase change. Speed adaptation follows a trapezoid:

1.  Decelerate/Accelerate to the calculated speed value
2.  Maintain the calculated speed value
3.  Accelerate back to the desired maximum speed before crossing the junction

## Speeding up

When a vehicle is approaching a green light and the time until reaching the stop
line is higher than the remaining green phase duration, a speed-up maneuver may
be undertaken.

If driving with a higher [speedFactor](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#speed_distributions)
(configured with glosa.max-speedfactor) allows to reach the stop line within
the remaining green time, the speedFactor is set to a higher value.

The speedFactor is changed back to the default after passing the traffic light.

## Queue length calculation (since v1.22)

By default, a vehicle with a glosa device approaches the stop line with the highest
possible velocity and ignores vehicles, that are already queuing at the traffic light.

To estimate the queue length of waiting vehicles, a traffic light can be equipped with sensors.
This information is sent to other vehicles, which then adapt their arrival time.
The behavior can be simulated by setting param `device.glosa.use-queue` to true.

```xml
    <vehicle id="v0" route="route0" depart="0">
        <param key="has.glosa.device" value="true"/>
        <param key="device.glosa.use-queue" value="true"/>
    </vehicle>
```

The traffic light also needs to be equipped with detectors, e.g. by setting the tls to `delay_based`.
The maximal detection length can be changed via `detectorRange`.

```xml
    <tlLogic id="C" type="delay_based" programID="0">
        <param key="detectorRange" value="100" />
    </tlLogic>
```

!!! note
    Although the traffic light may be of type `delay_based`, it can still run a static program.

# Traffic lights

All traffic lights types supply time-to-switch information via the duration attribute of their phases.
Since v1.22, the glosa device not only calculates the time to the next phase switch,
but rather retrieves the phase when the vehicle arrives at the intersection (may be multiple phase switches ahead).

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
- device.glosa.add-switchtime (v1.21): A vehicle with GLOSA tries to reach the intersection
  with maximal speed just before the light signal turns green. This results in late braking maneuvers.
  By adding a time offset to the phase change, the vehicle arrives X s later (default 0 s).
- device.glosa.override-safety (v1.21): Default behavior is that a vehicle with glosa receives a speed advice
  and the driver tries to match this speed. When approaching a red light, the driver would start to decelerate,
  despite a higher speed advice. That's because we can't be sure, if the traffic light will really switch
  By setting this boolean parameter to true, the vehicle will ignore the current traffic light phase and strictly follow the speed advice (default false).
  This would somewhat be possible with automated vehicles and guaranteed phase durations.
- device.glosa.ignore-cfmodel (v1.21): Default behavior is that a vehicle with glosa receives a speed advice
  and this advice changes the speedFactor. Therefore, the speed of the vehicle in the next time step is dependent on the car-following model.
  By setting this boolean parameter to true, the device calculates a specific speed time line (default false). This is explicitly followed by the vehicle.
  This would somewhat mimic the behavior of an automated vehicle.
- device.glosa.use-queue (v1.22): By setting this boolean parameter to true,
  the vehicle takes the queue length in front of the traffic light into account (default false).
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
