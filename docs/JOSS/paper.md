--
title: "SUMO: A software suite for microscopic tranportation system simulations"
tags:
  - C++
  - Python
  - civil engineering
  - traffic simulation
  - emissions
  - dynamic traffic assignment
authors:
  - name: Robert Alms
    orcid: 0000-0001-9950-3596
    equal-contrib: true
    affiliation: 1
  - name: Angelo Banse
    orcid: 0000-0001-7615-627X
    equal-contrib: true
    affiliation: 1
  - name: Mirko Barthauer
    orcid: 0000-0003-3177-3260
    equal-contrib: true
    affiliation: 1
  - name: Michael Behrisch
    orcid: 0000-0002-0032-7930
    equal-contrib: true
    affiliation: 1
  - name: Benjamin Couéraud
    orcid: 0009-0001-3739-730X
    equal-contrib: true
    affiliation: 1
  - name: Jakob Erdmann
    orcid: 0000-0002-4195-4535
    equal-contrib: true
    affiliation: 1
  - name: Yun-Pang Flötteröd
    orcid: 0000-0003-3620-2715
    equal-contrib: true
    affiliation: 1
  - name: Robert Hilbrich
    orcid: 0000-0003-3793-3982
    equal-contrib: true
    affiliation: 1
  - name: Pablo Alvarez Lopez
    orcid: 0000-0002-0651-6767
    equal-contrib: true
    affiliation: 1
  - name: Johannes Rummel
    orcid: 0000-0003-4275-6271
    equal-contrib: true
    affiliation: 1
  - name: Peter Wagner
    orcid: 0000-0001-9097-8026
    equal-contrib: true
    affiliation: "1, 2" # (Multiple affiliations must be quoted)

affiliations:
 - name: German Aerospace Center (DLR), Institute of Transportation Systems, Berlin, Germany
   index: 1
 - name: Technical University of Berlin, Institute of Land and Sea Traffic, Berlin, Germany
   index: 2
 - name: Independent Researcher, Country
   index: 3
date: 18 April 2024
bibliography: paper.bib
---

# Summary

Simulating a transportation system microscopically requires a solid framework that maps real-life features into the software. The Simulation of Urban MObility (SUMO) suite [@SUMO2018] features a wide range of software tools to facilitate research and application projects alike, and to help users to set-up a traffic simulation for a chosen real or synthetic area with minimum effort. At its heart is the simulation itself, and an editor for transportation networks, which can run any scenario from a single intersection up to a multi-modal network the size of the city of Berlin, Germany. The size is only limited by the hardware and the patience of the user. Build into the simulation itself, but also into several hundredth of accompanying python tools, are a good amount of methods that help to assess the effect that any change in the base scenario may have, like changes in travel times, capacity, emissions, and the like.

# Statement of need

A microscopic simulation of multi-modal transportation networks requires a description of the network itself, and a detailed plan how objects (motorized and non-motorized vehicles, persons, goods) move through such a network. In the case of `SUMO`, this is achieved by assigning a route to each object in the simulation network that it has to follow from origin to destination, starting at a particular moment in time. The sum of all these trips is the so called origin-destination matrix, which is in general time-dependent. `SUMO` excels especially at the detailed modelling of how individual objects move in transportation networks, it is weaker when it comes to the computation of the demand itself, which can be achieved by other, more dedicated tools like TAPAS [@tapas] or MATSim [@MATSim:2016].

For larger networks, an additional step it needed to transfer the demand (the time-dependent OD-matrix) into real routes that the various objects follow through the network. This is called the dynamic traffic assignment, which assumes that there is a kind of equilibrium between the demand and the supply given by the network. Which is a fairly old (first ideas are from 1924), but still prevailing concept formalized by Wardrop [Wardrop:1952] and Beckmann [BeckmannEtAl:1955]. `SUMO` contains several tools to compute this equilibrium.

`SUMO` has been designed to be useful in the assessment of a wide range of transport questions. This may start from the estimation of new control algorithms for traffic signals, to the exploration of on-demand public transport services, or to the improvement of railway operations, for which the modelling of railway systems, the vehicles as well as the infrastructure itself, has been considerably improved during the last years. It has been used for research, for business, and even by laypersons to answer all kind of what-happens-if questions.

`SUMO` has also being used for teaching, in courses at the TH Wildau, Germany, and the TU Berlin, Germany. Many tutorials do exist, which have also been extended in order to teach traffic modelling and simulation.


# Similar and/or related packages

Of all: MATSim [@MATSim:2016]. Martin Treiber's [movsim](https://github.com/movsim/traffic-simulation-de)? Something non-German, even non-European? The DTA JOSS paper [@Ortmann2022]?

# Outlook and use cases

Any cool ideas?

`SUMO` is under constant development, in line with the permanently changing landscape of real transportation networks.

# Acknowledgements

We acknowledge financial support by the German Aerospace Center, and from multiple projects (an the related funds) that have been performed over the 20+ years of SUMO's development.

# References
