---
title: Tutorials
---

!!! note
    These tutorials assume minor computer skills. If you run into any questions please read the page [Basics/Basic Computer Skills](../Basics/Basic_Computer_Skills.md).

# Beginner Tutorials
* [Hello World](Hello_World.md) - Creating a simple network and demand scenario with [netedit](../Netedit/index.md) and visualizing it using [sumo-gui](../sumo-gui.md)
* [OSMWebWizard](OSMWebWizard.md) - Setting up a scenario with just a few clicks using osmWebWizard.py; getting a network from OpenStreetMap
* [Quick Start](quick_start.md) - A more complex tutorial with [netedit](../Netedit/index.md); first steps in SUMO
* [Driving in Circles](Driving_in_Circles.md) - Work with [netedit](../Netedit/index.md); define a flow; let vehicles drive in circles using rerouters
* [SUMOlympics](SUMOlympics.md) - Create special lanes and simple traffic lights in netedit, more about flows and vehicle types, working with vehicle
* [Autobahn](Autobahn.md) - Build a highway, create a mixed highway flow, visualize vehicle speed, save view settings
* [Manhattan](Manhattan.md) - Build a [Manhattan mobility model](https://en.wikipedia.org/wiki/Manhattan_mobility_model)
* [Public Transport](PublicTransport.md) - Build a public transport scenario from scratch
* [TaxiService](TaxiService.md) - Build a taxi service from scratch

# SUMO User Conference Tutorials

The SUMO User Conference is an annual event organized by the German Aerospace Center (DLR) in Berlin. Since 2015, each conference begins with a SUMO tutorial session. Below you can find the tutorial material (slide deck and input files). Since 2019, the tutorials have also been recorded on video.

## Files
* [SUMO 2015](https://sumo.dlr.de/daily/sumo2015_tutorial.zip): network editing with xml patch, persons,
* [SUMO 2016](https://sumo.dlr.de/daily/sumo2016_tutorial.zip): network editing, meso, containers, [New Features 2016 (Slides)](https://sumo.dlr.de/daily/SUMO2016_new_features.pdf)
* [SUMO 2017](https://sumo.dlr.de/daily/sumo2017_tutorial.zip): network editing, randomTrips, calibrators (xml only), public transport (obsolete)
* [SUMO 2018](https://sumo.dlr.de/daily/sumo2018_tutorial.zip): fixing intermodal junctions, calibrators in netedit, junction model parameters, editing shapes
* [SUMO 2019](https://sumo.dlr.de/daily/sumo2019_tutorial.zip): network editing, visualizing traffic data, public transport from OSM, parking, netgenerate
* [SUMO 2020](https://sumo.dlr.de/daily/sumo2020_tutorial.zip): turn lanes, routeSampler.py, defining counting data in netedit, taxi/DRT
* [SUMO 2021](https://sumo.dlr.de/daily/sumo2021_tutorial.zip): traffic light layout, indirect left turn, TAZ, OD-traffic, GTFS
* [SUMO 2022](https://sumo.dlr.de/daily/sumo2022_tutorial.zip): network editing, flows, opposite driving, pedestrian crossings, parking search
* [SUMO 2023](https://sumo.dlr.de/daily/sumo2023_tutorial.zip): graphical diff, personFlow, plotting tools, analyzing repeated runs
* [SUMO 2024](https://sumo.dlr.de/daily/sumo2024_tutorial.zip): debugging simulation warnings, pedestrian models, flying taxis (a transcription is available [here](2024.md#transcription))

## Videos

<a class="no-arrow-link" data-youtube href="https://www.youtube.com/watch?v=UeaeCdLt_1o">Watch the 2019 Tutorial</a>
<br>

<a class="no-arrow-link" data-youtube href="https://www.youtube.com/watch?v=aiOQbaB-pWo">Watch the 2020 Tutorial</a>
<br>

<a class="no-arrow-link" data-youtube href="https://www.youtube.com/watch?v=tlshWdzFWpY">Watch the 2021 Tutorial</a>
<br>

<a class="no-arrow-link" data-youtube href="https://www.youtube.com/watch?v=urKtJj87X5M">Watch the 2022 Tutorial</a>
<br>

<a class="no-arrow-link" data-youtube href="https://www.youtube.com/watch?v=3J5KqOPT2qI">Watch the 2023 Tutorial</a>
(a transcription is available [here](2023.md#transcription))
<br>

<a class="no-arrow-link" data-youtube href="https://www.youtube.com/watch?v=9WCGxJDdY9s">Watch the 2024 Tutorial</a>
(a transcription is available [here](2024.md#transcription))

# Advanced Tutorials
* [Hello SUMO](Hello_SUMO.md) - The simplest net and a single car set up "by hand"
* [ScenarioGuide](ScenarioGuide.md) - High level outline of the steps needed to build a simulation scenario
* [HighwayDetector](HighwayDetector.md) - How to create a highway scenario based on induction loop data
* [FundamentalDiagram](FundamentalDiagram.md) - How to compute a fundamental diagram with SUMO
* [PT from OpenStreetMap](PT_from_OpenStreetMap.md) - Creating a runnable public transit scenario entirely from [OpenStreetMap](https://www.openstreetmap.org/)
* [Importing GTFS](GTFS.md) - Importing public transport schedules from public sources using the General Transit Feed Specification
* [Port](port.md) - Creating a scenario that simulates the port logistics of an arbitrary port from [OpenStreetMap](https://www.openstreetmap.org/)

# TraCI Tutorials
These tutorials use the [Python-TraCI Library](../TraCI/Interfacing_TraCI_from_Python.md) for interfacing a python script with a running [sumo](../sumo.md) simulation.

* [TraCI4Traffic Lights](TraCI4Traffic_Lights.md) - An example for how to connect an external application to SUMO via TraCI for traffic lights control
* [TraCIPedCrossing](TraCIPedCrossing.md) - An example for building a pedestrian-actuated traffic light via TraCI
* [Simulating dynamic vehicular detours based on edge travel time (*external*)](https://medium.com/@fazekade/simulating-dynamic-vehicular-detours-based-on-edge-travel-time-in-sumo-e57a50457dba)

# Other

## Curso de Simulação em Mobilidade
[Udemy tutorial](https://www.udemy.com/course/ferramenta-de-microssimulacao-de-trafego-sumo/) in Portuguese courtesy of Ednardo Ferreira.

## ITSC 2015

* Download the Tutorial Files at [https://sumo.dlr.de/daily/ITSC2015_tutorial.zip](https://sumo.dlr.de/daily/ITSC2015_tutorial.zip)

## Im- and Export
* [Trace File Generation](Trace_File_Generation.md) - Shows how to obtain vehicular trace files, usable for simulating vehicular communication

## Calibration/Validation
* [Calibration/San Pablo Dam](Calibration/San_Pablo_Dam.md) - Calibration of car-following parameter using vehicle passing times on observation points as used during the NEARCTIS summer school
<!--* [[Calibration/Berlin]] - Validation of a small inner-urban scenario of Berlin-->

<!-- ==Traffic Light Signal Control with MultiAgent Network== -->
<!--* [[MultiAgentControl]] - MultiAgent Control of Traffic Light Signals with Python -->

## Misc

- [Railway Tutorial](https://sumo.dlr.de/daily/workshop_rail_db2019.7z)

# Further Sources for Examples
## Using Examples from the Test Suite
SUMO comes with a large set of tests, just browse them at [{{SUMO}}/tests](https://github.com/eclipse-sumo/sumo/blob/main/tests). They are set up to be running by using a testing environment, but it is also possible to extract them and execute using [sumo](../sumo.md) and/or the other tools of the package. In order to do so you can either [download the complete sumo package](../Downloads.md#sources) or use the [online test extraction](https://sumo.dlr.de/extractTest.php). In the online tool you enter the path to the test you like (e.g. [{{SUMO}}/tests/sumo/extended/rerouter/use_routing_device](https://github.com/eclipse-sumo/sumo/blob/main/tests/sumo/extended/rerouter/use_routing_device)) into the form and get a zip containing all the files.

If you have downloaded and unzipped the all inclusive package, you do not need the online form. Just go into the folder and execute the "extractTest.py" script. For example, you may get the same example of using rerouters as following:

```
cd <SUMO_HOME>/tests
../tools/extractTest.py -o /tmp/test sumo/extended/rerouter/use_routing_device
```

will extract you an example for [rerouting vehicles](../Simulation/Rerouter.md) into /tmp/test.

# Unfinished Tutorials
The following tutorials are not yet completed

* [Output Parsing](Output_Parsing.md) - A complex tutorial using rerouters to drive in circles and analyzing simulation output

# Outdated Tutorials
The following tutorials are kept for completeness but are superseded by other tutorials/documentation

* [Import from OpenStreetMap](Import_from_OpenStreetMap.md) - Shows how to prepare a map from [OpenStreetMap](https://www.openstreetmap.org/) for traffic simulation
* [Quick_Start_old_style](Quick_Start_old_style.md) - Build a scenario by editing the edge and node files in a text editor instead of [netedit](../Netedit/index.md)
* [CityMobil](CityMobil.md) - Simulation of a parking lot management using automated buses. This TraCI tutorial was written before the implementation of pedestrians and parkingAreas
