---
title: Open and Simulate
---

# Things to know first

## What is a scenario in SUMOPy?

A scenario in SUMOPy is an object (a Python object of course) that
contains all data of a traffic scenario (network, trips, routes,
population, plans, buildings, maps, etc.) and basic methods to
manipulate the scenario (routing, creating new trips, etc). One of the
most important methods is the SUMO microsimulation of the scenario.
However, before a simulation can be run, all necessary data needs to be
created beforehand.

# Opening a traffic scenario