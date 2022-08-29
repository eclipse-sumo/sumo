---
title: Randomness
---

Stochasticity is an important aspect of reproducing reality in a
simulation scenario. There are multiple ways of adding stochasticity to
a simulation. They are described below.

# Random number generation (RNG)

Sumo implements the [Mersenne
Twister](http://en.wikipedia.org/wiki/Mersenne_twister) algorithm for
generating random numbers. This random number generator (RNG) is
initialized with a seed value which defaults to the (arbitrary) value
**23423**. This setup makes all applications deterministic by default as
the sequence of random numbers is fixed for a given seed. The seed may
be changed using the option **--seed** {{DT_INT}}. When using the option **--random** the seed will be
chosen based on the current system time resulting in truly random
behavior.

The simulation uses multiple RNG instances to decouple different
simulation aspects

- randomness when loading vehicles (typeDistribution, speed
  deviation,...)
- probabilistic flows
- vehicle driving dynamics
- vehicle devices

The decoupling is done to ensure that loading vehicles does not affect
simulation behavior of earlier vehicles. All RNGs use the same seed.

# Route Distributions

Vehicles can be added to the simulation with a fixed route (`<vehicle>`) or with an origin-destination pair (`<trip>`).
A third alternative is to specify a set of routes (`<routeDistribution>`) and let the vehicle draw a random route from such a distribution. For details, see [route distributions](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#route_distributions).

# Vehicle Type Distributions

A simple way of of modelling a heterogeneous vehicle fleet works by defining a `<vTypeDistribution>` and let each vehicle pick it's type randomly from this distribution. For details, see [vehicle type distributions](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#vehicle_type_distributions).

# Speed distribution

By default, vehicles in SUMO adhere to the maximum speed defined for the
lane they are driving on (if the maximumSpeed of their vehicle type
allows it). This behavior can be modified using the `<vType>`-attribute `speedFactor` which
makes vehicles drive with that factor of the current speed limit. The
attribute also allows the specification of the parameters of a normal
distribution with optional cutoffs. The random value is selected once
for each vehicle at the time of its creation. Using a speed deviation is
the recommended way for getting a heterogenous mix of vehicle speeds.
By default, a speed distribution with a standard deviation of 10% is active.
For details, see [speed distribution](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#speed_distributions)

# Car-Following

The default [car-following model
*Krauss*](../Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md#car-following_models)
supports stochastic driving behavior through the `vType`-attribute `sigma` (default
0.5). When this value is non-zero drivers will randomly vary their speed
based on the RNG described above. Other car-following models also use
this attribute.

# Departure times

The departure times of all vehicles may be varied randomly by using the
option **--random-depart-offset** {{DT_TIME}}. When this option is used each vehicle receives a random offset
to its departure time, equidistributed on \[0, {{DT_TIME}}\].

# Flows with a random number of vehicles
The following features for random flows apply to [duarouter](../duarouter.md) and [sumo](../sumo.md)

## Binomial distribution
By definining a `<flow>` with attributes `end` and `probability` (instead of `vehsPerHour,number`, or `period`), 
a vehicle will be emitted randomly with the given probability each second until the end time is reached. 
The number of vehicles inserted this way will be [binomially distributed](https://en.wikipedia.org/wiki/Binomial_distribution).
When modeling such a flow on a multi-lane road it is recommended to define a `<flow>` for each individual lane because the insertion rate is limited to at most 1 vehicle per second.

When simulating with subsecond time resolution, the random decision for insertion is taken in every simulation step and the probability for insertion is scaled with step-length so that the per-second probability of insertion is independent of the step-length. 
!!! note
    The effective flow may be higher at lower step-length because the discretization error is reduced (vehicles usually cannot be inserted in subsequent seconds due to safety constraints and insertion in [every other second does not achieve maximum flow](VehicleInsertion.md#effect_of_simulation_step-length)).
    
For low probability the distribution of inserted vehicles approximates a [Poisson
Distribution](https://en.wikipedia.org/wiki/Poisson_distribution)

## Poisson distribution
Since version 1.13.0 flow can also be defined with attribute `end` and `period="exp(X)"` where `X` is a positive value.
This will cause the time gaps between vehicle insertions to follow an [exponential distribution](https://en.wikipedia.org/wiki/Exponential_distribution) with rate parameter `X`. Effectively insertion an expected value of `X` vehicles per second.
The number of vehicles inserted this way will follow the [Poisson distribution](https://en.wikipedia.org/wiki/Poisson_distribution).

!!! note
    The effective [insertion rate](VehicleInsertion.md#forcing_insertion_avoiding_depart_delay) is limited by network capacity and other flow attributes such as `departSpeed` and `departLane`
    
# Flows with a fixed number of vehicles

The following 2 sections describe attributes for random flows that apply to [duarouter](../duarouter.md) and [sumo](../sumo.md). They are quite similar to [flows with a random number of vehicles](#flows_with_a_random_number_of_vehicles) but substitute the `number` attribute for the `end` attribute.

## Bernoulli Process
By definining a `<flow>` with attributes `number` and `probability` (instead of `vehsPerHour,number`, or `period`),
a vehicle will be emitted randomly with the given probability each second until the specified number is reached.

## Poisson Process
By definining a `<flow>` with attributes `number` and `period="exp(X)"` (instead of `vehsPerHour,number`, or `period`),
vehicles will emitted with random time-gaps that follow an exponential distribution until the specified number is reached.

## Router options

The [duarouter](../duarouter.md), [dfrouter](../dfrouter.md)
and [jtrrouter](../jtrrouter.md) applications support the option **--randomize-flows**.

When this option is used, each vehicle defined by a `<flow>`-element will be
given a random departure time which is equidistributed within the time
interval of the flow. (By default vehicles of a flow are spaced equally
in time). The departure times computed this way also achieve a [Poisson process](Poisson_point_process#Simulation)

# Departure and arrival attributes

The `<flow>`, `<trip>` and `<vehicle>` elements support the value "random" for their attributes `departLane`, `departPos`,
`departSpeed` and `arrivalPos`. The value will be chosen randomly on every insertion try (for the
departure attributes) or whenever there is a need to revalidate the
arrival value (i.e. after rerouting). The attribute `departPosLat` also supports the value "random". 
The lateral offset at departure will only affect simulation behavior when using the [sublane model](SublaneModel.md) though it will be visible without this model too.

# Lateral Variation
When setting the lane change mode attribute `lcSigma` to a positive value, Vehicles will exhibit some random lateral drift.

# Further sources of randomness

- The tool [randomTrips.py](../Tools/Trip.md#randomtripspy) allows generating traffic between random edges. It also supports randomizing arrival rates.
- [od2trips](../od2trips.md) randomly selecting depart and arrival edges for each trip when disaggregating the O/D-Matrix
- [duarouter](../duarouter.md) adds randomness when performing [Demand/Dynamic_User_Assignment](../Demand/Dynamic_User_Assignment.md)
- [duarouter](../duarouter.md) can randomly disturb the fastest-paths by setting option **--weights.random-factor**
- [Simulation routing can be randomized](../Demand/Automatic_Routing.md#randomness) to ensure usage of alternative routes.


# Reproducibility

Generally, all SUMO applicaitons and tools are expected to produce the same results when running the same version repeatedly with the same arguments and inputs. This also includes running on different platforms (Windows/Linux/Mac). 

There are some situations that are know to violate this rule (either by design or due to technical reasons) and they are listed in the following.

## Violated reproducibility by design

Option **--random** is supported by many applications ([sumo](../sumo.md), [duarouter](../duarouter.md), ...) and tools ([randomTrips.py](../Tools/Trip.md#randomtripspy), ...). It randomizes the [random seed](#random_number_generation_rng) and thereby gives a different random behavior each run.

## Violated reproducibility for other reasons

The following differences are either due to bugs or hard-to-solve problems in libraries used by SUMO.

### Platform differences

- **Differences in generated networks by platform**: When importing network data with geo-coordinates, the transformation to x,y (Cartesian) coordinates are performed by the [Proj](https://proj.org/). This library has differences from one version to the next and differnt platforms usually provide different library versions. The same problem may also manifest when using TraCI-functions for coordinate transformations.
- **Non-deterministic vehicle routing with options --device.rerouting.threads --weights.random-factor** (Issue #10292)
- **Different Simulation behavior due to using the `log` function**. Observed differences for the EIDM-Model (Issue #8921) and also for Simulations with the DriverState-device. Could in principe also affect the Wiedemann-Model and the ToC-device.

