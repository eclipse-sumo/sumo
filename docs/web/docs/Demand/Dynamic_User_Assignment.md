---
title: Dynamic User Assignment
---

# Introduction

For a given set of vehicles with of origin-destination relations
(trips), the simulation must determine routes through the network (list
of edges) that are used to reach the destination from the origin edge.
The simplest method to find these routes is by computing shortest or
fastest routes through the network using a routing algorithm such as
Dijkstra or A\*. These algorithms require assumptions regarding the
travel time for each network edge which is commonly not known before
running the simulation due to the fact that travel times depend on the
number of vehicles in the network.

!!! caution
    A frequent problem with naive user assignment is that all vehicles take the fastest path under the assumption that they are alone in the network and are then jammed at bottlenecks due to the sheer amount of traffic.

The problem of determining suitable routes that take into account travel
times in a traffic-loaded network is called *user assignment*. SUMO
provides different tools to solve this problem and they are described
below.

# Iterative Assignment (**D**ynamic **U**ser **E**quilibrium)

The tool [duaIterate.py](../Tools/Assign.md#duaiteratepy) can be used to compute the
(approximate) dynamic user equilibrium.

!!! caution
    This script will require copious amounts of disk space

```
python tools/assign/duaIterate.py -n <network-file> -t <trip-file> -l <nr-of-iterations>
```

*duaIterate.py* supports many of the same options as
[sumo](../sumo.md). Any options not listed when calling
*duaIterate.py* ***--help*** can be passed to [sumo](../sumo.md) by adding **sumo--long-option-name arg**
after the regular options (i.e. **sumo--step-length 0.5**). The same is true for duarouter options
using **duarouter--long-option-name arg**. Be aware that those options have to come *after* the regular
options.

This script tries to calculate a user equilibrium, that is, it tries to
find a route for each vehicle (each trip from the trip-file above) such
that each vehicle cannot reduce its travel cost (usually the travel
time) by using a different route. It does so iteratively (hence the
name) by

1.  calling [duarouter](../duarouter.md) to route the vehicles in a
    network with the last known edge costs (starting with empty-network
    travel times)
2.  calling [sumo](../sumo.md) to simulate "real" travel times
    result from the calculated routes. The result edge costs are used in
    the net routing step.

The number of iterations may be set to a fixed number of determined
dynamically depending on the used options. In order to ensure
convergence there are different methods employed to calculate the route
choice probability from the route cost (so the vehicle does not always
choose the "cheapest" route). In general, new routes will be added by
the router to the route set of each vehicle in each iteration (at least
if none of the present routes is the "cheapest") and may be chosen
according to the route choice mechanisms described below.

## Alternative Route Selection

Between successive calls of duarouter, the *.rou.alt.xml* format is used
to record not only the current *best* route but also previously computed
alternative routes. By default the fasted route is considered the *best* but this criterion may be changed with option **--eco-measure** to instead minimize a pollutant (i.e. *CO2*).

This way, a new alternative route may be added in each iteration.
The total number of alternatives is limited to 5 and the limit may be changed with option **--max-alternatives**.
In each iteration, the route usage probability is calculated for each route (see below).
When the number of routes for a vehicle is larger than the defined amount, the route with smallest probability is removed.

These best routes are collected within a route
distribution and used when deciding the actual route to drive in the
next simulation step. This isn't always the one with the currently
lowest cost but is rather sampled from the distribution of alternative
routes by a configurable algorithm described below.

## Route-Choice algorithm

The two methods which are implemented are called
[Gawron](../Publications.md#publications_using_sumo) and
[Logit](https://en.wikipedia.org/wiki/Discrete_choice) in the following.
The input for each of the methods is a weight or cost function \(w\) on
the edges of the net, coming from the simulation or default costs (in
the first step or for edges which have not been traveled yet), and a set
of routes <img src="https://latex.codecogs.com/gif.latex?R" border="0" style="margin:0;"/> where each route <img src="https://latex.codecogs.com/gif.latex?r" border="0" style="margin:0;"/> has an old cost <img src="https://latex.codecogs.com/gif.latex?c_r" border="0" style="margin:0;"/> and an
old probability <img src="https://latex.codecogs.com/gif.latex?p_r" border="0" style="margin:0;"/> (from the last iteration) and needs a new cost
<img src="https://latex.codecogs.com/gif.latex?c_r'" border="0" style="margin:0;"/> and a new probability <img src="https://latex.codecogs.com/gif.latex?p_r'" border="0" style="margin:0;"/>.

### Gawron (default)

The Gawron algorithm computes probabilities for choosing from a set of
alternative routes for each driver. The following values are considered
to compute these probabilities:

- the travel time along the used route in the previous simulation step
- the sum of edge travel times for a set of alternative routes
- the previous probability of choosing a route

#### Updates of Travel Time

The update rule is explained with the following example. Driver d chooses Route r in Iteration i. The travel time Tau_d(r, i+1) is calculated according to the aggregated and averaged link travel times per defined interval (default: 900 s) in Iteration i. The travel time for Driver d's Route r in Iteration i+1 equals to Tau_d(r, i) as indicated in Formula (1). The travel times of the other routes in Driver d's route set are then updated with Formula (2) respectively, where Tau_d(s, i) is the travel time needed to travel on Route s in Iteration i and calculated with the same way used for calculating Tau_d(r, i) an T_d(s, i-1). The parameter beta is to prevent travellers from strongly "remembering" the latest trave time of each route in their route sets. The current default value for beta is 0.3.

T_d(r, i+1) = Tau_d(r, i) ------------------------------------(1)

T_d(s, i+1) = beta * Tau_d(s, i) + (1 - beta) * T_d(s, i-1) ---(2)

, where s is one of the routes, which are not selected to use in Iteration i, in Driver d's route set.

The aforementioned update rules also apply when other travel cost units are used. The way to use simulated link costs for calculating route costs may result in cost underestimation especially when significant congestion only on one of traffic movements (e.g. left-turn or right-turn) exists. The existing ticket #2566 deals with this issue. In Formula (1), it is also possible to use Driver d's actual travel cost in Iteration i as Tau_d(r, i).

### Logit

The Logit mechanism applies a fixed formula to each route to calculate
the new probability. It ignores old costs and old probabilities and
takes the route cost directly as the sum of the edge costs from the last
simulation.

<img src="https://latex.codecogs.com/gif.latex?c_r' = \sum_{e\in r}w(e)" border="0" style="margin:0;"/>

The probabilities are calculated from an exponential function with
parameter <img src="https://latex.codecogs.com/gif.latex?\theta" border="0" style="margin:0;"/> scaled by the sum over all route values:

<img src="https://latex.codecogs.com/gif.latex?p_r' = \frac{\exp(\theta c_r')}{\sum_{s\in R}\exp(\theta c_s')}" border="0" style="margin:0;"/>

!!! caution
    It is recommended to set option **--convergence-steps** (i.e. to the same number as **-last-step**) to ensure convergence. Otherwise Logit route choice may keep oscillating, especially with higher values of **--logittheta**.

## Termination

DuaIterate convergence is hard to predict and results may continue to vary even after 1000 iterations.
There are several strategies in this regard:

### Default

By default, a fixed number of iterations, configured via **--first-step** and **--last-step** (default 50) is performed.

### Deviation in Average Travel times

The option **--max-convergence-deviation** may be used to detect convergence and abort iterations
automatically. In each iteration, the average travel time of all trips is computed. From the sequence of these values (one per iteration), the relative standard deviation is computed. Once a minimum number of iterations has been computed (**--convergence-iterations**, default 10) and this deviation falls below the max-convergence deviation threshold, iterations are aborted

### Forced convergence

Option **--convergence-steps** may used to force convergence by iteratively reducing the fraction of vehicles that may alter their route.

- If a positive value x is used, the fraction of vehicles that keep their old route is set to `max(0, min(step / x, 1)` which prevents changes in assignment after step x.
- If a negative value x is used, the fraction of vehicles that keep their old route is set to `1 - 1.0 / (step - |x|)` for steps after `|x|` which asymptotically reduces assignment after `|x|` steps.

## Speeding up Iterations

There is currently now way to speed up duaIterate.py by parallelization.
However, the total running time of duaIterate is strongly influenced by the total running time of "jammed" iterations.
This is a frequent occurrence in the early iterations where many cars try to take the fastest route while disregarding capacity.
There are several options to mitigate this:

- by ramping up the traffic scaling so the first iterations have fewer traffic (**--inc-start, --inc-base, --inc-max, --incrementation**)
- by aborting earlier iterations at an earlier time (**--time-inc**)
- by giving the initial demand with a sensible starting solution (i.e. computed by marouter) along with option **--skip-first-routing**
- by trying to carry more information between runs (**--weight-memory, --pessimism**)

## Usage Examples

### Loading vehicle types from an additional file

By default, vehicle types are taken from the input trip file and are
then propagated through [duarouter](../duarouter.md) iterations
(always as part of the written route file).

In order to use vehicle type definitions from an {{AdditionalFile}}, further options must
be set

```
duaIterate.py -n ... -t ... -l ...
  --additional-file <FILE_WITH_VTYPES>
  duarouter--aditional-file <FILE_WITH_VTYPES>
  duarouter--vtype-output dummy.xml
```

Options preceded by the string *duarouter--* are passed directly to
duarouter and the option *vtype-output dummy.xml* must be used to
prevent duplicate definition of vehicle types in the generated output
files.


# Iterative Assignment (Approximating System Optimum)

The system optimal condition can be achieved by replacing the path travel time with the path marginal travel time (MTT). There are two ways to calculate the path MTT: 1) global approximation, which represents the changes in the total system travel time caused by an additional vehicle that is added to the path at a certain time interval, and 2) local approximation, which represents the changes in the path travel time caused by an additional vehicle that is added to the route at a certain time interval. duaIterate.py supports an approach where the path MTT is approximated as a summation of the corresponding link MTTs. bBcause the global approximation of MTT is computationally expensive and is not practical for large-scale DTA, the tool implements the local approximation of MTT.

Given that SUMO provides the average travel time of each link, it is not possible to directly calculate the additional travel time that one vehicle inflicts on the link. An alternative approach to compute the link MTT is to calculate the average travel time in successive iterations (with a different number of vehicles assigned to each link in each iteration) and compute the difference in link average travel time. Using this method, the average inflicted additional travel time on the link can be calculated. Therefore, a surrogate model of MTT is used to achieve SO as follows:

![grafik](https://github.com/eclipse-sumo/sumo/assets/6240630/f539a81c-8f5a-4bb5-afe4-bfd0c17866fe)

where ![grafik](https://github.com/eclipse-sumo/sumo/assets/6240630/c8927274-d95f-4247-a973-7a2bed921b65)
 is the surrogate MTT of link *a* at simulation step *i*;  ![grafik](https://github.com/eclipse-sumo/sumo/assets/6240630/1de5b8e7-66a3-4256-bf28-ddd484f2803e)
 and  ![grafik](https://github.com/eclipse-sumo/sumo/assets/6240630/6eb312f7-ea13-4246-a30b-5517511397f7)
 are, respectively, the travel time (cost) of link *a* at simulation steps *i-1* and *i-2* ; and ![grafik](https://github.com/eclipse-sumo/sumo/assets/6240630/b5e4ade9-6e19-40b1-9d14-c69063a853cd)
 and ![grafik](https://github.com/eclipse-sumo/sumo/assets/6240630/4f0dd98f-4ad4-4d3a-b0f3-4f181be25feb) are, respectively, the traffic flow of link *a* at simulation steps *i-1* and *i-2*.

The Dynamic System Optimal traffic assignment can be activated by setting options **--marginal-cost, --marginal-cost.exp**, in duaIterate.py. As the MTTs in the proposed algorithm are calculated based on a local approximation, it may lead to its overestimation. Therefore, it is recommended to use the second term of the MTT equation for calibration (option **--marginal-cost.exp**)
For more information about dynamic system optimal modelling in SUMO please refer to https://doi.org/10.52825/scp.v3i.119.

# Iterative Assignment (Mixing DUE and SO)

If traffic consists of a mix of human driven vehicles and computed controlled vehicles one might assume that the former follow an "egoistic" routing approach (DUE) whereas the latter may be configured to route according to system optimum considerations.
The tool [duaIterateMix.py](../Tools/Assign.md#duaiteratemixpy) can be used to compute the Multiclass Simulation-based Traffic Assignment Problem for Mixed traffic flow. This problem involves a dynamic Traffic Assignment Problem with two vehicle classes: one class follows the System Optimum (SO) principle, while the other class follows the User Equilibrium (UE) principles. The objective is to achieve a multiclass traffic assignment where the travel time for UE-seeking vehicles and the marginal travel time for SO-seeking vehicles between the same origin-destination (OD) pair are equal and minimized. For detailed information on solving the multiclass traffic assignment problem using SUMO, please refer to the following link: https://doi.org/10.1080/23249935.2023.2257805

# oneShot-assignment

An alternative to the iterative user assignment above is incremental
assignment. This happens automatically when using `<trip>` input directly in
[sumo](../sumo.md) instead of `<vehicle>`s with pre-defined routes. In this
case each vehicle will compute a fastest-path computation at the time of
departure which prevents all vehicles from driving blindly into the same
jam and works pretty well empirically (for larger scenarios).

The routes for this incremental assignment are computed using the
[Automatic Routing / Routing Device](../Demand/Automatic_Routing.md) mechanism.
It is also possible to enable periodic rerouting to allow increased reactivity to developing jams.

Since automatic rerouting allows for various configuration options, the script
[Tools/Assign\#one-shot.py](../Tools/Assign.md#one-shotpy) may be
used to automatically try different parameter settings.

# [marouter](../marouter.md)

The [marouter](../marouter.md) application computes a *classic*
macroscopic assignment. It employs mathematical functions (resistive
functions) that approximate travel time increases when increasing flow.
This allows to compute an iterative assignment without the need for
time-consuming microscopic simulation.
