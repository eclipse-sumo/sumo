---
title: Purgatory/RouteGuessing
permalink: /Purgatory/RouteGuessing/
---

!!! missing
    This text is not complete. We hope to find someone who actually does the job as a part of his study. Please [contact](../Contact.md) us if you are interested in this topic and know someone who can contribute.

# Why random Routes are evil and some Bullets

In the past, both route computation applications from the SUMO suite,
namely [DUAROUTER](../DUAROUTER.md) and
[JTRROUTER](../JTRROUTER.md) had the possibility to generate random
routes. Some people were happy with this feature; we were not,
especially because of the frequent questions on the usage of this
feature.

This text explains why it is strongly NOT recommended to use random
routes. It also shows some attempts for solving the problem of having no
demand by some more sophisticated approaches for route generation.

As a base, we will use two example networks for which we know the
demand:

- The first is a network from the city of Bologna. It stems from the
  iTETRIS project and arrived us as a Vissim simulation scenario,
  including both the network and the routes. It is an urban scenario.
- The second is a higway scenario. We have reconstructed the routes
  using information collected on observation points (induction loops)
  for the TrafficOnline project. We suppose the demand to be valid.

Ok, now we build random routes for both scenarios, choosing randomly a
source edge ("road") and a destination edge. We generate as many
vehicles as are simulated within the real, correct demand. Here are the
results:

Ok, what has happened, what is different?

- The probability to use a road differs between the original and the
  random demand
- The mean velocities differ
- In the original scenarios, there is less traffic during night, more
  at day, most during the peek hours.

We will now address the issues one by one.

## Probability to use a Road

When using a random demand, we choose a small road with the same
probability to be a starting/ending road as we do with a big one. One
could argue that the capacity for standing vehicles of big roads are
similar to those of small roads - may be, but I personally would assume
that smaller roads are statistically less frequented during work time,
mainly only during the rush hour where persons are leaving their homes
or coming back. Shopping places are found rather on the major roads what
makes them more probably to be used as origins/destinations during the
day. Also, there are parking places etc, also mostly accessed from the
major roads. Of course, this may be not true for shopping areas, where
people try to find parking places in covered small streets. Nonetheless,
this is supposed to be seldom - and, you don't know this from just
looking at the road network.

As a conclusion, it is rather not assumed to choose minor and major
roads for origins and sources with the same frequency. Still, we have no
solution for this. But read further...

## Transit Traffic

Choosing randomly an edge from within the network completely ignores the
fact that the smaller the simulated area is, the more traffic is just
transiting the network - enters the network at its boundaries and leaves
the network at its boundaries.

Of course, for our highway scenario, this is probably the largest source
of mismodelling - no one starts or ends his trip in at a place in the
middle of the highway.

So for improving our method for route generation, we have to determine
the network's boundaries and the edges which are incoming into it or
outgoing from it. We can then try to use those as the major
origins/destination of our routes.

## Turning Directions

Some people who tried the random routes generator may have noticed that
many vehicles were turning. This is BTW also the case with our first
"sophisticated" approach. The reason is very simple: if we choose two
roads randomly, the probability to choose an origin which points to the
opposite direction then the destination is located in is about 50%;
sure, in detail this depends on the network topology. The same counts
for the destination; the probability that the route ends at the road
into the opposite direction is also high, maybe not 50%, but still high.

In real life, you have to turn at least once, too. Nonetheless, due to
not having transit traffic, the overall number of turnings is much
larger in random routes than in reality.