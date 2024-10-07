---
title: MATsim
---
# Import MATSim networks

For reading MATsim networks, give [netconvert](../../netconvert.md)
the option **--matsim-files** {{DT_FILE}}\[,{{DT_FILE}}\]\* or **--matsim** {{DT_FILE}}\[,{{DT_FILE}}\]\* for short. For example:

```
netconvert --matsim myMATsimNetwork.xml -o mySUMOnetwork.net.xml
```

MATsim does not work with the lane numbers of streets, instead, the
streets' capacities are used. This means that under circumstances the
information about the edges' lane numbers are missing. The importer tries
to determine the lane number from the given edges' capacities using:

```
LANE_NUMBER = MAXIMUM_FLOW / CAPACITY_NORM
```

The value of CAPACITY_NORM is controlled via the option **--capacity-norm** {{DT_FLOAT}} (default:
1800).

## Access permissions

MATSim features a number of so-called transport modes defined in its **org.matsim.api.core.v01.TransportMode** class. The lane access
permissions in SUMO are derived from the MATSim transport modes.

!!! caution
    Currently the access permissions depend on whether XML validation is conducted or not. When **--xml-validation auto** is used, the default
    transport mode "car" from the MATSim Document Type Definition (DTD) is used when no other mode is given in the `link` element. Otherwise
    in the absence of a mode value, all vehicle classes are allowed in SUMO.
