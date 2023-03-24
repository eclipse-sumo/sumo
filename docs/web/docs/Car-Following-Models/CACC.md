---
title: CACC
---

# Overview

The integrated CACC car-following model is based on the work of Milanés
& Shladover [\[1\]](#references), Xiao, Wang & van Arem [\[2\]](#references) and Xiao, Wang, Schakel &
van Arem [\[3\]](#references), wherein the developed control law in the CACC control
algorithm is explicitly divided into three modes: (i) speed (or
cruising) control, (ii) gap control and (iii) gap-closing control. A
fourth mode (i.e. collision avoidance mode) has been introduced within
the project [TransAID](https://www.transaid.eu), when the CACC model was first integrated to SUMO [\[4\]](#references).

see also [ACC model](ACC.md).

## (i) Speed control mode

The speed control mode is designed to maintain the pre-defined by the
driver desired speed and is activated when there are no preceding
vehicles in the range covered by the sensors or when the time-gap is
larger than 2 s.

Additionally, to ensure a smooth acceleration profile when closing in on a stopped vehicle, the `speedControlMinGap` is also evaluated to activate the speed control mode. (We found a default of 1.66m to be feasible.)

## (ii) Gap control mode

The gap control mode aims to maintain a constant time gap between the
CACC-equipped vehicle and its predecessor and is activated when the gap
and speed deviations (with respect to the preceding vehicle) are
concurrently smaller than 0.2 m and 0.1 m/s, respectively.

## (iii) Gap-closing control mode

The gap-closing controller enables the smooth transition from speed
control mode to gap control mode and is triggered when the time-gap is
less than 1.5 s. If the time-gap is between 1.5 s and 2 s, the
CACC-equipped vehicle retains the previous control strategy to provide
hysteresis in the control loop and perform a smooth transfer between the
two strategies.

## (iv) Collision avoidance control mode

The collision avoidance mode prevents rear-end collisions when safety
critical conditions prevail. This mode is activated when the time-gap is
less than 1.5 s and the gap deviation is negative.

Also, If the `followSpeed` computed by the CACC model grows higher than the safe `followSpeed` as computed by the default Krauss model by a given margin (configured by `collisionAvoidanceOverride`), the speed is limited to the value of Krauss-speed + margin. The override margin defaults to 2m/s.

# Notes

- The implemented model can be found in [{{SUMO}}/src/microsim/cfmodels/MSCFModel_CACC.cpp]({{Source}}src/microsim/cfmodels/MSCFModel_CACC.cpp).
- This part of SUMO was developed and extended within the project
  [TransAID](https://www.transaid.eu).
- The model is primarily intended for use in specific traffic
  situations.
- When there is no leader vehicle, the model uses the same speed as the Krauss model to approach junctions and speed limits
  
!!! caution
    The model is known to produce collisions at the default step-length of 1s. Better results can be achieved by setting a lower step length.

# References

1.  Milanés, V., & Shladover, S. E. (2014). Modeling cooperative and
    autonomous adaptive cruise control dynamic responses using
    experimental data. Transportation Research Part C: Emerging
    Technologies, 48, pp. 285–300.
    <https://doi.org/10.1016/j.trc.2014.09.001>
2.  Xiao, L., Wang, M., & van Arem, B. (2017). Realistic Car-Following
    Models for Microscopic Simulation of Adaptive and Cooperative
    Adaptive Cruise Control Vehicles. Transportation Research Record:
    Journal of the Transportation Research Board, 2623, pp. 1–9.
    <https://doi.org/10.3141/2623-01>
3.  Xiao, L., Wang, M., Schakel, W., & van Arem, B. (2018). Unravelling
    effects of cooperative adaptive cruise control deactivation on
    traffic flow characteristics at merging bottlenecks. Transportation
    Research Part C: Emerging Technologies, 96, 380–397.
4.	Porfyri, K. N., Mintsis, E., & Mitsakis, E. (2018). Assessment of ACC and CACC systems using SUMO. 
	EPiC Series in Engineering, 2, 82-93. 
	<https://doi.org/10.29007/r343>
