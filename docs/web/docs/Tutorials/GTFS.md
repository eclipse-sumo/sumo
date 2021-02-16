---
title: GTFS
---

# Introduction

This tutorial shows you how to refine your existing public transit (PT) simulation
scenario with data in the [General Transit Specification Format](https://developers.google.com/transit/gtfs) (GTFS)
which is available for many regions. It is advisable to do the
[Public Transport Tutorial](PT_from_OpenStreetMap.md) first.

The creation of PT simulation consists of three steps:

1. Initial network requirements
2. Finding a data source for GTFS
3. Mapping GTFS data to the network

# Initial network requirements

!!! note
    Using the [osmWebWizard tool](OSMWebWizard.md), may be the easiest way to get a network but the process works with every (geo referenced) SUMO network.

The process works best with a network which comes from OpenStreetMap.
The network should contain edges for all traffic types (modes) you want
to model and the relevant edge types. The script currently supports
the following mapping:

- tram -> railway.tram or highway.*|railway.tram
- subway -> railway.subway
- light_rail -> railway.light_rail
- rail -> railway.rail
- bus -> highway.*
- ship -> ship

If the network does not contain edge types only a bus mapping is possible which will use all available edges.

For this tutorial we will use the readymade example network of Eichwalde.

# Finding a data source for GTFS

There are several sources for GTFS data usually on the website of you local
transit agency. [Some examples from Germany](https://gist.github.com/highsource/67d0846029a43ea28dfd90540bacb1ee):

- [Berlin and Brandenburg (VBB)](https://www.vbb.de/unsere-themen/vbbdigital/api-entwicklerinfos/datensaetze)
- [Lower Saxony and Bremen](https://www.vbn.de/service/entwicklerinfos/)
- [Hamburg](https://suche.transparenz.hamburg.de/?q=gtfs)

The GTFS file should be in zip format and contain at least routes.txt, stops.txt, stop_times.txt, trips.txt and calendar_dates.txt.

This tutorial uses the [June 2016 dataset by VBB](https://www.vbb.de/media/download/5068). You can either download the data
or give the URL of the file directly to the script. To keep this tutorial stable, the file has been mirrored at https://sumo.dlr.de/daily/GTFS_VBB_Juni-Dezember-2016.zip.

# Mapping GTFS data to the network

If you have all the data available you need to decide on the date you want to model. The script cannot find a representative date
by itself, you need to find one yourself. It is usually advisable to check the calendar for school (and other) holidays in advance.

For this tutorial we choose 13.07.2016 (a Wednesday).

If you have downloaded the network and the GTFS data (or have the URL) it is as easy as

```
tools/import/gtfs/gtfs2pt.py -n eichwalde.net.xml.gz --gtfs https://sumo.dlr.de/daily/GTFS_VBB_Juni-Dezember-2016.zip --date 20160713 --vtype-output pt_vtypes.xml
```

The script runs for about five minutes and generates several subdirectories but in the end it provides three output files:

- pt_vtypes.xml
- GTFS_VBB_Juni-Dezember-2016_publictransport.add.xml (defining the static routes and stops)
- GTFS_VBB_Juni-Dezember-2016_publictransport.rou.xml (defining the single public transport vehicles)

The vtypes output generates very simple vehicle type definitions for the different public transport modes in use. You are encouraded to modify this file and adapt
it to the real situation especially concerning capacity (number of seats) for the different modes. You may of course also use vehicle types from another source and skip this output.

In order to use them in a simulation you should pass them as additional files (not route files!) in the order given above.

```
sumo-gui -n eichwalde.net.xml.gz --additional pt_vtypes.xml,GTFS_VBB_Juni-Dezember-2016_publictransport.add.xml,GTFS_VBB_Juni-Dezember-2016_publictransport.rou.xml
```
