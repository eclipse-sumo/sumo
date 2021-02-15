---
title: GTFS
---

# Introduction

[The General Transit Feed Specification](https://developers.google.com/transit/gtfs) defines a common format
for public transportation schedules and associated geographic information. The tools described here 
facilitate the import of schedules (and also routes to a limited degree) into SUMO.

# gtfs2pt.py

This tool is the main script to import GTFS.

The minimal call is:

```
python tools/import/gtfs/gtfs2pt.py --network my.net.xml --gtfs gtfsdata.zip --date 20210617
```

It generates as output two files containing the stops and static routes (gtfsdata_public_transport.add.xml) and the
individual vehicles (gtfsdata_public_transport.rou.xml).

See also the [GTFS tutorial](../../Tutorials/GTFS.md).


# gtfs2fcd.py

This is ahelper script which converts the stop sequences found in GTFS data into SUMO's FCD format and 
optionally also in GPS tracks for further processing. It is used by the script gtfs2pt.py.

The minimal call is:

```
python tools/import/gtfs/gtfs2fcd.py --gtfs gtfsdata.zip --date 20210617
```
