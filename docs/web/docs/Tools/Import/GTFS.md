---
title: GTFS
---

# Introduction

[The General Transit Feed Specification](https://developers.google.com/transit/gtfs) defines a common format
for public transportation schedules and associated geographic information. The tools described here 
facilitate the import of schedules (and also routes to a limited degree) into SUMO.

See also the [GTFS tutorial](../../Tutorials/GTFS.md).

# gtfs2pt.py

This tool is the main script to import public transport from GTFS. Depending on 
the input files available, you can import the routes by:

- searching for the fastest path between stops. This option only needs as input 
the sumo network and the GTFS files: routes.txt, stops.txt, stop_times.txt, 
trips.txt, calendar.txt and calendar_dates.txt.

- importing the route paths from OSM. For this you also need the GTFS file
shapes.txt and the public transport line definitions from OSM (see [ptLines output](../../Networks/Further_Outputs.md#public_transport_lines)).

The minimal call is:

```
python tools/import/gtfs/gtfs2pt.py --network <net-file> --gtfs <gtfs-data-file> --date <YYYYMMDD>
```

or for the option with OSM routes:

```
python tools/import/gtfs/gtfs2pt.py --network <net-file> --gtfs <gtfs-data-file> --date <YYYYMMDD> --osm-routes <ptlines-file>
```

In both cases you can use the **--modes** option that allows to select which public transport 
modes should be imported (bus, tram, train, subway and/or ferry). As default all modes are imported.

As output an additional file (.add.xml) containing the stops and a route file 
(.rou.xml) containing the vehicle definitions are created. If the option **--osm-routes**
was not given, the additional file also contains the static routes.

If the option **--osm-routes** was used, an extra file "gtfs_missing.xml"
that contains the elements (stops and ptLines) of GTFS that could not be imported
is generated. The given routes from OSM may have connectivity problems, which can 
be repaired with the **--repair** option. When this option is enabled and by the 
repair some warnings and errors occurs, these are written in a new output file 
"invalid_osm_routes.txt".

# gtfs2fcd.py

This is a helper script which converts the stop sequences found in GTFS data into SUMO's FCD format and 
optionally also in GPS tracks for further processing. It is used by the script gtfs2pt.py.

The minimal call is:

```
python tools/import/gtfs/gtfs2fcd.py --gtfs <gtfs-data-file> --date <YYYYMMDD>
```

# gtfs2osm.py

This helper script contains the necessary functions to import public transport
from GTFS and OSM. It is used by the script gtfs2pt.py when the **--osm-routes** option is given.