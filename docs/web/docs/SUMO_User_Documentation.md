---
title: SUMO User Documentation
permalink: /SUMO_User_Documentation/
---

"**S**imulation of **U**rban **MO**bility" (SUMO) is an open source,
highly portable, microscopic road traffic simulation package designed to
handle large road networks. It is mainly developed by employees of the
[Institute of Transportation Systems](https://www.dlr.de/ts) at the
[German Aerospace Center](https://www.dlr.de). SUMO is licensed under the
[EPL](https://eclipse.org/legal/epl-v20.html).

Please use the [mailing lists](Contact.md) for sharing your
results or for asking questions. Answers to some common questions may
also be found in the [FAQ](FAQ.md).

If You use SUMO, please tell us about your
[Publications](Publications.md).

!!! note
    When citing SUMO in general please use our current reference publication: ["Microscopic Traffic Simulation using SUMO"](https://elib.dlr.de/127994/); Pablo Alvarez Lopez, Michael Behrisch, Laura Bieker-Walz, Jakob Erdmann, Yun-Pang Flötteröd, Robert Hilbrich, Leonhard Lücken, Johannes Rummel, Peter Wagner, and Evamarie Wießner. IEEE Intelligent Transportation Systems Conference (ITSC), 2018.

Any help is appreciated - corrections and contributions to the
documentation, submission of code, or other results.

# Introduction

- [The traffic simulation SUMO](Sumo_at_a_Glance.md)

# Basic Usage

- [Notation in this Documentation](Basics/Notation.md)
- [Needed, basic Computer
  Skills](Basics/Basic_Computer_Skills.md)
- [Installing SUMO](Installing.md)
- [Using SUMO Command Line Applications](Basics/Using_the_Command_Line_Applications.md)
- [Tutorials](Tutorials.md)
- [Validating application inputs](XMLValidation.md)

# Network Building

- Introduction to [SUMO Road Networks](Networks/SUMO_Road_Networks.md)
- [Abstract networks generation](Networks/Abstract_Network_Generation.md)
- Importing networks with [NETCONVERT](NETCONVERT.md)
  - [Defining own networks using XML](Networks/PlainXML.md)
  - [Importing non-SUMO networks](Networks/Import.md)
    - [from OpenStreetMap](Networks/Import/OpenStreetMap.md)
      - [3-Click Scenario Generator](Networks/Import/OpenStreetMap.md#3-click_scenario_generation)
    - [from VISUM](Networks/Import/VISUM.md)
    - [from Vissim](Networks/Import/Vissim.md)
    - [from OpenDRIVE](Networks/Import/OpenDRIVE.md)
    - [from MATsim](Networks/Import/MATsim.md)
    - [from ArcView (shapefiles)](Networks/Import/ArcView.md)
    - [from DlrNavTeq](Networks/Import/DlrNavteq.md)
    - [from Robocup Simulation League](Networks/Import/RoboCup.md)
  - [Importing SUMO networks](Networks/Import/SUMO_Road_Networks.md)
  - [Building networks for motorway simulation](Simulation/Motorways.md#building_a_network_for_motorway_simulation)
  - [Building networks for pedestrian simulation](Simulation/Pedestrians.md#building_a_network_for_pedestrian_simulation)
  - [Further NETCONVERT options](Networks/Further_Options.md)
  - [Additional output](Networks/Further_Outputs.md)
- [Creating and modifying networks with NETEDIT](NETEDIT.md)
- [Including elevation data](Networks/Elevation.md)
- [Geo-Coordinates](Geo-Coordinates.md)

# Demand Modelling

- [Introduction to SUMO Demand Modelling](Demand/Introduction_to_demand_modelling_in_SUMO.md)
- [Definition of Vehicles, Vehicle Types, and Routes](Definition_of_Vehicles,_Vehicle_Types,_and_Routes.md)
- [Simulation of public transport](Simulation/Public_Transport.md)
- [Simulation of individual persons and trip chains](Specification/Persons.md)
- [Simulation of logistics](Specification/Logistics.md)
- [Shortest or Optimal Path Routing](Demand/Shortest_or_Optimal_Path_Routing.md)
- [Intermodal Routing](IntermodalRouting.md)
- [Routing in the Simulation](Demand/Automatic_Routing.md)
- [Computing Dynamic User Assignment](Demand/Dynamic_User_Assignment.md)
- [Generating pedestrian traffic demand](Simulation/Pedestrians.md#generating_pedestrian_demand)
- [Generate a vehicle type distribution to model the fleet](Tools/Misc.md#createvehtypedistributionspy)

### Data sources for demand generation

- [Importing O/D Matrices](Demand/Importing_O/D_Matrices.md)
  - [Other VISUM Demand Importers](Demand/Further_Ways_to_import_VISUM_Demand_Definitions.md)
  - [Other Vissim Demand Importers](Demand/Further_Ways_to_import_Vissim_Demand_Definitions.md)
- [Routes from Counting Data (road counts, turn counts)](Demand/Routes_from_Observation_Points.md)
- [Routing by Turn Probabilities](Demand/Routing_by_Turn_Probabilities.md)
- [Activity-based Demand Generation](Demand/Activity-based_Demand_Generation.md)
- [Random Trips](Tools/Trip.md#randomtripspy)
- [Multi-modal random traffic](Tools/Import/OSM.md#osmwebwizardpy)

# Simulation

- [Basic Definition](Simulation/Basic_Definition.md)
- [Saving and Loading Simulation State](Simulation/SaveAndLoad.md)

## Output
- [Simulation output overview](Simulation/Output.md)

## TraCI (On-line Interaction)
- [TraCI overview](TraCI.md)

## Traffic Management and Other Structures

- [Traffic Lights](Simulation/Traffic_Lights.md)
- [Public Transport](Simulation/Public_Transport.md)
- [Variable Speed Signs](Simulation/Variable_Speed_Signs.md)
- [Rerouter / Alternative Route Signage](Simulation/Rerouter.md)
- [Vaporizer](Simulation/Vaporizer.md) (deprecated, use Calibrator instead)
- [Dynamic calibration of flow and speed and type](Simulation/Calibrator.md)
- [Parking areas](Simulation/ParkingArea.md)

## Traffic Modes

- [Pedestrian simulation](Simulation/Pedestrians.md)
- [Bicycle simulation](Simulation/Bicycles.md)
- [Railway simulation](Simulation/Railways.md)
- [Waterway simulation](Simulation/Waterways.md)

## Additional Features

- [Emissions](Models/Emissions.md)
- [Electric Vehicles](Models/Electric.md)
- [Electric Hybrid Vehicles, overhead lines, power substations](Models/ElectricHybrid.md)
- [Logistics](Specification/Logistics.md)
- [Generic Parameters](Simulation/GenericParameters.md)
- [Shapes Visualisation](Simulation/Shapes.md)
- [Wireless Device Detection](Simulation/Bluetooth.md)
- [Emergency Vehicles](Simulation/Emergency.md)
- [Simple Platooning (Simpla)](Simpla.md)
- [Demand Responsive Transport (Taxis)](Simulation/Taxi.md)

## Model details

- [Vehicle speed](Simulation/VehicleSpeed.md)
- [Vehicle insertion](Simulation/VehicleInsertion.md)
- [Vehicle permissions (access restrictions)](Simulation/VehiclePermissions.md)
- [Intersection dynamics](Simulation/Intersections.md)
- [Randomness](Simulation/Randomness.md)
- [Routing and Re-routing](Simulation/Routing.md)
- [Sublane Model](Simulation/SublaneModel.md)
- [Opposite Direction Driving](Simulation/OppositeDirectionDriving.md)
- [Safety](Simulation/Safety.md)
- [Mesoscopic model](Simulation/Meso.md)

## Common Problems

- [Why Vehicles are teleporting](Simulation/Why_Vehicles_are_teleporting.md)
- [Unexpected jamming](FAQ.md#the_simulation_has_lots_of_jamsdeadlocks_what_can_i_do)
- [Too many turn-arounds](Simulation/Turnarounds.md)
- [Unexpected lane-changing maneuvers?](FAQ.md#why_do_the_vehicles_perform_unexpected_lane-changing_maneuvers)
- [How to get high flows?](FAQ.md#how_do_i_get_high_flowsvehicle_densities)

# [Additional Tools](Tools.md)

For an index of all tools see

- [Tool index](Tools.md)

In addition to the [main applications (SUMO, SUMO-GUI, NETCONVERT, etc.)](Sumo_at_a_Glance.md#included_applications), there are
over 150 additional tools. They cover topics from traffic network
analysis, demand generation, demand modification to output analysis.
Most of them are written in [python](https://www.python.org/). All tools
can be found in the SUMO-distribution under <SUMO_HOME\>/tools.

Below are links to some of the most important tools:

- [osmWebWizard](Tools/Import/OSM.md#osmwebwizardpy)
- [TraCI/Interfacing_TraCI_from_Python](TraCI/Interfacing_TraCI_from_Python.md)
- [Reading SUMO networks and outputs with Python (sumolib)](Tools/Sumolib.md)
- [Tools for converting SUMO outputs to CSV/Spreadsheet](Tools/Xml.md)
- [Exporting mobility Traces (traceExporter)](Tools/TraceExporter.md)
- [Determining the differences between two networks](Tools/Net.md#netdiffpy)
- [Tools/Visualization](Tools/Visualization.md)

# Theory

- [Traffic simulations in general](Theory/Traffic_Simulations.md)

# Application Manuals

- [SUMO](SUMO.md)
- [SUMO-GUI](SUMO-GUI.md)
- [NETCONVERT](NETCONVERT.md)
- [NETEDIT](NETEDIT.md)
- [NETGENERATE](NETGENERATE.md)
- [OD2TRIPS](OD2TRIPS.md)
- [DUAROUTER](DUAROUTER.md)
- [JTRROUTER](JTRROUTER.md)
- [DFROUTER](DFROUTER.md)
- [MAROUTER](MAROUTER.md)
- [POLYCONVERT](POLYCONVERT.md)
- [ACTIVITYGEN](ACTIVITYGEN.md)
- [EMISSIONSMAP](Tools/Emissions.md)
- [EMISSIONSDRIVINGCYCLE](Tools/Emissions.md#emissionsdrivingcycle)

# Appendices

- [ChangeLog](ChangeLog.md)
- [Glossary](Other/Glossary.md)
- [FAQ](FAQ.md)
- [Additional Tools](Tools.md)
- [Known File Extensions](Other/File_Extensions.md)
