---
title: OpenDRIVE
---

# signal_POIs_from_xodr.py

This tool allows to:

- extract signal records from an `.xodr` file, that was converted to a SUMO network using [netconvert](../../netconvert.md)
- generate an {{AdditionalFile}} containing pois of `type='signal'`
- ensure POIs are positioned and associated to the appropriate edge's lanes


Example use:
```
python tools/import/opendrive/signal_POIs_from_xodr.py data/OpenDrive/scen.xodr data/sumo/net.net.xml
```
will create the file `data/sumo/signals.add.xml`