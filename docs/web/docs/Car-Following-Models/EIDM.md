---
title: Extended Intelligent Driver Model
---

## Overview

The Extended Intelligent Driver Model (EIDM) [1] is based on many known model extensions of the Intelligent Driver Model (IDM) by Treiber and Kesting.
Additionally, it includes calculations to reduce the jerk in different driving situations (lane changes, accelerating from standstill, etc.).
By changing the parameters (mostly to 0), each extension can individually be turned "off".
The aim of the model is to correctly replicate submicroscopic acceleration profiles of single vehicles and drivers.

## Notes

- The model is intended to run with subsecond timesteps (0.05-0.5s). The model will produce unwanted emergency braking and collisions at higher step-lengths.
- The EIDM uses lower safe gaps than the IDM (e.g. when changing lanes).
- The model is based on the Improved Intelligent Driver Model, which can currently not be changed via parameters (e.g. to the IDM as base model).
- The model is still in development, future releases may include:
    - spatial anticipation
    - a reaction time added to the action point time
    - acceleration thresholds depending on vehicle dynamics and resistance
    - situation dependent driving
  
!!! caution
    When simulating with estimated variables, the function randNorm() is used.
    Due to the calculation and rounding of the log()-function, results differ depending on the compiler/processor/operating system. For more information and a workaround, see [2].

## References

1. ["Extending the Intelligent Driver Model in SUMO and Verifying the Drive Off Trajectories with Aerial Measurements"](https://sumo.dlr.de/2020/SUMO2020_paper_28.pdf);
   Dominik Salles, Stefan Kaufmann, Hans-Christian Reuss. SUMO User Conference 2020.
2. Platform-dependent behavior: ["Github-issue #8921"](https://github.com/eclipse/sumo/issues/8921)
3. ["SUMO Conference presentation"](https://www.youtube.com/watch?v=0VuSguxDVv8)
