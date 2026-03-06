---
title: eichstaett.net.xml
---

[{{SUMO}}/tests/complex/tutorial/osm_activitygen/eichstaett.net.xml]({{Source}}tests/complex/tutorial/osm_activitygen/eichstaett.net.xml)

SUMO network of the German city Eichstätt. It is based on an [optimized
OpenStreetMap file](../../Tutorials/OSMActivityGen/eichstaett.osm.md)
following the [Tutorials/Import from
OpenStreetMap](../../Tutorials/Import_from_OpenStreetMap.md). The
netconvert call was:

```
    netconvert --type-files ${SUMO_HOME}/data/typemap/osmNetconvert.typ.xml,${SUMO_HOME}/data/typemap/osmNetconvertUrbanDe.typ.xml  --remove-edges.by-vclass hov,taxi,bus,delivery,transport,lightrail,cityrail,
    rail_slow,rail_fast,motorcycle,bicycle,pedestrian --remove-geometry --remove-isolated --try-join-tls --verbose --seed 1 --osm-files eichstaett.osm --output-file
    eichstaett.net.xml
```

You can use this file if you need a road network of a town with about
13,000 inhabitants. All roads were verified to have the right highway
type, speed limit and one-way attribute. All traffic lights of cars (not
all pedestrian lights) were also verified. See also
[Tutorials/OSMActivityGen/eichstaett.osm](../../Tutorials/OSMActivityGen/eichstaett.osm.md).

| |                                             |
| ------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| Author  | [W. Bamberger](https://sourceforge.net/users/w-bamberger)                                                                                                                                                              |
| Source  | [Tutorials/OSMActivityGen/eichstaett.osm](../../Tutorials/OSMActivityGen/eichstaett.osm.md) by W. Bamberger. Map data © [OpenStreetMap contributors](https://www.openstreetmap.org/), [CC-BY-SA](https://creativecommons.org/licenses/by-sa/2.0/) |
| History | This file is located in the SUMO repository. See the log there.                                                                                                                                                        |
| License | <img src="../../images/CC-BY-SA-small.png"> This work is licensed under a [Creative Commons Attribution-ShareAlike 3.0 Unported License](https://creativecommons.org/licenses/by-sa/3.0/). The authors are listed in the history. |
