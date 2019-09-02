---
title: Tutorials
permalink: /Tutorials/
---

!!! note
    These tutorials assume minor computer skills. If you run into any questions please read the page [Basics/Basic Computer Skills](Basics/Basic_Computer_Skills.md).

# Beginner Tutorials
* [Tutorials/OSMWebWizard](Tutorials/OSMWebWizard.md) - setting up your first scenario with only some clicks using osmWebWizard.py
* [Tutorials/quick start](Tutorials/quick_start.md) - a more complex tutorial with [NETEDIT](NETEDIT.md); first steps in SUMO
* [Tutorials/Driving in Circles](Tutorials/Driving_in_Circles.md) - Work with netedit; define a flow; let vehicles drive in circles using rerouters
* [Tutorials/SUMOlympics](Tutorials/SUMOlympics.md) - Create special lanes and simple traffic lights in netedit, more about flows and vehicle types, working with vehicle 
* [Tutorials/Autobahn](Tutorials/Autobahn.md) - Build a highway, create a mixed highway flow, visualize vehicle speed, save view settings
* [Tutorials/Manhattan](Tutorials/Manhattan.md) - Build a [Manhattan mobility model](https://en.wikipedia.org/wiki/Manhattan_mobility_model)
* [Tutorials/Evaluate simulation output](Tutorials/Evaluate_simulation_output.md) - Evaluate simulation output with python

# Advanced Tutorials
* [Tutorials/Hello Sumo](Tutorials/Hello_Sumo.md) - the simplest net and a single car set up "by hand"
* [Tutorials/ScenarioGuide](Tutorials/ScenarioGuide.md) - High level outline of the steps needed to build a simulation scenario
* [Tutorials/HighwayDetector](Tutorials/HighwayDetector.md) - how to create a highway scenario based on induction loop data
* [Tutorials/FundamentalDiagram](Tutorials/FundamentalDiagram.md) - how to compute a fundamental diagram with SUMO
* [Tutorials/PT from OpenStreetMap](Tutorials/PT_from_OpenStreetMap.md) - shows how to create a runnable public transit scenario entirely from [OpenStreetMap](https://www.openstreetmap.org/)

# TraCI Tutorials
These tutorials use the [Python-TraCI Library](TraCI/Interfacing_TraCI_from_Python.md) for interfacing a python script with a running [SUMO](SUMO.md) simulation.

* [Tutorials/TraCI4Traffic Lights](Tutorials/TraCI4Traffic_Lights.md) - an example for how to connect an external application to SUMO via TraCI for traffic lights control
* [Tutorials/TraCIPedCrossing](Tutorials/TraCIPedCrossing.md) - an example for building a pedestrian-actuated traffic light via TraCI

# Other

## Curso de Simulação em Mobilidade
[Udemy tutorial](https://www.udemy.com/ferramenta-de-microssimulacao-de-trafego-sumo/learn/v4/overview) in Portuguese courtesy of Ednardo Ferreira.

## SUMO User Conference
* [SUMO 2015](http://sumo.dlr.de/daily/sumo2015_tutorial.zip)
* [SUMO 2016](http://sumo.dlr.de/daily/sumo2016_tutorial.zip), [New Features 2016 (Slides)](http://sumo.dlr.de/daily/SUMO2016_new_features.pdf)
* [SUMO 2017](http://sumo.dlr.de/daily/sumo2017_tutorial.zip)
* [SUMO 2018](http://sumo.dlr.de/daily/sumo2018_tutorial.zip)
* [SUMO 2019](http://sumo.dlr.de/daily/sumo2019_tutorial.zip)

## ITSC 2015
Preparations for doing the tutorial:

* Installation of python2.7 [https://www.python.org/ftp/python/2.7.7/python-2.7.7.msi](https://www.python.org/ftp/python/2.7.7/python-2.7.7.msi)
* Installation of a text editing application with syntax highlighting (i.e. [http://download.tuxfamily.org/notepadplus/archive/6.7.5/npp.6.7.5.Installer.exe](http://download.tuxfamily.org/notepadplus/archive/6.7.5/npp.6.7.5.Installer.exe))
* Download and unpack [http://sumo.dlr.de/daily/sumo-msvc10Win32-svn.zip](http://sumo.dlr.de/daily/sumo-msvc10Win32-svn.zip) **to a directory that does not contain the space (' ') character**. We're using the latest development version because 0.24.0 does not yet have [NETEDIT](NETEDIT.md).
* Download the Tutorial Files at [http://sumo.dlr.de/daily/ITSC2015_tutorial.zip](http://sumo.dlr.de/daily/ITSC2015_tutorial.zip)

## Im- and Export
* [Tutorials/Trace File Generation](Tutorials/Trace_File_Generation.md) - shows how to obtain vehicular trace files, usable for simulating vehicular communication

## Calibration/Validation
* [Tutorials/Calibration/San Pablo Dam](Tutorials/Calibration/San_Pablo_Dam.md) - Calibration of car-following parameter using vehicle passing times on observation points as used during the NEARCTIS summer school
<!--* [[Tutorials/Calibration/Berlin]] - Validation of a small inner-urban scenario of Berlin-->

<!-- ==Traffic Light Signal Control with MultiAgent Network== -->
<!--* [[Tutorials/MultiAgentControl]] - MultiAgent Control of Traffic Light Signals with Python -->

# Further Sources for Examples
## Using Examples from the Test Suite
SUMO comes with a large set of tests, just browse them at [{{SUMO}}/tests](https://github.com/eclipse/sumo/blob/master/tests). They are set up to be running by using a testing environment, but it is also possible to extract them and execute using [SUMO](SUMO.md) and/or the other tools of the package. In order to do so you can either [download the complete sumo package](Downloads.md#all-inclusive-tarball) or use the [online test extraction](https://sumo.dlr.de/extractTest.php). In the online tool you enter the path to the test you like (e.g. [{{SUMO}}/tests/sumo/extended/rerouter/use_routing_device](https://github.com/eclipse/sumo/blob/master/tests/sumo/extended/rerouter/use_routing_device) into the form and get a zip containing all the files.

If you have downloaded and unzipped the all inclusive package, you do not need the online form. Just go into the folder and execute the "extractTest.py" script. For example, you may get the same example of using rerouters as following:

```
cd <SUMO_HOME>/tests
../tools/extractTest.py -o /tmp/test sumo/extended/rerouter/use_routing_device
```

will extract you an example for [rerouting vehicles](Simulation/Rerouter.md) into /tmp/test.

# Unfinished Tutorials
The following tutorials are not yet completed

* [Tutorials/Output Parsing](Tutorials/Output_Parsing.md) - a complex tutorial using rerouters to drive in circles and analyzing simulation output

# Outdated Tutorials
The following tutorials are kept for completeness but are superseded by other tutorials/documentation

* [Tutorials/Import from OpenStreetMap](Tutorials/Import_from_OpenStreetMap.md) - shows how to prepare a map from [OpenStreetMap](https://www.openstreetmap.org/) for traffic simulation.
* [Tutorials/Quick_Start_old_style](Tutorials/Quick_Start_old_style.md) - build a scenario by editing the edge and node files in a text editor instead of [NETEDIT](NETEDIT.md)
* [Tutorials/CityMobil](Tutorials/CityMobil.md) - simulation of a parking lot management using automated buses. This TraCI tutorial was written before the implementation of pedestrians and parkingAreas