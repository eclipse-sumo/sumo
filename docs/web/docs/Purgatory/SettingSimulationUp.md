---
title: Purgatory/SettingSimulationUp
permalink: /Purgatory/SettingSimulationUp/
---

# Setting up a Simulation for SUMO

## Needed Data

At first, you need the network the traffic to simulate takes place on.
As SUMO is meant to work with large networks, we mainly concentrated our
work on importing networks and the computation of needed values that are
missing within the imported data. Due to this, no graphical editor for
networks is available, yet. Beside information about a network's roads,
information about traffic lights is needed.

Further, you need information about the traffic demand. While many
traffic simulations use a statistical distribution which is laid over
the network, each vehicle within SUMO knows its route, where "route" is
a list of edges to pass.

You basically have to perform the following steps in order to implement
a simulation scenario:

**1. Build your network**

  This can be either done by a) generating an abstract network using
  [NETGENERATE](../NETGENERATE.md), b) setting up an own
  description in XML and importing it using
  [NETCONVERT](../NETCONVERT.md) or by c) importing an existing
  road network using [NETCONVERT](../NETCONVERT.md); see
  building the networks for further
  information

**2. Build the demand**

  This can be either done by a) describing explicit vehicle routes, b)
  using flows and turning percentages only, c) generating random
  routes, d) importing OD-matrices, or e) importing existing routes;
  see building the demand for further
  information

**3a. If needed, compute the dynamic user assignment**

**3b. Calibrate the simulation using given measures**

**4. Perform the simulation to get your desired output;**

  See performing the simulation and available simulation outputs

This process is also visualised within the next figure.

![Image:flow_complete_small.png](../images/Flow_complete_small.png
"Image:flow_complete_small.png")

**Figure 1.3. Process of simulation with SUMO (rounded: definite data
types; boxes: applications; octagons: abstract data types)**

## Best Practice

Choosing a proper scenario depends on the done investigation. Abstract,
generated networks may be the best solution if one wants to keep the
results easily evaluable. Abstract networks have for example been used
for evaluating the vulnerability of C2C-networks by [Clemens
Honomichl](../Publications.md#honomichl2008).

If one wants to investigate real life traffic, he should try to reuse
the data he has. If no data is available, one
could try to use the [free
scenarios](../Data/Networks.md#scenarios) available within SUMO.