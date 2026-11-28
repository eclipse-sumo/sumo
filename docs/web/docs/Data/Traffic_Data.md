---
title: Traffic Data
---

# Traffic Data

Use the [Highway Detector tutorial](../Tutorials/HighwayDetector.md) to map detector locations to a SUMO network and import their measurements. See [Routes from Observation Points](../Demand/Routes_from_Observation_Points.md) for tools that generate demand from traffic counts. For complete networks and demand files, see [SUMO scenarios](Scenarios.md).

## Traffic count datasets

- [Great Britain, Department for Transport](https://roadtraffic.dft.gov.uk/downloads) provides count point locations, raw vehicle counts, and annual average daily flows in CSV format.
- [Germany, BASt](https://www.govdata.de/suche/daten/automatische-zahlstellen-auf-autobahnen-und-bundesstrassen-stundenwerte) provides hourly counts from automatic stations on motorways and federal roads.
- [California, Caltrans PeMS](https://pems.dot.ca.gov/) provides five-minute traffic measurements and station configuration files.
- [UTD19, ETH Zurich](https://utd19.ethz.ch/) provides georeferenced detector measurements from 40 cities. Access requires registration and agreement to the academic or noncommercial use terms.
- [GraphHopper Open Traffic Collection](https://github.com/graphhopper/open-traffic-collection) lists additional sources of traffic information.

## FSP - Data from the *Freeway Service Patrol Evaluation Project*

This archive contains per-vehicle speed and length measurements from induction loops.

### Data Descriptions

#### per-vehicle speed and lengths

The data is given as a tar file named
[fsp_speed_and_lengths.tar](https://prdownloads.sourceforge.net/sumo/fsp_speed_and_lengths.tar?download)
(\~40MB). Within this tar-file you will find a folder which contains
compressed (gzip) data files. Each data file represents a day for one
direction (north/south). The following days are given:

- 16.02.93
- 17.02.93
- 18.02.93
- 19.02.93
- 22.02.93
- 24.02.93

The files are tab-delimited with the following fields:

- Detector position (in meters)
- Detector lane
- Timestamp
- Vehicle speed
- Vehicle length (in meters)

Verify the timestamp and speed units before converting these records for use in SUMO.

### References

Project and data descriptions:

- Petty K.F.; Noeimi H.; Sanwal K.; Rydzewski D.; Skabardonis A.;
  Varaiya P.; Al-Deek H.; "The freeway service patrol evaluation
  project: database support programs, and accessibility"; In:
  Transportation Research Part C: Emerging Technologies, Volume 4,
  Number 2, April 1996 , pp. 71-85(15), Elsevier

Usage examples:

- ["An Example of Microscopic Car Models Validation using the open
  source Traffic Simulation
  SUMO"](https://sumo.dlr.de/pdf/dkrajzew_ESS2002_LOG14_Validation.pdf);
  Daniel Krajzewicz, Georg Hertkorn, Christian Rössel, Peter Wagner;
  In: Proceedings of Simulation in Industry, 14th European Simulation
  Symposium, 2002, SCS European Publishing House, 2002, pp. 318-322,
  Simulation in Industry, 14th European Simulation Symposium, Oct.
  2002, Dresden / Germany

  Available as: [.pdf, \~320KB](https://sumo.dlr.de/pdf/dkrajzew_ESS2002_LOG14_Validation.pdf).
